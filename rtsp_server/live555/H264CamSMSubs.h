
#ifndef __H264_CAM_SMSUBS_HH__
#define __H264_CAM_SMSUBS_HH__
#include <memory>
#include <map>
#include "OnDemandServerMediaSubsession.hh"
#include "CamStreamSource.h"

class H264CamSMSubs: public OnDemandServerMediaSubsession
{
public:
    static std::shared_ptr<H264CamSMSubs> createNew(UsageEnvironment& env, Boolean reuseFirstSource);
    static void clientRemove(void *handle, void *object);

    bool deliverFrame(char *buf, unsigned int size/*, int itype*/);
    void driveOut(void);

public:
    H264CamSMSubs(UsageEnvironment& env, Boolean reuseFirstSource);
    // redefined virtual functions
    virtual ~H264CamSMSubs(); // called only by createNew();
    virtual FramedSource* createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate);
    virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource);

private:
    void clientRemove(void *object); 
    std::mutex m_lock;
	unsigned int m_count = 0;
    std::map<unsigned int, CamStreamSource*> m_client;
};

#endif /* __H264_CAM_SMSUBS_HH__ */
