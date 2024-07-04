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
										// TODO: 내일 정신 맑을 때 확인 해 볼 것들 :
										// 여기에 장착될 오브젝트는 웬만하면 NonAnim일테니, 부모 모델의 TWEENDESC를 전해주자
										// 트위닝은 모델의 트위닝 정보를 가져오면 굳이 따로 계산하지 않아도 된다.
										// 아예 소켓의 텍스쳐와 srv를 빼고, 모델의 텍스쳐와 srv를 바인딩한 후 뼈인덱스로 접근하면 텍스쳐를 안들고 있어도 될까?
										// 무기의 렌더는 srv를 가진 소켓에서 srv 바인딩 후 호출하도록 위임하면 어떨까
										// GetMatrix()로 Matrix변수 가져오기

public:									// 전부 private으로 해놓고 모델의 특정 함수에만 friend 로 열어 보는건 어떨까?
	HRESULT			Equip(CModel* pParts);
	HRESULT			UnEquip();

	HRESULT			LoadSRV(ID3D11ShaderResourceView*& pSocketSRV);
	HRESULT			LoadTweenDescFromBone(TWEENDESC& tweenDec);
	HRESULT			LoadTrasformFromBone(const Matrix& matPartsWorld);	// 소켓은 부모->파츠 일방향으로 트위닝과 srv를 전달해주는 통로 역할만 함.

	HRESULT			BindBoneIndex();

private:
	_int						m_iBoneIndex = 0;

	CModel*						m_pPartsModel		= nullptr;
	ID3D11ShaderResourceView*	m_pSocketSRV		= nullptr;

public:
	static CSocket* Create(const _int iBoneIndex);	// TODO: Model 클래스의 Create에서 Socket 사이즈 및 위치 전달하도록.
	virtual void Free();
};

END