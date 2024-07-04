#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Dot final : public CVIBuffer
{
	using Super = CVIBuffer;
private:
	CVIBuffer_Dot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Dot(const CVIBuffer_Dot& rhs);
	virtual ~CVIBuffer_Dot() = default;

public:
	virtual HRESULT Initialize_Prototype()	override;
	virtual HRESULT Initialize(void* pArg)	override;

public:
	static CVIBuffer_Dot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;
	virtual void Free() override;
};

END