#ifndef __AUTO_LOCK_CLASS_H__
#define __AUTO_LOCK_CLASS_H__

#include <Windows.h>

namespace WisdomUtils
{
	class _KEY_CLASS_
	{
	public:
		_KEY_CLASS_()
		{
			InitializeCriticalSection(&_cs);
		};
		~_KEY_CLASS_()
		{
			DeleteCriticalSection(&_cs);
		};
		CRITICAL_SECTION _cs;
	};

	class _AUTO_LOCK_CLASS_
	{
	public:
		_AUTO_LOCK_CLASS_(_KEY_CLASS_& key) : _key(key)
		{
			EnterCriticalSection(&(_key._cs));
		};
		~_AUTO_LOCK_CLASS_()
		{
			LeaveCriticalSection(&(_key._cs));
		};
		_KEY_CLASS_& _key;
	};
}

#define _KEY_ WisdomUtils::_KEY_CLASS_ m_AutoLockKey;
#define _LOCK_ WisdomUtils::_AUTO_LOCK_CLASS_ _auto_lock_(m_AutoLockKey);
#define _AUTO_LOCK_CLASS WisdomUtils::_KEY_CLASS_ m_AutoLockKey;
#endif