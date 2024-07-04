#include "..\Public\Model.h"
#include "GameObject.h"
#include "Shader.h"
#include "Bone.h"
#include "Mesh.h"
#include "Transform.h"
#include "Animation.h"
#include "Channel.h"
#include "Socket.h"
#include "Texture.h"
#include "Utils.h"
#include "FileUtils.h"
#include <filesystem>
#include "LevelManager.h"

_int CModel::m_iNextInstanceID = 0;
map<_int, ID3D11ShaderResourceView*> CModel::m_mapVTFExist;

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: Super(pDevice, pContext, ComponentType::Model)
{
}

CModel::CModel(const CModel& rhs)
	: Super(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_eModelType(rhs.m_eModelType)
	, m_matPivot(rhs.m_matPivot)
	, m_Bones(rhs.m_Bones)
	, m_Meshes(rhs.m_Meshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_Animations(rhs.m_Animations)
	, m_hashAnimIndices(rhs.m_hashAnimIndices)
	, m_vecSocketBones(rhs.m_vecSocketBones)
	, m_PartsModel(rhs.m_PartsModel)
	, m_HasParent(rhs.m_HasParent)
	, m_iSocketBoneIndex(rhs.m_iSocketBoneIndex)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_iInstanceID(rhs.m_iInstanceID)
	, m_pTexture(rhs.m_pTexture)
	, m_pSRV(rhs.m_pSRV)
	, m_vecSurfaceVtx(rhs.m_vecSurfaceVtx)
	, m_vecSurfaceIdx(rhs.m_vecSurfaceIdx)
{
	/* Bones */
	for (auto& pBone : m_Bones)
		Safe_AddRef(pBone);

	/* Meshes */
	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	/* Materials */
	for (auto& Material : m_Materials)
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i) /* TODO 이거 어차피 3개만 쓰는데 ? */
			Safe_AddRef(Material.pTextures[i]);

	/* Animations */
	for (auto& pAnimation : m_Animations)
		Safe_AddRef(pAnimation);
}

HRESULT CModel::Initialize_Prototype(const wstring& strModelFilePath, const SOCKETDESC& desc, _fmatrix& matPivot)
{
	XMStoreFloat4x4(&m_matPivot, matPivot);
	//XMStoreFloat4x4(&m_matPivot, XMMatrixScaling(0.2f, 0.2f, 0.2f) * XMMatrixRotationY(XMConvertToRadians(90.0f)));

	/* DirectX로 그려낼 수 있도록 데이터들을 정리한다. */

	if (FAILED(Ready_Bones(strModelFilePath, desc)))
		return E_FAIL;

	if (FAILED(Ready_Meshes(strModelFilePath, m_matPivot)))
		return E_FAIL;

	if (FAILED(Ready_Materials(strModelFilePath)))
		return E_FAIL;

	if (TYPE_ANIM == m_eModelType)
	{
		if (FAILED(Ready_Animations(strModelFilePath)))
			return E_FAIL;
	}

	m_iInstanceID = m_iNextInstanceID++;

	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{
	CLevelManager* pInstance = GET_INSTANCE(CLevelManager);

	/* Deep Copy - Bones */
	vector<CBone*>			Bones;
	Bones.reserve(m_Bones.size());

	for (auto& pPrototype : m_Bones)
	{
		CBone* pBone = (CBone*)pPrototype->Clone();
		if (nullptr == pBone)
			return E_FAIL;

		Bones.push_back(pBone);
		//Safe_Release(pPrototype);
	}
	m_Bones.clear();
	m_Bones = Bones;

	/* 본의 부모 인덱스를 바탕으로 실제 부모를 연결한다. */
	for (auto& pBone : m_Bones)
	{
		if (nullptr == pBone)
			return E_FAIL;

		pBone->Set_Parent(GetBone(pBone->Get_ParentIndex()));
	}

	/* Deep Copy - Meshes */
	vector<CMesh*>	Meshes;
	Meshes.reserve(m_Meshes.size());

	for (auto& pPrototype : m_Meshes)
	{
		CMesh* pMesh = (CMesh*)pPrototype->Clone(m_pGameObject, this);
		if (nullptr == pMesh)
			return E_FAIL;

		Meshes.push_back(pMesh);
		Safe_Release(pPrototype);
	}
	m_Meshes.clear();
	m_Meshes = Meshes;

	/* Deep Copy - Animations */
	if (TYPE_ANIM == m_eModelType)
	{
		vector<CAnimation*>		Animations;
		Animations.reserve(m_Animations.size());

		_uint iMax = 0;
		for (auto& pPrototype : m_Animations)
		{
			CAnimation* pAnimation = pPrototype->Clone(this);
			if (nullptr == pAnimation)
				return E_FAIL;

			iMax = max(iMax, pPrototype->GetMaxFrameCount());

			Animations.push_back(pAnimation);
			Safe_Release(pPrototype);
		}
		m_iMaxFrameCount = iMax;

		m_Animations.clear();
		m_Animations = Animations;

		map<_int, ID3D11ShaderResourceView*>::iterator iterSrv = m_mapVTFExist.find(m_iInstanceID);

		if (m_Animations.size() && (iterSrv == m_mapVTFExist.end()) && !m_pTexture) // TODO: 크흠...
		{
			if (FAILED(CreateVertexTexture2DArray()))
				return E_FAIL;

			/*for (auto& pParts : m_PartsModel)
				pParts->SetSRV(m_pSRV);*/

			if (3/*LEVEL_GAMETOOL*/ != pInstance->GetCurrentLevelIndex())
			{
				for (auto& pAnimation : m_Animations)
				{
					for (auto& pChannel : pAnimation->GetChannels())
						Safe_Release(pChannel);

					//m_Bones

					pAnimation->GetChannels().clear();
				}
			}

			m_mapVTFExist.emplace(m_iInstanceID, m_pSRV);
		}
		else  if (!(iterSrv == m_mapVTFExist.end()))
		{
			m_pSRV = iterSrv->second;
			Safe_AddRef(m_pSRV);
		}
	}

	m_pShader = m_pGameObject->GetShader();
	Safe_AddRef(m_pShader);

	if (2/*LEVEL_GAMEPLAY*/ == pInstance->GetCurrentLevelIndex())
	{
		m_vecSurfaceVtx.clear();
		m_vecSurfaceIdx.clear();
	}

	RELEASE_INSTANCE(CLevelManager);

	return S_OK;
}

void CModel::Tick(const _float& fTimeDelta)
{
	if (TYPE_ANIM == m_eModelType)
	{
		if (!m_IsAnimPaused)
			UpdateTweenData(fTimeDelta);

		for (auto& parts : m_PartsModel)
		{
			if(parts)
				parts->SetTweenDesc(m_TweenDesc);
		}
	}
	// Socket은 NonAnim 모델일테니 어차피 Tick이 안도는거나 마찬가지.
}

void CModel::DebugRender()
{
}

HRESULT CModel::Render()
{
	//if (TYPE_ANIM == m_eModelType)
	if (m_pSRV)
	{
		if (FAILED(m_pShader->Bind_Texture("g_TransformMap", m_pSRV)))
			return E_FAIL;

		/* 본의 최종 트랜스폼 계산 : <오프셋 * 루트 기준 * 사전변환> */
		if (FAILED(m_pShader->Bind_RawValue("g_Tweenframes", &m_TweenDesc, sizeof(TWEENDESC))))
			return E_FAIL;
		
		if(m_HasParent)
			if (FAILED(m_pShader->Bind_RawValue("g_iSocketBoneIndex", &m_iSocketBoneIndex, sizeof(_int))))
				return E_FAIL;
	}

	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		if (FAILED(BindMaterialTexture(m_pShader, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		if (FAILED(BindMaterialTexture(m_pShader, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;
	
		if (FAILED(m_pShader->Begin()))
			return E_FAIL;

		if (FAILED(m_Meshes[i]->Render()))
			return E_FAIL;
	}

	for (auto& pParts : m_PartsModel)
	{
		if(pParts)
			pParts->GetTransform()->Set_WorldMatrix(GetTransform()->WorldMatrix());
	}

	return S_OK;
}

HRESULT CModel::RenderShadowInstancing(CVIBuffer_Instance*& pInstanceBuffer)
{
	if (m_pSRV)
	{
		if (FAILED(m_pShader->Bind_Texture("g_TransformMap", m_pSRV)))
			return E_FAIL;
	}

	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		/*if (FAILED(BindMaterialTexture(m_pShader, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;*/

		if (FAILED(m_pShader->Begin()))
			return E_FAIL;

		if (FAILED(pInstanceBuffer->Render(m_Meshes[i])))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel::RenderInstancing(CVIBuffer_Instance*& pInstanceBuffer)
{
	if (m_pSRV)
	{
		if (FAILED(m_pShader->Bind_Texture("g_TransformMap", m_pSRV)))
			return E_FAIL;
	}

	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		if (FAILED(BindMaterialTexture(m_pShader, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(BindMaterialTexture(m_pShader, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pShader->Begin()))
			return E_FAIL;

		if (FAILED(pInstanceBuffer->Render(m_Meshes[i])))
			return E_FAIL;
	}

	return S_OK;
}

void CModel::PushTweenData(const InstancedTweenDesc& desc)
{
	/* 본의 최종 트랜스폼 계산 : <오프셋 * 루트 기준 * 사전변환> */
	if(FAILED(m_pGameObject->GetShader()->Bind_RawValue("g_TweenInstances", &desc, MAX_INSTANCE * sizeof(TWEENDESC))))
		__debugbreak();
}

HRESULT CModel::BindMaterialTexture(CShader* pShader, const _char* pConstantName, _uint iMaterialIndex, aiTextureType eType)
{
	if (iMaterialIndex >= m_Materials.size())
		return E_FAIL;

	if (m_Materials[iMaterialIndex].pTextures[eType] == nullptr)
		return S_OK;

	return m_Materials[iMaterialIndex].pTextures[eType]->Bind_ShaderResource(pShader, pConstantName, 0);
}

HRESULT CModel::UpdateTweenData(const _float& fTimeDelta)
{
	if (m_iCurrentAnimIndex < 0)
		m_iCurrentAnimIndex = m_Animations.size() - 1;

	TWEENDESC& desc = m_TweenDesc;

	CAnimation* currentAnim = m_Animations[m_iCurrentAnimIndex];

	desc.curr.sumTime += fTimeDelta;

	if (currentAnim)
	{
		_float timePerFrame = 1 / currentAnim->GetTickPerSecond();
		if (desc.curr.sumTime >= timePerFrame)
		{
			desc.curr.sumTime = 0.f;
			desc.curr.currFrame = (desc.curr.currFrame + 1) % currentAnim->GetMaxFrameCount();
			desc.curr.nextFrame = (desc.curr.currFrame + 1) % currentAnim->GetMaxFrameCount();
		}

		desc.curr.ratio = (desc.curr.sumTime / timePerFrame);
	}

	// 다음 애니메이션이 예약 되어 있다면
	if (desc.next.animIndex >= 0)
	{
		desc.tweenSumTime += fTimeDelta;
		desc.tweenRatio = desc.tweenSumTime / desc.tweenDuration;

		if (desc.tweenRatio >= 1.f)
		{
			// 애니메이션 교체 성공
			desc.curr = desc.next;
			m_iCurrentAnimIndex = desc.next.animIndex;
			desc.ClearNextAnim();
		}
		else
		{
			// 교체중
			CAnimation* nextAnim = m_Animations[desc.next.animIndex];
			desc.next.sumTime += fTimeDelta;

			_float timePerFrame = 1.f / nextAnim->GetTickPerSecond();

			if (desc.next.ratio >= 1.f)
			{
				desc.next.sumTime = 0;

				desc.next.currFrame = (desc.next.currFrame + 1) % nextAnim->GetMaxFrameCount();
				desc.next.nextFrame = (desc.next.currFrame + 1) % nextAnim->GetMaxFrameCount();
			}

			desc.next.ratio = desc.next.sumTime / timePerFrame;
		}
	}

	if (0 <= m_iNextAnimIndex)
	{
		desc.ClearNextAnim(); // 기존꺼 밀어주기
		m_iNextAnimIndex %= m_Animations.size();
		desc.next.animIndex = m_iNextAnimIndex;

		m_iNextAnimIndex = -1;
	}

	return S_OK;
}

CBone* CModel::GetBone(const _char* pNodeName)
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pNode)
		{
			return !strcmp(pNodeName, pNode->Get_Name());
		});

	if (iter == m_Bones.end())
		return nullptr;

	return *iter;
}

CBone* CModel::GetBone(const _int& iIndex)
{
	if (m_Bones.size() < iIndex || iIndex < 0)
		return nullptr;

	return m_Bones[iIndex];
}

_int CModel::GetAnimationIndexByName(const wstring& strAnimName)
{
	ANIMINDEX::iterator iter = m_hashAnimIndices.find(strAnimName);

	if (iter == m_hashAnimIndices.end())
	{
		return -1;
	}

	return iter->second;
}

_float CModel::GetAnimationTimeByIndex(const _int& iIndex)
{
	_float fAnimTime = m_Animations[iIndex]->GetDuaration() / m_Animations[iIndex]->GetTickPerSecond();
	return fAnimTime;
}

_uint CModel::GetMaterialIndex(_uint iMeshIndex)
{
	return m_Meshes[iMeshIndex]->Get_MaterialIndex();
}

void CModel::SetNextAnimationIndex(_int iAnimIndex)
{
	m_iNextAnimIndex = iAnimIndex;
}

HRESULT CModel::EquipParts(const _int& iSocketIndex, CModel* pModel)
{
	if (iSocketIndex >= m_vecSocketBones.size())
		return E_FAIL;

	m_PartsModel[iSocketIndex] = pModel;
	pModel->m_HasParent = true;
	pModel->m_iSocketBoneIndex = m_vecSocketBones[iSocketIndex];
	pModel->m_pSRV = m_pSRV;

	return S_OK;
}

HRESULT CModel::ReleaseAllVTF()
{
	for (auto iter = m_mapVTFExist.begin(); iter != m_mapVTFExist.end(); ++iter)
	{
		while (0 != Safe_Release(iter->second))
		{
			//Safe_Release(iter->second);
		}
		m_mapVTFExist.erase(iter);
	}

	if(!m_mapVTFExist.empty())
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Ready_Bones(const wstring& strModelFilePath, const SOCKETDESC& desc)
{
	/* 파일 셋업 */
	wstring folderName, filePath;
	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	{
		folderName = filesystem::path(strModelFilePath).stem();
		filePath = (filesystem::path(strModelFilePath) / wstring(folderName + TEXT(".bone")));
		Utils::Replace(filePath, TEXT("\\"), TEXT("/"));

		if (!filesystem::exists(filePath))
			return E_FAIL;

		file->Open(filePath, FileMode::Read);
	}

	/* 모든 본 순회 (부모는 인덱스로만 연결, 실제 포인터는 클론에서 수행) */
	size_t iSize = file->Read<size_t>();
	for (size_t i = 0; i < iSize; i++)
	{
		string	strName = file->Read<string>();
		Matrix	transformMat = file->Read<Matrix>();
		Matrix	offsetMat = file->Read<Matrix>();
		_int	iBoneIndex = file->Read<_int>();
		_int	iParentIndex = file->Read<_int>();
		_uint	iDepth = file->Read<_uint>();

		CBone* pBone = CBone::Create(strName, transformMat, offsetMat, iBoneIndex, iParentIndex, iDepth);
		if (nullptr == pBone)
			return E_FAIL;

		for (auto& strSocket : desc.vecSocketBoneNames)
		{
			if (strSocket == strName)
			{	// Initialize에서 해줘야하나?
				m_vecSocketBones.push_back(iBoneIndex);
			}
		}
		m_PartsModel.resize(m_vecSocketBones.size());

		m_Bones.push_back(pBone);
	}

	return S_OK;
}

HRESULT CModel::Ready_Meshes(const wstring& strModelFilePath, Matrix matPivot)
{
	/* 파일 셋업 */
	wstring folderName, filePath;
	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	{
		folderName = filesystem::path(strModelFilePath).stem();
		filePath = (filesystem::path(strModelFilePath) / wstring(folderName + TEXT(".mesh")));
		Utils::Replace(filePath, TEXT("\\"), TEXT("/"));

		if (!filesystem::exists(filePath))
			return E_FAIL;

		file->Open(filePath, FileMode::Read);
	}

	/* 모든 메시 순회 */
	size_t iNumMeshes = file->Read<size_t>();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		/* Name, Type */
		string					strName = file->Read<string>();
		_bool					bAnim = file->Read<_bool>();
		m_eModelType = bAnim ? TYPE_ANIM : TYPE_NONANIM;

		vector<VTXMESH>			StaticVertices;
		vector<VTXANIMMESH>		AnimVertices;
		vector<_int>			Indiecs;
		_uint					iMaterialIndex;
		vector<_int>			BoneIndices;

		/* Vertices */
		if (bAnim)
		{
			size_t iNumVertices = file->Read<size_t>();
			AnimVertices.reserve(iNumVertices);

			for (size_t j = 0; j < iNumVertices; j++)
			{
				VTXANIMMESH vertex;

				vertex.vPosition = file->Read<Vec3>();
				vertex.vNormal = file->Read<Vec3>();
				vertex.vTexture = file->Read<Vec2>();
				vertex.vTangent = file->Read<Vec3>();
				vertex.vBlendIndex = file->Read<XMUINT4>();
				vertex.vBlendWeight = file->Read<Vec4>();

				AnimVertices.push_back(vertex);
			}
		}
		else
		{
			size_t iNumVertices = file->Read<size_t>();
			StaticVertices.reserve(iNumVertices);

			for (size_t j = 0; j < iNumVertices; j++)
			{
				VTXMESH vertex;

				vertex.vPosition = file->Read<Vec3>();
				vertex.vNormal = file->Read<Vec3>();
				vertex.vTexcoord = file->Read<Vec2>();
				vertex.vTangent = file->Read<Vec3>();

				StaticVertices.push_back(vertex);
			}
		}

		/* Indices */
		size_t iNumIndices = file->Read<size_t>();
		Indiecs.reserve(iNumIndices);
		for (size_t j = 0; j < iNumIndices; j++)
			Indiecs.push_back(file->Read<_int>());

		/* Material Index */
		iMaterialIndex = file->Read<_uint>();

		/* Bone Indices*/
		size_t iNumBoneIndices = file->Read<size_t>();
		BoneIndices.reserve(iNumBoneIndices);
		for (size_t j = 0; j < iNumBoneIndices; j++)
			BoneIndices.push_back(file->Read<_int>());

		//CLevelManager* pInstance = GET_INSTANCE(CLevelManager);
		//if (/*LEVEL_GAMETOOL*//*3 == pInstance->GetCurrentLevelIndex() && */!bAnim)
		//{
		//	for (size_t i = 0; i < StaticVertices.size(); ++i)
		//	{
		//		m_vecSurfaceVtx.push_back(StaticVertices[i].vPosition);
		//	}
		//	for (size_t i = 0; i < Indiecs.size() / 3; ++i)
		//	{
		//		FACEINDICES32 idx = { Indiecs[3 * i], Indiecs[3 * i + 1], Indiecs[3 * i + 2] };
		//		m_vecSurfaceIdx.push_back(idx);
		//	}
		//}
		//RELEASE_INSTANCE(CLevelManager);

		/* Create Mesh */
		CMesh* pMesh = nullptr;
		{
			pMesh = (bAnim) ? CMesh::Create(m_pDevice, m_pContext, strName, AnimVertices, Indiecs, iMaterialIndex, BoneIndices, this) :
				CMesh::Create(m_pDevice, m_pContext, strName, StaticVertices, Indiecs, iMaterialIndex, BoneIndices, matPivot, this, m_vecSurfaceVtx, m_vecSurfaceIdx);

			if (nullptr == pMesh)
				return E_FAIL;
		}
		m_Meshes.push_back(pMesh);

		
	}

	m_iNumMeshes = iNumMeshes;

	return S_OK;
}

HRESULT CModel::Ready_Materials(const wstring& strModelFilePath)
{
	/* 파일 셋업 */
	wstring folderName, filePath;
	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	{
		folderName = filesystem::path(strModelFilePath).stem();
		filePath = (filesystem::path(strModelFilePath) / wstring(folderName + TEXT(".mat")));
		Utils::Replace(filePath, TEXT("\\"), TEXT("/"));

		if (!filesystem::exists(filePath))
			return E_FAIL;

		file->Open(filePath, FileMode::Read);
	}

	/* 모든 매태리얼 순회 */
	size_t iNumMaterials = file->Read<size_t>();
	for (size_t i = 0; i < iNumMaterials; i++)
	{
		MESH_MATERIAL		MaterialDesc;
		ZeroMemory(&MaterialDesc, sizeof(MESH_MATERIAL));
		{
			wstring path, fileName;

			fileName = Utils::ToWString(file->Read<string>());
			if (!fileName.empty())
			{
				path = strModelFilePath + TEXT("/") + fileName;
				MaterialDesc.pTextures[aiTextureType_DIFFUSE] = CTexture::Create(m_pDevice, m_pContext, path);
			}

			fileName = Utils::ToWString(file->Read<string>());
			if (!fileName.empty())
			{
				path = strModelFilePath + TEXT("/") + fileName;
				MaterialDesc.pTextures[aiTextureType_NORMALS] = CTexture::Create(m_pDevice, m_pContext, path);
			}
		}
		m_Materials.push_back(MaterialDesc);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations(const wstring& strModelFilePath)
{
	/* 파일 셋업 */
	wstring folderName, filePath;
	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	{
		folderName = filesystem::path(strModelFilePath).stem();
		filePath = (filesystem::path(strModelFilePath) / wstring(folderName + TEXT(".anim")));
		Utils::Replace(filePath, TEXT("\\"), TEXT("/"));

		if (!filesystem::exists(filePath))
			return E_FAIL;

		file->Open(filePath, FileMode::Read);
	}

	/* 모든 애니메이션 순회 */
	size_t iNumAnims = file->Read<size_t>();
	for (size_t i = 0; i < iNumAnims; i++)
	{
		string strAnimName = file->Read<string>();
		_float fDuration = file->Read<_float>();
		_float fTickPerSecond = file->Read<_float>();

		/* 모든 채널 순회 */
		size_t iNumChannels = file->Read<size_t>();
		vector<CChannel*> Channels;
		Channels.reserve(iNumChannels);
		for (size_t j = 0; j < iNumChannels; j++)
		{
			string strName = file->Read<string>();

			/* 모든 키프레임 순회 */
			size_t iNumKeyframes = file->Read<size_t>();
			vector<KEYFRAME> Keyframes;
			Keyframes.reserve(iNumKeyframes);
			for (size_t k = 0; k < iNumKeyframes; k++)
			{
				KEYFRAME keyframe;

				keyframe.fTime = file->Read<_float>();
				keyframe.vScale = file->Read<Vec3>();
				keyframe.vRotation = file->Read<Vec4>();
				keyframe.vPosition = file->Read<Vec3>();

				Keyframes.push_back(keyframe);
			}

			CChannel* pChannel = CChannel::Create(strName, Keyframes);
			if (nullptr == pChannel)
				return E_FAIL;

			Channels.push_back(pChannel);
		}

		CAnimation* pAnimation = CAnimation::Create(fDuration, fTickPerSecond, Channels, this, strAnimName);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
		m_hashAnimIndices.emplace(Utils::ToWString(strAnimName), i);
	}

	return S_OK;
}

HRESULT CModel::CreateVertexTexture2DArray()
{
	_uint iAnimCount = (_int)m_Animations.size();
	if (0 == iAnimCount)
		return S_OK;

	vector<AnimTransform>	_animTransforms;
	_animTransforms.resize(iAnimCount);

	for (uint32 i = 0; i < iAnimCount; i++)
	{
		//_uint iMaxFrameCount = m_Animations[i]->GetMaxFrameCount();
		CreateAnimationTransform(i, _animTransforms);
	}

	// Creature Texture
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = m_Bones.size() * 4;
		desc.Height = m_iMaxFrameCount;
		desc.ArraySize = iAnimCount;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // 16바이트
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		const _uint dataSize = m_Bones.size() * sizeof(Matrix);
		const _uint pageSize = dataSize * m_iMaxFrameCount;
		void* mallocPtr = ::malloc(pageSize * iAnimCount);

		// 파편화된 데이터를 조립한다.
		for (_uint c = 0; c < iAnimCount; c++)
		{
			_uint startOffset = c * pageSize;

			BYTE* pageStartPtr = reinterpret_cast<BYTE*>(mallocPtr) + startOffset;
		
			for (_uint f = 0; f < m_iMaxFrameCount; f++)
			{
				void* ptr = pageStartPtr + f * dataSize;
				::memcpy(ptr, _animTransforms[c].transforms[f].data(), dataSize);
			}
		}

		// 리소스 만들기
		vector<D3D11_SUBRESOURCE_DATA> subResources(iAnimCount);

		for (_uint c = 0; c < iAnimCount; c++)
		{
			void* ptr = (BYTE*)mallocPtr + c * pageSize;
			subResources[c].pSysMem = ptr;
			subResources[c].SysMemPitch = dataSize;
			subResources[c].SysMemSlicePitch = pageSize;
		}

		if (FAILED(m_pDevice->CreateTexture2D(&desc, subResources.data(), &m_pTexture)))
			return E_FAIL;
		
		::free(mallocPtr);
	}

	// Create SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipLevels = 1;
		desc.Texture2DArray.ArraySize = iAnimCount;

		if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture, &desc, &m_pSRV)))
			return E_FAIL;

		Safe_AddRef(m_pSRV);	// 레퍼런스 카운트 1올려줌으로써 오브젝트 다 죽어도 애니메이션 정보 유지
	}

	return S_OK;
}

void CModel::CreateAnimationTransform(_uint index, vector<AnimTransform>& animTransforms)
{
	CAnimation* pAnimation = m_Animations[index];

	for (uint32 f = 0; f < pAnimation->GetMaxFrameCount(); f++)
	{
		pAnimation->Calculate_Animation(f);

		for (uint32 b = 0; b < m_Bones.size(); b++)
		{
			m_Bones[b]->Set_CombinedTransformation();

			if (0 == m_Bones.size())
			{
				XMStoreFloat4x4(&animTransforms[index].transforms[f][b], XMMatrixIdentity());
				return;
			}

			XMStoreFloat4x4(&animTransforms[index].transforms[f][b], m_Bones[b]->Get_OffSetMatrix() * m_Bones[b]->Get_CombinedTransformation() * m_matPivot);
		}
	}
}

CModel * CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strModelFilePath, const SOCKETDESC& desc, _fmatrix matPivot)
{
	CModel*	pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strModelFilePath, desc, matPivot)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CModel::Clone(CGameObject* pGameObject, void * pArg)
{
	CModel*	pInstance = new CModel(*this);
	pInstance->m_pGameObject = pGameObject;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	Super::Free();

	/* HierarachyNode */
	for (auto& pBone : m_Bones)
		Safe_Release(pBone);
	m_Bones.clear();

	/* Material */
	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_Release(Material.pTextures[i]);
	}
	m_Materials.clear();

	/* Meshes */
	for (auto& pMeshContainer : m_Meshes)
		Safe_Release(pMeshContainer);
	m_Meshes.clear();

	/* Animations */
	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();

	if (TYPE_ANIM == m_eModelType)
	{
		Safe_Release(m_pTexture);
		Safe_Release(m_pSRV);
	}

	Safe_Release(m_pShader);
}
