#include "graph.h"

#include "utils.h"

namespace graph_constraint_solver {

    // Graph

    Graph::Graph(int order, Graph::Type type)
        : type_(type), order_(order), size_(0),
        g_(order, std::vector<int>()) {
//        ma_(order, std::vector<bool>(order)) {

    }

    Graph::Type Graph::type() {
        return type_;
    }

    bool Graph::empty() {
        return order_ == 0;
    }

    int Graph::vertex_degree(int index) {
        return g_[index].size();
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

//    void Graph::add_edge(int u, int v) {
//        greatest_used_vertex_index_ = std::max(greatest_used_vertex_index_, std::max(u, v));
//        if (type_ == Type::kDirected) {
//            add_directed_edge(u, v);
//        }
//        if (type_ == Type::kUndirected) {
//            add_undirected_edge(u, v);
//        }
//    }

//    std::pair<int, int> Graph::generate_random_edge() {
//        if (type_ == Type::kDirected) {
//            return generate_random_directed_edge();
//        }
//        if (type_ == Type::kUndirected) {
//            return generate_random_undirected_edge();
//        }
//    }

    void Graph::append_graph(GraphPtr other) {
        // TODO: exception?
//        if (greatest_used_vertex_index_ + other->order() == order_) {
//            throw std::exception();
//        }
//        int base = greatest_used_vertex_index_ + 1;
//        greatest_used_vertex_index_ += other->order();
//
//        auto adj_list = other->adjacency_list();
//        for (int i = 0; i < other->order(); ++i) {
//            g_[base + i] = adj_list[i];
//            for (auto edge : adj_list[i]) {
//                ma_.emplace(base + i, base + edge);
////                ma_[base + i][base + edge] = true;
//            }
//        }
    }

    void Graph::shuffle() {
        std::vector<std::vector<int>> new_g(order_);
        std::vector<int> index_map(order_);
        std::iota(index_map.begin(), index_map.end(), 0);
        std::shuffle(index_map.begin(), index_map.end(), random.rng());
        for (int i = 0; i < order_; ++i) {
            for (auto j : g_[i]) {
                new_g[index_map[i]].push_back(index_map[j]);
            }
        }
        g_ = new_g;
    }

    // UndirectedGraph

    UndirectedGraph::UndirectedGraph(int order)
        : Graph(order, Type::kUndirected) {

    }

    UndirectedGraph::UndirectedGraph(int order, const std::vector<std::pair<int, int>> &edges)
            : Graph(order, Type::kUndirected) {
        for (auto &edge : edges) {
            add_edge(edge.first, edge.second);
        }
    }

    GraphPtr UndirectedGraph::clone() {
        return std::make_shared<UndirectedGraph>(*this);
    }

    void UndirectedGraph::add_edge(int u, int v) {
        g_[u].emplace_back(v);
        g_[v].emplace_back(u);
        ++size_;
    }

    // DirectedGraph

    DirectedGraph::DirectedGraph(int order)
        : Graph(order, Type::kDirected) {

    }

    DirectedGraph::DirectedGraph(int order, const std::vector<std::pair<int, int>> &edges)
            : Graph(order, Type::kDirected) {
        for (auto &edge : edges) {
            add_edge(edge.first, edge.second);
        }
    }

    GraphPtr DirectedGraph::clone() {
        return std::make_shared<DirectedGraph>(*this);
    }

    void DirectedGraph::add_edge(int u, int v) {
        g_[u].emplace_back(v);
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
//
//    std::pair<int, int> Graph::generate_random_undirected_edge() {
//        return generate_random_directed_edge();
//    }

    // GraphComponents

    GraphComponents::GraphComponents()
        : components_(std::vector<GraphPtr>()) {

    }

    GraphComponents::GraphComponents(std::vector<GraphPtr> &components)
        : components_(components) {

    }

    std::vector<GraphPtr>& GraphComponents::components() {
        return components_;
    }

    std::shared_ptr<GraphComponents> GraphComponents::clone() {
        std::vector<GraphPtr> components;
        for (auto &component : components_) {
            components.push_back(component->clone());
        }
        return std::make_shared<GraphComponents>(components);
    }

    bool GraphComponents::empty() {
        return components_.empty();
    }

    void GraphComponents::add_component(GraphPtr component_ptr) {
        components_.push_back(component_ptr);
    }

    GraphPtr GraphComponents::get_component(int index) {
        if (index < 0 || index + 1 > components_.size()) {
            throw std::runtime_error("GraphComponents error: index out of range");
        }
        return components_.at(index);
    }
}