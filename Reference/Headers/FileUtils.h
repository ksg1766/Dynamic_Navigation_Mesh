#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

enum FileMode : uint8
{
	Write,
	Read,
};

class ENGINE_DLL FileUtils
{
public:
	FileUtils();
	~FileUtils();

	void Open(wstring filePath, FileMode mode);

	template<typename T>
	void Write(const T& data)
	{
		DWORD numOfBytes = 0;
		assert(::WriteFile(_handle, &data, sizeof(T), (LPDWORD)&numOfBytes, nullptr));
	}

	template<>
	void Write<string>(const string& data)
	{
		return Write(data);
	}

	void Write(void* data, uint32 dataSize);
	void Write(const string& data);

	template<typename T>
	_bool Read(OUT T& data)
	{
		DWORD numOfBytes = 0;

#ifdef _DEBUG
		assert(::ReadFile(_handle, &data, sizeof(T), (LPDWORD)&numOfBytes, nullptr));
#else
		::ReadFile(_handle, &data, sizeof(T), (LPDWORD)&numOfBytes, nullptr);
#endif

		if (0 == numOfBytes)
			return false;
		return true;
	}

	template<typename T>
	T Read()
	{
		T data;
		if (false == Read(data))
			ZeroMemory(&data, sizeof(T));
		return data;
	}

	void Read(void** data, uint32 dataSize);
	_bool Read(OUT string& data);

private:
	HANDLE _handle = INVALID_HANDLE_VALUE;
};

END