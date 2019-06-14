//#include <boost/graph/graphml.hpp>

#include <iostream>
#include <fstream>
#include <vector>

#include "utils.h"
#include "generator.h"
#include "graph_algorithms.h"
#include "program.h"

int main(int argc, char *argv[]) {

    auto json_file = graph_constraint_solver::Parser::JSONFile("tmp.json");
    graph_constraint_solver::Program program(json_file, std::vector<std::string>());
    return 0;

//    if (argc != 8 || std::string(argv[1]) != "tree") {
//        return 0;
//    }
//
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
