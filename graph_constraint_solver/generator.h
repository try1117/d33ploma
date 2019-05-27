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
        ConstrainedGraphPtr generate(ConstraintBlockPtr constraint_list_ptr);

        ConstrainedGraphPtr go_with_the_winners(GraphGenerator initial_graph_generator, GoNext go_next, bool to_print = false,
                int colony_size = 5, int growth_rate = 2, int outer_iterations = 100, int inner_iterations = 10000);

        ConstrainedGraphPtr generate_block(ConstraintBlockPtr constraint_list_ptr);

        ConstrainedGraphPtr generate_single_component(ConstraintBlockPtr constraint_list_ptr);
        ConstrainedGraphPtr generate_tree(int order, ConstraintBlockPtr constraint_list_ptr);

    private:
        std::vector<std::pair<int, int>> generate_2connected_graph(int order, int size, int components_number,
                std::pair<int, int> components_order_bounds);

        std::vector<std::pair<int, int>> generate_2connected_component(int order, int size,
                std::pair<int, int> components_order_bounds);
    };
}

#endif //GRAPH_CONSTRAINT_SOLVER_GENERATOR_H
