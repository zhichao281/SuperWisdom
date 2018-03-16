#ifndef __AUTO_LOCK_MACRO_H__
#define __AUTO_LOCK_MACRO_H__

#include <Windows.h>

class _CriticalSectionObject{
public:
	_CriticalSectionObject(){
		memset(&m_cs, 0, sizeof(CRITICAL_SECTION));
		InitializeCriticalSection(&m_cs);
	};
	~_CriticalSectionObject(){
		DeleteCriticalSection(&m_cs);
	};
	void Lock(){
		EnterCriticalSection(&m_cs);
	};
	void UnLock(){
		LeaveCriticalSection(&m_cs);
	};
	CRITICAL_SECTION m_cs;
};

#define IMPLEMENT_LOCKING(class_name) \
class AutoLock{\
public:\
	AutoLock(class_name* _baseClass) : m_baseClass(_baseClass){\
	m_baseClass->Lock();\
};\
	~AutoLock(){\
	m_baseClass->UnLock();\
};\
	class_name* m_baseClass;\
};\
	_CriticalSectionObject _LockObject;\
	void Lock(){\
	_LockObject.Lock();\
};\
	void UnLock(){\
	_LockObject.UnLock();\
};

#define AUTOLOCK AutoLock _lock(this);
#endif