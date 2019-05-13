#ifndef GRAPH_CONSTRAINT_SOLVER_GRAPH_H
#define GRAPH_CONSTRAINT_SOLVER_GRAPH_H

#include <memory>
#include <functional>
#include <vector>

namespace graph_constraint_solver {

    class Graph {
    public:
        int n;
        std::vector<std::vector<int>> g;
        std::vector<std::vector<bool>> ma;

        Graph(int n = 0);
        bool empty();

        void add_undirected_edge(int u, int v);
        void add_directed_edge(int u, int v);
        void add_random_edge(std::function<void(int, int)> notify);

        bool is_leaf(int v);

        // TODO: move this to a separate entity?
        // this functions are imposed on graph by specific constraints
        // in this case constrains related to bridges
        // there will be more constraints, that may or may not have common features
//        int _count_bridges(int v, int pr, std::pair<int, int> &res, bool save_bridges = false);
//        std::pair<int, int> count_bridges(bool save_bridges = false);
    };

    using GraphPtr = std::shared_ptr<Graph>;
}

#endif //GRAPH_CONSTRAINT_SOLVER_GRAPH_H
