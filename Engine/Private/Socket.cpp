#include "..\Public\Socket.h"
#include "GameObject.h"
#include "Model.h"
#include "Shader.h"
#include "Transform.h"

CSocket::CSocket()
{
}

HRESULT CSocket::Initialize(const _int iBoneIndex)
{
	m_iBoneIndex = iBoneIndex;

	return S_OK;
}

HRESULT CSocket::Equip(CModel* pParts)
{
	if(m_pPartsModel)
		return E_FAIL;

	m_pPartsModel = const_cast<CModel*>(pParts);
	m_pPartsModel->SetSRV(m_pSocketSRV);
	
	return S_OK;
}

HRESULT CSocket::UnEquip()
{
	if (!m_pPartsModel)
		return E_FAIL;

	m_pPartsModel = nullptr;

	return S_OK;
}

HRESULT CSocket::LoadSRV(ID3D11ShaderResourceView*& pSocketSRV)
{
	if (m_pSocketSRV)
		return E_FAIL;

	m_pSocketSRV = pSocketSRV;

	return S_OK;
}

HRESULT CSocket::LoadTweenDescFromBone(TWEENDESC& tweenDec)
{
	m_pPartsModel->SetTweenDesc(tweenDec);

	return S_OK;
}

HRESULT CSocket::LoadTrasformFromBone(const Matrix& matSocketWorld)
{
	_matrix		WorldMatrix = m_pPartsModel->GetTransform()->WorldMatrix() * matSocketWorld;

	WorldMatrix.r[0] = XMVector3Normalize(WorldMatrix.r[0]);
	WorldMatrix.r[1] = XMVector3Normalize(WorldMatrix.r[1]);
	WorldMatrix.r[2] = XMVector3Normalize(WorldMatrix.r[2]);

	m_pPartsModel->GetTransform()->Set_WorldMatrix(WorldMatrix);

	return S_OK;
}

HRESULT CSocket::BindBoneIndex()
{
	return S_OK;
//	return m_pPartsModel->SetSocketBoneIndex(m_iBoneIndex);
}

CSocket* CSocket::Create(const _int iBoneIndex)
{
	CSocket* pInstance = new CSocket();

	if (FAILED(pInstance->Initialize(iBoneIndex)))
	{
		MSG_BOX("Failed To Created : CSocket");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSocket::Free()
{
	Safe_Release(m_pSocketSRV);
}
