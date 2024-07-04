#pragma once

#include "Base.h"
#include "Component.h"
#include "ShaderManager.h"
/* Ŭ���̾���Ʈ���� ������ �پ��� ���ӿ�����Ʈ���� �θ𰡵ȴ�. */

BEGIN(Engine)

class CGameInstance;
class CMonoBehaviour;
class CRenderer;
class CShader;
class CTexture;
class CVIBuffer;
class CRigidBody;
class CModel;
class CNavMeshAgent;

class ENGINE_DLL CGameObject abstract : public CBase
{
	using Super = CBase;
protected:
	/* ������ ������ �� */
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	CGameObject(const CGameObject& rhs); /* ���� ������. */
	virtual ~CGameObject() = default;

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(void* pArg);
	virtual void			Tick(const _float& fTimeDelta);
	virtual void			LateTick(const _float& fTimeDelta);
	virtual void			DebugRender();
	virtual HRESULT			Render();
	virtual HRESULT			RenderInstance();
	virtual HRESULT			RenderShadow(const Matrix& matLightView, const Matrix& matLightProj) { return S_OK; }
	virtual HRESULT			AddRenderGroup() { return S_OK; }

	CComponent*				GetFixedComponent(const ComponentType& type);
	CTransform*				GetTransform();
	CVIBuffer* const		GetBuffer();
	CRigidBody*				GetRigidBody();
	//CCamera*				GetCamera();
	//CMeshRenderer*		GetMeshRenderer();
	//CModelRenderer*		GetModelRenderer();
	//CModelAnimator*		GetModelAnimator();
	CModel*					GetModel();
	CRenderer*				GetRenderer();
	CShader*				GetShader();
	CTexture*				GetTexture();
	//CLight*				GetLight();
	//CTerrain*				GetTerrain();
	CNavMeshAgent*			GetNavMeshAgent();


	vector<CMonoBehaviour*>& GetScripts()								{ return m_vecScripts; }

	CTransform*				GetOrAddTransform(_uint iLevelIndex);
	LAYERTAG				GetLayerTag()	const						{ return m_eLayerTag; }
	const wstring&			GetObjectTag()	const						{ return m_strObjectTag; }
	_bool					IsDead()		const						{ return m_IsDead; }
	_bool					IsInstance()	const						{ return m_IsInstance; }
	void					InitRendered()								{ m_bRendered = false; }

	void					SetLayerTag(LAYERTAG eLayerTag)				{ m_eLayerTag = eLayerTag; }
	void					SetObjectTag(const wstring strObjectTag)	{ m_strObjectTag = strObjectTag; }
	void					SetDeadState(_bool bDead)					{ m_IsDead = bDead; }
	void					SetInstance(_bool bInstance)				{ m_IsInstance = bInstance; }

	HRESULT					AddComponent(_uint iLevelIndex, const ComponentType& type, const wstring& strPrototypeTag, void* pArg = nullptr);

	/*void					SetLayerIndex(uint8 layer) { m_i8LayerIndex = layer; }
	uint8					GetLayerIndex() { return m_i8LayerIndex; }*/

	virtual	void			OnCollisionEnter(CGameObject* pOther)		{};
	virtual	void			OnCollisionStay(CGameObject* pOther)		{};
	virtual	void			OnCollisionExit(CGameObject* pOther)		{};

	virtual	void			OnTriggerEnter(CGameObject* pOther)			{};
	virtual	void			OnTriggerStay(CGameObject* pOther)			{};
	virtual	void			OnTriggerExit(CGameObject* pOther)			{};

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;
	CGameInstance*			m_pGameInstance = nullptr;
	_bool					m_bRendered = false;

protected:
	array<CComponent*, FIXED_COMPONENT_COUNT> m_arrComponents = { nullptr };
	vector<CMonoBehaviour*> m_vecScripts;

private:
	_bool					m_IsDead;
	_bool					m_IsInstance;
	LAYERTAG				m_eLayerTag;	// �ʿ������ �����غ���.
	wstring					m_strObjectTag;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

	HRESULT friend CShaderManager::SwapShader(CGameObject* pGameObject, const wstring& strShaderFileName);
};

END