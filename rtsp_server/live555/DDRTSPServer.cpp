
#include "DDRTSPServer.h"
#include "../dlog.h"
#include "NetCommon.h"
#include "../Singleton.hpp"

DDRTSPServer::DDRTSPServer(UsageEnvironment& env, int fd, Port port, UserAuthenticationDatabase* auth, unsigned resec)
    : RTSPServer(env, fd, port, auth, resec)
{
	
}

DDRTSPServer::~DDRTSPServer()
{
    // 清空所有的ServerMediaSession
	// for (auto itr = mSessionMap.begin(); itr != mSessionMap.end(); itr++)
	// {
	// 	closeAllClientSessionsForServerMediaSession(itr->first.c_str());
	// 	if (itr->second) {
	// 		itr->second->driveOut();
	// 		delete itr->second;
	// 		itr->second = NULL;
	// 	}
	// }
}

//创建拉流对象
std::shared_ptr<H264CamSMSubs> DDRTSPServer::createSession(char* stream_name)
{
    std::shared_ptr<H264CamSMSubs> newcam;
    ServerMediaSession* sms = NULL;
    
    // 如果存在先清除
    sms = RTSPServer::lookupServerMediaSession(stream_name);
    if (sms != NULL){
        removeServerMediaSession(sms);
        sms = NULL;
    }
    // 创建
    if (sms == NULL)
    {
        sms = ServerMediaSession::createNew(envir(), stream_name, NULL, "Session Streamed By Camera");
        if(sms == NULL){
            LogError("ServerMediaSession::createNew failed. %s", envir().getResultMsg());
            goto out;
        }
        newcam = H264CamSMSubs::createNew(envir(), false);
        if(NULL == newcam){
			LogError("H264CamSMSubs::createNew failed. %s", envir().getResultMsg());
            goto out1;
        }
        sms->addSubsession(newcam.get());
        addServerMediaSession(sms);
        if(sms->numSubsessions() <= 0){
			LogError("rtsp addSubsession failed. %s", envir().getResultMsg());
            goto out1;
        }
        LogInfo("rtspURL: %s", rtspURL(sms));
    }
    return newcam;

out1:
    if(sms != NULL){
        deleteServerMediaSession(sms);
    }
out:
    return NULL;
}
void DDRTSPServer::removeSession(const char* stream_name)
{
	ServerMediaSession* sms = NULL;
	sms = RTSPServer::lookupServerMediaSession(stream_name);
	if (sms != NULL) {
		deleteServerMediaSession(sms);
		sms = NULL;
	}
}
ServerMediaSession* DDRTSPServer::lookupServerMediaSession(char const* streamName, Boolean isFirstLookupInSession)
{
    //LogWarn("lookupServerMediaSession: %d, %s ", isFirstLookupInSession, streamName);
    ServerMediaSession* sms = RTSPServer::lookupServerMediaSession(streamName);
    if (sms != NULL){
		LogInfo("Exist ServerMediaSession: %d, %s", isFirstLookupInSession, streamName);
    }else{
		LogInfo("No exist ServerMediaSession: %d, %s", isFirstLookupInSession, streamName);
    }
    return sms;
}

std::shared_ptr<DDRTSPServer> DDRTSPServer::getObject(UsageEnvironment& env, Port port, UserAuthenticationDatabase* auth, unsigned int resec)
{
     int fd = -1;

    fd = setUpOurSocket(env, port);
    if (fd == -1){
		LogError("setUpOurSocket fail, Port:%d ==> %d", port.num(), ntohs(port.num()));
		return NULL;
    }
    std::shared_ptr<DDRTSPServer> obj = std::make_shared<DDRTSPServer>(env,fd,port,auth,resec);
    //Singleton<DDRTSPServer>::Instance(env, fd, port, authDb, resec);
    return obj;
}