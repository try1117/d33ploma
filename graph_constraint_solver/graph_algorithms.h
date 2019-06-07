#ifndef GRAPH_CONSTRAINT_SOLVER_GRAPH_ALGORITHMS_H
#define GRAPH_CONSTRAINT_SOLVER_GRAPH_ALGORITHMS_H

#include "graph.h"

namespace graph_constraint_solver {
    class GraphAlgorithms {
    public:
        static void count_bridges(GraphPtr graph_ptr, std::pair<int, int> &bridges_number,
                std::vector<std::pair<int, int>> &bridges_list);
    };

    namespace impl {
        class BridgeAlgorithm {
        public:
            BridgeAlgorithm(GraphPtr graph_ptr, std::pair<int, int> &bridges_number,
                    std::vector<std::pair<int, int>> &bridges_list);

        private:
            void count_bridges(GraphPtr graph_ptr, int v, int pr);
            int timer_;
            std::vector<int> tin_, fup_;
            std::pair<int, int> &bridges_number_;
            std::vector<std::pair<int, int>> &bridges_list_;
        };

        bool increase_stack_size();
    }
}

#endif //GRAPH_CONSTRAINT_SOLVER_GRAPH_ALGORITHMS_H
