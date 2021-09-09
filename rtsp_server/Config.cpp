#include "Config.h"
#include <sstream>
#include <fstream> 
#include <string>
#include "dlog.h"
using namespace ggicci;

Config::Config()
{
}

bool Config::LoadConfig(char* filename)
{
	try {
		ifstream ifs(filename);
		string jsonContent((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));

		Json confJson = Json::Parse(jsonContent.c_str());
		

		if (confJson.Contains("rtsp_server"))
		{
			strRtspServer = confJson["rtsp_server"].AsString();
		}
		else {
			LogError("has not member [rtsp_server]");
			strRtspServer = "rtmp";
		}
		if (confJson.Contains("rtsp_port"))
		{
			RtspPort = confJson["rtsp_port"].AsInt();	
		}
		else {
			LogError("has not member [rtsp_port]");
			RtspPort = 8080;
		}
		

		return true;
	}
	catch (...)
	{
		throw "read config.json error";
	}
	return false;
}

Config::~Config()
{
}
