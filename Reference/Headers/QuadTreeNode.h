#pragma once
#include "Base.h"

BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CQuadTreeNode :
    public CBase
{
public:
    CQuadTreeNode();
    CQuadTreeNode(const CGameObject& rhs) = delete;
    virtual ~CQuadTreeNode() = default;

public:
#ifdef _DEBUG
    HRESULT                 InitDebugSquare();
#endif

    void                    AddChildNode(CQuadTreeNode* pChild);
    void                    AddObject(CGameObject* pObject) { m_vecObjects.push_back(pObject); }

    CQuadTreeNode* const    GetParent()                     { return m_pParent; }
    vector<CQuadTreeNode*>& GetChildren()                   { return m_vecChildren; }
    CQuadTreeNode* const    GetChildNode(_uint _iChildNum)  { return m_vecChildren[_iChildNum]; }

    vector<CGameObject*>&   GetObjectList()                 { return m_vecObjects; }
    BoundingBox* const      GetBoundingBox()                { return &m_tBoundBox; }

    void                    SetParent(CQuadTreeNode* const _pParent) { m_pParent = _pParent; }

    void                    Render_QuadTreeNode(const _float& fTimeDelta);
#ifdef _DEBUG
    void                    DebugRender();
#endif

    void                    CullNode(_int _iCulled)         { m_iCulled = _iCulled; }
    _int                    IsCulled()                      { return m_iCulled; }

public:
    const static _uint      m_iChild_Node_Count = 4;

private:
    BoundingBox             m_tBoundBox;

    vector<CQuadTreeNode*>  m_vecChildren;
    vector<CGameObject*>    m_vecObjects;

    CQuadTreeNode*          m_pParent;

    _int                    m_iCulled;

#ifdef _DEBUG
private:
    PrimitiveBatch<VertexPositionColor>*    m_pBatch = nullptr;
    BasicEffect*                            m_pEffect = nullptr;
    ID3D11InputLayout*                      m_pInputLayout = nullptr;
#endif

public:
    virtual void Free() override;
};

END