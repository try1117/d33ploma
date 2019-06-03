#include "constrained_graph.h"

#include <vector>

namespace graph_constraint_solver {

    ConstrainedGraph::ConstrainedGraph()
        : constraint_list_ptr_(nullptr), components_ptr_(nullptr) {

    }

    ConstrainedGraph::ConstrainedGraph(ConstraintBlockPtr constraint_list_ptr, GraphComponentsPtr components_ptr)
        : constraint_list_ptr_(constraint_list_ptr), components_ptr_(components_ptr) {

        // TODO: bind these components ...
//        constraint_list_ptr->bind_graph(graph_ptr);
    }

    ConstrainedGraph::ConstrainedGraph(ConstrainedGraph &other)
        : ConstrainedGraph(std::make_shared<ConstraintBlock>(*other.constraint_list_ptr()),
                other.components_ptr()->clone()) {

    }

    ConstraintBlockPtr ConstrainedGraph::constraint_list_ptr() {
        return constraint_list_ptr_;
    }

    std::shared_ptr<GraphComponents> ConstrainedGraph::components_ptr() {
        return components_ptr_;
    }

    bool ConstrainedGraph::empty() {
        return components_ptr_ == nullptr || components_ptr_->empty();
    }

    void ConstrainedGraph::append_graph(GraphPtr other) {
        // TODO: think about this method
//        graph_ptr_->append_graph(other);
    }

    void ConstrainedGraph::add_edge(int from, int to) {
        // TODO: think about this method (seems like i don't really need it)
//        for (auto &c : constraint_list_ptr_->constraints()) {
//            c.second->add_edge(from, to);
//        }
//        graph_ptr_->add_edge(from, to);
    }

//    void ConstrainedGraph::add_random_edge() {
//        auto edge = graph_ptr_->generate_random_edge();
//        add_edge(edge.first, edge.second);
//    }

    Constraint::SatisfactionVerdict ConstrainedGraph::check() {
        return constraint_list_ptr_->check();
    }
}
