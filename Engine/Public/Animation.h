#pragma once

#include "Base.h"

BEGIN(Engine)

class CModel;
class CChannel;
class CBone;
class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize_Prototype(const _float& fDuration, const _float& fTickPerSecond, vector<CChannel*>& Channels, CModel* pModel, string strName);
	HRESULT Initialize(CModel* pModel);

public:
	HRESULT Play_Animation(_float fTimeDelta);
	HRESULT Calculate_Animation(_uint iFrame);

public:
	vector<CChannel*>& GetChannels()	{ return m_Channels; }
	_uint	GetMaxFrameCount()			{ return m_iMaxFrameCount; }
	_float	GetTickPerSecond()			{ return m_fTickPerSecond; }
	_float	GetDuaration()				{ return m_fDuration; }

private:
	string						m_strName;
	vector<CChannel*>			m_Channels;			/* ���Ǵ� ä�� ���� */

	_float						m_fDuration = 0.f;	/* ��ü ��� �ð� */
	_float						m_fPlayTime = 0.f;	/* ���� ��� �ð� */
	_float						m_fTickPerSecond = 0.f; /* �ʴ� ��� �ӵ� */

	vector<CBone*>				m_Bones;			/* �� �ִϸ��̼��� ����ϴµ� �ʿ��� ���� */
	vector<_uint>				m_ChannelKeyFrames;
	_uint						m_iMaxFrameCount = 0;

public:
	static CAnimation* Create(const _float& fDuration, const _float& fTickPerSecond, vector<CChannel*>& Channels, CModel* pModel, string strName);
	CAnimation* Clone(CModel* pModel);
	virtual void Free() override;

	friend class CAnimationView;
};

END