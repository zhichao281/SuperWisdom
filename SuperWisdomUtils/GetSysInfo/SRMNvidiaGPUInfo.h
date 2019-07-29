#ifndef SRMNVIDIAGPUINFO_H_B9DFEF68_12C7_4604_A025_8E75E8BA74B6
#define SRMNVIDIAGPUINFO_H_B9DFEF68_12C7_4604_A025_8E75E8BA74B6

/********************************************************************
 Nvidia显卡信息，需要用到 nvapi64.lib
 GPU信息读取，这是一个单例
*********************************************************************/
#include <Windows.h>
#include "SRMGPUInfoInf.h"
#include "NvAPI\nvapi.h"



// 类定义
class SRMNvidiaGPUInfo : public SRMGPUInfoInf
{
public:
	static SRMGPUInfoInf* createGPUInfoInf();
public:
	SRMNvidiaGPUInfo();
	~SRMNvidiaGPUInfo();
public:

	//获取显卡的频率
	virtual long getGPUEngineClock() override;

	//获取显存频率
	virtual long  getGPUMemoryClock() override;

	//获取显存
	virtual float   GetGPUMemorySize() override;

	//获取显卡名称
	virtual  std::string getGPUName() override;


	 int getGPUCount() ;
	 long getMemRate(int nIndex);
	 int getTemperature(int nIndex) ;
private:
	void init();
	void unInit();
private:
	bool m_bIsValid;
    NvU32               m_nGPUCount;                            // 显卡数量
    NvPhysicalGpuHandle m_hGPUHandles[NVAPI_MAX_PHYSICAL_GPUS]; // 显卡信息
   	int m_nIndex;
};
#endif //SRMNVIDIAGPUINFO_H_B9DFEF68_12C7_4604_A025_8E75E8BA74B6