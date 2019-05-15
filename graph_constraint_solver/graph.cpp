#include "graph.h"

#include "utils.h"

namespace graph_constraint_solver {
    Graph::Graph(int n) : n(n), g(n, std::vector<int>()), ma(n, std::vector<bool>(n))
    {
    }

    bool Graph::empty() {
        return n == 0;
    }

    bool Graph::is_leaf(int v) {
        return g[v].size() == 1;
    }

    void Graph::add_undirected_edge(int u, int v) {
        add_directed_edge(u, v);
        add_directed_edge(v, u);
    }

    void Graph::add_directed_edge(int u, int v) {
        g[u].emplace_back(v);
        ma[u][v] = true;
    }

    std::pair<int, int> Graph::generate_random_directed_edge() {
        int u = Random::next(n);
        while (g[u].size() == n) {
            u = Random::next(n);
//            ++edge_pick_iterations;
        }

        int v = Random::next(n);
        while (ma[u][v]) {
            v = Random::next(n);
//            ++edge_pick_iterations;
        }
        return {u, v};
    }

    std::pair<int, int> Graph::generate_random_undirected_edge() {
        return generate_random_directed_edge();
    }
}