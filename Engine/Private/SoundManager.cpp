#include "SoundManager.h"

IMPLEMENT_SINGLETON(CSoundManager)

CSoundManager::CSoundManager()
{
	//ZeroMemory(m_pChannelArr, sizeof(m_pChannelArr));
}

HRESULT CSoundManager::Reserve_Manager()
{
	/*FMOD_System_Create(&m_pSystem, FMOD_VERSION);

	FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);

	if (nullptr == m_pSystem)
		return E_FAIL;

	if (FAILED(LoadSoundFile()))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CSoundManager::PlaySoundFile(const wstring& strSoundKey, CHANNELID eCh, _float fVolume)
{
	/*auto iter = m_Sounds.find(strSoundKey);

	if (iter == m_Sounds.end())
		return E_FAIL;

	FMOD_Channel_Stop(m_pChannelArr[_uint(eCh)]);
	FMOD_System_PlaySound(m_pSystem, iter->second, NULL, FALSE, &m_pChannelArr[_uint(eCh)]);

	FMOD_Channel_SetVolume(m_pChannelArr[_uint(eCh)], fVolume);

	FMOD_System_Update(m_pSystem);*/

	return S_OK;
}

HRESULT CSoundManager::CheckPlaySoundFile(const wstring& strSoundKey, CHANNELID& eCh, _float& fVolume)
{
	/*auto iter = m_Sounds.find(strSoundKey);

	if (iter == m_Sounds.end())
		return E_FAIL;

	FMOD_BOOL	bPlay = FALSE;
	FMOD_RESULT bResult = FMOD_Channel_IsPlaying(m_pChannelArr[_uint(eCh)], &bPlay);

	if (bResult != FMOD_OK)
	{
		FMOD_Channel_Stop(m_pChannelArr[_uint(eCh)]);
		FMOD_System_PlaySound(m_pSystem, iter->second, NULL, FALSE, &m_pChannelArr[_uint(eCh)]);

		FMOD_Channel_SetVolume(m_pChannelArr[_uint(eCh)], fVolume);

		FMOD_System_Update(m_pSystem);
	}*/

	return S_OK;
}

HRESULT CSoundManager::PlayBGM(const wstring& strSoundKey, _float& fVolume)
{
	/*if (nullptr == m_pSystem)
		return E_FAIL;

	auto iter = m_Sounds.find(strSoundKey);

	if (iter == m_Sounds.end())
		return E_FAIL;

	FMOD_System_PlaySound(m_pSystem, iter->second, NULL, FALSE, &m_pChannelArr[_uint(CHANNELID::CHANNEL_BGM)]);
	FMOD_Channel_SetMode(m_pChannelArr[_uint(CHANNELID::CHANNEL_BGM)], FMOD_LOOP_NORMAL);
	FMOD_Channel_SetVolume(m_pChannelArr[_uint(CHANNELID::CHANNEL_BGM)], fVolume);
	FMOD_System_Update(m_pSystem);*/

	return S_OK;
} 

HRESULT CSoundManager::StopSound(CHANNELID& eCh)
{
	/*if (nullptr == m_pSystem)
		return E_FAIL;

	FMOD_Channel_Stop(m_pChannelArr[_uint(eCh)]);*/

	return S_OK;
}

HRESULT CSoundManager::StopSoundAll()
{
	/*if (nullptr == m_pSystem)
		return E_FAIL;

	for (auto iter : m_pChannelArr)
		FMOD_Channel_Stop(iter);*/

	return S_OK;
}

HRESULT CSoundManager::SetChannelVolume(CHANNELID& eCh, _float& fVolume)
{
	/*if (nullptr == m_pSystem)
		return E_FAIL;

	FMOD_Channel_SetVolume(m_pChannelArr[_uint(eCh)], fVolume);

	FMOD_System_Update(m_pSystem);*/

	return S_OK;
}

HRESULT CSoundManager::LoadSoundFile()
{
	/*_finddata_t fd;

	long long handle = _findfirst("../Bin/Resources/Audio/*.*", &fd);

	if (handle == -1)
		return E_FAIL;

	_int iResult = 0;

	wstring strCurPath = TEXT("../Bin/Resources/Audio/");
	
	while (iResult != -1)
	{
		wstring strFileName = wstring(fd.name, fd.name + strlen(fd.name));
		wstring strFullPath = strCurPath + strFileName;

		_int iRequiredSize = WideCharToMultiByte(CP_UTF8, 0, strFullPath.c_str(), -1, NULL, 0, NULL, NULL);
		if (iRequiredSize > 0)
		{
			char* szFullPath = new char[iRequiredSize];
			int result = WideCharToMultiByte(CP_UTF8, 0, strFullPath.c_str(), -1, szFullPath, iRequiredSize, NULL, NULL);

			FMOD_SOUND* pSound = nullptr;
			FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, 0, &pSound);

			Safe_Delete_Array(szFullPath);

			if (eRes == FMOD_OK)
				m_Sounds.emplace(strFileName, pSound);
		}

		iResult = _findnext(handle, &fd);
	}

	FMOD_System_Update(m_pSystem);

	_findclose(handle);*/

	return S_OK;
}

void CSoundManager::Free()
{
	/*Super::Free();

	for (auto& Pair : m_Sounds)
	{
		FMOD_Sound_Release(Pair.second);
	}
	m_Sounds.clear();

	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);*/

}
