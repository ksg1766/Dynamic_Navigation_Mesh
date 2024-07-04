#pragma once
#include "Base.h"
#include "hasher.h"

BEGIN(Engine)

class CSoundManager final : public CBase
{
	using Super = CBase;
	DECLARE_SINGLETON(CSoundManager)

private:
	CSoundManager();
	virtual ~CSoundManager() = default;

public:
	HRESULT Reserve_Manager();

public:
	HRESULT PlaySoundFile(const wstring& strSoundKey, CHANNELID eCh, _float fVolume);
	HRESULT CheckPlaySoundFile(const wstring& strSoundKey, CHANNELID& eCh, _float& fVolume);
	HRESULT PlayBGM(const wstring& strSoundKey, _float& fVolume);
	HRESULT StopSound(CHANNELID& eCh);
	HRESULT StopSoundAll();
	HRESULT SetChannelVolume(CHANNELID& eCh, _float& fVolume);

private:
	HRESULT LoadSoundFile();

private:
	/*FMOD_SYSTEM*	m_pSystem = { nullptr };

	FMOD_CHANNEL*	m_pChannelArr[_uint(CHANNELID::CHANNEL_END)];

	unordered_map<const wstring, FMOD_SOUND*, djb2Hasher> m_Sounds;*/


public:
	virtual void Free();

};

END