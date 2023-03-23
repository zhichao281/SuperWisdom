#pragma once


#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <memory>
#include <functional>
#include <atomic>
#include <future>

#include <stdexcept>


#define MAX_THREAD  10
namespace netlib
{
	using ThreadTask = std::function<void(void)>;
	
	class ThreadPool
	{
	public:		
		//typedef std::function<void(void )> ThreadTask;

		ThreadPool(int threadNumber);

		~ThreadPool();

		//������������������
		bool append(ThreadTask task, bool bPriority = false);

		ThreadTask get_one_task();

		//�����̳߳�
		bool start(void);

		//ֹͣ�̳߳�
		bool stop(void);

		// �̳߳��Ƿ�������
		bool IsRunning();
		
		int ThreadPool::getCompletedTaskCount() const;
		// ��ȡ�̳߳�ʵ��
		static ThreadPool* GetInstance();

		static void Destory()
		{
			if (nullptr != s_pThreadPool)
			{
				delete s_pThreadPool;
				s_pThreadPool = nullptr;
			}
		};
public:

		static ThreadPool*		s_pThreadPool;
private:
		//�߳���ִ�еĹ�������
		void threadWork(void);

private:
		std::mutex m_mutex;                                        //������
		
		std::atomic< bool> m_bRunning;                              //�̳߳��Ƿ�������
		int m_nThreadNumber;                                       //�߳���

		std::condition_variable_any m_condition_empty;             //���������Ϊ��ʱ����������
		std::queue<ThreadTask> m_taskList;                          //�������
		std::vector<std::shared_ptr<std::thread>> m_vecThread;     //���������̶߳���ָ��
		//�����߳�����
		std::atomic<int>  m_idlThrNum;
		std::atomic<int> m_nCompletedTasks{ 0 };

	};
}

#define GSThreadPool_AddFun(x)	netlib::ThreadPool::GetInstance()->append(x)

#define GSThreadPool	netlib::ThreadPool::GetInstance()




