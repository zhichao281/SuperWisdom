#pragma once


#include "thread_pool.h"
#include <stdio.h>
#include <iostream>

using namespace netlib;

ThreadPool* ThreadPool::s_pThreadPool = nullptr;

ThreadPool::ThreadPool(int threadNumber)
:m_nThreadNumber(threadNumber),
m_bRunning(true),
m_vecThread(m_nThreadNumber)
{
}

ThreadPool::~ThreadPool()
{	
	stop();
}

bool ThreadPool::start(void)
{
	m_bRunning.store(true);
	for (int i = 0; i < m_nThreadNumber; i++)
	{
		m_vecThread.push_back(std::make_shared<std::thread>(std::bind(&ThreadPool::threadWork, this)));//循环创建线程       
	}
	return true;
}

bool ThreadPool::stop(void)
{
	if (m_bRunning.load())
	{
		{
			std::unique_lock<std::mutex> _lock(m_mutex);
			m_bRunning.store(false);
		}	
		m_condition_empty.notify_all();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		for (auto & thread : m_vecThread)
		{		
			if (thread)
			{
				thread->join();
			}			
		}
	}
	return true;
}

bool ThreadPool::IsRunning()
{
	return m_bRunning.load();
}

ThreadPool * netlib::ThreadPool::GetInstance()
{
	
	if (NULL != s_pThreadPool)
	{
		return s_pThreadPool;
	}

	s_pThreadPool = new ThreadPool(MAX_THREAD);
	s_pThreadPool->start();
	atexit(Destory);
	return s_pThreadPool;


}


/*
*	Function:		append
*	Explanation:	添加工作到队列
*	Input:			ThreadTask			函数

bPriority	是否要优先执行

*	Return:			true 成功 false 失败
*/
bool ThreadPool::append(ThreadTask task,  bool bPriority /* = false */)
{
	if (m_bRunning.load() == false)
	{
		return false;
	}
	{
		std::lock_guard<std::mutex> guard(m_mutex);
		m_taskList.emplace(task);   //将该任务加入任务队列
	}

	m_condition_empty.notify_one();//唤醒某个线程来执行此任务
	return true;
}

ThreadTask ThreadPool::get_one_task()
{
	//在std::condition_variable的wait()函数中，当等待条件（即第一个参数）为false时，
	//线程会被阻塞。
	//在这个例子中，我们使用了lambda表达式来定义等待条件，
	//即当任务队列不为空(tasks_.empty()为false)或者线程池已经被停止(stop_为true)时
	//，线程应该继续执行下去。当等待条件为true时，线程会从wait()函数中返回，继续执行后面的代码。
	ThreadTask task = nullptr;
	std::unique_lock<std::mutex> _lock(m_mutex);
	if (m_taskList.empty())
	{
		m_condition_empty.wait(_lock);  //等待有任务到来被唤醒
	}
	if (!m_taskList.empty() && m_bRunning.load())
	{
		task=std::move(m_taskList.front());  //从任务队列中获取最开始任务
		m_taskList.pop();     //将取走的任务弹出任务队列			
	}
	return task;

}

void ThreadPool::threadWork(void)
{
	while (m_bRunning.load())
	{

		std::unique_lock<std::mutex> _lock(m_mutex);
		//在std::condition_variable的wait()函数中，当等待条件（即第一个参数）为false时，线程会被阻塞。
		//即当任务队列不为空(tasks_.empty()为false)或者线程池已经被停止(stop_为true)时
		//，线程应该继续执行下去。当等待条件为true时，线程会从wait()函数中返回，继续执行后面的代码。
		m_condition_empty.wait(_lock, [this]() {
			return !m_taskList.empty() || !m_bRunning.load();
			}
		);  //等待有任务到来被唤醒
		if (!m_bRunning.load()) {
			return;
		}
		auto task = std::move(m_taskList.front());
		m_taskList.pop();
		_lock.unlock();
		task(); //执行任务		
		//++m_nCompletedTasks; //任务完成，增加计数器

	}
	return;
}
int ThreadPool::getCompletedTaskCount() const
{
	return m_nCompletedTasks.load();
}