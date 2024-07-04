#pragma once

#include "Base.h"

BEGIN(Engine)

class CModel;
class ENGINE_DLL CSocket final : public CBase
{
private:
	CSocket();
	CSocket(const CSocket& rhs) = delete;
	virtual ~CSocket() = default;

public:
	HRESULT Initialize(const _int iBoneIndex);
										// TODO: ���� ���� ���� �� Ȯ�� �� �� �͵� :
										// ���⿡ ������ ������Ʈ�� �����ϸ� NonAnim���״�, �θ� ���� TWEENDESC�� ��������
										// Ʈ������ ���� Ʈ���� ������ �������� ���� ���� ������� �ʾƵ� �ȴ�.
										// �ƿ� ������ �ؽ��Ŀ� srv�� ����, ���� �ؽ��Ŀ� srv�� ���ε��� �� ���ε����� �����ϸ� �ؽ��ĸ� �ȵ�� �־ �ɱ�?
										// ������ ������ srv�� ���� ���Ͽ��� srv ���ε� �� ȣ���ϵ��� �����ϸ� ���
										// GetMatrix()�� Matrix���� ��������

public:									// ���� private���� �س��� ���� Ư�� �Լ����� friend �� ���� ���°� ���?
	HRESULT			Equip(CModel* pParts);
	HRESULT			UnEquip();

	HRESULT			LoadSRV(ID3D11ShaderResourceView*& pSocketSRV);
	HRESULT			LoadTweenDescFromBone(TWEENDESC& tweenDec);
	HRESULT			LoadTrasformFromBone(const Matrix& matPartsWorld);	// ������ �θ�->���� �Ϲ������� Ʈ���װ� srv�� �������ִ� ��� ���Ҹ� ��.

	HRESULT			BindBoneIndex();

private:
	_int						m_iBoneIndex = 0;

	CModel*						m_pPartsModel		= nullptr;
	ID3D11ShaderResourceView*	m_pSocketSRV		= nullptr;

public:
	static CSocket* Create(const _int iBoneIndex);	// TODO: Model Ŭ������ Create���� Socket ������ �� ��ġ �����ϵ���.
	virtual void Free();
};

END