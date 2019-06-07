#ifndef GRAPH_CONSTRAINT_SOLVER_GRAPH_ALGORITHMS_H
#define GRAPH_CONSTRAINT_SOLVER_GRAPH_ALGORITHMS_H

#include "graph.h"

namespace graph_constraint_solver {
    class GraphAlgorithms {
    public:
        static void find_bridges(GraphPtr graph_ptr, std::pair<int, int> &bridges_number,
                std::vector<std::pair<int, int>> &bridges_list);

        static void find_cut_points(GraphPtr graph_ptr, int &cut_points_number,
                std::vector<int> &cut_points_list);
    };

    namespace impl {
        class BridgeAlgorithm {
        public:
            BridgeAlgorithm(GraphPtr graph_ptr, std::pair<int, int> &bridges_number,
                    std::vector<std::pair<int, int>> &bridges_list);

        private:
            void find_bridges(int v, int pr);
            int timer_;
            std::vector<int> tin_, fup_;
            GraphPtr graph_ptr_;
            std::pair<int, int> &bridges_number_;
            std::vector<std::pair<int, int>> &bridges_list_;
        };

        class CutPointAlgorithm {
        public:
            CutPointAlgorithm(GraphPtr graph_ptr, int &cut_points_number,
                    std::vector<int> &cut_points_list);

        private:
            void find_cut_points(int v, int pr);
            int timer_;
            std::vector<int> tin_, fup_;
            GraphPtr graph_ptr_;
            int &cut_points_number_;
            std::vector<int> &cut_points_list_;
        };

        bool increase_stack_size();
    }
}

#endif //GRAPH_CONSTRAINT_SOLVER_GRAPH_ALGORITHMS_H
