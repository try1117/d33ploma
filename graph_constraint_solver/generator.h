#ifndef GRAPH_CONSTRAINT_SOLVER_GENERATOR_H
#define GRAPH_CONSTRAINT_SOLVER_GENERATOR_H

#include "graph.h"
#include "constraint.h"
#include "constrained_graph.h"
#include "utils.h"

namespace graph_constraint_solver {

    using GoNext = std::function<void(ConstrainedGraphPtr)>;
    // TODO: may be instead of using a function we need to use Generator.generate()
    using GraphGenerator = std::function<ConstrainedGraphPtr()>;

    class Generator {
    public:
        ConstrainedGraphPtr generate(ConstraintBlockPtr constraint_list_ptr);
        ConstrainedGraphPtr generate_block(ConstraintBlockPtr constraint_block_ptr);

        ConstrainedGraphPtr generate_two_connected_block(std::shared_ptr<TwoConnectedBlock> constraint_block_ptr);
        ConstrainedGraphPtr generate_two_edge_connected_block(std::shared_ptr<TwoEdgeConnectedBlock> constraint_block_ptr,
                bool check_satisfiability_only = false);
        ConstrainedGraphPtr generate_connected_block(std::shared_ptr<ConnectedBlock> constraint_block_ptr);
        ConstrainedGraphPtr generate_tree_block(std::shared_ptr<TreeBlock> constraint_block_ptr);
//        ConstrainedGraphPtr generate_single_component(ConstraintBlockPtr constraint_list_ptr);

        GraphPtr generate_tree_fixed_leaves_number(int order, int leaves_number, double merge_probability);
    private:
        ConstrainedGraphPtr go_with_the_winners(GraphGenerator initial_graph_generator, GoNext go_next, bool to_print = false,
                int colony_size = 5, int growth_rate = 2, int outer_iterations = 100, int inner_iterations = 10000);

        GraphPtr generate_tree(Graph::Type graph_type, std::pair<int, int> order_bounds, std::pair<int, int> diameter,
                int max_vertex_degree);
        GraphPtr generate_tree_fixed_diameter(Graph::Type graph_type, int order, int diameter, int max_vertex_degree);

        GraphComponentsPtr generate_two_connected_graph(int order, int size, int components_number,
                std::pair<int, int> components_order_bounds);
        GraphPtr generate_two_connected_component(int order, int size, std::pair<int, int> components_order_bounds);

        GraphPtr generate_two_edge_connected_component(int order, std::pair<long long, long long> size_bounds, int cut_points);
        GraphPtr generate_connected_component(Utils::IntPair order_bounds, Utils::LongPair size_bounds,
                Utils::IntPair cut_point_bounds, int bridges);

        void connect_components_in_vertices(GraphPtr graph, GraphComponentsPtr components, GraphPtr skeleton);
        void connect_components_dfs(GraphPtr graph, GraphComponentsPtr components, GraphPtr skeleton,
                std::vector<std::vector<std::pair<int, int>>> &selected_vertices, int &next_free_index,
                int current_component_index, int previous_component_index, int link_vertex);
    };
}

#endif //GRAPH_CONSTRAINT_SOLVER_GENERATOR_H
