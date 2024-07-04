#include "..\Public\TransformBurger.h"

CTransformBurger::CTransformBurger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext, ComponentType::Transform)
{
}

CTransformBurger::CTransformBurger(const CTransformBurger& rhs)
	: CComponent(rhs)
{
}

void CTransformBurger::Set_State(STATE eState, _fvector vState)
{
	_matrix		StateMatrix;

	StateMatrix = XMLoadFloat4x4(&m_WorldMatrix);

	StateMatrix.r[eState] = vState;

	XMStoreFloat4x4(&m_WorldMatrix, StateMatrix);
}

HRESULT CTransformBurger::Initialize_Prototype()
{
	/* XMFloat4x4 -> XMMatrix*/
	/*XMLoadFloat4x4(&m_WorldMatrix);*/

	/* XMMatrix -> XMFloat4x4*/
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	/*_float3	vTmp;
	_vector		vTmp1;

	vTmp1 = XMLoadFloat3(&vTmp);

	XMStoreFloat3(&vTmp, vTmp1);*/

	return S_OK;
}

HRESULT CTransformBurger::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		TRANSFORM_DESC	TransformDesc;

		memmove(&TransformDesc, pArg, sizeof TransformDesc);

		m_fSpeedPerSec = TransformDesc.fSpeedPerSec;
		m_fRotationRadianPerSec = TransformDesc.fRotationRadianPerSec;
	}

	return S_OK;
}

void CTransformBurger::Go_Straight(_float fTimeDelta)
{
	/* vLook : 현재 내가 바라보는 방향 + z스케일정보를 가지낟. */
	/* vLook의 길이(크기)를 내가 원하는 초당 움직여하는 속도로 변경하자. */
	_vector		vLook = Get_State(STATE_LOOK);

	_vector		vPosition = Get_State(STATE_POSITION);

	/* vLook을 정규화(길이1)하고 원하는 길이(초당속도)를 곱한다. */
	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransformBurger::Go_Backward(_float fTimeDelta)
{
	_vector		vLook = Get_State(STATE_LOOK);

	_vector		vPosition = Get_State(STATE_POSITION);

	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransformBurger::Go_Left(_float fTimeDelta)
{
	_vector		vRight = Get_State(STATE_RIGHT);

	_vector		vPosition = Get_State(STATE_POSITION);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransformBurger::Go_Right(_float fTimeDelta)
{
	_vector		vRight = Get_State(STATE_RIGHT);

	_vector		vPosition = Get_State(STATE_POSITION);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransformBurger::Fix_Rotation(_fvector vAxis, _float fRadian)
{
}

void CTransformBurger::Turn(_fvector vAxis, _float fTimeDelta)
{
}

CTransformBurger* CTransformBurger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransformBurger* pInstance = new CTransformBurger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTransformBurger::Clone(CGameObject* pGameObject, void* pArg)
{
	CTransformBurger* pInstance = new CTransformBurger(*this);
	pInstance->m_pGameObject = pGameObject;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTransformBurger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransformBurger::Free()
{
	__super::Free();
}
