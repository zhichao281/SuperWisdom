#define DOCTEST_CONFIG_IMPLEMENTS_WITH_MAIN
#include "doctest/doctest.h"
#include "CHardInfo.h"

TEST_CASE("VerifyMachineCode"){
    for(int i = 0 ; i < 2 ; ++i)
    {
        std::string strMachineCode=CHardInfo::GetMachineCode();
        CHECK(CHardInfo::VerifyMachineCode(strMachineCode));
    }
}

TEST_CASE("VerifyCPUID"){
    for(int i = 0 ; i < 2 ; ++i)
    {
        std::string strMachineCode=CHardInfo::GetFormatCPU_ID();
        CHECK(CHardInfo::VerifyCPUID(strMachineCode));
    }
}


TEST_CASE("VerifyMacAddr"){
    for(int i = 0 ; i < 2 ; ++i)
    {
        std::vector<std::string> macAddrs =CHardInfo::GetFormatALL_MAC_ADDR_ID();
        CHECK(CHardInfo::VerifyAllMAC(macAddrs));
    }
}

TEST_CASE("VerifyDiskID") {
	for (int i = 0; i < 2; ++i)
	{
		std::vector<std::string> diskIds = CHardInfo::GetFormatALL_DISK_ID();
		CHECK(CHardInfo::VerifyDiskID(diskIds));
	}
}

TEST_CASE("VerifyMainBoard") {
	for (int i = 0; i < 2; ++i)
	{
		std::string mainBoardID = CHardInfo::GetFormatMotherBoard_ID();
		CHECK(CHardInfo::VerifyMotherBoardID(mainBoardID));
	}
}


