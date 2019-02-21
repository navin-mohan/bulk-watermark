#pragma once


#include <thread>
#include <future>
#include <mutex>
#include <vector>
#include <queue>
#include "semaphore.hpp"

class ThreadPool{

private:
	std::vector<std::thread*> threads;
	std::queue<std::packaged_task<void()> > task_queue;

	std::mutex mtx;
	Semaphore sem;
	std::condition_variable cv;

	int tasks_given,tasks_completed;

	bool quit;

	static void worker(ThreadPool* context);

	std::packaged_task<void()> get_task();
	bool quit_status();

public:
	explicit ThreadPool(const int n = std::thread::hardware_concurrency()); 
	virtual ~ThreadPool();
	void addTask(std::packaged_task<void()>& task);
	void wait();
};