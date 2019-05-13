#include "graph.h"

namespace graph_constraint_solver {
    Graph::Graph(int n) : n(n), g(n, std::vector<int>()), ma(n, std::vector<bool>(n))
    {
    }

    bool Graph::empty() {
        return n == 0;
    }

    void Graph::add_undirected_edge(int u, int v) {
        add_directed_edge(u, v);
        add_directed_edge(v, u);
    }


    void Graph::add_directed_edge(int u, int v) {
        g[u].emplace_back(v);
        ma[u][v] = true;
    }

    void Graph::add_random_edge(std::function<void(int, int)> notify) {
        int u = rng() % n;
        while (g[u].size() == n) u = rng() % n, ++edge_pick_iterations;

        int v = rng() % n;
        while (ma[u][v]) v = rng() % n, ++edge_pick_iterations;

        add_undirected_edge(u, v);
    }

    bool Graph::is_leaf(int v) {
        return g[v].size() == 1;
    }
}