#pragma once
#include "Base.h"

BEGIN(Engine)

//class CFrustum;
class CQuadTreeNode;
class CTransform;
class CSphereCollider;

class CQuadTree final : public CBase
{
    DECLARE_SINGLETON(CQuadTree)

private:
    CQuadTree();
    virtual ~CQuadTree() = default;

public:
    HRESULT			Build_QuadTree(_uint iNumLevels);
    HRESULT			Clear_QuadTree(_uint iNumLevels);
    void            Update_QuadTree();
    void            Render_QuadTree(const _float& fTimeDelta);

    CQuadTreeNode*  GetCQuadTreeRoot() { return m_pQuadTreeRoot; }
    CQuadTreeNode*  GetCurrentNodeByPos(Vec3 vPos, CQuadTreeNode* const pNode);

private:
    CQuadTreeNode*  BuildQuadTree(Vec3 vCenter, Vec3 vHalfWidth, _int iDepthLimit = 5);
    void            AddObjectInNode(CTransform* pTransform, CQuadTreeNode* const pNode);

    void            Update_Frustum();
    void	        FrustumCull(CQuadTreeNode* pNode);
    void	        FrustumCull();

private:

    class CPipeLine* m_pPipeLine;

    const _int      m_iDepthLimit = 4;
    const _float    m_fLooseFactor = 1.5f;

    Vec3            m_vRootExtents = Vec3(1280.f, 128.f, 1280.f);
    CQuadTreeNode*  m_pQuadTreeRoot = nullptr;

    BoundingFrustum m_tBoundingFrustum;
#ifdef _DEBUG
    HRESULT         InitDebugFrustum();

    PrimitiveBatch<VertexPositionColor>* m_pBatch = nullptr;
    BasicEffect*    m_pEffect = nullptr;
    ID3D11InputLayout* m_pInputLayout = nullptr;
#endif

public:
    virtual void Free() override;
};

END