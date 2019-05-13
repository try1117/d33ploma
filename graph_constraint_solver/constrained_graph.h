#ifndef GRAPH_CONSTRAINT_SOLVER_CONSTRAINED_GRAPH_H
#define GRAPH_CONSTRAINT_SOLVER_CONSTRAINED_GRAPH_H

#include <vector>

#include "graph.h"
#include "constraint.h"

namespace graph_constraint_solver {

    class ConstrainedGraph {
    public:
//        ConstrainedGraph() = default;

        void add_edge(int from, int to);
        void add_random_edge();

    private:
        GraphPtr graph_ptr;
        std::vector<ConstraintPtr> constraints;
    };
}

#endif //GRAPH_CONSTRAINT_SOLVER_CONSTRAINED_GRAPH_H
