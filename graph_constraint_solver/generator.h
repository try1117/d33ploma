#ifndef GRAPH_CONSTRAINT_SOLVER_GENERATOR_H
#define GRAPH_CONSTRAINT_SOLVER_GENERATOR_H

#include "graph.h"
#include "constraint.h"

namespace graph_constraint_solver {

    using GoNext = std::function<void(GraphPtr)>;

    class Generator {
        GraphPtr go_with_the_winners(GraphPtr start, GoNext go_next, ConstraintPtr constraint,
                int sz, int mul, int max_iter);

        GraphPtr go_go(int n, GoNext go_next, ConstraintPtr constraint,
                int sz = 2, int mul = 2, int max_iter = 10000);
    };
}

#endif //GRAPH_CONSTRAINT_SOLVER_GENERATOR_H
