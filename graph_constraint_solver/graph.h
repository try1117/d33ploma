#ifndef GRAPH_CONSTRAINT_SOLVER_GRAPH_H
#define GRAPH_CONSTRAINT_SOLVER_GRAPH_H

#include <memory>
#include <functional>
#include <vector>

namespace graph_constraint_solver {

    class Graph {
    public:
        explicit Graph(int n = 0);
        bool empty();
        bool is_leaf(int v);
        int order();
        const std::vector<std::vector<int>>& edges();

        void add_undirected_edge(int u, int v);
        void add_directed_edge(int u, int v);

        std::pair<int, int> generate_random_directed_edge();
        std::pair<int, int> generate_random_undirected_edge();

    private:
        int n_;
        std::vector<std::vector<int>> g_;
        std::vector<std::vector<bool>> ma_;
    };

    using GraphPtr = std::shared_ptr<Graph>;
}

#endif //GRAPH_CONSTRAINT_SOLVER_GRAPH_H
