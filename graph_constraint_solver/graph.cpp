#include "graph.h"

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

    GraphPtr GraphComponents::get_component(Graph::OrderType index) {
        if (index < 0 || index + 1 > components_.size()) {
            throw std::runtime_error("GraphComponents error: index out of range");
        }
        return components_.at(index);
    }
}