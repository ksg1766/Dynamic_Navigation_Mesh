#pragma once
#include "Engine_Defines.h"

// Bone
#define MAX_MODEL_TRANSFORMS 800
#define MAX_MODEL_KEYFRAMES 1200

struct asBone
{
	string		name;

	Matrix		transform;  /* Relative(부모 기준) - 여기다가 부모의 m_CombinedTransformation을 곱하면 루트 기준으로 변환 */
	Matrix		offsetTransform;  /* Global(루트 기준 = m_CombinedTransformation) 의 역행렬 */
	_int		index = -1;
	_int		parent = -1;

	_uint		depth;  /* 트리에서의 깊이값, 즉 몇차 자식이냐 */
};

struct asMesh
{
	string				name;

	_bool				isAinm;

	vector<VTXMESH>		verticesStatic;
	vector<VTXANIMMESH>	verticesAnim;
	vector<_int>		indices;

	_uint 				materialIndex; /* 모델이 갖고 있는 매태리얼 중 이 메시를 그릴 매태리얼의 인덱스 */

	vector<_int>		bones; /* 현재 메시에 영향을 주는 뼈 인덱스 */
};

struct asMaterial
{
	string diffuseFilePath;
	string normalFilePath;
	//string emissiveFilePath;
};

struct asKeyFrame
{
	_float	fTime;
	Vec3	vScale;
	Quaternion	quatRotation;
	Vec3	vPosition;
};

struct asChannel
{
	string name; /* Bone Name */

	vector<shared_ptr<asKeyFrame>> keyframes;
};

struct asAnimation
{
	string name;

	_float	fDuration;		/* 전체 재생 시간 */
	_float	fTickPerSecond;	/* 초당 재생 속도 */

	vector<shared_ptr<asChannel>>	channels;
};

// Cache
struct asAnimationNode
{
	aiString name;
	vector<asKeyFrame> keyframe;
};


// Animation
struct asBlendWeight
{
	void Set(uint32 index, uint32 boneIndex, float weight)
	{
		float i = (float)boneIndex;
		float w = weight;

		switch (index)
		{
		case 0: indices.x = i; weights.x = w; break;
		case 1: indices.y = i; weights.y = w; break;
		case 2: indices.z = i; weights.z = w; break;
		case 3: indices.w = i; weights.w = w; break;
		}
	}

	XMUINT4 indices = XMUINT4(0, 0, 0, 0);
	Vec4 weights = Vec4(0, 0, 0, 0);
};

// 정점마다 -> (관절번호, 가중치)
struct asBoneWeights
{
	void AddWeights(uint32 boneIndex, float weight)
	{
		if (weight <= 0.0f)
			return;

		auto findIt = std::find_if(boneWeights.begin(), boneWeights.end(),
			[weight](const Pair& p) { return weight > p.second; });

		boneWeights.insert(findIt, Pair(boneIndex, weight));
	}

	asBlendWeight GetBlendWeights()
	{
		asBlendWeight blendWeights;

		for (uint32 i = 0; i < boneWeights.size(); i++)
		{
			if (i >= 4)
				break;

			blendWeights.Set(i, boneWeights[i].first, boneWeights[i].second);
		}

		return blendWeights;
	}

	void Normalize()
	{
		if (boneWeights.size() >= 4)
			boneWeights.resize(4);

		float totalWeight = 0.f;
		for (const auto& item : boneWeights)
			totalWeight += item.second;

		float scale = 1.f / totalWeight;
		for (auto& item : boneWeights)
			item.second *= scale;
	}

	using Pair = pair<_int, _float>;
	vector<Pair> boneWeights;
};