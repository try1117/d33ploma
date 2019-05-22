#include "graph.h"

#include "utils.h"

namespace graph_constraint_solver {
    Graph::Graph(int order, Graph::Type type)
        : type_(type), order_(order), size_(0),
        greatest_used_vertex_index_(-1),
        g_(order, std::vector<int>()), ma_(order, std::vector<bool>(order)) {

    }

    Graph::Graph(int order, Type type, std::vector<std::pair<int, int>> edges)
        : type_(type), order_(order), size_(0),
        greatest_used_vertex_index_(order - 1),
        g_(order, std::vector<int>()), ma_(order, std::vector<bool>(order)) {

        for (auto &edge : edges) {
            add_edge(edge.first, edge.second);
        }
    }

    Graph::Type Graph::type() {
        return type_;
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

    void Graph::add_edge(int u, int v) {
        greatest_used_vertex_index_ = std::max(greatest_used_vertex_index_, std::max(u, v));
        if (type_ == Type::kDirected) {
            add_directed_edge(u, v);
        }
        if (type_ == Type::kUndirected) {
            add_undirected_edge(u, v);
        }
    }

    std::pair<int, int> Graph::generate_random_edge() {
        if (type_ == Type::kDirected) {
            return generate_random_directed_edge();
        }
        if (type_ == Type::kUndirected) {
            return generate_random_undirected_edge();
        }
    }

    void Graph::append_graph(GraphPtr other) {
        // TODO: exception?
        if (greatest_used_vertex_index_ + other->order() == order_) {
            throw std::exception();
        }
        int base = greatest_used_vertex_index_ + 1;
        greatest_used_vertex_index_ += other->order();

        auto adj_list = other->adjacency_list();
        for (int i = 0; i < other->order(); ++i) {
            g_[base + i] = adj_list[i];
            for (auto edge : adj_list[i]) {
                ma_[base + i][base + edge] = true;
            }
        }
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