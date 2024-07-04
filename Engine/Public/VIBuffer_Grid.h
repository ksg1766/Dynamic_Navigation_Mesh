#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Grid final : public CVIBuffer
{
private:
	CVIBuffer_Grid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Grid(const CVIBuffer_Grid& rhs);
	virtual ~CVIBuffer_Grid() = default;

public:
	virtual HRESULT Initialize_Prototype()	override;
	virtual HRESULT Initialize(void* pArg)	override;

private:
	_uint	m_iGridWidth;
	_uint	m_iGridHeight;

public:
	static CVIBuffer_Grid* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;
	virtual void Free() override;
};

END