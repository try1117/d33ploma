cmake_minimum_required(VERSION 3.14)
project(graph_constraint_solver)

#set(Boost_INCLUDE_DIR /home/danilov/prog/boost_1_70_0)
#set(Boost_LIBRARY_DIR /home/danilov/prog/boost_1_70_0/libs)
#include_directories(${Boost_INCLUDE_DIR})
#link_directories(${Boost_LIBRARY_DIR})

set(nlohmann_include_dir /home/danilov/prog/nlohmann)
include_directories(${nlohmann_include_dir})

set(CMAKE_CXX_STANDARD 17)

set(graph_constraint_solver_headers
        utils.h
        graph.h graph_algorithms.h graph_components.h graph_printer.h
        constraint.h constraint_block.h constrained_graph.h
        generator.h
        program_block.h parser.h program.h)

set(graph_constraint_solver_sources
        utils.cpp
        graph.cpp graph_algorithms.cpp graph_components.cpp graph_printer.cpp
        constraint.cpp constraint_block.cpp constrained_graph.cpp
        generator.cpp
        program_block.cpp parser.cpp program.cpp main.cpp)

#add_definitions(-DGRAPH_CONSTRAINT_SOLVER_SINGLE_HEADER)
#add_executable(graph_constraint_solver main.cpp)
add_executable(graph_constraint_solver ${graph_constraint_solver_headers} ${graph_constraint_solver_sources})
#target_link_libraries(graph_constraint_solver ${Boost_LIBRARIES})