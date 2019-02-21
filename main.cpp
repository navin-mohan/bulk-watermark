#include <iostream>
#include <future>
#include <functional>
#include <Magick++.h>

#include "threadpool.hpp"

//using namespace std;

std::string append_filename(const std::string& filename, const std::string& text);
void do_watermark(const std::string& filename, const Magick::Image& right_image, const Magick::Image& left_image,const std::string& text);


int main(int argc, char** argv){
	Magick::InitializeMagick(NULL);
	Magick::Image right_logo,left_logo;
	std::string filename;

	ThreadPool pool;

	const std::string text("-wm");

	if(argc < 3){
		std::cout << "Insufficient arguments" << std::endl;
		std::cout << "Usage:" << std::endl;
		std::cout << "bulkimagewatermark <left_logo> <right_logo>" << std::endl;
		return 1;
	}

	try{
		left_logo.read(argv[1]);
		right_logo.read(argv[2]);
	}catch(std::exception &err_){
		std::cout << "Error reading the images :" << err_.what() << std::endl;  
		return 1;
	}

	std::packaged_task<void()>* task;

	while(getline(std::cin,filename)){
		if(filename.length() == 0) break;
		task = new std::packaged_task<void()>(std::bind(do_watermark,filename,right_logo,left_logo,text));
		pool.addTask(*task);
		delete task;
	}

	pool.wait();

	return 0;
}


std::string append_filename(const std::string& filename, const std::string& text){
	std::size_t pos = filename.find_last_of('.');
	return filename.substr(0,pos) + text + filename.substr(pos);
}

void do_watermark(const std::string& filename, const Magick::Image& right_image, const Magick::Image& left_image,const std::string& text){
    Magick::Image image;
    std::string output_fname = append_filename(filename,text);
	std::cout << "Processing " << filename << " ............." << std::endl;
	try{
		image.read(filename);
		image.composite(right_image,Magick::GravityType::NorthEastGravity,Magick::CompositeOperator::OverCompositeOp);
		image.composite(left_image,Magick::GravityType::NorthWestGravity,Magick::CompositeOperator::OverCompositeOp);
		image.write(output_fname);		      
	}catch(std::exception &err_){
		std::cout << "Error processing " << filename << " : " << err_.what() << std::endl;
	}
	std::cout << "Done Processing " << output_fname << "." << std::endl;
}