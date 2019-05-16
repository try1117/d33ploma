#include "graph.h"

#include "utils.h"

namespace graph_constraint_solver {
    Graph::Graph(int n)
        : n_(n), g_(n, std::vector<int>()), ma_(n, std::vector<bool>(n)) {

    }

    bool Graph::empty() {
        return n_ == 0;
    }

    bool Graph::is_leaf(int v) {
        return g_[v].size() == 1;
    }

    int Graph::order() {
        return n_;
    }

    const std::vector<std::vector<int>>& Graph::edges() {
        return g_;
    }

    void Graph::add_undirected_edge(int u, int v) {
        add_directed_edge(u, v);
        add_directed_edge(v, u);
    }

    void Graph::add_directed_edge(int u, int v) {
        g_[u].emplace_back(v);
        ma_[u][v] = true;
    }

    std::pair<int, int> Graph::generate_random_directed_edge() {
        int u = Random::next(n_);
        while (g_[u].size() == n_) {
            u = Random::next(n_);
//            ++edge_pick_iterations;
        }

        int v = Random::next(n_);
        while (ma_[u][v]) {
            v = Random::next(n_);
//            ++edge_pick_iterations;
        }
        return {u, v};
    }

    std::pair<int, int> Graph::generate_random_undirected_edge() {
        return generate_random_directed_edge();
    }
}