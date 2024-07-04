#include "QuadTree.h"
#include "QuadTreeNode.h"
#include "ObjectManager.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Transform.h"
#include "ColliderSphere.h"
#include "Layer.h"
#include "PipeLine.h"

#ifdef _DEBUG
#include "GraphicDevice.h"
#include "PipeLine.h"
#include "DebugDraw.h"
#endif // _DEBUG

IMPLEMENT_SINGLETON(CQuadTree)

CQuadTree::CQuadTree()
{
}

HRESULT CQuadTree::Build_QuadTree(_uint iNumLevels)
{
    m_pPipeLine = GET_INSTANCE(CPipeLine);
   
    CObjectManager* pObjectManager = GET_INSTANCE(CObjectManager);

    m_pQuadTreeRoot = BuildQuadTree(Vec3(0.f, -1.f, 0.f), 0.5f * m_vRootExtents, m_iDepthLimit);

    const map<LAYERTAG, CLayer*>& mapLayers = pObjectManager->GetCurrentLevelLayers();

    auto iter = mapLayers.find(LAYERTAG::WALL);

    if (mapLayers.end() != iter)
    {
        vector<CGameObject*>& vecObjects = iter->second->GetGameObjects();
        for (auto& _iter : vecObjects)
            AddObjectInNode(_iter->GetTransform(), m_pQuadTreeRoot);
    }
    else
    {
        //return E_FAIL;
    }

    iter = mapLayers.find(LAYERTAG::GROUND);
    if (mapLayers.end() != iter)
    {
        vector<CGameObject*>& vecObjects = iter->second->GetGameObjects();
        for (auto& _iter : vecObjects)
            AddObjectInNode(_iter->GetTransform(), m_pQuadTreeRoot);
    }
    else
    {
       // return E_FAIL;
    }
    /*
    iter = mapLayers.find(LAYERTAG::WALL);
    if (mapLayers.end() != iter)
    {
        vector<CGameObject*>& vecObjects = mapLayers[LAYERTAG::WALL]->GetGameObjects();
        for (auto& _iter : vecObjects)
            AddObjectInNode(_iter->GetTransform(), m_pQuadTreeRoot);
    }*/
    /*CGraphicDevice* pGrahicDevice = GET_INSTANCE(CGraphicDevice);

    m_pBatch = new PrimitiveBatch<VertexPositionColor>(pGrahicDevice->GetContext());

    m_pEffect = new BasicEffect(pGrahicDevice->GetDevice());
    m_pEffect->SetVertexColorEnabled(true);

    const void* pShaderByteCodes = nullptr;
    size_t		iLength = 0;
    m_pEffect->GetVertexShaderBytecode(&pShaderByteCodes, &iLength);

    if (FAILED(pGrahicDevice->GetDevice()->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCodes, iLength, &m_pInputLayout)))
    {
        Safe_Delete(m_pBatch);
        Safe_Delete(m_pEffect);
        Safe_Release(m_pInputLayout);
        RELEASE_INSTANCE(CGraphicDevice)
        return E_FAIL;
    }
    RELEASE_INSTANCE(CGraphicDevice)

    if (!m_pQuadTreeRoot)
    {
        RELEASE_INSTANCE(CObjectManager)
        return E_FAIL;
    }*/

    RELEASE_INSTANCE(CObjectManager)

#ifdef _DEBUG
        //InitDebugFrustum();
#endif // DEBUG


    return S_OK;
}

HRESULT CQuadTree::Clear_QuadTree(_uint iNumLevels)
{
    Safe_Release(m_pQuadTreeRoot);

    return S_OK;
}

void CQuadTree::Update_QuadTree()
{
    Update_Frustum();
    FrustumCull(m_pQuadTreeRoot);
}

void CQuadTree::Update_Frustum()
{
    static _bool bUpdateFrustum = true;
    if (KEY_PRESSING_EX(KEY::CTRL) && KEY_DOWN_EX(KEY::F7))
        bUpdateFrustum = !bUpdateFrustum;

    if (!bUpdateFrustum)
        return;

    BoundingFrustum::CreateFromMatrix(m_tBoundingFrustum, m_pPipeLine->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
    m_tBoundingFrustum.Transform(m_tBoundingFrustum, m_pPipeLine->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW));
}

void CQuadTree::Render_QuadTree(const _float& fTimeDelta)
{
    m_pQuadTreeRoot->Render_QuadTreeNode(fTimeDelta);
    FrustumCull();

#ifdef _DEBUG
    //m_pEffect->SetWorld(XMMatrixIdentity());

    //CGraphicDevice* pGrahicDevice = GET_INSTANCE(CGraphicDevice);
    //CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);

    //m_pEffect->SetView(pPipeLine->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    //m_pEffect->SetProjection(pPipeLine->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

    //RELEASE_INSTANCE(CPipeLine);

    //m_pEffect->Apply(pGrahicDevice->GetContext());

    //pGrahicDevice->GetContext()->IASetInputLayout(m_pInputLayout);

    //RELEASE_INSTANCE(CGraphicDevice);

    //m_pBatch->Begin();

    //DX::Draw(m_pBatch, m_tBoundingFrustum, Colors::FloralWhite);

    //m_pBatch->End();
#endif // DEBUG
}

CQuadTreeNode* CQuadTree::GetCurrentNodeByPos(Vec3 vPos, CQuadTreeNode* const pNode)
{
    // Recursive
    if (pNode->GetBoundingBox()->Contains(vPos))
    {
        if (pNode->GetChildren().empty())
            return pNode;
        else
        {
            for (_uint iIndex = 0; iIndex < CQuadTreeNode::m_iChild_Node_Count; ++iIndex)
            {
                CQuadTreeNode* pChild = GetCurrentNodeByPos(vPos, pNode->GetChildNode(iIndex));
                if (pChild)
                    return pChild;
            }

            return pNode;
        }
    }
    else
        return nullptr;
}

CQuadTreeNode* CQuadTree::BuildQuadTree(Vec3 vCenter, Vec3 vHalfExtents, _int iDepthLimit)
{
    if (iDepthLimit < 0)
        return nullptr;

    CQuadTreeNode*  pNode = new CQuadTreeNode;
    BoundingBox*    pBBox = pNode->GetBoundingBox();

    //vCenter.y += 10.f;  // °£Áö¿ë
    pBBox->Center = vCenter;
    pBBox->Extents = vHalfExtents * m_fLooseFactor;
    
    Vec3 vOffset;
    Vec3 vChildCenter;
    Vec3 vStep = Vec3(0.5f * vHalfExtents.x, vHalfExtents.y, 0.5f * vHalfExtents.z);

    for (_uint iTree = 0; iTree < CQuadTreeNode::m_iChild_Node_Count; ++iTree)
    {
        *((_float*)(&vOffset) + 0) = ((iTree & 1) ? vStep.x : -vStep.x);
        *((_float*)(&vOffset) + 2) = ((iTree & 2) ? vStep.z : -vStep.z);

        *((_float*)(&vChildCenter) + 0) = *((_float*)(&vOffset) + 0) + *((_float*)(&pBBox->Center) + 0);
        *((_float*)(&vChildCenter) + 1) = *((_float*)(&pBBox->Center) + 1);
        *((_float*)(&vChildCenter) + 2) = *((_float*)(&vOffset) + 2) + *((_float*)(&pBBox->Center) + 2);

        pNode->AddChildNode(BuildQuadTree(vChildCenter, vStep, iDepthLimit - 1));
    }

#ifdef _DEBUG
    //pNode->InitDebugSquare();
#endif

    return pNode;
}

void CQuadTree::FrustumCull(CQuadTreeNode* pNode)
{
    switch (m_tBoundingFrustum.Contains(*pNode->GetBoundingBox()))
    {
    case CONTAINS:
        pNode->CullNode(CONTAINS);
        return;
    case INTERSECTS:
        pNode->CullNode(INTERSECTS);
        break;
    case DISJOINT:
        pNode->CullNode(DISJOINT);
        return;
    }

    vector<CQuadTreeNode*>& vecChildren = pNode->GetChildren();

    if (!vecChildren.empty())
        for (_int i = 0; i < 4; ++i)
            FrustumCull(vecChildren[i]);
}

void CQuadTree::FrustumCull()
{
    CObjectManager* pObjectManager = GET_INSTANCE(CObjectManager);

    map<LAYERTAG, CLayer*>& mapLayers = pObjectManager->GetCurrentLevelLayers();

    for (_int i = 0; i < DYNAMIC_LAYER_COUNT; ++i)
    {
        const auto& pair = mapLayers.find((LAYERTAG)(i + DEFAULT_LAYER_COUNT + 1));
        if (mapLayers.end() == pair)
            continue;
        
        for (auto& iter : pair->second->GetGameObjects())
        {
            CRigidBody* pRigidBody = iter->GetRigidBody();
            if (!pRigidBody)
                continue;

            CSphereCollider*& pSphereCollider = pRigidBody->GetSphereCollider();
            if (!pSphereCollider)
                continue;

            if (m_tBoundingFrustum.Contains(pSphereCollider->GetBoundingSphere()))
                iter->AddRenderGroup();
        }
    }

    RELEASE_INSTANCE(CObjectManager);
}

#ifdef _DEBUG
HRESULT CQuadTree::InitDebugFrustum()
{
    CGraphicDevice* pGrahicDevice = GET_INSTANCE(CGraphicDevice);

    m_pBatch = new PrimitiveBatch<VertexPositionColor>(pGrahicDevice->GetContext());

    m_pEffect = new BasicEffect(pGrahicDevice->GetDevice());
    m_pEffect->SetVertexColorEnabled(true);

    const void* pShaderByteCodes = nullptr;
    size_t		iLength = 0;
    m_pEffect->GetVertexShaderBytecode(&pShaderByteCodes, &iLength);

    if (FAILED(pGrahicDevice->GetDevice()->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCodes, iLength, &m_pInputLayout)))
    {
        Safe_Delete(m_pBatch);
        Safe_Delete(m_pEffect);
        Safe_Release(m_pInputLayout);

        RELEASE_INSTANCE(CGraphicDevice);
        return E_FAIL;
    }
    RELEASE_INSTANCE(CGraphicDevice);

    return S_OK;
}
#endif // DEBUG

void CQuadTree::AddObjectInNode(CTransform* pTransform, CQuadTreeNode* const pNode)
{
    Vec3 vTransformCenter = pTransform->GetPosition();
    Vec3 vNodeCenter = pNode->GetBoundingBox()->Center;

    Vec3 vExtents = pNode->GetBoundingBox()->Extents;
    if (TEXT("Lava_East_B1") == pTransform->GetGameObject()->GetObjectTag())
        vExtents *= 1.7f;
    else
        vExtents *= m_fLooseFactor;
    
    if (vTransformCenter.x < vNodeCenter.x - vExtents.x || vTransformCenter.x > vNodeCenter.x + vExtents.x
        || vTransformCenter.z < vNodeCenter.z - vExtents.z || vTransformCenter.z > vNodeCenter.z + vExtents.z)
        return;

    pNode->AddObject(pTransform->GetGameObject());

    if (pNode->GetChildren().empty())
        return;

    for (_uint index = 0; index < CQuadTreeNode::m_iChild_Node_Count; ++index)
        AddObjectInNode(pTransform, pNode->GetChildNode(index));
}

void CQuadTree::Free()
{
#ifdef _DEBUG
    Safe_Delete(m_pBatch);
    Safe_Delete(m_pEffect);

    Safe_Release(m_pInputLayout);
#endif // _DEBUG

    Safe_Release(m_pQuadTreeRoot);
    //m_pQuadTreeRoot->Free();
    //Safe_Delete(m_pQuadTreeRoot);

    RELEASE_INSTANCE(CPipeLine);
}