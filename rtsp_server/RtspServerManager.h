#include <iostream>
#include <map>
#include <memory>
class H264CamSMSubs;
class DDRTSPServer;
class RtspServerManager{
public:
    RtspServerManager(std::shared_ptr<DDRTSPServer> p);
    ~RtspServerManager()=default;

    bool CreateSession(std::string strId);
    void PushPacket(std::string strId,char* data,size_t size);
    void RemoveSession(std::string strId);
private:
    std::map<std::string,std::shared_ptr<H264CamSMSubs>> _sessionMap;
    std::shared_ptr<DDRTSPServer> _rtspServer;
};
