//#include <bits/stdc++.h>
//#include <boost/graph/graphml.hpp>

#include <iostream>
#include <fstream>

#include "utils.h"
#include "generator.h"

// TODO: const GraphPtr ...

int main(int argc, char *argv[]) {
//    std::pair<int, int> order = {100, 100};
//    std::pair<int, int> size = {100, 100};
    std::pair<int, int> component_number = {1, 1};
    std::pair<int, int> component_order = {5, 20};
    std::pair<int, int> component_size = {10, 50};
    std::pair<int, int> component_cut_point = {3, 3};

    graph_constraint_solver::ConstrainedGraphPtr g;
    auto constraints = std::make_shared<graph_constraint_solver::TwoEdgeConnectedBlock>();

    constraints->add_constraint(std::make_shared<graph_constraint_solver::GraphTypeConstraint>(graph_constraint_solver::Graph::Type::kUndirected));
//    constraints->add_constraint(std::make_shared<graph_constraint_solver::OrderConstraint>(order));
//    constraints->add_constraint(std::make_shared<graph_constraint_solver::SizeConstraint>(size));
    constraints->add_constraint(std::make_shared<graph_constraint_solver::ComponentNumberConstraint>(component_number));
    constraints->add_constraint(std::make_shared<graph_constraint_solver::ComponentOrderConstraint>(component_order));
    constraints->add_constraint(std::make_shared<graph_constraint_solver::ComponentSizeConstraint>(component_size));
    constraints->add_constraint(std::make_shared<graph_constraint_solver::ComponentCutPointConstraint>(component_cut_point));

    graph_constraint_solver::ConstrainedGraphPtr constrained_graph;

    auto run_time = graph_constraint_solver::Utils::timeit([&]() {
        auto generator = graph_constraint_solver::Generator();
        constrained_graph = generator.generate(constraints);
    });

    auto graph = constrained_graph->components_ptr()->get_component(0);
    graph->shuffle();

    std::cout << graph->order() << " " << graph->size() << "\n";
    for (int i = 0; i < graph->order(); ++i) {
        for (auto child : graph->adjacency_list()[i]) {
            if (i <= child) {
                printf("g.add_edge(%d, %d, color='%s')\n", i, child, "blue");//(bridges_list.count({i, child}) ? "red" : "blue"));
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
////        auto br_cons = constraints->get_constraint<graph_constraint_solver::BridgeConstraint>(graph_constraint_solver::Constraint::Type::kBridge);
////        bridges_cnt = br_cons->count_bridges(g->graph_ptr(), true);
////        bridges_list = br_cons->get_bridges_list();
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

//    std::set<std::pair<int, int>> edges;
//
//    int undirected_edges = 0;
//    for (int i = 0; i < g->graph_ptr()->order(); ++i) {
//        for (auto child : g->graph_ptr()->adjacency_list()[i]) {
//            if (i <= child) {
////                ++undirected_edges;
//                if (edges.count({i, child})) {
//                    std::cout << "parallel" << std::endl;
//                }
//                edges.insert({i, child});
//                printf("g.add_edge(%d, %d, color='%s')\n", i, child, "blue");//(bridges_list.count({i, child}) ? "red" : "blue"));
//            }
//        }
//    }

//    std::cout << "Bridg, nonlv: " << bridges_cnt.first << ", " << bridges_cnt.second << std::endl;
//    std::cout << "Graph order : " << g->graph_ptr()->order() << std::endl;
//    std::cout << "Graph size  : " << g->graph_ptr()->size() << std::endl;
//    std::cout << std::endl;
//    std::cout << "Time        : " << run_time << std::endl;
}
