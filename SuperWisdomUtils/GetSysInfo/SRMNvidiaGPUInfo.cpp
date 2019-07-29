
#include "SRMNvidiaGPUInfo.h"
#include <iostream>

using namespace std;

#ifdef  _WIN64
#pragma comment(lib, "GetSysInfo/NVAPI/amd64/nvapi64.lib")
#else
#pragma comment(lib, "GetSysInfo/NVAPI/x86/nvapi.lib")
#endif

int SRMNvidiaGPUInfo::getGPUCount()
{
	return m_nGPUCount;
}
long SRMNvidiaGPUInfo::getMemRate(int nIndex)
{
    if (nIndex >= m_nGPUCount)
        return 0;

    NV_DISPLAY_DRIVER_MEMORY_INFO oMemoryInfo = { 0 };
    oMemoryInfo.version = NV_DISPLAY_DRIVER_MEMORY_INFO_VER_3;
    NvAPI_Status enResult = NvAPI_GPU_GetMemoryInfo(m_hGPUHandles[nIndex], &oMemoryInfo);
    if (enResult == NVAPI_INCOMPATIBLE_STRUCT_VERSION)
    {
        oMemoryInfo.version = NV_DISPLAY_DRIVER_MEMORY_INFO_VER_2;
        enResult = NvAPI_GPU_GetMemoryInfo(m_hGPUHandles[nIndex], &oMemoryInfo);
    }

    if (enResult == NVAPI_INCOMPATIBLE_STRUCT_VERSION)
    {
        oMemoryInfo.version = NV_DISPLAY_DRIVER_MEMORY_INFO_VER_1;
        enResult = NvAPI_GPU_GetMemoryInfo(m_hGPUHandles[nIndex], &oMemoryInfo);
    }

    if (enResult != NVAPI_OK)
        return 0;

	long nTotalMemory = oMemoryInfo.dedicatedVideoMemory;
    int nEmptyMemory = oMemoryInfo.version == NV_DISPLAY_DRIVER_MEMORY_INFO_VER_1 
        ? oMemoryInfo.availableDedicatedVideoMemory : oMemoryInfo.curAvailableDedicatedVideoMemory;


	return nTotalMemory;
}
//获取显卡的频率
long SRMNvidiaGPUInfo::getGPUEngineClock()
{
	NV_GPU_CLOCK_FREQUENCIES clockinfo = { 0 };
	clockinfo.version = NV_GPU_CLOCK_FREQUENCIES_VER;
	clockinfo.ClockType = NV_GPU_CLOCK_FREQUENCIES_BASE_CLOCK;
	NvAPI_GPU_GetAllClockFrequencies(m_hGPUHandles[m_nIndex], &clockinfo);
	long  lFre=	clockinfo.domain[NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS].frequency;
	return lFre / 1000;
}

//获取显存频率
long  SRMNvidiaGPUInfo::getGPUMemoryClock()
{
	NV_GPU_CLOCK_FREQUENCIES clockinfo = { 0 };
	clockinfo.version = NV_GPU_CLOCK_FREQUENCIES_VER;
	clockinfo.ClockType = NV_GPU_CLOCK_FREQUENCIES_BASE_CLOCK;
	NvAPI_GPU_GetAllClockFrequencies(m_hGPUHandles[m_nIndex], &clockinfo);
	long  lFre = clockinfo.domain[NVAPI_GPU_PUBLIC_CLOCK_MEMORY].frequency;

	return lFre / 1000;

}





//获取显存
float   SRMNvidiaGPUInfo::GetGPUMemorySize()
{
	if (m_nIndex >= m_nGPUCount)
		return 0;

	NV_DISPLAY_DRIVER_MEMORY_INFO oMemoryInfo = { 0 };
	oMemoryInfo.version = NV_DISPLAY_DRIVER_MEMORY_INFO_VER_3;
	NvAPI_Status enResult = NvAPI_GPU_GetMemoryInfo(m_hGPUHandles[m_nIndex], &oMemoryInfo);
	if (enResult == NVAPI_INCOMPATIBLE_STRUCT_VERSION)
	{
		oMemoryInfo.version = NV_DISPLAY_DRIVER_MEMORY_INFO_VER_2;
		enResult = NvAPI_GPU_GetMemoryInfo(m_hGPUHandles[m_nIndex], &oMemoryInfo);
	}

	if (enResult == NVAPI_INCOMPATIBLE_STRUCT_VERSION)
	{
		oMemoryInfo.version = NV_DISPLAY_DRIVER_MEMORY_INFO_VER_1;
		enResult = NvAPI_GPU_GetMemoryInfo(m_hGPUHandles[m_nIndex], &oMemoryInfo);
	}

	if (enResult != NVAPI_OK)
		return 0;

	cout << "nTotalMemory" << oMemoryInfo.dedicatedVideoMemory << endl;
	float nTotalMemory = oMemoryInfo.dedicatedVideoMemory / 1024  / 1024.0;

	return nTotalMemory;
}

//获取显卡名称
std::string SRMNvidiaGPUInfo::getGPUName()
{
	NvAPI_ShortString name;
	std::string  strName = "";
	NvAPI_Status enResult = NvAPI_GPU_GetFullName(m_hGPUHandles[m_nIndex], name);
	if (!enResult == NVAPI_OK)
	{
		return strName;
	}
	strName = name;
	return strName;
}


int SRMNvidiaGPUInfo::getTemperature(int nIndex)
{
    if (nIndex >= m_nGPUCount)
        return 0;

    NV_GPU_THERMAL_SETTINGS oThermalSettingsInfo;
    oThermalSettingsInfo.version = NV_GPU_THERMAL_SETTINGS_VER_2;
    NvAPI_Status enResult = NvAPI_GPU_GetThermalSettings(m_hGPUHandles[nIndex], NVAPI_THERMAL_TARGET_ALL, &oThermalSettingsInfo);
    if (enResult == NVAPI_INCOMPATIBLE_STRUCT_VERSION)
    {
        oThermalSettingsInfo.version = NV_GPU_THERMAL_SETTINGS_VER_1;
        NvAPI_Status enResult = NvAPI_GPU_GetThermalSettings(m_hGPUHandles[nIndex], NVAPI_THERMAL_TARGET_ALL, &oThermalSettingsInfo);
    }

    if (enResult != NVAPI_OK)
        return 0;

    return oThermalSettingsInfo.sensor[0].currentTemp;
}

SRMGPUInfoInf* SRMNvidiaGPUInfo::createGPUInfoInf()
{
	static SRMNvidiaGPUInfo s_oSRMGPUInfo;
	if (s_oSRMGPUInfo.m_bIsValid)
		return &s_oSRMGPUInfo;
	else
		return nullptr;
}


//************************************
// Method:    SRMGPUInfo
// FullName:  SRMGPUInfo::SRMGPUInfo
// Access:    private 
// Returns:   
// Qualifier:
// Note:      
//************************************
SRMNvidiaGPUInfo::SRMNvidiaGPUInfo()
    : m_bIsValid(false)
    , m_nGPUCount(0)
{
	m_nIndex = 0;
	init();
}

//************************************
// Method:    ~SRMGPUInfo
// FullName:  SRMGPUInfo::~SRMGPUInfo
// Access:    private 
// Returns:   
// Qualifier:
// Note:      
//************************************
SRMNvidiaGPUInfo::~SRMNvidiaGPUInfo()
{
	unInit();
}

//************************************
// Method:    Init
// FullName:  SRMGPUInfo::Init
// Access:    private 
// Returns:   void
// Qualifier:
// Note:      
//************************************
void SRMNvidiaGPUInfo::init()
{
   

    if (NvAPI_Initialize() != NVAPI_OK)
        return;

    memset(m_hGPUHandles, 0, sizeof(NvPhysicalGpuHandle) * NVAPI_MAX_PHYSICAL_GPUS);
    if (NvAPI_EnumPhysicalGPUs(m_hGPUHandles, &m_nGPUCount) != NVAPI_OK)
        return;

    if (m_nGPUCount <= 0)
        return;

    m_bIsValid = true;
}

//************************************
// Method:    Unit
// FullName:  SRMGPUInfo::Unit
// Access:    private 
// Returns:   void
// Qualifier:
// Note:      
//************************************
void SRMNvidiaGPUInfo::unInit()
{
    m_bIsValid = false;
    m_nGPUCount = 0;
  
}


