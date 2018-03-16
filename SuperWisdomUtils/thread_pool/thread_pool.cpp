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
:threadNumber_(threadNumber),
running_(true),
threads_(threadNumber_)
{
	threads_.clear();
}

ThreadPool::~ThreadPool()
{
	if (running_)
	{
		stop();
	}
}

bool ThreadPool::start(void)
{
	for (int i = 0; i < threadNumber_; i++)
	{
		threads_.push_back(std::make_shared<std::thread>(std::bind(&ThreadPool::threadWork, this)));//循环创建线程       
	}
	Sleep(500);
	std::cout << "start" << std::endl;

	return true;
}

bool ThreadPool::stop(void)
{
	std::cout << "stop" << std::endl;
	if (running_)
	{
		running_ = false;
		for (auto t : threads_)
		{			
			t->join();  //循环等待线程终止			
		}
	}
	return true;
}

bool ThreadPool::IsRunning()
{

	return running_;
}

ThreadPool * netlib::ThreadPool::GetInstance()
{
	static ThreadPool*		s_pThreadPool = NULL;

	if (NULL != s_pThreadPool)
	{
		return s_pThreadPool;
	}

	s_pThreadPool = new ThreadPool(10);

	return s_pThreadPool;
	return nullptr;
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
	std::lock_guard<std::mutex> guard(mutex_);
	tasks_.push_front(task);   //将该任务加入任务队列
	condition_empty_.notify_one();//唤醒某个线程来执行此任务
	return true;
}

void ThreadPool::threadWork(void)
{
	ThreadTask task = nullptr;
	while (running_)
	{
		{
			std::lock_guard<std::mutex> guard(mutex_);
			if (tasks_.empty())
			{
				condition_empty_.wait(mutex_);  //等待有任务到来被唤醒
			}
			if (!tasks_.empty())
			{
				task = tasks_.front();  //从任务队列中获取最开始任务
				tasks_.pop_front();     //将取走的任务弹出任务队列
			}
			else
			{
				continue;
			}
		}
		task(); //执行任务
	}
}
