#include "graph.h"

#include "utils.h"

namespace graph_constraint_solver {
    Graph::Graph(int order)
        : order_(order), size_(0),
        g_(order, std::vector<int>()), ma_(order, std::vector<bool>(order)) {

    }

    bool Graph::empty() {
        return order_ == 0;
    }

    bool Graph::is_leaf(int v) {
        return g_[v].size() == 1;
    }

    int Graph::order() {
        return order_;
    }

    int Graph::size() {
        return size_;
    }

    const std::vector<std::vector<int>>& Graph::adjacency_list() {
        return g_;
    }

    void Graph::add_undirected_edge(int u, int v) {
        add_directed_edge(u, v);
        add_directed_edge(v, u);
        ++size_;
    }

    void Graph::add_directed_edge(int u, int v) {
        g_[u].emplace_back(v);
        ma_[u][v] = true;
    }

    std::pair<int, int> Graph::generate_random_directed_edge() {
        int u = random.next(order_);
        while (g_[u].size() == order_) {
            u = random.next(order_);
//            ++edge_pick_iterations;
        }

        int v = random.next(order_);
        while (ma_[u][v]) {
            v = random.next(order_);
//            ++edge_pick_iterations;
        }
        return {u, v};
    }

    std::pair<int, int> Graph::generate_random_undirected_edge() {
        return generate_random_directed_edge();
    }
}