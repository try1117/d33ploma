#ifndef GRAPH_CONSTRAINT_SOLVER_GRAPH_H
#define GRAPH_CONSTRAINT_SOLVER_GRAPH_H

#include <memory>
#include <functional>
#include <vector>

namespace graph_constraint_solver {

    // TODO: directed/undirected graph
    // in future think about using boost::graph or something like that
    // now it is primarily undirected graph
    class Graph {
    public:
        explicit Graph(int order = 0);
        // number of vertices
        int order();
        // number of undirected edges
        int size();
        const std::vector<std::vector<int>>& adjacency_list();

        bool empty();
        bool is_leaf(int v);

        void add_undirected_edge(int u, int v);
        void add_directed_edge(int u, int v);

        std::pair<int, int> generate_random_directed_edge();
        std::pair<int, int> generate_random_undirected_edge();

    private:
        int order_;
        int size_;
        std::vector<std::vector<int>> g_;
        std::vector<std::vector<bool>> ma_;
    };

    using GraphPtr = std::shared_ptr<Graph>;
}

#endif //GRAPH_CONSTRAINT_SOLVER_GRAPH_H
