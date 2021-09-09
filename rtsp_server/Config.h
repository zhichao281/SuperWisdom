#pragma once
#include <iostream>
#include <string>
#include "jsonla/jsonla.h"

using namespace std;

class Config
{
public:
	Config();
	bool LoadConfig(char* confFile);
	~Config();
public:
	string strRtspServer = "";
	int RtspPort = 8080;
};
