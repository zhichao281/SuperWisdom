#pragma once
#pragma execution_character_set("utf-8")

#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>
#include <vector>
#include <memory>
#include <functional>



namespace netlib
{
	class ThreadPool
	{
	public:
		typedef std::function<void(void )> ThreadTask;

		ThreadPool(int threadNumber);

		~ThreadPool();

		//往任务队列里添加任务
		bool append(ThreadTask task, bool bPriority = false);

		//启动线程池
		bool start(void);

		//停止线程池
		bool stop(void);

		// 线程池是否在运行
		bool IsRunning();


		// 获取线程池实例
		static ThreadPool* GetInstance();

private:
		//线程所执行的工作函数
		void threadWork(void);

private:
		std::mutex m_mutex;                                        //互斥锁

		bool m_bRunning;                                           //线程池是否在运行
		int m_nThreadNumber;                                       //线程数

		std::condition_variable_any m_condition_empty;             //当任务队列为空时的条件变量
		std::list<ThreadTask> m_taskList;                          //任务队列
		std::vector<std::shared_ptr<std::thread>> m_vecThread;     //用来保存线程对象指针
	};
}

#define GSThreadPool_AddFun(x)	netlib::ThreadPool::GetInstance()->append(x)
#define GSThreadPool_Run()	netlib::ThreadPool::GetInstance()->start()