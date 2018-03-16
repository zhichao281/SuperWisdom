#ifndef __DLL_LOADER_H__
#define __DLL_LOADER_H__
#include <Windows.h>
#include <string>
#include <map>

namespace WisdomUtils
{
	class CDllLoader
	{
	public:	
		~CDllLoader(void)
		{
			if (hDll)
			{
				FreeLibrary(hDll);
				hDll = NULL;
			}
		};

		CDllLoader(char* pDllPath) : hDll(NULL)
		{
			hDll = LoadLibraryA(pDllPath);
		}

		//this->GetFunction<void(int, int)>("TestDll");
		template<typename T>
		std::function<T> GetFunction(std::string strFunctionName)
		{
			if (hDll == NULL)
			{
				return NULL;
			}

			auto iter = mapFunctions.find(strFunctionName);
			if (iter == mapFunctions.end())
			{
				mapFunctions[strFunctionName] = GetProcAddress(hDll, strFunctionName.c_str());
				if (mapFunctions[strFunctionName] == NULL)
				{
					return NULL;
				}
			}
			return std::function<T>((T*)mapFunctions[strFunctionName]);
		}

	private:
		HMODULE hDll;
		std::map<std::string, FARPROC> mapFunctions;
	};
}
#endif