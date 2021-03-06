#ifndef GRAPH_CONSTRAINT_SOLVER_CONSTRAINED_GRAPH_H
#define GRAPH_CONSTRAINT_SOLVER_CONSTRAINED_GRAPH_H

#include <vector>

#include "graph.h"
#include "graph_components.h"
#include "constraint.h"
#include "constraint_block.h"

namespace graph_constraint_solver {

    class ConstrainedGraph {
    public:
        ConstrainedGraph();
        ConstrainedGraph(ConstraintBlockPtr constraint_list_ptr, GraphComponentsPtr components_ptr);
        ConstrainedGraph(ConstrainedGraph &other);
        ConstraintBlockPtr constraint_list_ptr();
        GraphComponentsPtr components_ptr();

        void add_edge(int from, int to);
//        void add_random_edge();
        bool empty();
        void append_graph(GraphPtr other);

        Constraint::SatisfactionVerdict check();

    private:
        std::shared_ptr<GraphComponents> components_ptr_;
        ConstraintBlockPtr constraint_list_ptr_;
    };

    using ConstrainedGraphPtr = std::shared_ptr<ConstrainedGraph>;
}

#ifdef GRAPH_CONSTRAINT_SOLVER_SINGLE_HEADER
#include "constrained_graph.cpp"
#endif

#endif //GRAPH_CONSTRAINT_SOLVER_CONSTRAINED_GRAPH_H
