//#include <bits/stdc++.h>
//#include <boost/graph/graphml.hpp>

#include <iostream>
#include <fstream>
#include <vector>

#include "utils.h"
#include "generator.h"
#include "graph_algorithms.h"

// TODO: const GraphPtr ...

int main(int argc, char *argv[]) {
//    std::pair<int, int> order = {100, 100};
//    std::pair<int, int> size = {100, 100};
    std::pair<int, int> component_number = {1, 1};
    std::pair<int, int> component_order = {50, 70};
    std::pair<int, int> component_size = {10, 100};
    std::pair<int, int> component_cut_point = {5, 20};
    std::pair<int, int> component_bridge = {0, 10};

    graph_constraint_solver::ConstrainedGraphPtr g;
    auto constraints = std::make_shared<graph_constraint_solver::ConnectedBlock>();

    constraints->add_constraint(std::make_shared<graph_constraint_solver::GraphTypeConstraint>(graph_constraint_solver::Graph::Type::kUndirected));
//    constraints->add_constraint(std::make_shared<graph_constraint_solver::OrderConstraint>(order));
//    constraints->add_constraint(std::make_shared<graph_constraint_solver::SizeConstraint>(size));
    constraints->add_constraint(std::make_shared<graph_constraint_solver::ComponentNumberConstraint>(component_number));
    constraints->add_constraint(std::make_shared<graph_constraint_solver::ComponentOrderConstraint>(component_order));
    constraints->add_constraint(std::make_shared<graph_constraint_solver::ComponentSizeConstraint>(component_size));
    constraints->add_constraint(std::make_shared<graph_constraint_solver::ComponentCutPointConstraint>(component_cut_point));
    constraints->add_constraint(std::make_shared<graph_constraint_solver::ComponentBridgeConstraint>(component_bridge));

    graph_constraint_solver::ConstrainedGraphPtr constrained_graph;

    auto run_time = graph_constraint_solver::Utils::timeit([&]() {
        auto generator = graph_constraint_solver::Generator();
        constrained_graph = generator.generate(constraints);
    });

    auto graph = constrained_graph->components_ptr()->get_component(0);
//    auto generator = graph_constraint_solver::Generator();
//    auto graph = generator.generate_tree_fixed_leaves_number(20, 10, 0.4);//graph_constraint_solver::random.next());

//    graph->shuffle();

    // TODO: graph printer

    std::pair<int, int> bridges_number;
    std::vector<std::pair<int, int>> bridges_list;
    graph_constraint_solver::GraphAlgorithms::count_bridges(graph, bridges_number, bridges_list);
    std::set<std::pair<int, int>> bridges_set(bridges_list.begin(), bridges_list.end());

    std::cout << "Graph order : " << graph->order() << std::endl;
    std::cout << "Graph size  : " << graph->size() << std::endl;
    std::cout << "Bridg, nonlv: " << bridges_number.first << ", " << bridges_number.second << std::endl;
    std::cout << std::endl;
    std::cout << "Time        : " << run_time << std::endl << std::endl;

    std::set<std::pair<int, int>> edges;
    for (int i = 0; i < graph->order(); ++i) {
        for (auto child : graph->adjacency_list()[i]) {
            if (i <= child) {
                if (edges.count({i, child})) {
                    std::cout << "parallel" << std::endl;
                }
                edges.insert({i, child});
                printf("g.add_edge(%d, %d, color='%s')\n", i, child, bridges_set.count({i, child}) ? "red" : "blue");
//                std::cout << i + 1 << " " << child + 1 << "\n";
            }
        }
    }

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
