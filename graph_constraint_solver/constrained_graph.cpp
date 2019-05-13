#include "constrained_graph.h"

namespace graph_constraint_solver {
    void ConstrainedGraph::add_edge(int from, int to) {
        for (auto &c : constraints) {
            c->add_directed_edge(from, to);
        }
        graph_ptr->add_directed_edge(from, to);
    }

    void ConstrainedGraph::add_random_edge() {
        graph_ptr->add_random_edge([&](int from, int to) {
            for (auto &c : constraints) {
                c->add_directed_edge(from, to);
            }
        });
    }
}
