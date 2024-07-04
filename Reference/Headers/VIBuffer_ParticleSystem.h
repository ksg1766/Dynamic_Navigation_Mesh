#pragma once

#include "VIBuffer_Instance.h"

BEGIN(Engine)

class CShader;
class ENGINE_DLL CVIBuffer_ParticleSystem final : public CVIBuffer
{
public:
	typedef struct tagParticleSystem
	{

	}PARTICLESYSTEM_DESC;

	using Super = CVIBuffer;
private:
	CVIBuffer_ParticleSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_ParticleSystem(const CVIBuffer_ParticleSystem& rhs);
	virtual ~CVIBuffer_ParticleSystem() = default;

public:
	virtual HRESULT Initialize_Prototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual HRESULT Render(CShader* pStreamOutShader, CShader* pDrawingShader);

	void Reset();

public:
	void SetEyePos(const Vec3& eyePosW);
	void SetEmitPos(const Vec3& emitPosW);
	void SetEmitDir(const Vec3& emitDirW);

public:
	static CVIBuffer_ParticleSystem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;
	virtual void Free() override;

private:
	_uint m_fMaxParticles = 10000;
	_bool m_IsFirstRun = true;

	Vec3 m_vEyePosW;
	Vec3 m_vEmitPosW;
	Vec3 m_vEmitDirW;

	ID3D11Buffer* m_pDrawVB;
	ID3D11Buffer* m_pStreamOutVB;
};

END