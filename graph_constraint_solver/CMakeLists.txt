cmake_minimum_required(VERSION 3.14)
project(graph_constraint_solver)

set(Boost_INCLUDE_DIR /home/danilov/prog/boost_1_70_0)
set(Boost_LIBRARY_DIR /home/danilov/prog/boost_1_70_0/libs)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

set(CMAKE_CXX_STANDARD 17)

add_executable(graph_constraint_solver main.cpp generator.h graph.h constraint.h generator.cpp graph.cpp constrained_graph.h constraint.cpp constrained_graph.cpp utils.h utils.cpp)
target_link_libraries(graph_constraint_solver ${Boost_LIBRARIES})