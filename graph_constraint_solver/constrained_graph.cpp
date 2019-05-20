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

    ConstrainedGraph::ConstrainedGraph(ConstrainedGraph &other)
        : ConstrainedGraph(std::make_shared<ConstraintList>(*other.constraint_list_ptr()),
                std::make_shared<Graph>(*other.graph_ptr())) {

    }

    ConstraintListPtr ConstrainedGraph::constraint_list_ptr() {
        return constraint_list_ptr_;
    }

    GraphPtr ConstrainedGraph::graph_ptr() {
        return graph_ptr_;
    }

    bool ConstrainedGraph::empty() {
        return graph_ptr_ == nullptr || graph_ptr_->empty();
    }

    void ConstrainedGraph::add_edge(int from, int to) {
        for (auto &c : constraint_list_ptr_->constraints()) {
            c.second->add_edge(from, to);
        }
        graph_ptr_->add_edge(from, to);
    }

    void ConstrainedGraph::add_random_edge() {
        auto edge = graph_ptr_->generate_random_edge();
        add_edge(edge.first, edge.second);
    }

    Constraint::SatisfactionVerdict ConstrainedGraph::check() {
        return constraint_list_ptr_->check();
    }
}
