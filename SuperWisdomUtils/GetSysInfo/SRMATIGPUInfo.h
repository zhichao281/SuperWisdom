#ifndef SRMATIGPUINFO_H_8ADE5879_B99F_46F9_BC02_D750992626DB
#define SRMATIGPUINFO_H_8ADE5879_B99F_46F9_BC02_D750992626DB
/************************************************************************/
/* ATI�Կ���Ϣ 
���� atiadlxx.dll
���� atiadlxx.dll �� atiadlxy.dll:
xx��Զ������ϵͳ����һ�µġ�32λϵͳ�ϣ�xx����32λ��64λϵͳ�ϣ�xx����64λ��
xy����;�ǣ�64λϵͳ�ϣ�����32λ����ʱ��xxû���ã�����32λ��xy��
����ȼ���xx�����ʧ���ˣ��ټ���xy
ADL û���ṩ�Դ���Ϣ��ȡ�ӿ�
*/
/************************************************************************/
#include "ADL\adl_sdk.h"
#include "SRMGPUInfoInf.h"

#include <iostream>

#include <windows.h>
using namespace std;

#define MAX_ADAPTER_NUMBER 64

// API����
typedef int (*ADL_MAIN_CONTROL_CREATE) (ADL_MAIN_MALLOC_CALLBACK, int);
typedef int (*ADL_MAIN_CONTROL_DESTROY) ();
typedef int (*ADL_ADAPTER_NUMBEROFADAPTERS_GET) (int*);
typedef int (*ADL_ADAPTER_ADAPTERINFO_GET) (LPAdapterInfo, int);
typedef int (*ADL_DISPLAY_DISPLAYINFO_GET)	(int, int*, ADLDisplayInfo**, int);
typedef int (*ADL_OVERDRIVE5_TEMPERATURE_GET) (int, int, ADLTemperature*);
typedef int (*ADL_OVERDRIVE5_CURRENTACTIVITY_GET) (int, ADLPMActivity*);
typedef int (*ADL_ADAPTER_MEMORYINFO_GET) (int, ADLMemoryInfo*);


typedef int(*ADL_Display_ODClockInfo_Get) (int, ADLAdapterODClockInfo*);



//typedef int (*ADL_DISPLAY_COLORCAPS_GET) (int, int, int*, int*);
//typedef int (*ADL_DISPLAY_COLOR_GET) (int, int, int, int*, int*, int*, int*, int*);
//typedef int (*ADL_DISPLAY_COLOR_SET) (int, int, int, int);
//typedef int (*ADL_OVERDRIVE5_FANSPEED_GET) (int, int, ADLFanSpeedValue*);

class SRMATIGPUInfo : public SRMGPUInfoInf
{

public:
	static SRMGPUInfoInf* createGPUInfoInf();

public:
	SRMATIGPUInfo();
	~SRMATIGPUInfo();
     int getGPUCount() ;
     int getGPURate(int nIndex) ;
	 long long getMemRate(int nIndex);



     int getTemperature(int nIndex) ;

	 //��ȡ�Կ���Ƶ��
	 long getGPUEngineClock();

	 //��ȡ�Դ�Ƶ��
	 long  getGPUMemoryClock();

	 //��ȡ�Դ�
	 float   GetGPUMemorySize();

	 //��ȡ�Կ�����
	 std::string getGPUName();



private:

    void init();
    void unInit();

private:// API�ӿ�ָ��
    ADL_MAIN_CONTROL_CREATE				m_pFun_ADL_Main_Control_Create;
    ADL_MAIN_CONTROL_DESTROY			m_pFun_ADL_Main_Control_Destroy;
    ADL_ADAPTER_NUMBEROFADAPTERS_GET	m_pFun_ADL_Adapter_NumberOfAdapters_Get;
    ADL_ADAPTER_ADAPTERINFO_GET		    m_pFun_ADL_Adapter_AdapterInfo_Get;
    ADL_OVERDRIVE5_CURRENTACTIVITY_GET	m_pFun_ADL_Overdrive5_CurrentActivity_Get;
    ADL_OVERDRIVE5_TEMPERATURE_GET		m_pFun_ADL_Overdrive5_Temperature_Get;
    ADL_ADAPTER_MEMORYINFO_GET          m_pFun_ADL_Adapter_MemoryInfo_Get;
	ADL_Display_ODClockInfo_Get          m_pFun_ADL_Display_ODClockInfo_Get;

    //ADL_DISPLAY_COLORCAPS_GET			m_pFun_ADL_Display_ColorCaps_Get;
    //ADL_DISPLAY_COLOR_GET				m_pFun_ADL_Display_Color_Get;
    //ADL_DISPLAY_COLOR_SET				m_pFun_ADL_Display_Color_Set;
    //ADL_DISPLAY_DISPLAYINFO_GET		    m_pFun_ADL_Display_DisplayInfo_Get;    
    //ADL_OVERDRIVE5_FANSPEED_GET			m_pFun_ADL_Overdrive5_FanSpeed_Get;

private:
    bool m_bIsValid;
    int m_nAdaptersNumber;                           // �Կ�����
    AdapterInfo m_oAdapterInfos[MAX_ADAPTER_NUMBER]; // �Կ���Ϣ
    HINSTANCE m_hATIApiDll;                          // API dll ���
	int m_nIndex;

};
#endif //SRMATIGPUINFO_H_8ADE5879_B99F_46F9_BC02_D750992626DB