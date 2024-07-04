#include "stdafx.h"
#include "View.h"
#include "ViewMediator.h"
#include "GameInstance.h"

CView::CView(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	m_pDevice->AddRef();
	m_pContext->AddRef();
}

HRESULT CView::Initialize(void* pArg)
{
	m_pGameInstance = GET_INSTANCE(CGameInstance);

	return S_OK;
}

void CView::SetMediator(CViewMediator* pMediator)
{ 
	m_pMediator = pMediator;
	Safe_AddRef(m_pMediator);
	//m_pMediator->AddRef();
}

void CView::s2cPushBack(vector<_char*>& vecChar, string str)
{
	const _char* szSrc = str.c_str();
	size_t len = strlen(szSrc) + 1;
	_char* szCopy = new _char[len];
	strcpy_s(szCopy, len, szSrc);
	vecChar.push_back(szCopy);
}

void CView::s2cPushBack(vector<const _char*>& vecChar, string str)
{
	const _char* szSrc = str.c_str();
	size_t len = strlen(szSrc) + 1;
	_char* szCopy = new _char[len];
	strcpy_s(szCopy, len, szSrc);
	vecChar.push_back(szCopy);
}

void CView::s2cPushBackRef(vector<_char*>& vecChar, string& str)
{
	const _char* szSrc = str.c_str();
	size_t len = strlen(szSrc) + 1;
	_char* szCopy = new _char[len];
	strcpy_s(szCopy, len, szSrc);
	vecChar.push_back(szCopy);
}

void CView::s2cPushBackRef(vector<const _char*>& vecChar, string& str)
{
	const _char* szSrc = str.c_str();
	size_t len = strlen(szSrc) + 1;
	_char* szCopy = new _char[len];
	strcpy_s(szCopy, len, szSrc);
	vecChar.push_back(szCopy);
}

void CView::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pMediator);
	RELEASE_INSTANCE(CGameInstance);
}
