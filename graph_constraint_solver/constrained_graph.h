#ifndef GRAPH_CONSTRAINT_SOLVER_CONSTRAINED_GRAPH_H
#define GRAPH_CONSTRAINT_SOLVER_CONSTRAINED_GRAPH_H

#include <vector>

#include "graph.h"
#include "constraint.h"
#include "constraint_block.h"

namespace graph_constraint_solver {

    class ConstrainedGraph {
    public:
        ConstrainedGraph();
        ConstrainedGraph(ConstraintBlockPtr constraint_list_ptr, GraphPtr graph_ptr);
        ConstrainedGraph(ConstrainedGraph &other);
        ConstraintBlockPtr constraint_list_ptr();
        GraphPtr graph_ptr();

        void add_edge(int from, int to);
        void add_random_edge();
        bool empty();
        void append_graph(GraphPtr other);

        Constraint::SatisfactionVerdict check();

    private:
        GraphPtr graph_ptr_;
        ConstraintBlockPtr constraint_list_ptr_;
    };

    using ConstrainedGraphPtr = std::shared_ptr<ConstrainedGraph>;
}

#endif //GRAPH_CONSTRAINT_SOLVER_CONSTRAINED_GRAPH_H
