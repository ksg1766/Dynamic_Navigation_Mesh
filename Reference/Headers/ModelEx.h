#pragma once

/* .FBX������ �ε��Ͽ� �����͵��� �� ������ �°� �����Ѵ�. */
#include "Component.h"
#include "AsTypes.h"

BEGIN(Engine)

class ENGINE_DLL CModelEx final : public CComponent
{
	using Super = CComponent;

public:
	typedef struct ChangeAnimation
	{
		_uint	m_iNextAnim;
		_float	m_fChangeTime = 0.0f, m_fSumTime = 0.0f, m_fChangeRatio = 0.0f;
		_uint	m_iChangeFrame = 0, m_iNextAnimFrame = 0;
	}CHANGEANIM;

	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

private:
	CModelEx(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModelEx(const CModelEx& rhs);
	virtual ~CModelEx() = default;

public:
	virtual HRESULT Initialize_Prototype()	override;
	virtual HRESULT Initialize(void* pArg)	override;
	HRESULT			InitializeWithFile(const wstring& strFilePath, const wstring& strFileName, _fmatrix& matPivot);
	HRESULT			Render(_uint iMeshIndex, _uint iPassIndex = 0);
	
	_uint	Get_NumMeshes() const				{ return m_iNumMeshes; }
	_uint	Get_MaterialIndex(_uint iMeshIndex);

	void	Set_CurrAnim(_int iCurrAnim);
	_uint	Get_CurrAnim()						{ return m_iCurrAnim; }

	_uint	Get_MaxAnimIndex();
	Matrix	Get_PivotMatrix()					{ return m_matPivot; }

	_int	Find_AnimIndex(const wstring& szAnimName);
	_bool	Is_AnimationEnd(_uint iAnimation);
	_float	Get_Anim_MaxFrameRatio(_uint iAnimation);
	_uint	Get_Anim_Frame(_uint iAnimation);

	_int	Initailize_FindAnimation(const wstring& szAnimName, _float fSpeed);

public:
	HRESULT SetUp_OnShader(class CShader* pShader, _uint iMaterialIndex, aiTextureType eTextureType, const _char* strConstantName);
	HRESULT SetUpAnimation_OnShader(class CShader* pShader);
	/* �ִϸ��̼��� ����Ѵ�. */
	/* 1. �ش� �ִϸ��̼ǿ��� ����ϴ� ��� ������  Transformation ����� �����Ѵ�. */
	/* 2. Transformation�� �ֻ��� �θ�κ��� �ڽ����� ��� �������Ѱ���.(CombinedTransformation) */
	/* 3. �ִϸ��̼ǿ� ���� ������ ������ CombinedTransfromation�� �����Ѵ�. */

	HRESULT	Reserve_NextAnimation(_int iAnimIndex, _float fChangeTime, _uint iStartFrame, _uint iChangeFrame);
	HRESULT Set_NextAnimation();
	HRESULT Play_Animation(_float fTimeDelta);
	HRESULT Set_Animation_Transforms();
	HRESULT Set_AnimationBlend_Transforms();

	HRESULT Load_AssetFile_FromBinary(const wstring& pFilePath, const wstring& pFileName);

private:
	HRESULT Load_ModelData_FromFile(Matrix PivotMatrix);
	HRESULT Load_MaterialData_FromFile();
	HRESULT Load_AnimationData_FromFile(Matrix PivotMatrix);

	void	Change_NextAnimation();

private:
	Matrix						m_matPivot = Matrix::Identity;
	TYPE						m_eModelType = TYPE_END;

private:
	_uint							m_iNumMeshes = 0;
	vector<class CMeshEx*>			m_Meshes;
	using MESHES = vector<class CMeshEx*>;

private:
	_uint							m_iNumMaterials = 0;
	vector<MATERIALDESC>			m_Materials;

private:
	vector<class CHierarchyNode*>	m_HierarchyNodes;

private:
	_int							m_iCurrAnim = 0;
	_bool							m_bNext = false, m_bReserved = false;

	uint64							m_iNumAnimations = 0;
	vector<class CAnimation*>		m_Animations;

	//Animation
	vector<Matrix>					m_matCurrTransforms;
	vector<ModelKeyframeData>		m_CurrKeyFrameDatas;
	vector<ModelKeyframeData>		m_PrevKeyFrameDatas;

	CHANGEANIM						m_tCurrChange, m_tReserveChange;

	//Model Load
	wstring							m_strFilePath;
	wstring							m_strFileName;

	vector<shared_ptr<ModelBone>>	m_ModelBones;
	shared_ptr<ModelBone>			m_RootBone;

public:
	static	CModelEx* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;
	virtual void Free() override;
};

END