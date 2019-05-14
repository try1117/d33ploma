#include "constraint.h"

namespace graph_constraint_solver {
    void BridgeConstraint::add_directed_edge(int from, int to) {

    }

    BridgeConstraint::BridgeConstraint(int l_bound, int r_bound)
        : l_bound(l_bound), r_bound(r_bound), graph_ptr_(nullptr) {

    }

    void BridgeConstraint::bind_graph(GraphPtr graph_ptr) {
        graph_ptr_ = graph_ptr;
    }

    ConstraintSatisfactionVerdict BridgeConstraint::check() {
        return check(graph_ptr_);
    }

    ConstraintSatisfactionVerdict BridgeConstraint::check(GraphPtr graph_ptr) {
        auto t = count_bridges(graph_ptr);
        if (t.first < l_bound) return kImpossible;
        if (t.first > r_bound) return kPossible;
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

    void ConstraintList::add_constraint(ConstraintPtr constraint) {
        constraints_.push_back(constraint);
    }

    const std::vector<ConstraintPtr> ConstraintList::constraints() {
        return constraints_;
    }

    void ConstraintList::bind_graph(GraphPtr graph_ptr) {
        for (auto &c : constraints_) {
            c->bind_graph(graph_ptr);
        }
    }

    void ConstraintList::add_directed_edge(int from, int to) {
        for (auto &c : constraints_) {
            c->add_directed_edge(from, to);
        }
    }

    ConstraintSatisfactionVerdict ConstraintList::check() {
        ConstraintSatisfactionVerdict res = kOK;
        for (auto &c : constraints_) {
            res = std::min(res, c->check());
        }
        return res;
    }
    ConstraintSatisfactionVerdict ConstraintList::check(GraphPtr graph_ptr) {
        ConstraintSatisfactionVerdict res = kOK;
        for (auto &c : constraints_) {
            res = std::min(res, c->check(graph_ptr));
        }
        return res;
    }


}
