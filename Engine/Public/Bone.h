#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CBone final : public CBase
{
private:
	CBone();
	CBone(const CBone& rhs);
	virtual ~CBone() = default;

public:
	HRESULT Initialize_Prototype(string strName, Matrix transformMatrix, Matrix offsetMatrix, _int iBoneIndex, _int iParentIndex, _uint iDepth);
	HRESULT Initialize(void* pArg);
public:
	const _char*	Get_Name() const { return m_szName; }
	const _int&		Get_Index() const { return m_iIndex; }
	const _int&		Get_ParentIndex() const { return m_iParentIndex; }
	_matrix			Get_Transformation() { return XMLoadFloat4x4(&m_Transformation); }
	_matrix			Get_OffSetMatrix() { return XMLoadFloat4x4(&m_OffsetMatrix); }
	_matrix			Get_CombinedTransformation() { return XMLoadFloat4x4(&m_CombinedTransformation); }

public:
	HRESULT			Set_Parent(CBone* pParent);
	void			Set_Transformation(_fmatrix Transformation) { XMStoreFloat4x4(&m_Transformation, Transformation); }
	void			Set_CombinedTransformation();
	void			Set_OffsetMatrix(_fmatrix OffsetMatrix) { XMStoreFloat4x4(&m_OffsetMatrix, OffsetMatrix); }

private:
	_char			m_szName[MAX_PATH] = "";

	_float4x4		m_Transformation;			/* Relative(�θ� ����) - ����ٰ� �θ��� m_CombinedTransformation�� ���ϸ� ��Ʈ �������� ��ȯ */
	_float4x4		m_CombinedTransformation;	/* Global(��Ʈ ����) */
	_float4x4		m_OffsetMatrix;				/* Global(��Ʈ ���� = m_CombinedTransformation)�� ����� */

	CBone*			m_pParent = nullptr;
	_int			m_iParentIndex = 0;
	_int			m_iIndex = 0;

public:
	static CBone* Create(string strName, Matrix transformMatrix, Matrix offsetMatrix, _int iBoneIndex, _int iParentIndex, _uint iDepth);
	CBone* Clone(void* pArg = nullptr);
	virtual void Free();
};

END