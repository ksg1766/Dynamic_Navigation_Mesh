#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance : public CVIBuffer
{
	using Super = CVIBuffer;
protected:
	CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instance(const CVIBuffer_Instance& rhs);
	virtual ~CVIBuffer_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype()	override;
	virtual HRESULT Initialize(void* pArg)	override;
	HRESULT Render(class CMesh* pMesh);

private:
	HRESULT Create_Buffer(_uint iMaxCount = MAX_INSTANCE);

public:
	void ClearData();
	void AddData(InstancingData& data);
	vector<InstancingData>& GetInstanceData() {	return m_vecData; }

protected:
	_uint						m_iMaxCount = 0;
	vector<InstancingData>		m_vecData;

public:
	static CVIBuffer_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) { return nullptr; };
	virtual void Free() override;
};

END