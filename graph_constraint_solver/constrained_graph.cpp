#include "constrained_graph.h"

#include <vector>

namespace graph_constraint_solver {

    ConstrainedGraph::ConstrainedGraph()
        : constraint_list_ptr_(nullptr), graph_ptr_(nullptr) {

    }

    ConstrainedGraph::ConstrainedGraph(ConstraintListPtr constraint_list_ptr, GraphPtr graph_ptr)
        : constraint_list_ptr_(constraint_list_ptr), graph_ptr_(graph_ptr) {

        constraint_list_ptr->bind_graph(graph_ptr);
    }

    bool ConstrainedGraph::empty() {
        return graph_ptr_ == nullptr || graph_ptr_->empty();
    }

    void ConstrainedGraph::add_edge(int from, int to) {
        for (auto &c : constraint_list_ptr_->constraints()) {
            c->add_directed_edge(from, to);
        }
        graph_ptr_->add_directed_edge(from, to);
    }

    void ConstrainedGraph::add_random_edge() {
        graph_ptr_->add_random_edge([&](int from, int to) {
            for (auto &c : constraint_list_ptr_->constraints()) {
                c->add_directed_edge(from, to);
            }
        });
    }

    ConstraintSatisfactionVerdict ConstrainedGraph::check() {
        return constraint_list_ptr_->check();
    }

//    GraphPtr gen_rand_tree(int n) {
//        GraphPtr tree(new Graph(n));
//        for (int i = 1; i < n; ++i) {
//            int parent = rng() % i;
//            tree->add_undirected_edge(i, parent);
//        }
//        return tree;
//    }
}
