#pragma once

#include "Base.h"

/* 뷰, 투영행렬을 보관한다. */
/* Tick함수안에서 뷰, 투영의 역행렬을 구해놓느다 .*/

BEGIN(Engine)

class CPipeLine final : public CBase
{
	DECLARE_SINGLETON(CPipeLine)
public:
	enum TRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };
private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	HRESULT Initialize();
	void	Tick();

public:
	_float4x4	Get_Transform_float4x4(TRANSFORMSTATE eState) const;
	_matrix		Get_Transform_Matrix(TRANSFORMSTATE eState) const;
	_float4x4	Get_Transform_float4x4_Inverse(TRANSFORMSTATE eState) const;
	_matrix		Get_Transform_Matrix_Inverse(TRANSFORMSTATE eState) const;
	_float4		Get_CamPosition_Float4() const;
	_vector		Get_CamPosition_Vector() const;
	void		Set_Transform(TRANSFORMSTATE eState, _fmatrix TransformMatrix);

public:
	HRESULT		Bind_TransformToShader(class CShader* pShader, const _char* pConstantName, CPipeLine::TRANSFORMSTATE eState);	

private:
	_float4x4			m_TransformMatrices[D3DTS_END];
	_float4x4			m_TransformMatrices_Inverse[D3DTS_END];
	_float4				m_vCamPosition = { 0.0f, 0.f, 0.f, 1.f };

public:
	virtual void Free() override;
};

END