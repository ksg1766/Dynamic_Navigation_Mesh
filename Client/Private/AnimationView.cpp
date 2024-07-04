#include "stdafx.h"
#include "AnimationView.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Animation.h"
#include "Channel.h"
#include <filesystem>
#include "Utils.h"
#include "FileUtils.h"

CAnimationView::CAnimationView(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Super(pDevice, pContext)
{
}

HRESULT CAnimationView::Initialize(void* pArg)
{
	Super::Initialize(pArg);

	return S_OK;
}

HRESULT CAnimationView::Tick()
{
	Input();

	ImGui::Begin("Animations");

	InfoView();
	ItemGroup();
	AnimationGroup();
	DeleteReservedGroup();

	ImGui::End();

	return S_OK;
}

HRESULT CAnimationView::LateTick()
{
	return S_OK;
}

HRESULT CAnimationView::DebugRender()
{
	return S_OK;
}

void CAnimationView::Input()
{
	if (ImGui::GetIO().WantCaptureMouse)
		return;
}

void CAnimationView::InfoView()
{
	ImGui::Text("This window has some useful function for Animations.");
	ImGui::NewLine();

	ImGui::Text("Current Object :"); ImGui::SameLine(); ImGui::Text(m_strSelectedObject.c_str());
	
	_int iAnimCount = 0;
	if (m_pAnimModelObject)
		iAnimCount = m_pAnimModelObject->GetModel()->m_Animations.size();
	
	ImGui::Text("Number of Animation :  %d", iAnimCount);
	
	ImGui::NewLine();

	if (ImGui::Button("Load Model"))
	{
		if(m_pAnimModelObject)
			m_pGameInstance->DeleteObject(m_pAnimModelObject);
			
		m_vecAnimationNames.clear();
		m_vecDeleteReserved.clear();

		wstring strDirectory = TEXT("../Bin/Resources/Models/Skeletal/") + Utils::ToWString(m_strSelectedObject);
		
		wstring strPrototype = TEXT("Prototype_GameObject_") + Utils::ToWString(m_strSelectedObject);
		
		m_pAnimModelObject = m_pGameInstance->CreateObject(strPrototype, LAYERTAG::DEFAULT);
		
		LoadAnimations();	
	}
	ImGui::SameLine();
	if (ImGui::Button("Export Anim"))
	{
		ExportAnimations(TEXT("../Bin/Resources/Models/Skeletal/") + Utils::ToWString(m_strSelectedObject));

		if (m_pAnimModelObject)
		{
			m_pGameInstance->DeleteObject(m_pAnimModelObject);
			m_pAnimModelObject = nullptr;
		}
	}

	if (m_pAnimModelObject)
	{
		Matrix matTransformCam = m_pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_VIEW);
		m_pAnimModelObject->GetTransform()->SetPosition(matTransformCam.Translation() + 30.f * matTransformCam.Backward());
		m_pAnimModelObject->GetTransform()->SetRotation(Vec3(0.f, 180.f, 0.f));
	}

	ImGui::NewLine();
}

void CAnimationView::ItemGroup()
{
	vector<const _char*> vecItems;
	wstring strDirectory = TEXT("../Bin/Resources/Models/Skeletal/");

	if (filesystem::exists(strDirectory) && filesystem::is_directory(strDirectory))
	{
		for (const auto& entry : filesystem::directory_iterator(strDirectory))
		{
			if (entry.is_directory())
			{
				s2cPushBack(vecItems, Utils::ToString(entry.path().stem()));
			}
		}
	}

	ImGui::ListBox("Skeletal Files", &m_Item_Current, vecItems.data(), vecItems.size(), 5);

	if (vecItems.size())
	{
		m_strSelectedObject = string(vecItems[m_Item_Current]);
	}
}

void CAnimationView::AnimationGroup()
{
	if (m_vecAnimationNames.empty())
		return;

	_int iCurSelected = m_Animation_Current;
	ImGui::ListBox("Animation List", &m_Animation_Current, m_vecAnimationNames.data(), m_vecAnimationNames.size(), 5);

	if (iCurSelected != m_Animation_Current)
		m_pAnimModelObject->GetModel()->SetNextAnimationIndex(m_Animation_Current);

	m_strSelectedAnimation = string(m_vecAnimationNames[m_Animation_Current]);

	if (ImGui::Button("Delete"))
	{
		m_vecDeleteReserved.push_back(m_vecAnimationNames[m_Animation_Current]);
		m_vecAnimationNames[m_Animation_Current] = "-----Deleted-----";
	}
}

void CAnimationView::DeleteReservedGroup()
{
	if (m_vecDeleteReserved.empty())
		return;

	ImGui::ListBox("Delete List", &m_Delete_Current, m_vecDeleteReserved.data(), m_vecDeleteReserved.size(), 5);

	
	/*if (ImGui::Button("Reset"))
	{
		for (auto& iter : m_vecAnimationNames)
		{
			for (auto& _iter : m_vecDeleteReserved)
			{
				if (S_OK == strcmp(iter, _iter))
					iter = _iter;
			}
		}
		m_vecAnimationNames.push_back(m_vecDeleteReserved[m_Delete_Current]);
		m_vecDeleteReserved.erase(m_vecDeleteReserved.begin() + m_Delete_Current);
	}*/
}

HRESULT CAnimationView::LoadAnimations()
{
	for (auto& iter : m_pAnimModelObject->GetModel()->m_Animations)
	{
		s2cPushBack(m_vecAnimationNames, iter->m_strName);
	}

	return S_OK;
}

HRESULT CAnimationView::ExportAnimations(const wstring& strModelFilePath)
{
	wstring folderName = filesystem::path(strModelFilePath).stem();
	wstring filePath = (filesystem::path(strModelFilePath) / wstring(folderName + TEXT(".anim")));
	Utils::Replace(filePath, TEXT("\\"), TEXT("/"));

	if (!filesystem::exists(filePath))
		return E_FAIL;

	vector<CAnimation*>& vecAnimations = m_pAnimModelObject->GetModel()->m_Animations;

	// Export
	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(filePath, FileMode::Write);

	file->Write<size_t>(vecAnimations.size() - m_vecDeleteReserved.size());
	for (CAnimation*& pAnimation : vecAnimations)
	{
		_bool isDelete = false;
		for (const _char* szName : m_vecDeleteReserved)
		{
			string strCopy = pAnimation->m_strName.substr(pAnimation->m_strName.find_last_of("|") + 1);
			const _char* szSrc = strCopy.c_str();

			if (!strcmp(szSrc, szName))
			{
				isDelete = true;
				break;
			}
		}
		if (isDelete) continue;

		/*if (*find_if(m_vecDeleteReserved.begin(), m_vecDeleteReserved.end(), [&](const _char* szName)
		{
			string strCopy = pAnimation->m_strName.substr(pAnimation->m_strName.find_last_of("|") + 1);
			const _char* szSrc = strCopy.c_str();

			return !strcmp(szSrc, szName);
		}))	continue;*/

		string strCopy = pAnimation->m_strName.substr(pAnimation->m_strName.find_last_of("|") + 1);
		file->Write<string>(strCopy);
		//file->Write<string>(pAnimation->m_strName);
		file->Write<_float>(pAnimation->m_fDuration);
		file->Write<_float>(pAnimation->m_fTickPerSecond);

		vector<CChannel*>& vecChannels = pAnimation->m_Channels;
		file->Write<size_t>(vecChannels.size());
		for (CChannel*& pChannel : vecChannels)
		{
			file->Write<string>(pChannel->Get_Name());

			file->Write<size_t>(pChannel->Get_KeyFrames().size());
			for (const KEYFRAME& tKeyframe : pChannel->Get_KeyFrames())
			{
				file->Write<_float>(tKeyframe.fTime);
				file->Write<Vec3>(tKeyframe.vScale);
				file->Write<Quaternion>(tKeyframe.vRotation);
				file->Write<Vec3>(tKeyframe.vPosition);
			}
		}
	}
	return S_OK;
}

CAnimationView* CAnimationView::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAnimationView* pInstance = new CAnimationView(pDevice, pContext);

	if (FAILED(pInstance->Initialize(nullptr)))
	{
		MSG_BOX("Failed to Created : CAnimationView");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimationView::Free()
{
	Super::Free();
}
