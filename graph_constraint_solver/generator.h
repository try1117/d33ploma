#ifndef GRAPH_CONSTRAINT_SOLVER_GENERATOR_H
#define GRAPH_CONSTRAINT_SOLVER_GENERATOR_H

#include "graph.h"
#include "constraint.h"
#include "constrained_graph.h"

namespace graph_constraint_solver {

    using GoNext = std::function<void(ConstrainedGraphPtr)>;
    // TODO: may be instead of using a function we need to use Generator.generate()
    using GraphGenerator = std::function<ConstrainedGraphPtr()>;

    class Generator {
    public:
        ConstrainedGraphPtr generate(ConstraintListPtr constraint_list_ptr);

        ConstrainedGraphPtr go_with_the_winners(GraphGenerator initial_graph_generator, GoNext go_next, bool to_print = false,
                int colony_size = 5, int growth_rate = 2, int outer_iterations = 100, int inner_iterations = 10000);

        // we can get rid of first argument, as it's stored in constraint_list_ptr OrderContraint
        ConstrainedGraphPtr generate_single_component(int order, ConstraintListPtr constraint_list_ptr);
        ConstrainedGraphPtr generate_tree(int order, ConstraintListPtr constraint_list_ptr);
    };
}

#endif //GRAPH_CONSTRAINT_SOLVER_GENERATOR_H
