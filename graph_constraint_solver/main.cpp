//#include <bits/stdc++.h>
//#include <boost/graph/graphml.hpp>

#include <iostream>

#include "utils.h"
#include "generator.h"

// TODO: const GraphPtr ...

int main() {
//    int order = 250;
//    std::pair<int, int> size = {250, 500};
//    std::pair<int, int> components_number = {10, 10};

//    std::pair<int, int> order = {100, 100};
//    std::pair<int, int> size = {100, 100};
//    std::pair<int, int> components_number = {5, 20};
//    std::pair<int, int> components_order = {5, 20};

//    graph_constraint_solver::ConstrainedGraphPtr g;
//    auto constraints = std::make_shared<graph_constraint_solver::TwoConnectedBlock>();
//
//    constraints->add_constraint(std::make_shared<graph_constraint_solver::GraphTypeConstraint>(graph_constraint_solver::Graph::Type::kUndirected));
//    constraints->add_constraint(std::make_shared<graph_constraint_solver::OrderConstraint>(order));
//    constraints->add_constraint(std::make_shared<graph_constraint_solver::SizeConstraint>(size));
//    constraints->add_constraint(std::make_shared<graph_constraint_solver::ComponentsNumberConstraint>(components_number));
//    constraints->add_constraint(std::make_shared<graph_constraint_solver::ComponentsOrderConstraint>(components_order));

    std::pair<int, int> order = {800000, 1000000};
    std::pair<int, int> diameter = {8, 10};
    int vertex_maximum_degree = 30;

    graph_constraint_solver::ConstrainedGraphPtr g;
    auto constraints = std::make_shared<graph_constraint_solver::TreeBlock>();

    constraints->add_constraint(std::make_shared<graph_constraint_solver::GraphTypeConstraint>(graph_constraint_solver::Graph::Type::kUndirected));
    constraints->add_constraint(std::make_shared<graph_constraint_solver::OrderConstraint>(order));
    constraints->add_constraint(std::make_shared<graph_constraint_solver::DiameterConstraint>(diameter));
    constraints->add_constraint(std::make_shared<graph_constraint_solver::VertexMaxDegreeConstraint>(vertex_maximum_degree));

//    constraints->add_constraint(std::make_shared<graph_constraint_solver::ComponentsNumberConstraint>(components_number));
//    constraints->add_constraint(std::make_shared<graph_constraint_solver::ComponentsOrderConstraint>(components_order));

//    constraints->add_constraint(std::make_shared<graph_constraint_solver::TreeConstraint>());
//    constraints->add_constraint(std::make_shared<graph_constraint_solver::BridgeConstraint>(50, 60));

//    std::pair<int, int> bridges_cnt;
//    std::set<std::pair<int, int>> bridges_list;

    auto run_time = graph_constraint_solver::Utils::timeit([&]() {
        auto generator = graph_constraint_solver::Generator();
        g = generator.generate(constraints);

        if (g->empty()) {
            exit(0);
        }

//        auto br_cons = constraints->get_constraint<graph_constraint_solver::BridgeConstraint>(graph_constraint_solver::Constraint::Type::kBridge);
//        bridges_cnt = br_cons->count_bridges(g->graph_ptr(), true);
//        bridges_list = br_cons->get_bridges_list();
    });

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
    std::cout << "Graph order : " << g->graph_ptr()->order() << std::endl;
    std::cout << "Graph size  : " << g->graph_ptr()->size() << std::endl;
    std::cout << std::endl;
    std::cout << "Time        : " << run_time << std::endl;
}
