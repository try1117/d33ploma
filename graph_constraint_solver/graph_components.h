#ifndef GRAPH_CONSTRAINT_SOLVER_GRAPH_COMPONENTS_H
#define GRAPH_CONSTRAINT_SOLVER_GRAPH_COMPONENTS_H

#include <vector>

#include "graph.h"
#include "graph_printer.h"

namespace graph_constraint_solver {
    class GraphComponents {
    public:
        GraphComponents();
        GraphComponents(std::vector<GraphPtr> &components);
        std::vector<GraphPtr>& components();
        std::shared_ptr<GraphComponents> clone();
        bool empty();
        void add_component(GraphPtr component_ptr);
        GraphPtr get_component(Graph::OrderType index);

        GraphPtr merge_components();
        void print(GraphPrinter::OutputFormat output_format, bool debug);

    private:
        std::vector<GraphPtr> components_;
    };

    using GraphComponentsPtr = std::shared_ptr<GraphComponents>;
}

#ifdef GRAPH_CONSTRAINT_SOLVER_SINGLE_HEADER
#include "graph_components.cpp"
#endif

#endif //GRAPH_CONSTRAINT_SOLVER_GRAPH_COMPONENTS_H
