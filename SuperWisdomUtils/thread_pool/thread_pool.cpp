#pragma once
#pragma execution_character_set("utf-8")

#include "thread_pool.h"
#include <stdio.h>
#include <thread>
#include <mutex>
#include <memory>
#include <functional>
#include <windows.h>
#include <iostream>
using namespace netlib;

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
	std::cout << "start" << std::endl;

	return true;
}

bool ThreadPool::stop(void)
{
	std::cout << "stop" << std::endl;
	if (m_bRunning)
	{
		m_bRunning = false;
		for (auto t : m_vecThread)
		{			
			t->join();  //循环等待线程终止			
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
	static ThreadPool*		s_pThreadPool = NULL;

	if (NULL != s_pThreadPool)
	{
		return s_pThreadPool;
	}

	s_pThreadPool = new ThreadPool(10);
	s_pThreadPool->start();
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
			std::lock_guard<std::mutex> guard(m_mutex);
			if (m_taskList.empty())
			{
				m_condition_empty.wait(m_mutex);  //等待有任务到来被唤醒
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
		task(); //执行任务
	}
}
