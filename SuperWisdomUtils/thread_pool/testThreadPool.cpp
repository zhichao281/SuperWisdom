#include <iostream>
#include <chrono>
#include <vector>
#include <atomic>
#include "thread_pool/thread_pool.h"

using namespace std;
using namespace std::chrono;
using namespace netlib;

void task1() {
	int n1 = 0;
	while (true)
	{
		if (n1 > 10)
		{
			return;
		}
		n1++;
		cout << "task1 executed by thread " << this_thread::get_id() << endl;
		this_thread::sleep_for(milliseconds(21));
	}

}

void task2() {
	int n1 = 0;
	while (true)
	{
		if (n1 > 10)
		{
			return;
		}
		n1++;
		cout << "task2 executed by thread " << this_thread::get_id() << endl;
		this_thread::sleep_for(milliseconds(15));
	}

}

int main() {
	const int THREAD_NUM = 4;
	const int TASK_NUM = 8;
	atomic_int counter(0);

	ThreadPool* pool = ThreadPool::GetInstance();
	vector<ThreadTask> tasks;

	// create tasks
	for (int i = 0; i < TASK_NUM; ++i) {
		if (i % 2 == 0) {
			tasks.emplace_back(task1);
		}
		else {
			tasks.emplace_back(task2);
		}
	}

	// submit tasks to thread pool
	for (auto& task : tasks) {
		pool->append(task);
	}

	int n1 = 0;
	while (true)
	{
		if (n1 > 10)
		{
			pool->stop();
			return 0;
		}
		this_thread::sleep_for(milliseconds(500));
		n1++;
	}

	// wait for all tasks to complete
	while (counter.load() < TASK_NUM) {
		this_thread::sleep_for(milliseconds(10));
		counter.store(pool->getCompletedTaskCount());
	}

	pool->stop();
	return 0;
}
