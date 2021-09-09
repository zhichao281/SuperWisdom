#include "RtspServerManager.h"
#include "H264CamSMSubs.h"
#include "DDRTSPServer.h"


RtspServerManager::RtspServerManager(std::shared_ptr<DDRTSPServer> p)
{
    _rtspServer = p;
    _sessionMap.clear();
}
bool RtspServerManager::CreateSession(std::string strId)
{
    std::shared_ptr<H264CamSMSubs> pCam = _rtspServer->createSession((char*)(strId.c_str()));
    if(pCam == nullptr){
        return false;
    }
    
    _sessionMap.insert(std::make_pair(strId,pCam));
    return true;   
}

void RtspServerManager::PushPacket(std::string strId,char* data,size_t size)
{
    auto itr = _sessionMap.find(strId);
    if(itr == _sessionMap.end())
    {
        return;
    }
    itr->second->deliverFrame(data,size);
}

void RtspServerManager::RemoveSession(std::string strId)
{
    _rtspServer->removeSession(strId.c_str());
    auto itr = _sessionMap.find(strId);
    if(itr == _sessionMap.end())
    {
        return;
    }
    _sessionMap.erase(itr);
}