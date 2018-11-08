#include <iostream>
#include <Magick++.h>
#include <atomic>
#include <thread>
#include <queue>
#include <vector>
#include <mutex>
#include <functional> 


std::string append_filename(const std::string& filename, const std::string& text){
	std::size_t pos = filename.find_last_of('.');
	return filename.substr(0,pos) + text + filename.substr(pos);
}


typedef std::function<void()> task_function;

class ThreadPool{
public:
	ThreadPool(std::size_t num_threads);
	void add_task(task_function task);
	void clear_tasks();
	void wait();
	~ThreadPool();
private:
	std::queue<task_function> tasks;
	std::vector<std::thread> workers;
	std::atomic<int> n_task;
	std::mutex queue_mutex;
	bool alive;

	void thread_loop();
};

void ThreadPool::wait(){
	while(n_task.load() > 0){
		std::this_thread::yield();
	}
}

void ThreadPool::clear_tasks(){
	std::queue<task_function> empty;
	queue_mutex.lock();
	std::swap(tasks,empty);
	queue_mutex.unlock();
}

ThreadPool::~ThreadPool(){
	alive = false;
	for(std::thread& worker: workers){
		worker.join();
	}
}

void ThreadPool::add_task(task_function task){
	queue_mutex.lock();
	tasks.push(task);
	queue_mutex.unlock();
	n_task++;
}

void ThreadPool::thread_loop(){
	while(alive){
		queue_mutex.lock();
		if(!tasks.empty()){
			task_function task = tasks.front();
			tasks.pop();
			queue_mutex.unlock();
			task();
			n_task--;
		}else{
			queue_mutex.unlock();
			std::this_thread::yield();
		}
	}
}

ThreadPool::ThreadPool(std::size_t num_threads = std::thread::hardware_concurrency())
: n_task{0}, alive{true} {
	workers.reserve(num_threads);
	for(std::size_t i = 0; i < num_threads; i++){
		workers.push_back(std::thread(&ThreadPool::thread_loop,this));
	}
}


void do_watermark(const std::string& filename, const Magick::Image& excel_logo, const Magick::Image& mec_logo,const std::string& text){
        Magick::Image image;
	std::cout << "Processing " << filename << " ............." << std::endl;
	try{
		image.read(filename);
		image.composite(excel_logo,Magick::GravityType::NorthEastGravity,Magick::CompositeOperator::OverCompositeOp);
		image.composite(mec_logo,Magick::GravityType::NorthWestGravity,Magick::CompositeOperator::OverCompositeOp);
		image.write(append_filename(filename,text));		      
	}catch(std::exception &err_){
		std::cout << "Error processing " << filename << " : " << err_.what() << std::endl;
	}
	std::cout << "Done Processing " << filename << "." << std::endl;
}

int main(int argc,char **argv){
	Magick::InitializeMagick(NULL);
	Magick::Image excel_logo,mec_logo;
	std::string filename;
	ThreadPool tp;
	const std::string text("-wm");

	try{
		excel_logo.read("excel_logo.png");
		mec_logo.read("mec_logo.png");
	}catch(std::exception &err_){
		std::cout << "Error reading the logos :" << err_.what() << std::endl;  
		return 1;
	}

	while(getline(std::cin, filename)){
		tp.add_task(std::bind(do_watermark,filename,excel_logo,mec_logo,text));
	}

	tp.wait();

	return 0;
}
