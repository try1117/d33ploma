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

    ConstraintListPtr ConstrainedGraph::constraint_list_ptr() {
        return constraint_list_ptr_;
    }


    bool ConstrainedGraph::empty() {
        return graph_ptr_ == nullptr || graph_ptr_->empty();
    }

    void ConstrainedGraph::add_directed_edge(int from, int to) {
        for (auto &c : constraint_list_ptr_->constraints()) {
            c.second->add_directed_edge(from, to);
        }
        graph_ptr_->add_directed_edge(from, to);
    }

    void ConstrainedGraph::add_undirected_edge(int from, int to) {
//        add_directed_edge(from, to);
//        add_directed_edge(to, from);
        for (auto &c : constraint_list_ptr_->constraints()) {
            c.second->add_undirected_edge(from, to);
        }
        graph_ptr_->add_undirected_edge(from, to);
    }

    void ConstrainedGraph::add_random_directed_edge() {
        auto edge = graph_ptr_->generate_random_directed_edge();
        add_directed_edge(edge.first, edge.second);
    }

    void ConstrainedGraph::add_random_undirected_edge() {
        auto edge = graph_ptr_->generate_random_undirected_edge();
        add_undirected_edge(edge.first, edge.second);
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
