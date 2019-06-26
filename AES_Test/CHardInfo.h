
#pragma  once

#include <string>
#include <vector>


class CHardInfo
{
public:
	CHardInfo() = default;
	static std::string GetMachineCode();
	static bool VerifyMachineCode(const std::string strCode);
	static std::string GetPrintabeMaichineCode();
	//cpuid
	static std::string Get_CPU_ID();

	static std::vector<std::string> Get_ALL_MAC_ADDR_ID();
	static std::vector<std::string> Get_ALL_DISK_ID();
	static std::string Get_MotherBoard_ID();

	static std::string GetFormatCPU_ID();
	static std::vector<std::string> GetFormatALL_MAC_ADDR_ID();
	static std::vector<std::string> GetFormatALL_DISK_ID();
	static std::string GetFormatMotherBoard_ID();

	static bool VerifyCPUID(const std::string strCpuID);
	static bool VerifyAllMAC(const std::vector<std::string> strMacAddrArray);
	static bool VerifyDiskID(const std::vector<std::string> strDiskIDArray);
	static bool VerifyMotherBoardID(const std::string strBoardId);
	static std::vector<std::string> MachineCodeToArray(const std::string strMachineCode);
	static std::string  ArrayToMachinCode(const std::vector<std::string> strArray);
	static int GetStringCommonPrefix(const std::string strFirst, const std::string strSecond);
};
