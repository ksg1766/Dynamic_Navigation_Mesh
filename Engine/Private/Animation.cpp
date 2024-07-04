#include "..\Public\Animation.h"
#include "Model.h"
#include "Bone.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_strName(rhs.m_strName)
	, m_fDuration(rhs.m_fDuration)
	, m_Channels(rhs.m_Channels)
	, m_fTickPerSecond(rhs.m_fTickPerSecond)
	, m_fPlayTime(rhs.m_fPlayTime)
	, m_iMaxFrameCount(rhs.m_iMaxFrameCount)
{
	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);
}

HRESULT CAnimation::Initialize_Prototype(const _float& fDuration, const _float& fTickPerSecond, vector<class CChannel*>& Channels, CModel* pModel, string strName)
{
	m_strName = strName;
	m_fDuration = fDuration;
	m_fTickPerSecond = fTickPerSecond;

	/* ���Ϳ� ����� �� ä��� ��ī�� �ϸ� ū�ϳ���. */
	//memcpy(&m_Channels, &Channels, sizeof(Channels)); 

	m_Channels.reserve(Channels.size());
	for (auto& iter : Channels)
		m_Channels.push_back(iter);

	for (_uint i = 0; i < m_Channels.size(); ++i)
	{
		m_iMaxFrameCount = max(m_iMaxFrameCount, (_uint)m_Channels[i]->Get_KeyFrames().size());

		m_ChannelKeyFrames.push_back(0);

		CBone* pBone = pModel->GetBone(m_Channels[i]->Get_Name().c_str());
		{
			if (nullptr == pBone)
				return E_FAIL;

			m_Bones.push_back(pBone);
		}
		Safe_AddRef(pBone);
	}

	return S_OK;
}

HRESULT CAnimation::Initialize(CModel* pModel)
{
	/* �ִϸ��̼��� ����ϱ� ���� ���Ǵ� ���� ��� �����Ѵ�.  */
	for (_uint i = 0; i < m_Channels.size(); ++i)
	{
		m_ChannelKeyFrames.push_back(0);

		CBone*	pBone = pModel->GetBone(m_Channels[i]->Get_Name().c_str());
		{
			if (nullptr == pBone)
				return E_FAIL;		

			m_Bones.push_back(pBone);
		}
		Safe_AddRef(pBone);
	}
	return S_OK;
}

HRESULT CAnimation::Play_Animation(_float fTimeDelta)
{
	/* ���� ��� �ð��� ���Ѵ�. */
	m_fPlayTime += m_fTickPerSecond * fTimeDelta;

	/* �ִϸ��̼��� �����ٸ� */
	if (m_fPlayTime >= m_fDuration)
	{
		m_fPlayTime = 0.f;

		for (auto& pChannel : m_Channels)
		{
			for (auto& iCurrentKeyFrame : m_ChannelKeyFrames)
				iCurrentKeyFrame = 0;			
		}
	}

	/* �� �ִϸ��̼��� ��� ä���� Ű�������� �����Ѵ�. (���� �θ� ����)*/
	_uint iChannelIndex = 0;
	for (auto& pChannel : m_Channels)
	{	// iChannelIndex�� �÷����� ��ȸ�ϸ鼭 Ʈ������ ������Ʈ �� �ְ�, ���� Ű�����ӵ� �ٽ� ����ؼ� ���� �� �ִµ�.
		m_ChannelKeyFrames[iChannelIndex] = pChannel->Update_Transformation(m_fPlayTime, m_ChannelKeyFrames[iChannelIndex], m_Bones[iChannelIndex]);

		++iChannelIndex;
	}

	return S_OK;
}

HRESULT CAnimation::Calculate_Animation(_uint iFrame)
{
	for (auto& pChannel : m_Channels)
	{
		for (auto& iCurrentKeyFrame : m_ChannelKeyFrames)
			iCurrentKeyFrame = iFrame;
	}

	/* �� �ִϸ��̼��� ��� ä���� Ű�������� �����Ѵ�. (���� �θ� ����)*/
	_uint iChannelIndex = 0;
	for (auto& pChannel : m_Channels)
	{	// iChannelIndex�� �÷����� ��ȸ�ϸ鼭 Ʈ������ ������Ʈ �� �ְ�, ���� Ű�����ӵ� �ٽ� ����ؼ� ���� �� �ִµ�.
		pChannel->Update_Transformation_NotLerp(m_ChannelKeyFrames[iChannelIndex], m_Bones[iChannelIndex]);

		++iChannelIndex;
	}

	return S_OK;
}

CAnimation* CAnimation::Create(const _float& fDuration, const _float& fTickPerSecond, vector<class CChannel*>& Channels, CModel* pModel, string strName)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize_Prototype(fDuration, fTickPerSecond, Channels, pModel, strName)))
	{
		MSG_BOX("Failed To Created : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation * CAnimation::Clone(CModel* pModel)
{
	CAnimation*			pInstance = new CAnimation(*this);

	if (FAILED(pInstance->Initialize(pModel)))
	{
		MSG_BOX("Failed To Cloned : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	/* Channel */
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);
	m_Channels.clear();

	/* HierarachyNode */
	for (auto& pBone : m_Bones)
		Safe_Release(pBone);
	m_Bones.clear();
}
