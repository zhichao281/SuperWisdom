#include "CamStreamSource.h"
#include "InputFile.hh"
#include "GroupsockHelper.hh"
#include "../dlog.h"
#include <thread>

using namespace std;

CamStreamSource* CamStreamSource::createNew(
    UsageEnvironment& env,
	unsigned clientSessionId,
    void (*callback)(void *handle, void *object), void *handle,
    unsigned preferredFrameSize, unsigned playTimePerFrame)
{
    LogInfo("%s", __func__);
	CamStreamSource* obj = new CamStreamSource(env, clientSessionId, preferredFrameSize, playTimePerFrame);
    if(obj == NULL){
        return NULL;
    }
    obj->m_remove_callback = callback;
    obj->m_handle = handle;
    return obj;
out:
	return NULL;
}

CamStreamSource::CamStreamSource(UsageEnvironment& env, unsigned clientSessionId, unsigned preferredFrameSize, unsigned playTimePerFrame)
  : FramedSource(env)
{
	LogInfo("%s", __func__);
    m_remove_callback = NULL;
    m_handle = NULL;
	m_clientSessionId = clientSessionId;
	_stop = false; 
}

CamStreamSource::~CamStreamSource()
{
	LogInfo("%s", __func__);
    if(NULL != m_remove_callback && NULL != m_handle){
        m_remove_callback(m_handle, this);
    }
	_stop = true;
    driveOut();
}

void CamStreamSource::driveOut(void)
{
	LogInfo("CamStreamSource::%s", __func__);
}

// 根据实际情况设置一个合理的值
unsigned CamStreamSource::maxFrameSize() const
{
	return 600 * 1000;//100000;// 
}

/*
 * 把数据放入队列
 */
int CamStreamSource::queueFrame(char *buffer, unsigned int size)
{
    // log_i("CamStreamSource::%s", __func__);
	int ret = -1;
    if(NULL == buffer || size <= 0){
		LogError("%s, NULL == buffer || size <= 0", __func__);
        return -1;
    }
	{
		std::lock_guard<std::mutex> lock(m_lock);
		//if(size > 30000)
		//LogInfo("buffer size=%d",size);
		// if (m_queue.size() > 200) {
		// 	return -1;
		// }
		frame_node node;
		memset(&node, 0, sizeof(node));
		node.buffer = new char[size];
		node.size = size;
		memcpy(node.buffer, buffer, size);
		m_queue.push(node);
	}
	return 0;
}

static void mydoGetNextFrame(void * p) 
{
	CamStreamSource * source = (CamStreamSource *)p;
	source->doGetNextFrame();
	//LogInfo("mydoGetNextFrame");
}

/*
 * 从队列取出帧并发送,应当建立一个线程
 */
void CamStreamSource::doGetNextFrame() 
{
	if (_stop) {
		return;
	}
	bool no_data = false;
	frame_node node;
	do{
		std::lock_guard<std::mutex> lock(m_lock);
		if (m_queue.size() <= 0) {
			no_data = true;
			break;
		}
		node = m_queue.front();
		m_queue.pop();
	} while (0);
	if (no_data) {
		//LogInfo("no_data");
		nextTask() = envir().taskScheduler().scheduleDelayedTask(50, (TaskFunc*)mydoGetNextFrame, this);
		return;
	}
	if ((0 < node.size) && (NULL != node.buffer)) {
		//LogInfo("has_data,size=%d",node.size);
		gettimeofday(&fPresentationTime, NULL);
		fFrameSize = node.size;
		if (fFrameSize > fMaxSize) {
			fNumTruncatedBytes = fFrameSize - fMaxSize;
			fFrameSize = fMaxSize;
			//LogInfo("fNumTruncatedBytes:%d, fMaxSize: %d, m_clientSessionId:%u", fNumTruncatedBytes, fMaxSize, m_clientSessionId);
		}
		else {
			fNumTruncatedBytes = 0;
		}
		memmove(fTo, node.buffer, fFrameSize);
		nextTask() = envir().taskScheduler().scheduleDelayedTask(0, (TaskFunc*)FramedSource::afterGetting, this);
		delete[] node.buffer;
		node.buffer = NULL;
	}
	else {
		//LogInfo("node = null");
		nextTask() = envir().taskScheduler().scheduleDelayedTask(50, (TaskFunc*)mydoGetNextFrame, this);
	}
}



