//#include <boost/graph/graphml.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "utils.h"
#include "generator.h"
#include "graph_algorithms.h"
#include "program.h"

int main(int argc, char *argv[]) {

    if (argc < 3) {
        throw std::runtime_error("graph_constraint_solver error: too few arguments");
    }

    try {
        auto random_sid = std::stoll(argv[1]);
        graph_constraint_solver::random.set_seed(random_sid);
    }
    catch (...) {
        throw std::runtime_error("graph_constraint_solver random seed error: expected 64-bit integer number");
    }

    graph_constraint_solver::Parser::JSONFile json_file;
    try {
        auto file_name = argv[2];
        json_file.open(file_name);
    }
    catch (std::exception &e) {
        throw std::runtime_error("graph_constraint_solver file error: " + static_cast<std::string>(e.what()));
    }

    graph_constraint_solver::impl::increase_stack_size();

    graph_constraint_solver::InputBlock::Arguments arguments(argv + 3, argv + argc);

    // TODO: pass output stream (now just output to stdout)
    graph_constraint_solver::Program program(json_file, arguments);
    return 0;

//    std::pair<int, int> order = {std::stoi(argv[2]), std::stoi(argv[3])};
//    std::pair<int, int> diameter = {std::stoi(argv[4]), std::stoi(argv[5])};
//    int vertex_maximum_degree = std::stoi(argv[6]);
//
//    graph_constraint_solver::ConstrainedGraphPtr constrained_graph;
//    auto constraints = std::make_shared<graph_constraint_solver::TreeBlock>(
//            graph_constraint_solver::Graph::Type::kUndirected,
//            std::make_pair(1, 1),
//            order, diameter, vertex_maximum_degree);
//
////    constraints->add_constraint(std::make_shared<graph_constraint_solver::TreeConstraint>());
////    constraints->add_constraint(std::make_shared<graph_constraint_solver::BridgeConstraint>(50, 60));
//
////    std::pair<int, int> bridges_cnt;
////    std::set<std::pair<int, int>> bridges_list;
//
//    auto run_time = graph_constraint_solver::Utils::timeit([&]() {
//        auto generator = graph_constraint_solver::Generator();
//        constrained_graph = generator.generate(constraints);
//    });
//
//    // TODO: return from Generator::generate GraphPtr of GraphComponentsPtr ???
//    auto graph = constrained_graph->components_ptr()->get_component(0);
//    graph->shuffle();
//
//    std::ofstream out(argv[7]);
//    out << graph->order() << "\n";
//    for (int i = 0; i < graph->order(); ++i) {
//        for (auto child : graph->adjacency_list()[i]) {
//            if (i <= child) {
//                out << i + 1 << " " << child + 1 << "\n";
//            }
//        }
//    }
//    out.close();
//
}
