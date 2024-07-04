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
	vector<CChannel*>			m_Channels;			/* 사용되는 채널 집합 */

	_float						m_fDuration = 0.f;	/* 전체 재생 시간 */
	_float						m_fPlayTime = 0.f;	/* 현재 재생 시간 */
	_float						m_fTickPerSecond = 0.f; /* 초당 재생 속도 */

	vector<CBone*>				m_Bones;			/* 이 애니메이션을 재생하는데 필요한 뼈들 */
	vector<_uint>				m_ChannelKeyFrames;
	_uint						m_iMaxFrameCount = 0;

public:
	static CAnimation* Create(const _float& fDuration, const _float& fTickPerSecond, vector<CChannel*>& Channels, CModel* pModel, string strName);
	CAnimation* Clone(CModel* pModel);
	virtual void Free() override;

	friend class CAnimationView;
};

END