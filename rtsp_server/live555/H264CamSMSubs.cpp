#include <memory>
#include "H264CamSMSubs.h"
#include "H264VideoRTPSink.hh"
#include "ByteStreamFileSource.hh"
#include "H264VideoStreamFramer.hh"
#include "../dlog.h"


H264CamSMSubs::H264CamSMSubs(UsageEnvironment& env, Boolean reuseFirstSource)
  : OnDemandServerMediaSubsession(env, reuseFirstSource)
{

}

H264CamSMSubs::~H264CamSMSubs()
{
    driveOut();
}

std::shared_ptr<H264CamSMSubs> H264CamSMSubs::createNew(UsageEnvironment& env, Boolean reuseFirstSource)
{
	LogInfo("new H264CamSMSubs");
	std::shared_ptr<H264CamSMSubs> cam = std::make_shared<H264CamSMSubs>(env, reuseFirstSource);
    return cam;
}

void H264CamSMSubs::clientRemove(void *object)
{
    m_lock.lock();
    if(0 < m_count){
        for(auto iter = m_client.begin(); iter != m_client.end(); iter++){
            if(iter->second == object){
                m_client.erase(iter);
                m_count--;
                break;
            }
        }
    }
    m_lock.unlock();
}

void H264CamSMSubs::clientRemove(void *handle, void *object)
{
    H264CamSMSubs *pClass = (H264CamSMSubs *)handle;
    if(NULL != pClass){
        pClass->clientRemove(object);
    }
}

/* 
 * 有新的客户端连接
 */
FramedSource* H264CamSMSubs::createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate)
{
    CamStreamSource *source = NULL;

	LogInfo("Create the video source: clientSessionId: %ld", clientSessionId);
    m_lock.lock();
    if(m_count < 5){
        auto it = m_client.find(clientSessionId);
        if(it == m_client.end()) {
            estBitrate = 500;
            source = CamStreamSource::createNew(envir(), clientSessionId, H264CamSMSubs::clientRemove, this);
            if(NULL != source){
                m_client[clientSessionId] = source;
                m_count++;
            }else{
				LogError("CamStreamSource::createNew failed");
            }
        }else{
			LogInfo("Exist client clientSessionId:%d", clientSessionId);
            source = m_client[clientSessionId];
            estBitrate = 500;
        }
    }
    m_lock.unlock();
    if(NULL != source){
        return H264VideoStreamFramer::createNew(envir(), source);
    }else{
        LogError("createNewStreamSource failed");
        return NULL;
    }
}

RTPSink* H264CamSMSubs::createNewRTPSink(
	Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* /*inputSource*/)
{
	LogInfo("%s, rtpPayloadTypeIfDynamic:%d", __func__, rtpPayloadTypeIfDynamic);
	OutPacketBuffer::maxSize = 600 * 1000;  //100000;//  RTP输出的Buffer
    return H264VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
}

bool H264CamSMSubs::deliverFrame(char *buf, unsigned int size/*, int itype*/)
{
	//LogInfo("deliverFrame len=%d", size);
    if(NULL == buf && size <= 0){
		LogInfo("buf is Null or sizt <= 0");
        return false;
    }
	bool hasClient = false;
    // 所有客户端都发送
    m_lock.lock();
	if (m_client.size() > 0) {
		for (auto iter = m_client.begin(); iter != m_client.end(); iter++) {
			auto source = iter->second;
			source->queueFrame(buf, size);
			hasClient = true;
		}
	}
    m_lock.unlock();
	return hasClient;
}

void H264CamSMSubs::driveOut(void)
{
	LogInfo("%s", __func__);
    m_lock.lock();
    for(auto iter = m_client.begin(); iter != m_client.end(); ) {
        iter->second->driveOut();
		m_client.erase(iter++);
    }
    m_lock.unlock();
}

