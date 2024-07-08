#include "stdafx.h"
#include "Converter.h"
#include <filesystem>

CConverter::CConverter()
{
	_importer = make_shared<Assimp::Importer>();
}

CConverter::~CConverter()
{

}

HRESULT CConverter::Binarize_Model(wstring fileName, wstring savePath, const MODEL_TYPE& modelType)
{	
	wstring filePath{};
	
	/* Read Asset Data */
	{
		filePath = (filesystem::path(sourceUpperPath + fileName) / fileName).wstring() + TEXT(".fbx");
		Utils::Replace(filePath, TEXT("\\"), TEXT("/"));
		
		if (!filesystem::exists(filePath))
			return E_FAIL;
		
		if (FAILED(Read_AssetFile(filePath, modelType)))
			return E_FAIL;
	}

	/* Export Bone Data */
	{
		/* 읽기 먼저 */
		Read_BoneData(_scene->mRootNode, 0, -1, 0);	
	}
	
	wstring _destUpperPath;

	if (MODEL_TYPE::NONANIM == modelType)
		_destUpperPath = destUpperPath + TEXT("Static/");
	else
		_destUpperPath = destUpperPath + TEXT("Skeletal/");

	/* Export Mesh Data */
	{
		wstring strDirectory(_destUpperPath + savePath);
		filePath = (filesystem::path(strDirectory) / fileName).wstring() + TEXT(".mesh");
		Utils::Replace(filePath, TEXT("\\"), TEXT("/"));
		filesystem::create_directory(strDirectory);
		if (false == filesystem::is_directory(strDirectory))
			return E_FAIL;

		if (FAILED(Export_MeshData(filePath, modelType)))
			return E_FAIL;

		/* 메시에 영향주는 본들의 작업을 마치고 본을 익스포트 해야한다. */
		{
			filePath = (filesystem::path(strDirectory) / fileName).wstring() + TEXT(".bone");
			Utils::Replace(filePath, TEXT("\\"), TEXT("/"));

			if (FAILED(Write_BoneData(filePath)))
				return E_FAIL;
		}
	}

	/* Export Materail Data */
	{
		/* Src Path */
		wstring srcPath = filesystem::path(sourceUpperPath + fileName).wstring();

		if (!filesystem::exists(srcPath))
			return E_FAIL;

		/* Save Path */
		wstring strDirectory(_destUpperPath + savePath);
		filePath = filesystem::path(strDirectory).wstring();
		filesystem::create_directory(strDirectory);
		if (false == filesystem::is_directory(strDirectory))
			return E_FAIL;

		if (FAILED(Export_MaterialData(srcPath, filePath)))
			return E_FAIL;
	}

	/* Export Anim Data */
	if(MODEL_TYPE::ANIM == modelType)
	{
		wstring strDirectory(_destUpperPath + savePath);
		filePath = (filesystem::path(strDirectory) / fileName).wstring() + TEXT(".anim");
		Utils::Replace(filePath, TEXT("\\"), TEXT("/"));
		filesystem::create_directory(strDirectory);
		if (false == filesystem::is_directory(strDirectory))
			return E_FAIL;

		if (FAILED(Export_AnimData(filePath)))
			return E_FAIL;
	}

	cout << "Complete (Flie Name : " << Utils::ToString(fileName) + ".fbx)" << endl;

	return S_OK;
}

HRESULT CConverter::Binarize_All_Model()
{
	sourceUpperPath += TEXT("Static/");
	for (const auto& entry : filesystem::recursive_directory_iterator(sourceUpperPath))
	{
		if (entry.is_regular_file())
		{
			if (TEXT(".fbx") == entry.path().extension())
			{
				wstring strFileName(entry.path().stem());
				if (FAILED(Binarize_Model(strFileName, strFileName, MODEL_TYPE::NONANIM)))
					return E_FAIL;
				
				Free();
			}
		}
	}

	sourceUpperPath = wstring(filesystem::path(sourceUpperPath).parent_path().parent_path()) + TEXT("/Skeletal/");
	for (const auto& entry : filesystem::recursive_directory_iterator(sourceUpperPath))
	{
		if (entry.is_regular_file())
		{
			if (TEXT(".fbx") == entry.path().extension())
			{
				wstring strFileName(entry.path().stem());
				if (FAILED(Binarize_Model(strFileName, strFileName, MODEL_TYPE::ANIM)))
					return E_FAIL;

				Free();
			}
		}
	}

	return S_OK;
}

HRESULT CConverter::Binarize_One_Model(wstring fileName, wstring savePath, const MODEL_TYPE& modelType)
{
	if (MODEL_TYPE::NONANIM == modelType)
		sourceUpperPath += TEXT("Static/");
	else
		sourceUpperPath += TEXT("Skeletal/");

	for (const auto& entry : filesystem::recursive_directory_iterator(sourceUpperPath))
	{
		if (entry.is_regular_file())
		{
			if (TEXT(".fbx") == entry.path().extension() && fileName == entry.path().stem())
			{
				if (FAILED(Binarize_Model(fileName, savePath, modelType)))
					return E_FAIL;

				Free();

				return S_OK;
			}
		}
	}

	return S_OK;
}

HRESULT CConverter::Read_AssetFile(wstring srcPath, const MODEL_TYPE& modelType)
{
	if (MODEL_TYPE::TYPEEND == modelType)
		return E_FAIL;

	int	iFlag = 0;
	{
		/* aiProcess_PreTransformVertices : 모델을 구성하는 메시 중, 이 메시의 이름과 뼈의 이름이 같은 상황이라면 이 뼈의 행렬을 메시의 정점에 다 곱해서 로드한다. */
		/* 모든 애니메이션 정보는 폐기된다. */

		if (MODEL_TYPE::NONANIM == modelType)
			iFlag |= aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace;
			//iFlag |= aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_MaxQuality;
		else
			iFlag |= aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace;
	}

	_scene = _importer->ReadFile(Utils::ToString(srcPath), iFlag);
	{
		if (nullptr == _scene)
			__debugbreak();//ASSERT_LOG();
	}

	return S_OK;
}

HRESULT CConverter::Export_MeshData(wstring savePath, const MODEL_TYPE& modelType)
{
	if (FAILED(Read_MeshData(modelType)))
		return E_FAIL;

	if (FAILED(Write_MeshData(savePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CConverter::Export_MaterialData(wstring srcPath, wstring savePath)
{
	if (FAILED(Read_MaterialData()))
		__debugbreak();//ASSERT_LOG();

	if (FAILED(Write_MaterialData(srcPath, savePath)))
		__debugbreak();//ASSERT_LOG();

	return S_OK;
}

HRESULT CConverter::Export_AnimData(wstring savePath)
{
	if (FAILED(Read_AnimData()))
		return E_FAIL;

	if (FAILED(Write_AnimData(savePath)))
		return E_FAIL;

	return S_OK;
}

void CConverter::Read_BoneData(aiNode* node, _int index, _int parent, _int depth)
{
	shared_ptr<asBone> bone = make_shared<asBone>();
	{
		bone->name = node->mName.C_Str(); /* 노드 이름 = 뼈 이름 */

		bone->parent = parent;
		bone->index = index;
		bone->depth = depth;
	
		Matrix transform(node->mTransformation[0]);
		bone->transform = transform.Transpose();
	}

	_bones.push_back(bone);

	for (_uint i = 0; i < node->mNumChildren; i++)
		Read_BoneData(node->mChildren[i], (_int)_bones.size(), index, bone->depth + 1);
}

HRESULT CConverter::Write_BoneData(wstring savePath)
{
	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(savePath, FileMode::Write);

	file->Write<size_t>(_bones.size());
	for (shared_ptr<asBone>& bone : _bones)
	{
		file->Write<string>(bone->name);
		file->Write<Matrix>(bone->transform);
		file->Write<Matrix>(bone->offsetTransform);
		file->Write<_int>(bone->index);
		file->Write<_int>(bone->parent);
		file->Write<_uint>(bone->depth);
	}

	return S_OK;
}

_int CConverter::Find_BoneIndex(const _char* szBoneName)
{
	for (_int i = 0; i < _bones.size(); ++i)
	{
		if (!strcmp(_bones[i]->name.c_str(), szBoneName))
			return i;
	}

	return 0;
}

HRESULT CConverter::Read_MeshData(MODEL_TYPE modelType)
{
	if (nullptr == _scene)
		return E_FAIL;

	/* 모델이 가진 메시를 전부 순회하며 메시 구조체를 만들어 _meshes에 푸시한다. */
	for (_uint i = 0; i < _scene->mNumMeshes; i++)
	{
		shared_ptr<asMesh> mesh = make_shared<asMesh>();
		mesh->name = _scene->mMeshes[i]->mName.data;

		const aiMesh* srcMesh = _scene->mMeshes[i];

		/* Vertices - Static */ 
		if (modelType == MODEL_TYPE::NONANIM) 
		{
			mesh->isAinm = (_uint)modelType;

			mesh->verticesStatic.reserve(srcMesh->mNumVertices);

			VTXMESH vertex{};
			for (_uint j = 0; j < srcMesh->mNumVertices; j++)
			{
				memcpy(&vertex.vPosition, &srcMesh->mVertices[j], sizeof(Vec3));
				memcpy(&vertex.vNormal, &srcMesh->mNormals[j], sizeof(Vec3));
				memcpy(&vertex.vTexcoord, &srcMesh->mTextureCoords[0][j], sizeof(Vec2));
				memcpy(&vertex.vTangent, &srcMesh->mTangents[j], sizeof(Vec3));
				
				mesh->verticesStatic.push_back(vertex);	
			}
		}
		/* Vertices - Amim */
		else if (modelType == MODEL_TYPE::ANIM)
		{
			mesh->isAinm = (_uint)modelType;

			mesh->verticesAnim.reserve(srcMesh->mNumVertices);
			for (size_t j = 0; j < srcMesh->mNumVertices; j++)
				mesh->verticesAnim.push_back(VTXANIMMESH{});

			for (_uint j = 0; j < mesh->verticesAnim.size(); j++)
			{
				memcpy(&mesh->verticesAnim[j].vPosition, &srcMesh->mVertices[j], sizeof(Vec3));
				memcpy(&mesh->verticesAnim[j].vNormal, &srcMesh->mNormals[j], sizeof(Vec3));
				memcpy(&mesh->verticesAnim[j].vTexture, &srcMesh->mTextureCoords[0][j], sizeof(Vec2));
				memcpy(&mesh->verticesAnim[j].vTangent, &srcMesh->mTangents[j], sizeof(Vec3));
			}

			/* Static과 달리 해당 메시에 영향을 주는 뼈의 정보를 저장한다. */
			for (_uint j = 0; j < srcMesh->mNumBones; ++j)
			{
				aiBone* pAIBone = srcMesh->mBones[j];

				for (_uint k = 0; k < pAIBone->mNumWeights; ++k)
				{
					_uint		iVertexIndex = pAIBone->mWeights[k].mVertexId;

					if (0.0f == mesh->verticesAnim[iVertexIndex].vBlendWeight.x)
					{
						mesh->verticesAnim[iVertexIndex].vBlendIndex.x = Find_BoneIndex(pAIBone->mName.C_Str());
						mesh->verticesAnim[iVertexIndex].vBlendWeight.x = pAIBone->mWeights[k].mWeight;
					}

					else if (0.0f == mesh->verticesAnim[iVertexIndex].vBlendWeight.y)
					{
						mesh->verticesAnim[iVertexIndex].vBlendIndex.y = Find_BoneIndex(pAIBone->mName.C_Str());
						mesh->verticesAnim[iVertexIndex].vBlendWeight.y = pAIBone->mWeights[k].mWeight;
					}

					else if (0.0f == mesh->verticesAnim[iVertexIndex].vBlendWeight.z)
					{
						mesh->verticesAnim[iVertexIndex].vBlendIndex.z = Find_BoneIndex(pAIBone->mName.C_Str());
						mesh->verticesAnim[iVertexIndex].vBlendWeight.z = pAIBone->mWeights[k].mWeight;
					}

					else if (0.0f == mesh->verticesAnim[iVertexIndex].vBlendWeight.w)
					{
						mesh->verticesAnim[iVertexIndex].vBlendIndex.w = Find_BoneIndex(pAIBone->mName.C_Str());
						mesh->verticesAnim[iVertexIndex].vBlendWeight.w = pAIBone->mWeights[k].mWeight;
					}
				}
			}
		}
		/* Indices*/
		mesh->indices.reserve(srcMesh->mNumFaces * 3);
		for (_uint j = 0; j < srcMesh->mNumFaces; j++)
		{
			aiFace& face = srcMesh->mFaces[j];

			for (_uint  k = 0; k < 3; k++)
			{
				mesh->indices.push_back(face.mIndices[k]);
			}
		}

		/* Material Index */
		mesh->materialIndex = srcMesh->mMaterialIndex;

		/* Bones (현재 메시에 영향을 주는 뼈들을 순회하며 행렬정보를 저장하고 뼈들을 컨테이너에 모아둔다. */
		_uint numBones = srcMesh->mNumBones;

		for (_uint j = 0; j < numBones; j++)
		{
			aiBone* srcMeshBone = srcMesh->mBones[j];

			Matrix offsetMatrix;
			memcpy(&offsetMatrix, &srcMeshBone->mOffsetMatrix, sizeof(Matrix));

			_uint boneIndex = Get_BoneIndex(srcMeshBone->mName.C_Str());
			_bones[boneIndex]->offsetTransform = offsetMatrix.Transpose();

			mesh->bones.push_back(boneIndex);
		}

		_meshes.push_back(mesh);
	}

	return S_OK;
}

HRESULT CConverter::Write_MeshData(wstring savePath)
{
	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(savePath, FileMode::Write);

	file->Write<size_t>(_meshes.size());
	for (shared_ptr<asMesh>& mesh : _meshes)
	{
		/* name */
		file->Write<string>(mesh->name);

		/* isAnim */
		file->Write<_bool>(mesh->isAinm);

		/* Vertices */
		if ((_uint)MODEL_TYPE::NONANIM == mesh->isAinm)
		{
			file->Write<size_t>(mesh->verticesStatic.size());
			for (VTXMESH& vertex : mesh->verticesStatic)
			{
				file->Write<Vec3>(vertex.vPosition);
				file->Write<Vec3>(vertex.vNormal);
				file->Write<Vec2>(vertex.vTexcoord);
				file->Write<Vec3>(vertex.vTangent);
			}
		}
		else
		{
			file->Write<size_t>(mesh->verticesAnim.size());
			for (VTXANIMMESH& vertex : mesh->verticesAnim)
			{
				file->Write<Vec3>(vertex.vPosition);
				file->Write<Vec3>(vertex.vNormal);
				file->Write<Vec2>(vertex.vTexture);
				file->Write<Vec3>(vertex.vTangent);
				file->Write<XMUINT4>(vertex.vBlendIndex);
				file->Write<Vec4>(vertex.vBlendWeight);
			}
		}

		/* Indices */
		file->Write<size_t>(mesh->indices.size());
		for (int& index : mesh->indices)
		{
			file->Write<int>(index);
		}

		/* Material Index */
		file->Write<_uint>(mesh->materialIndex);

		/* Bone Indices */
		file->Write<size_t>(mesh->bones.size());
		for (int& index : mesh->bones)
		{
			file->Write<int>(index);
		}
	}
	return S_OK;
}

HRESULT CConverter::Read_MaterialData()
{
	/* 모델이 가지고 있는 매태리얼을 순회한다. */
	for (_uint i = 0; i < _scene->mNumMaterials; i++)
	{
		aiMaterial* srcMaterial = _scene->mMaterials[i];
		shared_ptr<asMaterial> material = make_shared<asMaterial>();
		{
			/* 매태리얼이 사용하는 텍스처의 경로를 저장한다. */
			aiString file;
			string name;
			{
				if (aiReturn_SUCCESS == srcMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file))
				{
					name = file.C_Str();
					material->diffuseFilePath = filesystem::path(name).filename().string();
				}

				if (aiReturn_SUCCESS == srcMaterial->GetTexture(aiTextureType_NORMALS, 0, &file))
				{
					name = file.C_Str();
					material->normalFilePath = filesystem::path(name).filename().string();
				}

				/*if (aiReturn_SUCCESS == srcMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &file))
				{
					name = file.C_Str();
					material->emissiveFilePath = filesystem::path(name).filename().string();
				}*/
			}
		}
		_materials.push_back(material);
	}
	return S_OK;
}

HRESULT CConverter::Write_MaterialData(wstring srcPath, wstring savePath)
{
	wstring path;
	wstring finalSrcPath;
	wstring finalSavePath;

	/* 텍스처 복사 */
	for (shared_ptr<asMaterial> material : _materials)
	{
		wstring filePath;
		if (!material->diffuseFilePath.empty())
		{
			/* Diffuse */
			finalSrcPath = srcPath + TEXT("/") + Utils::ToWString(material->diffuseFilePath);
			finalSrcPath = filesystem::absolute(finalSrcPath).wstring();
			finalSavePath = filesystem::absolute(savePath).wstring();

			filePath = finalSavePath + TEXT("\\") + Utils::ToWString(material->diffuseFilePath);
			if (!filesystem::exists(filePath))
				filesystem::copy(filesystem::path(finalSrcPath), filesystem::path(finalSavePath));
		}

		if (!material->normalFilePath.empty())
		{
			/* Normal */
			finalSrcPath = srcPath + TEXT("/") + Utils::ToWString(material->normalFilePath);
			finalSrcPath = filesystem::absolute(finalSrcPath).wstring();
			finalSavePath = filesystem::absolute(savePath).wstring();

			filePath = finalSavePath + TEXT("\\") + Utils::ToWString(material->normalFilePath);
			if (!filesystem::exists(filePath))
				filesystem::copy(filesystem::path(finalSrcPath), filesystem::path(finalSavePath));
		}

		//if (!material->emissiveFilePath.empty())
		//{
		//	/* Specular */
		//	finalSrcPath = srcPath + TEXT("/") + Utils::ToWString(material->emissiveFilePath);
		//	finalSrcPath = filesystem::absolute(finalSrcPath).wstring();
		//	finalSavePath = filesystem::absolute(savePath).wstring();

		//	filePath = finalSavePath + TEXT("\\") + Utils::ToWString(material->emissiveFilePath);
		//	if (!filesystem::exists(filePath))
		//		filesystem::copy(filesystem::path(finalSrcPath), filesystem::path(finalSavePath));
		//}
	}

	/* 매태리얼 정보 저장 */
	shared_ptr<FileUtils> file = make_shared<FileUtils>();

	wstring fileName = filesystem::path(savePath).filename().wstring();
	wstring filePath = (filesystem::path(savePath) / wstring(fileName + TEXT(".mat")));
	Utils::Replace(filePath, TEXT("\\"), TEXT("/"));
	file->Open(filePath, FileMode::Write);

	file->Write<size_t>(_materials.size());
	for (shared_ptr<asMaterial> material : _materials)
	{
		if (!material->diffuseFilePath.empty()) { file->Write<string>(material->diffuseFilePath); }
		if (!material->normalFilePath.empty()) { file->Write<string>(material->normalFilePath); }
		//if (!material->emissiveFilePath.empty()) { file->Write<string>(material->emissiveFilePath); }
	}

	return S_OK;
}

HRESULT CConverter::Read_AnimData()
{
	/* 모델이 들고 있는 모든 애니메이션 순회 */
	for (_uint i = 0; i < _scene->mNumAnimations; i++)
	{
		aiAnimation* srcAnimation = _scene->mAnimations[i];
		shared_ptr<asAnimation> animation = make_shared<asAnimation>();

		animation->name = srcAnimation->mName.C_Str();
		animation->fDuration = srcAnimation->mDuration;
		animation->fTickPerSecond = srcAnimation->mTicksPerSecond;

		/* 현재 애니메이션의 모든 채널 순회*/
		for (_uint j = 0; j < srcAnimation->mNumChannels; j++)
		{
			shared_ptr<asChannel> channel = make_shared<asChannel>();
			aiNodeAnim* srcNode = srcAnimation->mChannels[j];

			channel->name = srcNode->mNodeName.data;

			_uint numKeyframes = max(srcNode->mNumScalingKeys, srcNode->mNumRotationKeys);
			numKeyframes = max(numKeyframes, srcNode->mNumPositionKeys);

			Vec3			vScale{};
			Quaternion		quatRotation{};
			Vec3			vPosition{};

			/* 현재 채널의 모든 키프레임 순회 */
			for (_uint k = 0; k < numKeyframes; k++)
			{
				shared_ptr<asKeyFrame>	keyframe = make_shared<asKeyFrame>();

				if (k < srcNode->mNumScalingKeys)
				{
					memcpy(&vScale, &srcNode->mScalingKeys[k].mValue, sizeof(Vec3));
					keyframe->fTime = srcNode->mScalingKeys[k].mTime;
				}
				if (k < srcNode->mNumRotationKeys)
				{
					aiQuatKey key = srcNode->mRotationKeys[k];

					quatRotation.x = key.mValue.x;
					quatRotation.y = key.mValue.y;
					quatRotation.z = key.mValue.z;
					quatRotation.w = key.mValue.w;
					keyframe->fTime = (_float)key.mTime;
				}
				if (k < srcNode->mNumPositionKeys)
				{
					if ("Root" == channel->name.substr(channel->name.find_last_of("_") + 1))
					{
						Vec3 Zero = Vec3(0.f, 0.f, 0.f);
						memcpy(&vPosition, &Zero, sizeof(Vec3));
					}
					else
						memcpy(&vPosition, &srcNode->mPositionKeys[k].mValue, sizeof(Vec3));
					
					keyframe->fTime = srcNode->mPositionKeys[k].mTime;
				}

				keyframe->vScale = vScale;
				keyframe->quatRotation = quatRotation;
				keyframe->vPosition = vPosition;

				channel->keyframes.push_back(keyframe);
			}
			animation->channels.push_back(channel);
		}
		_animations.push_back(animation);
	}
	return S_OK;
}

HRESULT CConverter::Write_AnimData(wstring savePath)
{
	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(savePath, FileMode::Write);

	file->Write<size_t>(_animations.size());
	for (shared_ptr<asAnimation>& animation : _animations)
	{
		file->Write<string>(animation->name);
		file->Write<_float>(animation->fDuration);
		file->Write<_float>(animation->fTickPerSecond);

		file->Write<size_t>(animation->channels.size());
		for (shared_ptr<asChannel>& channel : animation->channels)
		{
			file->Write<string>(channel->name);

			file->Write<size_t>(channel->keyframes.size());
			for (shared_ptr<asKeyFrame>& keyframe : channel->keyframes)
			{
				file->Write<_float>(keyframe->fTime);
				file->Write<Vec3>(keyframe->vScale);
				file->Write<Quaternion>(keyframe->quatRotation);
				file->Write<Vec3>(keyframe->vPosition);
			}
		}
	}
	return S_OK;
}

_uint CConverter::Get_BoneIndex(const string& name)
{
	for (shared_ptr<asBone>& bone : _bones)
	{
		if (bone->name == name)
			return bone->index;
	}

	assert(false);
	return 0;
}

HRESULT CConverter::Free()
{
	//Safe_Delete(_scene);
	//_scene->~aiScene();

	_bones.clear();
	_meshes.clear();
	_materials.clear();
	_animations.clear();

	return S_OK;
}
