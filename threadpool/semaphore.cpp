#include "semaphore.hpp"

void Semaphore::wait(){
	std::unique_lock<std::mutex> lk(mtx);
	cv.wait(lk,[&](){ return count > 0; });
	count--;
}

void Semaphore::notify_all(){
	std::unique_lock<std::mutex> lk(mtx);
	count++;
	lk.unlock();
	cv.notify_all();
}

void Semaphore::notify_one(){
	std::unique_lock<std::mutex> lk(mtx);
	count++;
	lk.unlock();
	cv.notify_one();
}
