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
    constraints->add_constraint(std::make_shared<graph_constraint_solver::TreeConstraint>());
//    constraints->add_constraint(std::make_shared<graph_constraint_solver::BridgeConstraint>(2, 3));

//    std::pair<int, int> bridges_cnt;
    auto run_time = graph_constraint_solver::timeit([&]() {
        auto generator = graph_constraint_solver::Generator();
        g = generator.generate(constraints);
//        g = gen_rand_graph_bridges(n, 2, 3);
//        bridges_cnt = g->count_bridges(true);
    });

    int undirected_edges = 0;
    for (int i = 0; i < g->graph_ptr()->order(); ++i) {
        for (auto child : g->graph_ptr()->edges()[i]) {
            if (i <= child) {
                ++undirected_edges;
                printf("g.add_edge(%d, %d, color='%s')\n", i, child, "blue");//(bridges_list.count({i, child}) ? "red" : "blue"));
            }
        }
    }

    printf("\n%d\n", undirected_edges);

//    cout << "Bridg, nonlv: " << bridges_cnt.first << ", " << bridges_cnt.second << endl;
//    cout << "Edges       : " << edges << endl;
//    cout << endl;
    std::cout << "Time        : " << run_time << std::endl;
}
