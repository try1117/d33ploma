//#include <bits/stdc++.h>
//#include <boost/graph/graphml.hpp>

#include <iostream>

#include "utils.h"
#include "generator.h"

// TODO: const GraphPtr ...

int main() {

    int n = 20;

    graph_constraint_solver::ConstrainedGraphPtr g;
    graph_constraint_solver::ConstraintListPtr constraints = std::make_shared<graph_constraint_solver::ConstraintList>();

    constraints->add_constraint(std::make_shared<graph_constraint_solver::OrderConstraint>(n));
//    constraints->add_constraint(std::make_shared<graph_constraint_solver::TreeConstraint>());
    constraints->add_constraint(std::make_shared<graph_constraint_solver::BridgeConstraint>(2, 3));

    std::pair<int, int> bridges_cnt;
    std::set<std::pair<int, int>> bridges_list;

    auto run_time = graph_constraint_solver::timeit([&]() {
        auto generator = graph_constraint_solver::Generator();
        g = generator.generate(constraints);

        auto cons = constraints->get_constraint(graph_constraint_solver::kBridge);
        auto br_cons = std::static_pointer_cast<graph_constraint_solver::BridgeConstraint>(cons);

        bridges_cnt = br_cons->count_bridges(g->graph_ptr(), true);
        bridges_list = br_cons->get_bridges_list();
    });

    int undirected_edges = 0;
    for (int i = 0; i < g->graph_ptr()->order(); ++i) {
        for (auto child : g->graph_ptr()->adjacency_list()[i]) {
            if (i <= child) {
                ++undirected_edges;
                printf("g.add_edge(%d, %d, color='%s')\n", i, child, (bridges_list.count({i, child}) ? "red" : "blue"));
            }
        }
    }

    std::cout << "Bridg, nonlv: " << bridges_cnt.first << ", " << bridges_cnt.second << std::endl;
    std::cout << "Edges       : " << undirected_edges << std::endl;
    std::cout << std::endl;
    std::cout << "Time        : " << run_time << std::endl;
}
