#ifndef GRAPH_CONSTRAINT_SOLVER_GENERATOR_H
#define GRAPH_CONSTRAINT_SOLVER_GENERATOR_H

#include "graph.h"
#include "constraint.h"
#include "constrained_graph.h"
#include "utils.h"

namespace graph_constraint_solver {

    class Generator {
    public:
        GraphComponentsPtr generate(ConstraintBlockPtr constraint_list_ptr);
        GraphComponentsPtr generate_block(ConstraintBlockPtr constraint_block_ptr);

        GraphComponentsPtr generate_two_connected_block(std::shared_ptr<TwoConnectedBlock> constraint_block_ptr);
        GraphComponentsPtr generate_two_edge_connected_block(std::shared_ptr<TwoEdgeConnectedBlock> constraint_block_ptr,
                bool check_satisfiability_only = false);
        GraphComponentsPtr generate_connected_block(std::shared_ptr<ConnectedBlock> constraint_block_ptr);
        GraphComponentsPtr generate_tree_block(std::shared_ptr<TreeBlock> constraint_block_ptr);

        GraphComponentsPtr generate_strongly_connected_block(std::shared_ptr<StronglyConnectedBlock> constraint_block_ptr);

    private:
        // TODO: remove this 'go_with_the_winners' thing???
        using GoNext = std::function<void(ConstrainedGraphPtr)>;
        using GraphGenerator = std::function<ConstrainedGraphPtr()>;
        ConstrainedGraphPtr go_with_the_winners(GraphGenerator initial_graph_generator, GoNext go_next, bool to_print = false,
                int colony_size = 5, int growth_rate = 2, int outer_iterations = 100, int inner_iterations = 10000);

        GraphPtr generate_tree(Graph::Type graph_type, Constraint::OrderBounds order_bounds,
                Constraint::OrderBounds diameter_bounds, Graph::OrderType max_vertex_degree);

        GraphPtr generate_tree_fixed_diameter(Graph::Type graph_type, Graph::OrderType order, Graph::OrderType diameter,
                Graph::OrderType max_vertex_degree);
        GraphPtr generate_tree_fixed_leaves_number(Graph::OrderType order, Graph::OrderType leaves_number,
                double merge_probability = 0.3);

        GraphPtr generate_strongly_connected_component(Constraint::OrderBounds order_bounds,
                Constraint::SizeBounds size_bounds);

        GraphComponentsPtr generate_two_connected_graph(Graph::Type graph_type, Graph::OrderType order,
                Graph::SizeType size, Graph::OrderType components_number,
                Constraint::OrderBounds components_order_bounds);

        GraphPtr generate_two_connected_component(Graph::Type graph_type, Graph::OrderType order, Graph::SizeType size,
                Graph::OrderType min_loop_size, double loop_ear_probability = 0.0);

        GraphPtr generate_two_edge_connected_component(Graph::Type graph_type, Graph::OrderType order,
                Constraint::SizeBounds size_bounds, Graph::OrderType cut_points);

        GraphPtr generate_connected_component(Graph::Type graph_type, Constraint::OrderBounds order_bounds,
                Constraint::SizeBounds size_bounds, Constraint::OrderBounds cut_point_bounds, Graph::OrderType bridges);

        void connect_components_in_vertices(GraphPtr graph, GraphComponentsPtr components, GraphPtr skeleton);
        void connect_components_in_vertices_dfs(GraphPtr graph, GraphComponentsPtr components, GraphPtr skeleton,
                std::vector<std::vector<std::pair<int, int>>> &selected_vertices, int &next_free_index,
                int current_component_index, int previous_component_index, int link_vertex);

        void connect_components_with_edges(GraphPtr graph, GraphComponentsPtr components, GraphPtr skeleton);
        void connect_components_with_edges_dfs(GraphPtr graph, GraphComponentsPtr components, GraphPtr skeleton,
                std::vector<std::vector<int>> &local_to_global_index,
                int current_component_index, int previous_component_index);
    };
}

#endif //GRAPH_CONSTRAINT_SOLVER_GENERATOR_H
