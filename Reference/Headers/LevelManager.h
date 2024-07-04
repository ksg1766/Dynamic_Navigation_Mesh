#pragma once

#include "Base.h"

/* Open_Level*/
/* 현재 게임내에서 활용하고 있는 레벨객체의 주소를 보관한다. */
/* 레벨 교체시에 기존 레벨을 파괴한다. + 기존 레벨용으로 만들어놓은 자원들을 삭제한다. */

/* 보관하고 있는 레벨의 틱을 반복적으로 호출해준다. */


BEGIN(Engine)

class CLevelManager final : public CBase
{
	DECLARE_SINGLETON(CLevelManager)
private:
	CLevelManager();
	virtual ~CLevelManager() = default;

public:
	void Tick(const _float& fTimeDelta);
	void LateTick(const _float& fTimeDelta);
	void DebugRender();

public:
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);
	_uint	GetCurrentLevelIndex() const { return m_iCurrentLevelIndex; }
	void	SetCurrentLevelIndex(_uint iLevelIndex) { m_iPreviousLevelIndex = m_iCurrentLevelIndex; m_iCurrentLevelIndex = iLevelIndex; }

private:
	_uint						m_iCurrentLevelIndex = { 0 };
	_uint						m_iPreviousLevelIndex = { 0 };
	class CLevel*				m_pCurrentLevel = { nullptr };

public:
	virtual void Free() override;
};

END