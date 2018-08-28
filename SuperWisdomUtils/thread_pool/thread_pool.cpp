#pragma once


#include "thread_pool.h"
#include <stdio.h>
#include <thread>
#include <mutex>
#include <memory>
#include <functional>
#include <windows.h>
#include <iostream>
using namespace netlib;



ThreadPool* ThreadPool::s_pThreadPool = nullptr;

ThreadPool::ThreadPool(int threadNumber)
:m_nThreadNumber(threadNumber),
m_bRunning(true),
m_vecThread(m_nThreadNumber)
{
	m_vecThread.clear();
}

ThreadPool::~ThreadPool()
{
	if (m_bRunning)
	{
		stop();
	}
}

bool ThreadPool::start(void)
{
	for (int i = 0; i < m_nThreadNumber; i++)
	{
		m_vecThread.push_back(std::make_shared<std::thread>(std::bind(&ThreadPool::threadWork, this)));//循环创建线程       
	}
	return true;
}

bool ThreadPool::stop(void)
{
	if (m_bRunning)
	{
		m_bRunning = false;
		m_condition_empty.notify_all();
		for (auto t : m_vecThread)
		{
			if (t->joinable())
			{
				t->join();  //循环等待线程终止		
			}
		}
	}
	return true;
}

bool ThreadPool::IsRunning()
{
	return m_bRunning;
}

ThreadPool * netlib::ThreadPool::GetInstance()
{

	if (NULL != s_pThreadPool)
	{
		return s_pThreadPool;
	}

	s_pThreadPool = new ThreadPool(3);
	s_pThreadPool->start();
	//atexit(Destory);
	return s_pThreadPool;


}


/*
*	Function:		append
*	Explanation:	添加工作到队列
*	Input:			ThreadTask			函数

bPriority	是否要优先执行

*	Return:			true 成功 false 失败
*/
bool ThreadPool::append(ThreadTask task, bool bPriority /* = false */)
{
	if (m_bRunning == false)
	{
		return false;
	}
	std::lock_guard<std::mutex> guard(m_mutex);
	m_taskList.push_front(task);   //将该任务加入任务队列
	m_condition_empty.notify_one();//唤醒某个线程来执行此任务
	return true;
}

void ThreadPool::threadWork(void)
{
	ThreadTask task = nullptr;
	while (m_bRunning)
	{
		{
			std::unique_lock<std::mutex> _lock(m_mutex);
			task = nullptr;
			if (m_taskList.empty())
			{
				m_condition_empty.wait(_lock);  //等待有任务到来被唤醒

			}
			if (!m_taskList.empty())
			{
				task = m_taskList.front();  //从任务队列中获取最开始任务
				m_taskList.pop_front();     //将取走的任务弹出任务队列			
			}
			else
			{
				continue;
			}
		}
		if (task != nullptr && m_bRunning)
		{
			task(); //执行任务
		}
	}
	return;
}
