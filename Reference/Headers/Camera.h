#pragma once
#include "Component.h"

BEGIN(Engine)

enum class PROJECTION_MODE
{
    PERSPECTIVE,
    ORTHOGRAPHIC
};

class ENGINE_DLL CCamera :
    public CComponent
{
    using Super = CComponent;

public:
    typedef struct tagCameraDesc
    {
        _float4			vEye, vAt;
        _float			fFovy, fAspect, fNear, fFar;
    } CAMERA_DESC;

private:
    CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CCamera(const CCamera& rhs);
    virtual ~CCamera() = default;

public:
    virtual HRESULT Initialize_Prototype()              override;
    virtual HRESULT Initialize(void* pArg = nullptr)    override;
    virtual void    Tick(_float fTimeDelta)             override;
    virtual void    LateTick(_float fTimeDelta)         override;

private:
    class CTransform*   m_pTransform = nullptr;
    class CPipeLine*    m_pPipeLine = { nullptr };

    PROJECTION_MODE     m_eMode = PROJECTION_MODE::PERSPECTIVE;

    Vec4			    m_vEye, m_vAt;
    _float			    m_fFovy, m_fAspect, m_fNear, m_fFar;

public:
    static CCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;
    virtual void Free() override;
};

END