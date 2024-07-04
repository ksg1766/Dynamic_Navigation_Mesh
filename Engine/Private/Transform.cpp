#include "Transform.h"
#include "Shader.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Super(pDevice, pContext, ComponentType::Transform)
{
}

CTransform::CTransform(const CTransform& rhs)
	:Super(rhs)
	, m_matWorld(rhs.m_matWorld)
{
}

HRESULT CTransform::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	return S_OK;
}

void CTransform::Tick(const _float& fTimeDelta)
{
}

void CTransform::SetScale(Vec3& vScale)
{
	/*Vec3 vRight = m_matWorld.Right();
	Vec3 vUp = m_matWorld.Up();
	Vec3 vLook = m_matWorld.Backward();

	vRight.Normalize();
	vUp.Normalize();
	vLook.Normalize();*/

	for (_int i = 0; i < 3; ++i)
	{
		Vec3 v(m_matWorld.m[i]);
		v.Normalize();

		for (_int j = 0; j < 3; ++j)
			m_matWorld.m[i][j] = *(((_float*)&v) + j) * *(((_float*)&vScale) + i);
	}
}

void CTransform::Translate(Vec3& vTranslation)
{
	for(_uint i = 0; i < 3; ++i)
		*((_float*)(&m_matWorld.m[3]) + i) += *((_float*)&vTranslation + i);
}

void CTransform::Rotate(Vec3& vEulers)
{
	Matrix matRotation = Matrix::Identity;
	Quaternion quat = Quaternion::Identity;

	vEulers.x = XMConvertToRadians(vEulers.x);
	vEulers.y = XMConvertToRadians(vEulers.y);
	vEulers.z = XMConvertToRadians(vEulers.z);

	quat = Quaternion::CreateFromYawPitchRoll(vEulers.y, vEulers.x, vEulers.z);
	
	matRotation = Matrix::CreateFromQuaternion(quat);

	for (_uint i = 0; i < 3; ++i)
	{
		Vec3 v(m_matWorld.m[i]);
		v = Vec3::TransformNormal(v, matRotation);

		for (_uint j = 0; j < 3; ++j)
			m_matWorld.m[i][j] = *((_float*)&v + j);
	}
}

void CTransform::RotateYAxisFixed(Vec3& vEulers)
{
	Matrix matRotation = Matrix::Identity;
	Quaternion quat = Quaternion::Identity;

	if (0.f != vEulers.y)
	{
		Vec3 v(m_matWorld.m[1]);
		quat = Quaternion::CreateFromAxisAngle(Vec3(0.f, 1.f, 0.f), XMConvertToRadians(vEulers.y));
	}
	if (0.f != vEulers.x)
	{
		Vec3 v(m_matWorld.m[0]);
		quat *= Quaternion::CreateFromAxisAngle(v, XMConvertToRadians(vEulers.x));
	}
	if (0.f != vEulers.z)
	{
		Vec3 v(m_matWorld.m[2]);
		quat *= Quaternion::CreateFromAxisAngle(v, XMConvertToRadians(vEulers.z));
	}

	matRotation = Matrix::CreateFromQuaternion(quat);

	for (_uint i = 0; i < 3; ++i)
	{
		Vec3 v(m_matWorld.m[i]);
		v = Vec3::TransformNormal(v, matRotation);

		for (_uint j = 0; j < 3; ++j)
			m_matWorld.m[i][j] = *((_float*)&v + j);
	}
}

void CTransform::Rotate(Quaternion& vQuaternion)
{
	Matrix matRotation = Matrix::CreateFromQuaternion(vQuaternion);

	for (_uint i = 0; i < 3; ++i)
	{
		Vec3 v(m_matWorld.m[i]);
		v = Vec3::TransformNormal(v, matRotation);

		for (_uint j = 0; j < 3; ++j)
			m_matWorld.m[i][j] = *((_float*)&v + j);
	}
}

void CTransform::RotateAround(const Vec3& vPoint, const Vec3& vAxis, const _float& fAngle)
{
	Quaternion quat;
	Matrix matRotation;
	Vec3 vOut;
	Vec3 v(m_matWorld.m[3]);

	quat = Quaternion::CreateFromAxisAngle(vAxis, XMConvertToRadians(fAngle));
	matRotation = Matrix::CreateFromQuaternion(quat);

	vOut = Vec3::Transform(v - vPoint, matRotation);
	m_matWorld *= matRotation;

	Vec3 _v = vOut + vPoint;
	for (_uint i = 0; i < 3; ++i)
		*((_float*)m_matWorld.m[3] + i) = *((_float*)&_v + i);

	/*Matrix matRotation = Matrix::Identity;
	Quaternion quat = Quaternion::Identity;

	if (0.f != vAxis.y)
	{
		quat = Quaternion::CreateFromAxisAngle(vAxis, XMConvertToRadians(fAngle));
	}
	if (0.f != vAxis.x)
	{
		quat *= Quaternion::CreateFromAxisAngle(vAxis, XMConvertToRadians(fAngle));
	}
	if (0.f != vAxis.z)
	{
		quat *= Quaternion::CreateFromAxisAngle(vAxis, XMConvertToRadians(fAngle));
	}

	matRotation = Matrix::CreateFromQuaternion(quat);

	for (_uint i = 0; i < 3; ++i)
	{
		Vec3 v(m_matWorld.m[i][0], m_matWorld.m[i][1], m_matWorld.m[i][2]);
		v = Vec3::TransformNormal(v, matRotation);

		for (_uint j = 0; j < 3; ++j)
			m_matWorld.m[i][j] = *((_float*)&v + j);
	}*/
}

void CTransform::SetRotationQuaternion(Quaternion& quatRotation)
{
	Vec3 vScale;
	Quaternion quatDummy;
	Vec3 vPosition;
	m_matWorld.Decompose(vScale, quatDummy, vPosition);

	Matrix matScale = Matrix::CreateScale(vScale);
	Matrix matRotation = Matrix::CreateFromQuaternion(quatRotation);
	Matrix matTranslation = Matrix::CreateTranslation(vPosition);

	m_matWorld = matScale * matRotation * matTranslation;
}

void CTransform::SetRotation(Vec3& vRotation)
{
	Vec3 vScale;
	Quaternion quatDummy;
	Vec3 vPosition;
	m_matWorld.Decompose(vScale, quatDummy, vPosition);

	vRotation.x = XMConvertToRadians(vRotation.x);
	vRotation.y = XMConvertToRadians(vRotation.y);
	vRotation.z = XMConvertToRadians(vRotation.z);

	Matrix matScale = Matrix::CreateScale(vScale);
	Matrix matRotation = Matrix::CreateFromYawPitchRoll(vRotation.y, vRotation.x, vRotation.z);
	Matrix matTranslation = Matrix::CreateTranslation(vPosition);

	m_matWorld = matScale * matRotation * matTranslation;
}

Vec3 CTransform::GetLocalScale()
{
	Vec3 vScale;

	vScale.x = m_matWorld.Right().Length();
	vScale.y = m_matWorld.Up().Length();
	vScale.z = m_matWorld.Backward().Length();

	/*for (_int i = 0; i < STATE_POSITION; ++i)
		*(((_float*)&vScale) + i) = Vec3(m_matWorld.m[i]).Length();*/

	return vScale;
}

Vec3 CTransform::GetRotation()
{
	Vec3 vDummy;
	Quaternion quat;
	m_matWorld.Decompose(vDummy, quat, vDummy);

	//vDummy = ToEulerAngles(quat);

	return ToEulerAngles(quat);
}

Quaternion CTransform::GetRotationQuaternion()
{
	Vec3 vDummy;
	Quaternion quat;
	m_matWorld.Decompose(vDummy, quat, vDummy);

	return quat;
}

Vec3 CTransform::ToEulerAngles(Quaternion q)
{
	Vec3 angles;

	// roll (x-axis rotation)
	_float sinr_cosp = 2.f * (q.w * q.x + q.y * q.z);
	_float cosr_cosp = 1.f - 2.f * (q.x * q.x + q.y * q.y);
	angles.x = std::atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	_float sinp = std::sqrt(1.f + 2.f * (q.w * q.y - q.x * q.z));
	_float cosp = std::sqrt(1.f - 2.f * (q.w * q.y - q.x * q.z));
	angles.y = 2.f * std::atan2(sinp, cosp) - 3.14159265f / 2.f;

	// yaw (z-axis rotation)
	_float siny_cosp = 2.f * (q.w * q.z + q.x * q.y);
	_float cosy_cosp = 1.f - 2.f * (q.y * q.y + q.z * q.z);
	angles.z = std::atan2(siny_cosp, cosy_cosp);

	return angles;
}

HRESULT CTransform::Bind_ShaderResources(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_matWorld);
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTransform::Clone(CGameObject* pGameObject, void* pArg)
{
	CTransform* pInstance = new CTransform(*this);
	pInstance->m_pGameObject = pGameObject;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTransform::Free()
{
	Super::Free();
}