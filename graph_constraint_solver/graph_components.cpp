#include "graph_components.h"

namespace graph_constraint_solver {
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

    GraphPtr GraphComponents::merge_components() {
        if (components_.empty()) return std::make_shared<UndirectedGraph>();
        size_t total_order = 0;
        for (auto component : components_) {
            total_order += component->order();
        }
        GraphPtr graph = Graph::create(total_order, components_.front()->type());
        size_t current_order = 0;
        for (auto component : components_) {
            graph->append_graph(component, current_order);
            current_order += component->order();
        }
        return graph;
    }

    void GraphComponents::print(GraphPrinter::OutputFormat output_format, bool debug) {
        auto graph = merge_components();
        GraphPrinter::print(graph, output_format, debug);
    }
}