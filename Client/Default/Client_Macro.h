#pragma once

#define SAFE_REALLOC(_type, _ptr, _size)							\
	{															\
		if (nullptr == _ptr)									\
		{														\
			_ptr = (_type*)malloc(_size * sizeof(_type));		\
		}														\
		else													\
		{														\
			_ptr = (_type*)realloc(_ptr, _size * sizeof(_type));\
		}														\
	};