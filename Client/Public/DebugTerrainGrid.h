#pragma once

#include "Client_Defines.h"
#include "MonoBehaviour.h"
#include "RigidDynamic.h"

BEGIN(Engine)

class CShader;
class CVIBuffer_Grid;

END

BEGIN(Client)

class CDebugTerrainGrid : public CMonoBehaviour
{
	using Super = CMonoBehaviour;

private:
	CDebugTerrainGrid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDebugTerrainGrid(const CDebugTerrainGrid& rhs);
	virtual ~CDebugTerrainGrid() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Tick(_float fTimeDelta)		override;
	virtual void	LateTick(_float fTimeDelta)	override;
	virtual void	DebugRender()						override;

private:
	HRESULT			Bind_ShaderResources();

private:
	CShader*			m_pShader;
	CVIBuffer_Grid*	m_pBufferGrid;

public:
	static	CDebugTerrainGrid* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;
	virtual void Free() override;
};

END