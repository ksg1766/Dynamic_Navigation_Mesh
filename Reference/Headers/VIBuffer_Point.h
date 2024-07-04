#pragma once

#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point final : public CVIBuffer
{
	using Super = CVIBuffer;
private:
	CVIBuffer_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point(const CVIBuffer_Point& rhs);
	virtual ~CVIBuffer_Point() = default;

public:
	virtual HRESULT Initialize_Prototype()	override;
	virtual HRESULT Initialize(void* pArg)	override;
	friend HRESULT CVIBuffer_Instance::Render(CVIBuffer_Point* pPoint);

	_int GetInstanceID() { return m_iInstanceID; }

public:
	static CVIBuffer_Point* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;
	virtual void Free() override;

private:
	static _int					m_iNextInstanceID;
	_int						m_iInstanceID;

};

END