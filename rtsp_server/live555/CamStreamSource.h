
#ifndef __CAMSTREAMFILESOURCE_H__
#define __CAMSTREAMFILESOURCE_H__

#include <mutex>
#include <queue>
#include <memory>
#include <condition_variable>
#include "FramedSource.hh"
//#include "rexkit\BlockingRingBuffer.h"
//#include "rexkit\AutoIncressContent.h"
struct frame_node
{
	unsigned int size;
	char *buffer;
};

class CamStreamSource: public FramedSource
{
public:
    static CamStreamSource* createNew(
        UsageEnvironment& env,
		unsigned clientSessionId,
        void (*callback)(void *handle, void *object), void *handle,
        unsigned preferredFrameSize=0, unsigned playTimePerFrame=0);
    int queueFrame(char *buffer, unsigned int size);
	virtual void doGetNextFrame();
    void driveOut(void);
	bool _stop = false;

public:
    CamStreamSource(UsageEnvironment& env, unsigned clientSessionId, unsigned preferredFrameSize, unsigned playTimePerFrame);
    virtual ~CamStreamSource();

private:
    // 清除回调函数
    void (*m_remove_callback)(void *handle, void *object);
    void *m_handle = NULL;
    virtual unsigned maxFrameSize() const;

private:
    std::mutex m_lock;
	std::queue<struct frame_node> m_queue;
	unsigned m_clientSessionId = 0;
};

#endif /* __CAMSTREAMFILESOURCE_H__ */
