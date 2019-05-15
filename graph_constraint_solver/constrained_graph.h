#ifndef GRAPH_CONSTRAINT_SOLVER_CONSTRAINED_GRAPH_H
#define GRAPH_CONSTRAINT_SOLVER_CONSTRAINED_GRAPH_H

#include <vector>

#include "graph.h"
#include "constraint.h"

namespace graph_constraint_solver {

    class ConstrainedGraph {
    public:
        ConstrainedGraph();
        ConstrainedGraph(ConstraintListPtr constraint_list_ptr, GraphPtr graph_ptr);
        ConstraintListPtr constraint_list_ptr();

        void add_directed_edge(int from, int to);
        void add_undirected_edge(int from, int to);
        void add_random_directed_edge();
        void add_random_undirected_edge();
        bool empty();
        ConstraintSatisfactionVerdict check();

    private:
        GraphPtr graph_ptr_;
        ConstraintListPtr constraint_list_ptr_;
    };

    using ConstrainedGraphPtr = std::shared_ptr<ConstrainedGraph>;
}

#endif //GRAPH_CONSTRAINT_SOLVER_CONSTRAINED_GRAPH_H
