



#include "SRMATIGPUInfo.h"

// 
void* __stdcall ADL_Main_Memory_Alloc(int nSize)
{
    void* pBuffer = malloc(nSize);
    return pBuffer;
}

void __stdcall ADL_Main_Memory_Free(void** ppBuffer)
{
    if (*ppBuffer != nullptr)
    {
        free(*ppBuffer);
        *ppBuffer = nullptr;
    }
}

int SRMATIGPUInfo::getGPUCount()
{
    return m_nAdaptersNumber;
}

int SRMATIGPUInfo::getGPURate(int nIndex)
{
    if (nIndex >= m_nAdaptersNumber)
        return 0;

    int iAdapterIndex = m_oAdapterInfos[nIndex].iAdapterIndex;

    ADLPMActivity m_ADLPMActivity;
    if (m_pFun_ADL_Overdrive5_CurrentActivity_Get(iAdapterIndex, &m_ADLPMActivity) != ADL_OK)
        return 0;
    return m_ADLPMActivity.iActivityPercent;
}




//获取显卡的频率
long SRMATIGPUInfo::getGPUEngineClock()
{

	if (m_nIndex >= m_nAdaptersNumber)
		return 0;

	long  nRes = 0;
	
	int iAdapterIndex = m_oAdapterInfos[m_nIndex].iAdapterIndex;

	ADLAdapterODClockInfo m_ADLPMActivity;
	if (m_pFun_ADL_Display_ODClockInfo_Get(iAdapterIndex, &m_ADLPMActivity) != ADL_OK)
		return 0;

	nRes = m_ADLPMActivity.sEngineClock.iCurrentClock / 100;
	return   nRes;


	
}

//获取显存频率
long  SRMATIGPUInfo::getGPUMemoryClock()
{
	if (m_nIndex >= m_nAdaptersNumber)
		return 0;

	long  nRes = 0;

	int iAdapterIndex = m_oAdapterInfos[m_nIndex].iAdapterIndex;

	ADLAdapterODClockInfo m_ADLPMActivity;
	if (m_pFun_ADL_Display_ODClockInfo_Get(iAdapterIndex, &m_ADLPMActivity) != ADL_OK)
		return 0;

	nRes = m_ADLPMActivity.sMemoryClock.iCurrentClock / 100;
	return   nRes;
}

//获取显存
float   SRMATIGPUInfo::GetGPUMemorySize()
{
	if (m_nIndex >= m_nAdaptersNumber)
		return 0;

	int iAdapterIndex = m_oAdapterInfos[m_nIndex].iAdapterIndex;
	
	ADLMemoryInfo oADLMemoryInfo;
	if (m_pFun_ADL_Adapter_MemoryInfo_Get(iAdapterIndex, &oADLMemoryInfo) != ADL_OK)
		return 0;

	float   iMemorySize = oADLMemoryInfo.iMemorySize/1024/1024/1024.0;
	return  iMemorySize;
}

//获取显卡名称
std::string SRMATIGPUInfo::getGPUName()
{
	std::string strName= m_oAdapterInfos[m_nIndex].strAdapterName ;
	return strName;

}
SRMGPUInfoInf* SRMATIGPUInfo::createGPUInfoInf()
{
	static SRMATIGPUInfo s_oSRMGPUInfo;
	if (s_oSRMGPUInfo.m_bIsValid)
		return &s_oSRMGPUInfo;
	else
		return nullptr;
}


//************************************
// Remark:    ATI显卡没有提供显存使用读取接口
// Method:    getMemRate
// Access:    virtual public 
// Returns:   int
// Qualifier:
// Parameter: int nIndex
//************************************
long long SRMATIGPUInfo::getMemRate(int nIndex)
{
    if (nIndex >= m_nAdaptersNumber)
        return 0;

    int iAdapterIndex = m_oAdapterInfos[nIndex].iAdapterIndex;
    ADLMemoryInfo oADLMemoryInfo;
    if (m_pFun_ADL_Adapter_MemoryInfo_Get(iAdapterIndex, &oADLMemoryInfo) != ADL_OK)
        return -1;

    return  oADLMemoryInfo.iMemorySize ;
}

int SRMATIGPUInfo::getTemperature(int nIndex)
{
    if (nIndex >= m_nAdaptersNumber)
        return 0;

    int iAdapterIndex = m_oAdapterInfos[nIndex].iAdapterIndex;
    ADLTemperature oADLTemperature;
    if (m_pFun_ADL_Overdrive5_Temperature_Get(iAdapterIndex, 0, &oADLTemperature) != ADL_OK)
        return 0;
    
    return oADLTemperature.iTemperature / 1000;
}



SRMATIGPUInfo::SRMATIGPUInfo()
    : m_bIsValid(false)
    , m_nAdaptersNumber(0)
    , m_hATIApiDll(nullptr)
    , m_pFun_ADL_Main_Control_Create(nullptr)
    , m_pFun_ADL_Main_Control_Destroy(nullptr)
    , m_pFun_ADL_Adapter_NumberOfAdapters_Get(nullptr)
    , m_pFun_ADL_Adapter_AdapterInfo_Get(nullptr)
    , m_pFun_ADL_Overdrive5_CurrentActivity_Get(nullptr)
    , m_pFun_ADL_Overdrive5_Temperature_Get(nullptr)
    , m_pFun_ADL_Adapter_MemoryInfo_Get(nullptr)
{
    init();
}

SRMATIGPUInfo::~SRMATIGPUInfo()
{
    unInit();
}

void SRMATIGPUInfo::init()
{
    m_hATIApiDll = LoadLibrary(L"atiadlxx.dll");
	if (!m_hATIApiDll)
	{
		// A 32 bit calling application on 64 bit OS will fail to LoadLIbrary.
		// Try to load the 32 bit library (atiadlxy.dll) instead
		m_hATIApiDll = LoadLibrary(L"atiadlxy.dll");
		if (!m_hATIApiDll)
		{
			return;
		}
	}
    

    // APIs initialization
    m_pFun_ADL_Main_Control_Create = (ADL_MAIN_CONTROL_CREATE)GetProcAddress(m_hATIApiDll, "ADL_Main_Control_Create");
    m_pFun_ADL_Main_Control_Destroy = (ADL_MAIN_CONTROL_DESTROY)GetProcAddress(m_hATIApiDll, "ADL_Main_Control_Destroy");
    m_pFun_ADL_Adapter_NumberOfAdapters_Get = (ADL_ADAPTER_NUMBEROFADAPTERS_GET)GetProcAddress(m_hATIApiDll, "ADL_Adapter_NumberOfAdapters_Get");
    m_pFun_ADL_Overdrive5_Temperature_Get = (ADL_OVERDRIVE5_TEMPERATURE_GET)GetProcAddress(m_hATIApiDll, "ADL_Overdrive5_Temperature_Get");
    m_pFun_ADL_Overdrive5_CurrentActivity_Get = (ADL_OVERDRIVE5_CURRENTACTIVITY_GET)GetProcAddress(m_hATIApiDll, "ADL_Overdrive5_CurrentActivity_Get");
    m_pFun_ADL_Adapter_AdapterInfo_Get = (ADL_ADAPTER_ADAPTERINFO_GET)GetProcAddress(m_hATIApiDll, "ADL_Adapter_AdapterInfo_Get");
	m_pFun_ADL_Adapter_MemoryInfo_Get = (ADL_ADAPTER_MEMORYINFO_GET)GetProcAddress(m_hATIApiDll, "ADL_Adapter_MemoryInfo_Get");
	m_pFun_ADL_Display_ODClockInfo_Get = (ADL_Display_ODClockInfo_Get)GetProcAddress(m_hATIApiDll, "ADL_Display_ODClockInfo_Get");

	
	//m_pFun_ADL_Overdrive5_FanSpeed_Get = (ADL_OVERDRIVE5_FANSPEED_GET)GetProcAddress(m_hATIApiDll, "ADL_Overdrive5_FanSpeed_Get");
    //m_pFun_ADL_Display_ColorCaps_Get = (ADL_DISPLAY_COLORCAPS_GET)GetProcAddress(m_hATIApiDll, "ADL_Display_ColorCaps_Get");
    //m_pFun_ADL_Display_Color_Get = (ADL_DISPLAY_COLOR_GET)GetProcAddress(m_hATIApiDll, "ADL_Display_Color_Get");
    //m_pFun_ADL_Display_Color_Set = (ADL_DISPLAY_COLOR_SET)GetProcAddress(m_hATIApiDll, "ADL_Display_Color_Set");
    //m_pFun_ADL_Display_DisplayInfo_Get = (ADL_DISPLAY_DISPLAYINFO_GET)GetProcAddress(m_hATIApiDll, "ADL_Display_DisplayInfo_Get");

    if (!m_pFun_ADL_Main_Control_Create
        || !m_pFun_ADL_Main_Control_Destroy
        || !m_pFun_ADL_Adapter_NumberOfAdapters_Get        
        || !m_pFun_ADL_Overdrive5_Temperature_Get 
        || !m_pFun_ADL_Overdrive5_CurrentActivity_Get
        || !m_pFun_ADL_Adapter_AdapterInfo_Get
        || !m_pFun_ADL_Adapter_MemoryInfo_Get
        //|| !m_pFun_ADL_Overdrive5_FanSpeed_Get
        //|| !m_pFun_ADL_Display_ColorCaps_Get
        //|| !m_pFun_ADL_Display_Color_Get
        //|| !m_pFun_ADL_Display_Color_Set
        //|| !m_pFun_ADL_Display_DisplayInfo_Get
        )
        return;

    if (m_pFun_ADL_Main_Control_Create(static_cast<ADL_MAIN_MALLOC_CALLBACK>(&ADL_Main_Memory_Alloc), 1) != ADL_OK)
        return;

    if (m_pFun_ADL_Adapter_NumberOfAdapters_Get(&m_nAdaptersNumber) != ADL_OK)
        return;

    if (m_nAdaptersNumber <= 0)
        return;

    if (m_pFun_ADL_Adapter_AdapterInfo_Get(m_oAdapterInfos, sizeof(AdapterInfo) * m_nAdaptersNumber) != ADL_OK)
        return;

    m_bIsValid = true;
}

void SRMATIGPUInfo::unInit()
{
    m_bIsValid = false;
    m_nAdaptersNumber = 0;

    if (m_hATIApiDll)
    {
        FreeLibrary(m_hATIApiDll);
        m_hATIApiDll = nullptr;
    }
}

