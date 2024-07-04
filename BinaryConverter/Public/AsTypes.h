#pragma once
#include "Engine_Defines.h"

// Bone
#define MAX_MODEL_TRANSFORMS 800
#define MAX_MODEL_KEYFRAMES 1200

struct asBone
{
	string		name;

	Matrix		transform;  /* Relative(�θ� ����) - ����ٰ� �θ��� m_CombinedTransformation�� ���ϸ� ��Ʈ �������� ��ȯ */
	Matrix		offsetTransform;  /* Global(��Ʈ ���� = m_CombinedTransformation) �� ����� */
	_int		index = -1;
	_int		parent = -1;

	_uint		depth;  /* Ʈ�������� ���̰�, �� ���� �ڽ��̳� */
};

struct asMesh
{
	string				name;

	_bool				isAinm;

	vector<VTXMESH>		verticesStatic;
	vector<VTXANIMMESH>	verticesAnim;
	vector<_int>		indices;

	_uint 				materialIndex; /* ���� ���� �ִ� ���¸��� �� �� �޽ø� �׸� ���¸����� �ε��� */

	vector<_int>		bones; /* ���� �޽ÿ� ������ �ִ� �� �ε��� */
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

	_float	fDuration;		/* ��ü ��� �ð� */
	_float	fTickPerSecond;	/* �ʴ� ��� �ӵ� */

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

// �������� -> (������ȣ, ����ġ)
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