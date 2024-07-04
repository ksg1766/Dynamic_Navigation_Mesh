#include "QuadTreeNode.h"
#include "GameObject.h"
#ifdef _DEBUG
#include "GraphicDevice.h"
#include "PipeLine.h"
#include "DebugDraw.h"
#endif // _DEBUG


CQuadTreeNode::CQuadTreeNode()
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif
}

#ifdef _DEBUG
HRESULT CQuadTreeNode::InitDebugSquare()
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
#endif

void CQuadTreeNode::AddChildNode(CQuadTreeNode* pChild)
{
    if (pChild)
    {
        m_vecChildren.push_back(pChild);
        pChild->SetParent(this);
    }
}

void CQuadTreeNode::Render_QuadTreeNode(const _float& fTimeDelta)
{
	if (!m_vecChildren.empty())
	{
		for (auto& pNode : m_vecChildren)
		{
			if (DISJOINT == pNode->IsCulled())
				continue;
			else if (INTERSECTS == pNode->IsCulled())
				pNode->Render_QuadTreeNode(fTimeDelta);
			else // CONTAINS
			{
				for (auto& _iter : pNode->GetObjectList())
				{
					_iter->Tick(fTimeDelta);
					_iter->LateTick(fTimeDelta);
					//pNode->CullNode(DISJOINT);
				}
#ifdef _DEBUG
				//DebugRender();
#endif // _DEBUG
			}
		}
	}
	else
	{
		for (auto& _iter : m_vecObjects)
		{
			_iter->Tick(fTimeDelta);
			_iter->LateTick(fTimeDelta);
			//m_iCulled = DISJOINT;
		}
	}
}
#ifdef _DEBUG
void CQuadTreeNode::DebugRender()
{
	m_pEffect->SetWorld(XMMatrixIdentity());

	CGraphicDevice* pGrahicDevice = GET_INSTANCE(CGraphicDevice);
	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);

	m_pEffect->SetView(pPipeLine->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(pPipeLine->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	RELEASE_INSTANCE(CPipeLine);

	m_pEffect->Apply(pGrahicDevice->GetContext());

	pGrahicDevice->GetContext()->IASetInputLayout(m_pInputLayout);

	RELEASE_INSTANCE(CGraphicDevice);

	m_pBatch->Begin();

	DX::Draw(m_pBatch, m_tBoundBox, Colors::DarkCyan);

	m_pBatch->End();
}
#endif // _DEBUG
void CQuadTreeNode::Free()
{
#ifdef _DEBUG
	//Safe_Delete(m_pBatch);
	//Safe_Delete(m_pEffect);

	//Safe_Release(m_pInputLayout);
#endif // _DEBUG

	if (!m_vecChildren.empty())
	{
		for (auto& iter : m_vecChildren)
		{
			while (0 != Safe_Release(iter))
			{
			}
			/*iter->Free();
			Safe_Delete(iter);*/
		}
		m_vecObjects.clear();
		m_vecChildren.clear();
	}
}