#include <Windows.h>
#include <iostream>  
#include <DXGI.h>  
#include <vector>  

using namespace std;
std::string WStringToString(const std::wstring &wstr)
{
	std::string str(wstr.length(), ' ');
	std::copy(wstr.begin(), wstr.end(), str.begin());
	return str;
}
#include "HttpClientLib/HttpClient.h"



int main()
{

	CHttpClient http;

	http.AddReqHeaders(_T("Accept-Language: zh-cn \r\nContent-Type: application/x-www-form-urlencoded\r\n"));
	http.OpenRequest(_T("http://127.0.0.1:9988"), REQ_METHOD_POST);

	CHAR lpData[128] = {"zhe ge shi yi ge ce shi cheng xu"};
	http.SendRequest(lpData, 128);
	http.CloseRequest();

	// 参数定义  
	IDXGIFactory * pFactory;
	IDXGIAdapter * pAdapter;
	std::vector <IDXGIAdapter*> vAdapters;            // 显卡  


	// 显卡的数量  
	int iAdapterNum = 0;


	// 创建一个DXGI工厂  
	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory));

	if (FAILED(hr))
		return -1;

	// 枚举适配器  
	while (pFactory->EnumAdapters(iAdapterNum, &pAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		vAdapters.push_back(pAdapter);
		++iAdapterNum;
	}

	// 信息输出   
	cout << "===============获取到" << iAdapterNum << "块显卡===============" << endl;
	for (size_t i = 0; i < vAdapters.size(); i++)
	{
		// 获取信息  
		DXGI_ADAPTER_DESC adapterDesc;
		vAdapters[i]->GetDesc(&adapterDesc);
		wstring aa(adapterDesc.Description);
		std::string bb = WStringToString(aa);
		// 输出显卡信息  
		cout << "系统视频内存:" << adapterDesc.DedicatedSystemMemory / 1024 / 1024 << "M" << endl;
		cout << "专用视频内存:" << adapterDesc.DedicatedVideoMemory / 1024 / 1024 << "M" << endl;
		cout << "共享系统内存:" << adapterDesc.SharedSystemMemory / 1024 / 1024 << "M" << endl;
		cout << "设备描述:" << bb.c_str() << endl;
		cout << "设备ID:" << adapterDesc.DeviceId << endl;
		cout << "PCI ID修正版本:" << adapterDesc.Revision << endl;
		cout << "子系统PIC ID:" << adapterDesc.SubSysId << endl;
		cout << "厂商编号:" << adapterDesc.VendorId << endl;

		// 输出设备  
		IDXGIOutput * pOutput;
		std::vector<IDXGIOutput*> vOutputs;
		// 输出设备数量  
		int iOutputNum = 0;
		while (vAdapters[i]->EnumOutputs(iOutputNum, &pOutput) != DXGI_ERROR_NOT_FOUND)
		{
			vOutputs.push_back(pOutput);
			iOutputNum++;
		}

		cout << "-----------------------------------------" << endl;
		cout << "获取到" << iOutputNum << "个显示设备:" << endl;
		cout << endl;

		for (size_t n = 0; n < vOutputs.size(); n++)
		{
			// 获取显示设备信息  
			DXGI_OUTPUT_DESC outputDesc;
			vOutputs[n]->GetDesc(&outputDesc);

			// 获取设备支持  
			UINT uModeNum = 0;
			DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
			UINT flags = DXGI_ENUM_MODES_INTERLACED;

			vOutputs[n]->GetDisplayModeList(format, flags, &uModeNum, 0);
			DXGI_MODE_DESC * pModeDescs = new DXGI_MODE_DESC[uModeNum];
			vOutputs[n]->GetDisplayModeList(format, flags, &uModeNum, pModeDescs);

			cout << "显示设备名称:" << outputDesc.DeviceName << endl;
			cout << "显示设备当前分辨率:" << outputDesc.DesktopCoordinates.right - outputDesc.DesktopCoordinates.left << "*" << outputDesc.DesktopCoordinates.bottom - outputDesc.DesktopCoordinates.top << endl;
			cout << endl;

			// 所支持的分辨率信息  
			cout << "分辨率信息:" << endl;
			for (UINT m = 0; m < uModeNum; m++)
			{
				cout << "== 分辨率:" << pModeDescs[m].Width << "*" << pModeDescs[m].Height << "     刷新率" << (pModeDescs[m].RefreshRate.Numerator) / (pModeDescs[m].RefreshRate.Denominator) << endl;
			}
		}
		vOutputs.clear();

	}
	vAdapters.clear();

	system("pause");
	return 0;
}