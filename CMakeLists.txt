cmake_minimum_required(VERSION 3.8)
project(messaging)

set(CMAKE_CXX_STANDARD 14)

find_package(Threads REQUIRED)

add_executable(message_example example/sample.cpp)

target_include_directories(message_example PUBLIC include)

target_link_libraries(message_example Threads::Threads)
