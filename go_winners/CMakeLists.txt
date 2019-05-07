cmake_minimum_required(VERSION 3.14)
project(go_winners)

set(Boost_INCLUDE_DIR /home/danilov/prog/boost_1_70_0)
set(Boost_LIBRARY_DIR /home/danilov/prog/boost_1_70_0/libs)
#find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

set(CMAKE_CXX_STANDARD 17)

add_executable(go_winners main.cpp)
target_link_libraries(go_winners ${Boost_LIBRARIES})