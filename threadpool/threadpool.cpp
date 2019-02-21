#include "threadpool.hpp"
#include <chrono>


std::packaged_task<void()> ThreadPool::get_task(){

	std::packaged_task<void()> task;

	std::lock_guard<std::mutex> lk(mtx);

	task = std::move(task_queue.front());

	task_queue.pop();


	return std::move(task);
}


bool ThreadPool::quit_status(){
	sem.wait();
	std::lock_guard<std::mutex> lk(mtx);
	return quit;
}

void ThreadPool::worker(ThreadPool* context){

	while(!context->quit_status()){

		std::packaged_task<void()> task;
		task = context->get_task();
		task();
		{
			std::lock_guard<std::mutex> lk(context->mtx);
			context->tasks_completed++;
		}
		context->cv.notify_all();

	}
}

void ThreadPool::wait(){
	int count;
	{
		std::lock_guard<std::mutex> lk(mtx);
		count = tasks_given - tasks_completed;
	}

	while(count > 0){
		std::unique_lock<std::mutex> lk(mtx);
		auto now = std::chrono::system_clock::now();
		cv.wait_until(lk,now + std::chrono::seconds(1));
		count = tasks_given - tasks_completed;
	}

	{
		std::lock_guard<std::mutex> lk(mtx);
		tasks_given = tasks_completed = 0;
	}

}

void ThreadPool::addTask(std::packaged_task<void()>& task){
	std::lock_guard<std::mutex> lk(mtx);
	task_queue.push(std::move(task));
	tasks_given++;
	sem.notify_one();
}


ThreadPool::ThreadPool(const int n){

	quit = false;

	tasks_given = tasks_completed = 0;
	
	for(int i=0;i<n;++i){
		threads.push_back(new std::thread(ThreadPool::worker,this));
	}
}

ThreadPool::~ThreadPool(){

	{
		std::lock_guard<std::mutex> lk(mtx);
		quit = true;

		for(int i=0;i<threads.size();++i)
			sem.notify_one();
	}


	for(int i=0;i<threads.size();++i){
		threads[i]->join();
		delete threads[i];
	}

}

