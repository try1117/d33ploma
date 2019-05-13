#include "constraint.h"

namespace graph_constraint_solver {
    void BridgeConstraint::add_directed_edge(int from, int to) {

    }

    BridgeConstraint::BridgeConstraint(int l_bound, int r_bound, GraphPtr graph_ptr)
        : l_bound(l_bound), r_bound(r_bound), graph_ptr(graph_ptr) {

    }

    ConstraintSatisfactionVerdict BridgeConstraint::check() {
        return check(graph_ptr);
    }

    ConstraintSatisfactionVerdict BridgeConstraint::check(GraphPtr graph_ptr) {
        auto t = count_bridges(graph_ptr);
        if (t.first < l_bound) return kFail;
        if (t.first > r_bound) return kNotYet;
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
}
