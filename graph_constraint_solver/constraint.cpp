#include "constraint.h"

#include "utils.h"

namespace graph_constraint_solver {

    Constraint::Constraint(ConstraintType type)
        : type_(type), graph_ptr_(nullptr) {

    }

    ConstraintType Constraint::type() {
        return type_;
    }

    void Constraint::bind_graph(GraphPtr graph_ptr) {
        graph_ptr_ = graph_ptr;
    }

    void Constraint::add_directed_edge(int from, int to) {

    }

    void Constraint::add_undirected_edge(int from, int to) {

    }

    std::pair<int, int> Constraint::recommend_directed_edge() {
        return graph_ptr_->generate_random_directed_edge();
    }

    std::pair<int, int> Constraint::recommend_undirected_edge() {
        return graph_ptr_->generate_random_undirected_edge();
    }

    TreeConstraint::TreeConstraint()
        : Constraint(kTree), latest_connected_vertex_(0) {

    }

    std::pair<int, int> TreeConstraint::recommend_directed_edge() {
        // TODO: check for latest_connected_vertex_ == n - 1
        return {Random::next(latest_connected_vertex_ + 1), latest_connected_vertex_ + 1};
    }

    std::pair<int, int> TreeConstraint::recommend_undirected_edge() {
        return recommend_directed_edge();
    }

    void TreeConstraint::add_directed_edge(int from, int to) {
        // TODO: normal version
        latest_connected_vertex_ = std::max(latest_connected_vertex_, std::max(from, to));
    }

    void TreeConstraint::add_undirected_edge(int from, int to) {
        latest_connected_vertex_ = std::max(latest_connected_vertex_, std::max(from, to));
    }

    ConstraintSatisfactionVerdict TreeConstraint::check() {
        if (latest_connected_vertex_ > graph_ptr_->n - 1) return kImpossible;
        if (latest_connected_vertex_ < graph_ptr_->n - 1) return kPossible;
        return kOK;
    }

    ConstraintSatisfactionVerdict TreeConstraint::check(GraphPtr g) {
        // TODO: check whether given graph is a tree or not
        return kImpossible;
    }

//    void BridgeConstraint::add_directed_edge(int from, int to) {
//
//    }

    BridgeConstraint::BridgeConstraint(int left_bound, int right_bound)
        : Constraint(kBridge), left_bound_(left_bound), right_bound_(right_bound) {

    }

//    void BridgeConstraint::bind_graph(GraphPtr graph_ptr) {
//        graph_ptr_ = graph_ptr;
//    }

    ConstraintSatisfactionVerdict BridgeConstraint::check() {
        return check(graph_ptr_);
    }

    ConstraintSatisfactionVerdict BridgeConstraint::check(GraphPtr graph_ptr) {
        auto t = count_bridges(graph_ptr);
        if (t.first < left_bound_) return kImpossible;
        if (t.first > right_bound_) return kPossible;
        return kOK;
    }

    int BridgeConstraint::_count_bridges(GraphPtr graph_ptr, int v, int pr, std::pair<int, int> &res, bool save_bridges) {
        tin[v] = fup[v] = ++timer;
        for (auto child : graph_ptr->g[v]) {
            if (child != pr) {
                if (!tin[child]) {
                    _count_bridges(graph_ptr, child, v, res, save_bridges);
                    if (fup[child] > tin[v]) {
                        ++res.first;
                        if (!graph_ptr->is_leaf(v) && !graph_ptr->is_leaf(child)) {
                            ++res.second;
                        }
                        if (save_bridges) {
                            bridges_list.emplace(std::min(v, child), std::max(v, child));
                        }
                    }
                    fup[v] = std::min(fup[v], fup[child]);
                }
                else {
                    fup[v] = std::min(fup[v], tin[child]);
                }
            }
        }
    }

    std::pair<int, int> BridgeConstraint::count_bridges(GraphPtr graph_ptr, bool save_bridges) {
        timer = 0;
        fill(tin.begin(), tin.end(), 0);
        fill(fup.begin(), fup.end(), 0);
        std::pair<int, int> res;
        _count_bridges(graph_ptr, 0, -1, res, save_bridges);
        return res;
    }

    ConstraintList::ConstraintList()
        : Constraint(kNone) {

    }

    void ConstraintList::add_constraint(ConstraintPtr constraint_ptr) {
        constraints_[constraint_ptr->type()] = constraint_ptr;
    }

    std::map<ConstraintType, ConstraintPtr> ConstraintList::constraints() {
        return constraints_;
    }

    void ConstraintList::bind_graph(GraphPtr graph_ptr) {
        for (auto &c : constraints_) {
            c.second->bind_graph(graph_ptr);
        }
    }

    void ConstraintList::add_directed_edge(int from, int to) {
        for (auto &c : constraints_) {
            c.second->add_directed_edge(from, to);
        }
    }

    void ConstraintList::add_undirected_edge(int from, int to) {
        for (auto &c : constraints_) {
            c.second->add_undirected_edge(from, to);
        }
    }

    ConstraintSatisfactionVerdict ConstraintList::check() {
        ConstraintSatisfactionVerdict res = kOK;
        for (auto &c : constraints_) {
            res = std::min(res, c.second->check());
        }
        return res;
    }

    ConstraintSatisfactionVerdict ConstraintList::check(GraphPtr graph_ptr) {
        ConstraintSatisfactionVerdict res = kOK;
        for (auto &c : constraints_) {
            res = std::min(res, c.second->check(graph_ptr));
        }
        return res;
    }


}
