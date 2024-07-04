#include "stdafx.h"
#include "TransformView.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Transform.h"
#include "Terrain.h"
#include "Utils.h"
//
#include "StaticBase.h"

CTransformView::CTransformView(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Super(pDevice, pContext)
{
}

HRESULT CTransformView::Initialize(void* pArg)
{
	Super::Initialize(pArg);

	return S_OK;
}

HRESULT CTransformView::Tick()
{
	Input();

	ImGui::Begin("Transform");

	InfoView();
	TransformController();

	ImGui::End();

	return S_OK;
}

HRESULT CTransformView::LateTick()
{

	return S_OK;
}

HRESULT CTransformView::DebugRender()
{

	return S_OK;
}

void CTransformView::SetObject(CGameObject* pGameObject)
{
	m_pGameObject = pGameObject;
}

void CTransformView::Input()
{
	if (ImGui::GetIO().WantCaptureMouse && ImGui::IsMouseClicked(0))
		return;


}

void CTransformView::InfoView()
{
	ImGui::Text("This window has some useful function to control Transform of Object.");
	ImGui::NewLine();

	
}

void CTransformView::TransformController()
{
	Vec3 vPosition = Vec3::Zero;
	Vec3 vRotation = Vec3::Zero;
	Vec3 vScale = Vec3::One;

	if (m_pGameObject && !m_pGameObject->IsDead())
	{
		CTransform* pTransform = m_pGameObject->GetTransform();

		vPosition = pTransform->GetPosition();
		Quaternion quatRotation = pTransform->GetRotationQuaternion();
		vRotation = CTransform::ToEulerAngles(quatRotation);
		vRotation = Vec3(XMConvertToDegrees(vRotation.x), XMConvertToDegrees(vRotation.y), XMConvertToDegrees(vRotation.z));
		vScale = pTransform->GetLocalScale();
	}
	else
		return;

	_float varrPosition[3] = { vPosition.x, vPosition.y, vPosition.z };
	_float varrRotation[3] = { 0.f };
	_float varrLocalScale[3] = { vScale.x, vScale.y, vScale.z };

	ImGui::InputFloat3("Position", varrPosition);
	ImGui::Text("X: %.3f  Y: %.3f  Z: %.3f  Rotation", vRotation.x, vRotation.y, vRotation.z);
	_float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
	if (ImGui::ArrowButton("RotationXp", ImGuiDir_Up))
		varrRotation[0] += 5.f;
	ImGui::SameLine(0.0f, spacing);
	if(ImGui::ArrowButton("RotationXm", ImGuiDir_Down))
		varrRotation[0] -= 5.f;
	ImGui::SameLine(0.0f, 7.f * spacing);
	if(ImGui::ArrowButton("RotationYp", ImGuiDir_Up))
		varrRotation[1] += 5.f;
	ImGui::SameLine(0.0f, spacing);
	if(ImGui::ArrowButton("RotationYm", ImGuiDir_Down))
		varrRotation[1] -= 5.f;
	ImGui::SameLine(0.0f, 7.f * spacing);
	if(ImGui::ArrowButton("RotationZp", ImGuiDir_Up))
		varrRotation[2] += 5.f;
	ImGui::SameLine(0.0f, spacing);
	if(ImGui::ArrowButton("RotationZm", ImGuiDir_Down))
		varrRotation[2] -= 5.f;
	ImGui::InputFloat3("Scale", varrLocalScale);

	if (m_pGameObject && !m_pGameObject->IsDead())
	{
		CTransform* pTransform = m_pGameObject->GetTransform();

		vPosition = Vec3(varrPosition);
		vRotation = Vec3(varrRotation);
		vScale = Vec3(varrLocalScale);

		pTransform->SetScale(vScale);
		pTransform->Rotate(vRotation);
		pTransform->SetPosition(vPosition);
	}
}

CTransformView* CTransformView::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransformView* pInstance = new CTransformView(pDevice, pContext);

	if (FAILED(pInstance->Initialize(nullptr)))
	{
		MSG_BOX("Failed to Created : CTransformView");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransformView::Free()
{
	Super::Free();
}
