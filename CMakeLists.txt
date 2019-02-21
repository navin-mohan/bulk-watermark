cmake_minimum_required(VERSION 3.8)

project(bulkwatermark)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(
    CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_HOME_DIRECTORY}/bin
    )

set(
    CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_HOME_DIRECTORY}/lib
    )

add_subdirectory(threadpool)

find_package(Threads)
find_package(ImageMagick COMPONENTS Magick++ REQUIRED)


add_executable(bulkwatermark main.cpp)

target_include_directories( bulkwatermark PRIVATE ${ImageMagick_INCLUDE_DIRS})

target_link_libraries(bulkwatermark threadpool ${CMAKE_THREAD_LIBS_INIT} ${ImageMagick_LIBRARIES})