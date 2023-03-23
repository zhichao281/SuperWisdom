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
		m_vecThread.push_back(std::make_shared<std::thread>(std::bind(&ThreadPool::threadWork, this)));//ѭ�������߳�       
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
*	Explanation:	��ӹ���������
*	Input:			ThreadTask			����

bPriority	�Ƿ�Ҫ����ִ��

*	Return:			true �ɹ� false ʧ��
*/
bool ThreadPool::append(ThreadTask task,  bool bPriority /* = false */)
{
	if (m_bRunning.load() == false)
	{
		return false;
	}
	{
		std::lock_guard<std::mutex> guard(m_mutex);
		m_taskList.emplace(task);   //������������������
	}

	m_condition_empty.notify_one();//����ĳ���߳���ִ�д�����
	return true;
}

ThreadTask ThreadPool::get_one_task()
{
	//��std::condition_variable��wait()�����У����ȴ�����������һ��������Ϊfalseʱ��
	//�̻߳ᱻ������
	//����������У�����ʹ����lambda���ʽ������ȴ�������
	//����������в�Ϊ��(tasks_.empty()Ϊfalse)�����̳߳��Ѿ���ֹͣ(stop_Ϊtrue)ʱ
	//���߳�Ӧ�ü���ִ����ȥ�����ȴ�����Ϊtrueʱ���̻߳��wait()�����з��أ�����ִ�к���Ĵ��롣
	ThreadTask task = nullptr;
	std::unique_lock<std::mutex> _lock(m_mutex);
	if (m_taskList.empty())
	{
		m_condition_empty.wait(_lock);  //�ȴ���������������
	}
	if (!m_taskList.empty() && m_bRunning.load())
	{
		task=std::move(m_taskList.front());  //����������л�ȡ�ʼ����
		m_taskList.pop();     //��ȡ�ߵ����񵯳��������			
	}
	return task;

}

void ThreadPool::threadWork(void)
{
	while (m_bRunning.load())
	{

		std::unique_lock<std::mutex> _lock(m_mutex);
		//��std::condition_variable��wait()�����У����ȴ�����������һ��������Ϊfalseʱ���̻߳ᱻ������
		//����������в�Ϊ��(tasks_.empty()Ϊfalse)�����̳߳��Ѿ���ֹͣ(stop_Ϊtrue)ʱ
		//���߳�Ӧ�ü���ִ����ȥ�����ȴ�����Ϊtrueʱ���̻߳��wait()�����з��أ�����ִ�к���Ĵ��롣
		m_condition_empty.wait(_lock, [this]() {
			return !m_taskList.empty() || !m_bRunning.load();
			}
		);  //�ȴ���������������
		if (!m_bRunning.load()) {
			return;
		}
		auto task = std::move(m_taskList.front());
		m_taskList.pop();
		_lock.unlock();
		task(); //ִ������		
		//++m_nCompletedTasks; //������ɣ����Ӽ�����

	}
	return;
}
int ThreadPool::getCompletedTaskCount() const
{
	return m_nCompletedTasks.load();
}