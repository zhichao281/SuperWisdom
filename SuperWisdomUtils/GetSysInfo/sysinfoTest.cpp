


#include "SRMGPUInfoInf.h"
#include "SRMATIGPUInfo.h"
#include <iostream>
#include "SRMNvidiaGPUInfo.h"
using namespace std;
// Application entry point
int main(int argc, char* argv[])
{

	SRMGPUInfoInf* m_pGPUInfoInf;
	m_pGPUInfoInf = SRMNvidiaGPUInfo::createGPUInfoInf();
	if (!m_pGPUInfoInf) // ATI显卡没有提供显存使用读取接口
		m_pGPUInfoInf = SRMATIGPUInfo::createGPUInfoInf();

	cout << "获取显卡核心的频率：" << m_pGPUInfoInf->getGPUEngineClock() << "MHZ" << endl;
	cout << "获取显卡显存的频率：" << m_pGPUInfoInf->getGPUMemoryClock() << "MHZ" << endl;
	cout << "获取显卡的显存：" << m_pGPUInfoInf->GetGPUMemorySize() << "G" << endl;
	cout << "获取显卡名称：" << m_pGPUInfoInf->getGPUName() << endl;
	system("pause");
	return 0;
}