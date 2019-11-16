cmake_minimum_required(VERSION 3.12)
project(messaging)

set(CMAKE_CXX_STANDARD 17)

add_executable(message_example example/sample.cpp)

target_include_directories(message_example PUBLIC include)
