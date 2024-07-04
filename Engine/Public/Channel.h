#pragma once

/* ���� �ִϸ��̼� �ð��뺰 Ʈ������ �� ���� ���� Ű�������� �� ���� �ִ�. */

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
	using Super = CBase;
public:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const string strName, vector<KEYFRAME>& Keyframes);
	_uint	Update_Transformation(_float fPlayTime, _uint iCurrentKeyFrame, class CBone* pNode); 
	void	Update_Transformation_NotLerp(_uint iCurrentKeyFrame, class CBone* pNode); 

public:
	string Get_Name() const { return m_szName; }
	const vector<KEYFRAME>& Get_KeyFrames() const { return m_KeyFrames; }

private:
	string				m_szName = {};
	vector<KEYFRAME>	m_KeyFrames;

public:
	static CChannel* Create(const string strName, vector<KEYFRAME>& Keyframes);
	virtual void Free() override;
};

END