#include "..\Public\PipeLine.h"
#include "Shader.h"

IMPLEMENT_SINGLETON(CPipeLine)

CPipeLine::CPipeLine()
{
}

void CPipeLine::Set_Transform(TRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	if (eState >= D3DTS_END)
		return;

	XMStoreFloat4x4(&m_TransformMatrices[eState], TransformMatrix);
}

_float4x4 CPipeLine::Get_Transform_float4x4(TRANSFORMSTATE eState) const
{
	if (eState >= D3DTS_END)
		return _float4x4();

	return m_TransformMatrices[eState];	
}

_matrix CPipeLine::Get_Transform_Matrix(TRANSFORMSTATE eState) const
{
	if (eState >= D3DTS_END)
		return XMMatrixIdentity();

	return XMLoadFloat4x4(&m_TransformMatrices[eState]);
}

_float4x4 CPipeLine::Get_Transform_float4x4_Inverse(TRANSFORMSTATE eState) const
{
	if (eState >= D3DTS_END)
		return _float4x4();

	return m_TransformMatrices_Inverse[eState];
}

_matrix CPipeLine::Get_Transform_Matrix_Inverse(TRANSFORMSTATE eState) const
{
	if (eState >= D3DTS_END)
		return XMMatrixIdentity();

	return XMLoadFloat4x4(&m_TransformMatrices_Inverse[eState]);
}

_float4 CPipeLine::Get_CamPosition_Float4() const
{
	return m_vCamPosition;
}

_vector CPipeLine::Get_CamPosition_Vector() const
{
	return XMLoadFloat4(&m_vCamPosition);
}

HRESULT CPipeLine::Bind_TransformToShader(CShader * pShader, const _char * pConstantName, CPipeLine::TRANSFORMSTATE eState)
{	
	return pShader->Bind_Matrix(pConstantName, &m_TransformMatrices[eState]);	
}

HRESULT CPipeLine::Initialize()
{
	for (size_t i = 0; i < D3DTS_END; i++)
	{
		XMStoreFloat4x4(&m_TransformMatrices[i], XMMatrixIdentity());
		XMStoreFloat4x4(&m_TransformMatrices_Inverse[i], XMMatrixIdentity());
	}

	return S_OK;
}

void CPipeLine::Tick()
{
	for (size_t i = 0; i < D3DTS_END; i++)
	{
		XMStoreFloat4x4(&m_TransformMatrices_Inverse[i], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrices[i])));
	}

	memcpy(&m_vCamPosition, &m_TransformMatrices_Inverse[D3DTS_VIEW].m[3][0], sizeof(_float4));	
}

void CPipeLine::Free()
{
	__super::Free();

}
