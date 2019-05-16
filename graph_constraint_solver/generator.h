#ifndef GRAPH_CONSTRAINT_SOLVER_GENERATOR_H
#define GRAPH_CONSTRAINT_SOLVER_GENERATOR_H

#include "graph.h"
#include "constraint.h"
#include "constrained_graph.h"

namespace graph_constraint_solver {

    using GoNext = std::function<void(ConstrainedGraphPtr)>;

    class Generator {
    public:
        ConstrainedGraphPtr generate(ConstraintListPtr constraint_list_ptr);

        ConstrainedGraphPtr go_with_the_winners(ConstrainedGraphPtr start_graph_ptr, GoNext go_next,
                int colony_size = 1, int growth_rate = 2, int outer_iterations = 100, int inner_iterations = 10000);

        ConstrainedGraphPtr single_component_generator(int n, ConstraintListPtr constraint_list_ptr);
    };
}

#endif //GRAPH_CONSTRAINT_SOLVER_GENERATOR_H
