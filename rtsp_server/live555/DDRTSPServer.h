#ifndef __MY_RTSP_SERVER_H__
#define __MY_RTSP_SERVER_H__

#include <string.h>
#include <stdio.h>
#include <string>
#include <map>
#include <memory>
#include "RTSPServer.hh"
#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "H264CamSMSubs.h"

class DDRTSPServer: public RTSPServer 
{
public:
    static std::shared_ptr<DDRTSPServer> getObject(UsageEnvironment& env, Port port, UserAuthenticationDatabase* authDb=NULL, unsigned int resec=65);
    ~DDRTSPServer();

public:
	std::shared_ptr<H264CamSMSubs> createSession(char* stream_name);
	void removeSession(const char* stream_name);
    DDRTSPServer(UsageEnvironment& env, int fd, Port port, UserAuthenticationDatabase* auth, unsigned resec);
	

protected: // redefined virtual functions
    virtual ServerMediaSession* lookupServerMediaSession(char const* streamName, Boolean isFirstLookupInSession);

private:
	//std::map<std::string, H264CamSMSubs*> mSessionMap;
};
extern std::shared_ptr<DDRTSPServer> g_RtspServer;
#endif /* __MY_RTSP_SERVER_H__ */
