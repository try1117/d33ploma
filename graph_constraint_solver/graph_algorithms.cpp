#include "graph_algorithms.h"

#include <sys/resource.h>

namespace graph_constraint_solver {
    void GraphAlgorithms::count_bridges(GraphPtr graph_ptr, std::pair<int, int> &bridges_number,
            std::vector<std::pair<int, int>> &bridges_list) {

        impl::BridgeAlgorithm(graph_ptr, bridges_number, bridges_list);
    }

    namespace impl {

        // from stackoverflow
        // https://stackoverflow.com/questions/2275550/change-stack-size-for-a-c-application-in-linux-during-compilation-with-gnu-com/2284691#2284691
        bool increase_stack_size() {
            const rlim_t kStackSize = 64 * 1024 * 1024;
            struct rlimit rl;
            int result;

            result = getrlimit(RLIMIT_STACK, &rl);
            if (result == 0)
            {
                if (rl.rlim_cur < kStackSize)
                {
                    rl.rlim_cur = kStackSize;
                    result = setrlimit(RLIMIT_STACK, &rl);
                    if (result != 0) return false;
                }
            }
            return true;
        }

        BridgeAlgorithm::BridgeAlgorithm(graph_constraint_solver::GraphPtr graph_ptr, std::pair<int, int> &bridges_number,
                std::vector<std::pair<int, int>> &bridges_list)
                : bridges_number_(bridges_number), bridges_list_(bridges_list),
                timer_(0), tin_(graph_ptr->order()), fup_(graph_ptr->order()) {

            for (int i = 0; i < graph_ptr->order(); ++i) {
                if (!tin_[i]) {
                    count_bridges(graph_ptr, i, -1);
                }
            }
        }

        void BridgeAlgorithm::count_bridges(GraphPtr graph_ptr, int v, int pr) {
            tin_[v] = fup_[v] = ++timer_;
            for (auto child : graph_ptr->adjacency_list()[v]) {
                if (child != pr) {
                    if (!tin_[child]) {
                        count_bridges(graph_ptr, child, v);
                        if (fup_[child] > tin_[v]) {
                            ++bridges_number_.first;
                            // not a leaf
                            if (graph_ptr->adjacency_list()[v].size() != 1 &&
                                graph_ptr->adjacency_list()[child].size() != 1) {
                                ++bridges_number_.second;
                            }
                            bridges_list_.emplace_back(std::min(v, child), std::max(v, child));
                        }
                        fup_[v] = std::min(fup_[v], fup_[child]);
                    }
                    else {
                        fup_[v] = std::min(fup_[v], tin_[child]);
                    }
                }
            }
        }

    } //impl
} //graph_constraint_solver