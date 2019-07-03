#include "graph.h"

#include <algorithm>

#include "utils.h"

namespace graph_constraint_solver {

    // Graph

    GraphPtr Graph::create(OrderType order, Graph::Type type) {
        if (type == Type::kDirected) {
            return std::make_shared<DirectedGraph>(order);
        }
        return std::make_shared<UndirectedGraph>(order);
    }

    Graph::Graph(OrderType order, Graph::Type type)
        : type_(type), order_(order), size_(0),
          adjacency_list_(order, std::vector<OrderType>()) {
//        ma_(order, std::vector<bool>(order)) {

    }

    Graph::Type Graph::type() {
        return type_;
    }

    bool Graph::empty() {
        return order_ == 0;
    }

    Graph::OrderType Graph::vertex_degree(OrderType index) {
        return adjacency_list_[index].size();
    }

    Graph::OrderType Graph::order() {
        return order_;
    }

    Graph::SizeType Graph::size() {
        return size_;
    }

    const std::vector<std::vector<Graph::OrderType>>& Graph::adjacency_list() {
        return adjacency_list_;
    }

    void Graph::add_edge(EdgeType e) {
        add_edge(e.first, e.second);
    }

    void Graph::add_edges(const std::vector<EdgeType> &edges) {
        for (auto &edge : edges) {
            add_edge(edge.first, edge.second);
        }
    }

    void Graph::append_graph(GraphPtr other, size_t shift, std::vector<std::pair<size_t, size_t>> except_vertices) {
        if (shift + other->order() > order_) {
            throw std::runtime_error("append_graph error: can't append graph(not enough space)");
        }
        auto transform_index = [&](size_t index) {
            for (size_t i = 0; i < except_vertices.size(); ++i) {
                if (except_vertices[i].first == index) {
                    return except_vertices[i].second;
                }
            }
            for (auto i = static_cast<Graph::OrderType>(except_vertices.size()) - 1; i >= 0; --i) {
                if (index > except_vertices[i].first) {
                    return shift + index - i - 1;
                }
            }
            return shift + index;
        };

        auto adj_list = other->adjacency_list();
        for (size_t i = 0; i < other->order(); ++i) {
            auto ii = transform_index(i);
            for (auto edge : adj_list[i]) {
                auto jj = transform_index(edge);
                adjacency_list_[ii].emplace_back(jj);
            }
        }
        size_ += other->size();
    }

    void Graph::shuffle() {
        std::vector<std::vector<OrderType>> new_g(order_);
        std::vector<OrderType> index_map(order_);
        std::iota(index_map.begin(), index_map.end(), 0);
        std::shuffle(index_map.begin(), index_map.end(), random.rng());
        for (OrderType i = 0; i < order_; ++i) {
            for (auto j : adjacency_list_[i]) {
                new_g[index_map[i]].push_back(index_map[j]);
            }
        }
        adjacency_list_ = new_g;
    }

    void Graph::shrink_order(size_t new_order) {
        order_ = new_order;
        adjacency_list_.resize(new_order);
    }

    size_t Graph::pick_anchor() {
        return random.next(order_);
    }

    std::pair<size_t, size_t> Graph::pick_two_anchors() {
        // ZALEPA
        return std::make_pair(0, order_ - 1);

        auto first = pick_anchor();
        auto second = pick_anchor();
        while (first == second) {
            second = pick_anchor();
        }
        return std::make_pair(first, second);
    }

    // UndirectedGraph

    UndirectedGraph::UndirectedGraph(OrderType order)
        : Graph(order, Type::kUndirected) {

    }

    GraphPtr UndirectedGraph::clone() {
        return std::make_shared<UndirectedGraph>(*this);
    }

    void UndirectedGraph::add_edge(OrderType from, OrderType to) {
        adjacency_list_[from].emplace_back(to);
        adjacency_list_[to].emplace_back(from);
        ++size_;
    }

    // DirectedGraph

    DirectedGraph::DirectedGraph(OrderType order)
        : Graph(order, Type::kDirected) {

    }

    GraphPtr DirectedGraph::clone() {
        return std::make_shared<DirectedGraph>(*this);
    }

    void DirectedGraph::add_edge(OrderType from, OrderType to) {
        adjacency_list_[from].emplace_back(to);
        ++size_;
//        ma_.emplace(u, v);
//        ma_[u][v] = true;
    }

//    std::pair<int, int> Graph::generate_random_directed_edge() {
//        int u = random.next(order_);
//        while (g_[u].size() == order_) {
//            u = random.next(order_);
////            ++edge_pick_iterations;
//        }
//
//        int v = random.next(order_);
//        while (ma_.count({u, v})) {
//            v = random.next(order_);
////            ++edge_pick_iterations;
//        }
//        return {u, v};
//    }

}