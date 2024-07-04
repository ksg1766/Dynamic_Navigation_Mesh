#pragma once

/* .FBX파일을 로드하여 데이터들을 내 구조에 맞게 정리한다. */
#include "VIBuffer.h"
#include "Hasher.h"

BEGIN(Engine)

// Bone
#define MAX_BONES 172
#define MAX_KEYFRAMES 441

struct AnimTransform
{
	// [ ][ ][ ][ ][ ][ ][ ] ... 150개
	using TransformArrayType = array<Matrix, MAX_BONES>;
	// [ ][ ][ ][ ][ ][ ][ ] ... 300 개
	array<TransformArrayType, MAX_KEYFRAMES> transforms;
};

typedef struct tagSocketDesc
{
	vector<string> vecSocketBoneNames;
}SOCKETDESC;

class CBone;
class CSocket;
class CMesh;
class CAnimation;
class CVIBuffer_Instance;
class ENGINE_DLL CModel final : public CComponent
{
	using Super = CComponent;
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	HRESULT			Initialize_Prototype(const wstring& strModelFilePath, const SOCKETDESC& desc, _fmatrix& matPivot);
	virtual HRESULT Initialize(void* pArg)			override;
	virtual void	Tick(const _float& fTimeDelta)	override;
	void			DebugRender()					override;
	HRESULT			Render();
	HRESULT			RenderShadowInstancing(CVIBuffer_Instance*& pInstanceBuffer);
	HRESULT			RenderInstancing(CVIBuffer_Instance*& pInstanceBuffer);

public:
	HRESULT			UpdateTweenData(_float fTimeDelta);
	void			PauseAnimation(_bool bPauseAnimation) { m_IsAnimPaused = bPauseAnimation;	}

	void			PushTweenData(const InstancedTweenDesc& desc);
	HRESULT			BindMaterialTexture(class CShader* pShader, const _char* pConstantName, _uint iMaterialIndex, aiTextureType eType);

private:
	TYPE						m_eModelType = TYPE_END;
	_uint						m_iNumMeshes = { 0 };

	_uint						m_iNumMaterials = { 0 };

	vector<CBone*>				m_Bones;
	vector<CMesh*>				m_Meshes;
	vector<CAnimation*>			m_Animations;

	using ANIMINDEX = unordered_map<wstring, _int, djb2Hasher>;
	ANIMINDEX					m_hashAnimIndices;

	vector<MESH_MATERIAL>		m_Materials;

	_float4x4					m_matPivot;

	_int						m_iCurrentAnimIndex = 0;
	_int						m_iNextAnimIndex = -1;
	_uint						m_iMaxFrameCount = 0;

private:
	// Animation
	_bool						m_IsAnimPaused = false;
	TWEENDESC					m_TweenDesc;
	ID3D11Texture2D*			m_pTexture = nullptr;
	ID3D11ShaderResourceView*	m_pSRV = nullptr;

	// Socket
	vector<CModel*>				m_PartsModel;
	vector<_int>				m_vecSocketBones;
	_bool						m_HasParent = false;
	_int						m_iSocketBoneIndex = 0;
	
	class CShader*				m_pShader;

	// Instance
	static map<_int, ID3D11ShaderResourceView*>		m_mapVTFExist;	// TODO:다른곳에 텍스쳐를 가지고 있는지 기록할 수 있도록 하자.
	static _int					m_iNextInstanceID;
	_int						m_iInstanceID;

	// Tool : NavMeshView
	vector<Vec3>				m_vecSurfaceVtx;
	vector<FACEINDICES32>		m_vecSurfaceIdx;

public:
	_uint			GetNumMeshes() const					{ return (_uint)m_Meshes.size(); }
	vector<CMesh*>& GetMeshes()								{ return m_Meshes; }
	_uint			GetMaterialIndex(_uint iMeshIndex);
	CBone*			GetBone(const _char* pNodeName);
	CBone*			GetBone(_int iIndex);
	_int			GetAnimationIndexByName(const wstring& strAnimName);
	_float			GetAnimationTimeByIndex(_int iIndex);
	const _int		GetCurAnimationIndex() const			{ return m_iCurrentAnimIndex; }
	const _int		GetNextAnimationIndex() const			{ return m_iNextAnimIndex; }
	
	// Instancing
	_bool			IsAnimModel()							{ return m_eModelType; }
	TweenDesc&		GetTweenDesc()							{ return m_TweenDesc; }
	_int			GetInstanceID() const					{ return m_iInstanceID; }
	const _float4x4&GetPivotMatrix() const					{ return m_matPivot; }
	const _int&		GetSocketBoneIndex() const				{ return m_iSocketBoneIndex; }

	// Tool : NavMeshView
	vector<Vec3>&	GetSurfaceVtx()							{ return m_vecSurfaceVtx; }
	vector<FACEINDICES32>&	GetSurfaceIdx()					{ return m_vecSurfaceIdx; }

public:
	ID3D11ShaderResourceView*& GetSRV()						{ return m_pSRV; }
	void			SetAnimation(_int iAnimIndex)			{ m_iCurrentAnimIndex = iAnimIndex; }
	void			SetNextAnimationIndex(_int iAnimIndex);
	void			SetTweenDesc(TweenDesc& TweenDesc)		{ m_TweenDesc = TweenDesc; }
	void			SetSRV(ID3D11ShaderResourceView*& pSRV)	{ m_pSRV = pSRV; }
	void			SetShader(CShader* pShader)				{ m_pShader = pShader; }
	HRESULT			EquipParts(_int iSocketIndex, CModel* pModel);

	static HRESULT	ReleaseAllVTF();

private:
	HRESULT			Ready_Bones(const wstring& strModelFilePath, const SOCKETDESC& desc);
	HRESULT			Ready_Meshes(const wstring& strModelFilePath, Matrix matPivot);
	HRESULT			Ready_Materials(const wstring& strModelFilePath);
	HRESULT			Ready_Animations(const wstring& strModelFilePath);

private:
	HRESULT			CreateVertexTexture2DArray();
	void			CreateAnimationTransform(_uint index, vector<AnimTransform>& animTransforms);

public:
	static	CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strModelFilePath, const SOCKETDESC& desc = SOCKETDESC(), _fmatrix matPivot = XMMatrixRotationY(XMConvertToRadians(90.0f))/*XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.0f))*/);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;
	virtual void Free() override;

	friend class CAnimationView;
	//friend class CSocket;
};

END