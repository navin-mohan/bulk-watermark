#pragma once

#include <mutex>
#include <condition_variable>

class Semaphore{

private:
	std::mutex mtx;
	std::condition_variable cv;
	int count;

public:
	Semaphore(const int _count = 0): count{_count} {}

	void notify_all();
	void notify_one();
	void wait();
	inline int val();

};


int Semaphore::val(){
	std::lock_guard<std::mutex> lk(mtx);
	return count;
}