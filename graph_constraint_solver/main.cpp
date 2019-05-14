#include <bits/stdc++.h>
//#include <boost/graph/graphml.hpp>

#include "utils.h"
#include "generator.h"

// TODO: const GraphPtr ...

int main() {
    int n = 2000;

    graph_constraint_solver::ConstrainedGraphPtr g;
    graph_constraint_solver::ConstraintListPtr constraints = std::make_shared<graph_constraint_solver::ConstraintList>();
    constraints->add_constraint(std::make_shared<graph_constraint_solver::BridgeConstraint>(2, 3));

    //    std::pair<int, int> bridges_cnt;

    auto tim = graph_constraint_solver::timeit([&]() {
        auto generator = graph_constraint_solver::Generator();
        g = generator.generate(constraints);
//        g = gen_rand_graph_bridges(n, 2, 3);
//        bridges_cnt = g->count_bridges(true);
    });

//    int edges = 0;
//    for (int i = 0; i < n; ++i) {
//        for (auto child : g->g[i]) {
//            if (i <= child) {
//                ++edges;
////                printf("g.add_edge(%d, %d, color='%s')\n", i, child, (bridges_list.count({i, child}) ? "red" : "blue"));
//            }
//        }
//    }

//    cout << "Bridg, nonlv: " << bridges_cnt.first << ", " << bridges_cnt.second << endl;
//    cout << "Edges       : " << edges << endl;
//    cout << endl;
//    cout << "Time        : " << tim << endl;
}
