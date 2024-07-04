#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
	using Super = CComponent;

public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;	// Start
	virtual void	Tick(const _float& fTimeDelta)		override;

	void	SetScale(Vec3& vScale);
	void	SetScale(const Vec3& vScale)			{ SetScale(const_cast<Vec3&>(vScale)); }

	void	RotateYAxisFixed(Vec3& vEulers);	// TODO : Constraint변수를 따로 두고 Rotate는 하나로 통합
	void	RotateYAxisFixed(const Vec3& vEulers)	{ RotateYAxisFixed(const_cast<Vec3&>(vEulers)); }
	void	Rotate(Vec3& vEulers);
	void	Rotate(const Vec3& vEulers)				{ Rotate(const_cast<Vec3&>(vEulers)); }
	void	Rotate(Quaternion& vQuaternion);
	void	Rotate(const Quaternion& vQuaternion)	{ Rotate(const_cast<Quaternion&>(vQuaternion)); }
	void	Rotate(const _float& fXangle, const _float& fYangle, const _float& fZangle)
													{ Rotate(Vec3(fXangle, fYangle, fZangle)); }
	void	RotateAround(const Vec3& vPoint, const Vec3& vAxis, const _float& fAngle);	// Revolution;
	void	SetRotation(Vec3& vRotation);
	void	SetRotation(const Vec3& vRotation)		{ SetRotation(const_cast<Vec3&>(vRotation)); }
	void	SetRotationQuaternion(Quaternion& quatRotation);
	void	SetRotationQuaternion(const Quaternion& quatRotation)
													{ SetRotationQuaternion(const_cast<Quaternion&>(quatRotation)); }

	void	Translate(Vec3& vTranslation);
	void	Translate(const Vec3& vTranslation)		{ Translate(const_cast<Vec3&>(vTranslation)); }
	void	SetPosition(Vec3& vPosition)			{ m_matWorld.Translation(vPosition); }
	void	SetPosition(const Vec3& vPosition)		{ SetPosition(const_cast<Vec3&>(vPosition)); }

	//const	Matrix	WorldMatrix()					{ return m_matWorld; }
	Matrix&	WorldMatrix()							{ return m_matWorld; }
	void	Set_WorldMatrix(const Matrix& matWorld) { ::CopyMemory(&m_matWorld, &matWorld, sizeof(Matrix)); }

	Vec3	GetLocalScale();
	Vec3	GetRotation();
	Quaternion	GetRotationQuaternion();

	Vec3	GetRight()								{ return static_cast<Vec3>(m_matWorld.Right()); }
	Vec3	GetUp()									{ return static_cast<Vec3>(m_matWorld.Up()); }
	Vec3	GetForward()							{ return static_cast<Vec3>(m_matWorld.Backward()); }
	Vec3	GetPosition()							{ return static_cast<Vec3>(m_matWorld.Translation()); }
	void	SetRight(Vec3& vRight)					{ m_matWorld.Right(vRight); }
	void	SetUp(Vec3& vUp)						{ m_matWorld.Up(vUp); }
	void	SetForward(Vec3& vForward)				{ m_matWorld.Backward(vForward); }

public:
	// RUL의 크기가 각 축의 길이(크기)
	// 1, 2, 3 행을 Normalize하고 크기를 곱하는것이 Scaling
	// 회전값 초기화 : 기존 월드 행렬의 1, 2, 3 행의 크기를 저장
	// RUL을 초기화 후 다시 크기를 곱하면 회전행렬 초기화

	static Vec3 ToEulerAngles(Quaternion q);
	HRESULT	Bind_ShaderResources(class CShader* pShader, const _char* pConstantName);

private:
	Matrix m_matWorld = Matrix::Identity;

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg) override;	// Awake
	virtual void Free() override;
};

END