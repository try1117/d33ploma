#include "generator.h"

#include <vector>
#include <memory>
#include <iostream>
#include <utility>
#include <unordered_set>
#include <algorithm>

#include "utils.h"

namespace graph_constraint_solver {

    GraphComponentsPtr Generator::generate(ConstraintBlockPtr constraint_list_ptr) {
        return generate_block(constraint_list_ptr);
    }

//    std::vector<int> generate_n_numbers(int n, int sum, std::function<bool(std::vector<int>&)> check) {
//        std::vector<int> result;
//        int minimal_size = 1;
//        for (int i = 0; i < n - 1; ++i) {
//            result.push_back(random.next(minimal_size, sum - minimal_size * (n - i - 1)));
//            sum -= result.back();
//        }
//        result.push_back(sum);
//        return result;
//    }

    GraphComponentsPtr Generator::generate_block(ConstraintBlockPtr constraint_block_ptr) {
        if (constraint_block_ptr->component_type() == ConstraintBlock::ComponentType::kTwoConnected) {
            return generate_two_connected_block(std::static_pointer_cast<TwoConnectedConstraintBlock>(constraint_block_ptr));
        }
        if (constraint_block_ptr->component_type() == ConstraintBlock::ComponentType::kTwoEdgeConnected) {
            return generate_two_edge_connected_block(std::static_pointer_cast<TwoEdgeConnectedConstraintBlock>(constraint_block_ptr));
        }
        if (constraint_block_ptr->component_type() == ConstraintBlock::ComponentType::kConnected) {
            return generate_connected_block(std::static_pointer_cast<ConnectedConstraintBlock>(constraint_block_ptr));
        }
        if (constraint_block_ptr->component_type() == ConstraintBlock::ComponentType::kStronglyConnected) {
            return generate_strongly_connected_block(std::static_pointer_cast<StronglyConnectedConstraintBlock>(constraint_block_ptr));
        }
        if (constraint_block_ptr->component_type() == ConstraintBlock::ComponentType::kTree) {
            return generate_tree_block(std::static_pointer_cast<TreeConstraintBlock>(constraint_block_ptr));
        }
    }

    GraphComponentsPtr Generator::generate_strongly_connected_block(std::shared_ptr<StronglyConnectedConstraintBlock> constraint_block_ptr) {
        auto graph_type = constraint_block_ptr->get_graph_type();
        auto component_number_bounds = constraint_block_ptr->template get_constraint<ComponentNumberConstraint>()->bounds();
        auto component_order_bounds = constraint_block_ptr->template get_constraint<ComponentOrderConstraint>()->bounds();
        auto component_size_bounds = constraint_block_ptr->template get_constraint<ComponentSizeConstraint>()->bounds();

        auto component_number = random.next(component_number_bounds);
        auto components = std::make_shared<GraphComponents>();
        for (Graph::OrderType i = 0; i < component_number; ++i) {
            auto component = generate_strongly_connected_component(component_order_bounds, component_size_bounds);
            components->add_component(component);
        }
        return components;
    }

    GraphComponentsPtr Generator::generate_connected_block(std::shared_ptr<ConnectedConstraintBlock> constraint_block_ptr) {
        auto graph_type = constraint_block_ptr->get_graph_type();
        auto component_number_bounds = constraint_block_ptr->template get_constraint<ComponentNumberConstraint>()->bounds();
        auto component_order_bounds = constraint_block_ptr->template get_constraint<ComponentOrderConstraint>()->bounds();
        auto component_size_bounds = constraint_block_ptr->template get_constraint<ComponentSizeConstraint>()->bounds();
        auto component_cut_point_bounds = constraint_block_ptr->template get_constraint<ComponentCutPointConstraint>()->bounds();
        auto component_bridge_bounds = constraint_block_ptr->template get_constraint<ComponentBridgeConstraint>()->bounds();

        component_cut_point_bounds.second = std::min(component_cut_point_bounds.second, component_order_bounds.second);
        component_bridge_bounds.second = std::min(component_bridge_bounds.second, component_size_bounds.second);
        component_bridge_bounds.second = std::min<Graph::SizeType>(component_bridge_bounds.second, component_order_bounds.second - 1);

        if (Utils::invalid_segment(component_cut_point_bounds) || Utils::invalid_segment(component_bridge_bounds)) {
            throw std::runtime_error("generate_connected_component error: given constraints cannot be satisfied");
        }

        std::vector<Graph::SizeType> suitable_number_of_bridges;
        if (component_bridge_bounds.first == 0) {
            auto two_edge_connected_block_ptr = constraint_block_ptr->template clone<TwoEdgeConnectedConstraintBlock>();
            auto component_number_constraint = two_edge_connected_block_ptr->template get_constraint<ComponentNumberConstraint>();
            component_number_constraint->set_bounds(1, 1);
            try {
                if (generate_two_edge_connected_block(two_edge_connected_block_ptr, true)) {
                    suitable_number_of_bridges.push_back(0);
                }
            }
            catch (...) {}
        }

        auto loop_left_bound = std::max<Graph::SizeType>(1, component_bridge_bounds.first);
        for (Graph::SizeType bridges = loop_left_bound; bridges <= component_bridge_bounds.second; ++bridges) {

            // first we need to generate some tree with 'bridges' edges
            // right_bound of 'initial_cut_point_bounds' is a maximum number of inner vertices we allowed to have in a tree
            auto initial_cut_point_bounds = Constraint::OrderBounds(
                    std::min<Graph::SizeType>(bridges - 1, 1),
                    std::min<Graph::SizeType>(bridges - 1, component_cut_point_bounds.second));

            Graph::OrderType initial_order = bridges + 1;
            Graph::SizeType initial_size = bridges;

            if (initial_cut_point_bounds.second == 0) {
                if (bridges == 1 && Utils::in_range(component_order_bounds, 2) &&
                        Utils::in_range(component_size_bounds, 1)) {
                    suitable_number_of_bridges.push_back(1);
                }
                continue;
            }

            // now we need to check whether we can or not ...
            Graph::OrderType need_order = std::max(0, component_order_bounds.first - initial_order);
            Graph::SizeType need_size = std::max<Graph::SizeType>(0, component_size_bounds.first - initial_size);

            Graph::OrderType can_order = component_order_bounds.second - initial_order;
            Graph::SizeType can_size = component_size_bounds.second - initial_size;

            auto current_order_bounds = Constraint::OrderBounds(need_order, can_order);
            auto current_order_to_size_bounds = Constraint::SizeBounds(need_order, Utils::complete_graph_size(can_order));
            auto current_size_bounds = Constraint::SizeBounds(need_size, can_size);

            // REMEMBER: for undirected graphs 'order' steps from 1 to 3 (can't go from 1 to 2)

            if (!Utils::non_empty_segments_intersection(current_order_to_size_bounds, current_size_bounds)) {
                continue;
            }

            Graph::OrderType need_cut_point = component_cut_point_bounds.first;
            if (need_cut_point <= initial_cut_point_bounds.second) {
                suitable_number_of_bridges.push_back(bridges);
            }
            else {
                // TODO: check 'second formulae' too
                // it works for tighter cases, while this formulae does not
                Graph::SizeType to_add = 2 * (need_cut_point - bridges + 1);
                if (to_add > can_order || to_add > can_size) {
                    continue;
                }
                // TODO: check whether it's possible to satisfy SizeConstraints
//                if ()
                suitable_number_of_bridges.push_back(bridges);
            }
        }

        if (suitable_number_of_bridges.empty()) {
            throw std::runtime_error("generate_connected_component error: given constraints cannot be satisfied");
        }

        auto generate_component = [&](Graph::SizeType bridges) -> GraphPtr {
            if (bridges == 0) {
                auto two_edge_connected_block_ptr = constraint_block_ptr->template clone<TwoEdgeConnectedConstraintBlock>();
                auto component_number_constraint = two_edge_connected_block_ptr->template
                        get_constraint<ComponentNumberConstraint>();
                component_number_constraint->set_bounds(1, 1);

                auto block = generate_two_edge_connected_block(two_edge_connected_block_ptr);
                return block->get_component(0);
            }
            if (bridges == 1) {
                // TODO: directed/undirected
                auto graph = std::make_shared<UndirectedGraph>(2);
                graph->add_edge(0, 1);
                return graph;
            }
            return generate_connected_component(graph_type, component_order_bounds, component_size_bounds,
                    component_cut_point_bounds, bridges);
        };

        auto component_number = random.next(component_number_bounds);
        auto components = std::make_shared<GraphComponents>();
        for (Graph::OrderType i = 0; i < component_number; ++i) {
            auto bridges = suitable_number_of_bridges.at(random.next(suitable_number_of_bridges.size()));
            components->add_component(generate_component(bridges));
        }
        return components;
    }

    GraphComponentsPtr Generator::generate_two_connected_block(std::shared_ptr<TwoConnectedConstraintBlock> constraint_block_ptr) {
        // TODO: default values
        auto graph_type = constraint_block_ptr->get_graph_type();
        auto component_number_bounds = constraint_block_ptr->template get_constraint<ComponentNumberConstraint>()->bounds();
        auto component_order_bounds = constraint_block_ptr->template get_constraint<ComponentOrderConstraint>()->bounds();
        auto component_size_bounds = constraint_block_ptr->template get_constraint<ComponentSizeConstraint>()->bounds();

        auto component_number = random.next(component_number_bounds);
        auto components = std::make_shared<GraphComponents>();
        for (Graph::OrderType i = 0; i < component_number; ++i) {
            auto component = generate_two_connected_component(graph_type, component_order_bounds, component_size_bounds);
            components->add_component(component);
        }
        return components;
    }

    // TODO: remove this?
    ConstrainedGraphPtr Generator::go_with_the_winners(GraphGenerator initial_graph_generator, GoNext go_next, bool to_print,
            int colony_size, int growth_rate, int outer_iterations, int inner_iterations) {

        int threshold = colony_size / growth_rate;

        int total_inner_iterations = 0;
        int total_inner_true_iterations = 0;
        int total_colony_multiplications = 0;

        for (int outer = 0; outer < outer_iterations; ++outer) {

            auto initial_graph_ptr = initial_graph_generator();
            if (initial_graph_ptr->check() == Constraint::SatisfactionVerdict::kOK) {
                return initial_graph_ptr;
            }

            std::vector<ConstrainedGraphPtr> graphs;
            for (int i = 0; i < colony_size; ++i) {
                // TODO: copy constructor (as we have pointers in ConstrainedGraph)
                graphs.push_back(std::make_shared<ConstrainedGraph>(*initial_graph_ptr));
            }

            for (int inner = 0; inner < inner_iterations && !graphs.empty(); ++inner) {
                ++total_inner_iterations;
                std::vector<ConstrainedGraphPtr> graphs_nxt;
                for (auto &g : graphs) {
                    ++total_inner_true_iterations;
                    go_next(g);
                    auto constraint_verdict = g->check();
                    if (constraint_verdict == Constraint::SatisfactionVerdict::kOK) {
                        if (to_print) {
                            std::cout << "Outer           : " << outer + 1 << std::endl;
                            std::cout << "Inner last      : " << inner + 1 << std::endl;
                            std::cout << "Inner avg       : " << 1.0 * total_inner_iterations / (outer + 1) << std::endl;
                            std::cout << "Total mult      : " << total_colony_multiplications << std::endl;
                            std::cout << "Inner total     : " << total_inner_iterations << std::endl;
                            std::cout << "Inner total true: " << total_inner_true_iterations << std::endl;
                            std::cout << std::endl;
                        }
                        return g;
                    }
                    if (constraint_verdict == Constraint::SatisfactionVerdict::kPossible) {
                        graphs_nxt.push_back(g);
                    }
                }

                graphs.clear();
                if (graphs_nxt.size() <= threshold) {
                ++total_colony_multiplications;
                    for (auto &g: graphs_nxt) {
                        for (int i = 0; i < growth_rate; ++i) {
                            graphs.push_back(g);
                        }
                    }
                }
                else {
                    graphs = graphs_nxt;
                }
            }
        }
        return std::make_shared<ConstrainedGraph>();
    }

    GraphPtr Generator::generate_strongly_connected_component(Constraint::OrderBounds order_bounds,
            Constraint::SizeBounds size_bounds) {

        auto get_size_bounds = [&](Graph::OrderType order) {
            Constraint::SizeBounds order_to_size_bounds, res;
            order_to_size_bounds.first = order;
            order_to_size_bounds.second = static_cast<Graph::SizeType>(order) * (order - 1);
            if (Utils::non_empty_segments_intersection(order_to_size_bounds, size_bounds)) {
                res = Utils::segments_intersection(order_to_size_bounds, size_bounds);
            }
            else {
                res = Constraint::SizeBounds(-1, -1);
            }
            return res;
        };

        std::vector<Graph::OrderType> suitable_order;
        for (auto order = order_bounds.first; order <= order_bounds.second; ++order) {
            auto current_size_bounds = get_size_bounds(order);
            if (current_size_bounds.first != -1) {
                suitable_order.push_back(order);
            }
        }

        if (suitable_order.empty()) {
            throw std::runtime_error("generate_strongly_connected_component error: given constraints cannot be satisfied");
        }

        auto order = suitable_order.at(random.next(suitable_order.size()));
        auto size = random.next(get_size_bounds(order));
        return generate_two_connected_component(Graph::Type::kDirected, order, size, 2, 0.2);
    }

    GraphPtr Generator::generate_tree(Graph::Type graph_type, Constraint::OrderBounds order_bounds,
            Constraint::OrderBounds diameter_bounds, Graph::OrderType max_vertex_degree) {

        // TODO: use TreeBroadness coefficient
        // for now only use diameter

        // TODO: maybe check all this bounds in TreeConstraintBlock
        Utils::assert_segment_inside(1, static_cast<int>(3e6), order_bounds.first, order_bounds.second,
                "Tree generator: given order bounds");
        Utils::assert_segment_inside(0, static_cast<int>(3e6), diameter_bounds.first, diameter_bounds.second,
                "Tree generator: given diameter bounds");
        Utils::assert_value_inside(0, static_cast<int>(3e6), max_vertex_degree,
                "Tree generator: given maximum vertex degree");

        auto bad = []() {
            throw std::runtime_error("Tree generator error: given constraints cannot be satisfied");
        };

        if (diameter_bounds.first >= order_bounds.second) {
            bad();
        }
        diameter_bounds.second = std::min<Graph::SizeType>(diameter_bounds.second, order_bounds.second - 1);

        // TODO: maybe there are some other bad cases
        if (max_vertex_degree <= 1 && diameter_bounds.first > max_vertex_degree) {
            bad();
        }

        // give this guy a graph...
        if (max_vertex_degree <= 1) {
            auto graph = std::make_shared<UndirectedGraph>(max_vertex_degree + 1);
            if (max_vertex_degree == 1) {
                graph->add_edge(0, 1);
            }
            return graph;
        }

//        diameter_bounds.first = std::max(diameter_bounds.first, 2);
        // TODO: refactor this?
        long long v = max_vertex_degree;
        long long inf = order_bounds.second;

        // true formula is: (d + 1) + (v - 2) * [ 2 * SUM_{i = 0}^{floor[(d - 1) / 2]}((v - 1) ^ i) + (d % 2 ? 0 : (v - 1) ^ (d / 2) ]
        // we stop calculation when value exceeds 'order_bounds.second'
        // 'd' is diameter
        // 'v' is max_vertex_degree
        // function works in O(log2(order_bounds.second)) as minimal v = 3 gives us powers of (v - 1) = 2
        auto maximum_vertices_number = [&](int d) -> long long {
            long long res = d + 1;
            if (d == 0) {
                return 1;
            }
            if (v == 2) {
                return res;
            }
            long long base = v - 1;
            long long cur_power = 1;
            long long cur_sum = 0;
            for (int i = 0; i < (d - 1) / 2; ++i) {
                cur_sum += cur_power;
                cur_power *= base;
                if (cur_sum >= inf) {
                    break;
                }
            }
            if (cur_sum >= inf) {
                return inf;
            }
            res += (v - 2) * (2 * cur_sum + (d % 2 ? 0 : cur_power));
            return std::min(res, inf);
        };

        // without max_vertex_degree we can build tree with any diameter
        // but with max_vertex_degree we must find 'good' subsegment
        Graph::SizeType L = diameter_bounds.first;
        Graph::SizeType R = diameter_bounds.second;
        auto f = [&](Graph::SizeType diameter) {
            return Utils::non_empty_segments_intersection(order_bounds.first, order_bounds.second,
                    diameter + 1, maximum_vertices_number(diameter));
        };
        // check invariant f(R) == true
        if (!f(R)) {
            bad();
        }
        // do not search if it's already 'good'
        if (f(L)) {
            R = L;
        }
        while (R - L > 1) {
            auto mid = (L + R) / 2;
            if (f(mid)) R = mid;
            else L = mid;
        }

        auto diameter_left = static_cast<Graph::OrderType>(R);
        auto diameter_right = diameter_bounds.second;
        auto diameter = random.next(diameter_left, diameter_right);

        order_bounds.first = std::max<Graph::OrderType>(order_bounds.first, diameter + 1);
        order_bounds.second = std::min<Graph::OrderType>(order_bounds.second, maximum_vertices_number(diameter));
        auto order = random.next(order_bounds);

        auto tree = generate_tree_fixed_diameter(graph_type, order, diameter, max_vertex_degree);
        return tree;
    }

    GraphPtr Generator::generate_tree_fixed_diameter(Graph::Type graph_type, Graph::OrderType order,
            Graph::OrderType diameter, Graph::OrderType max_vertex_degree) {

//        std::cout << diameter << std::endl;
        GraphPtr g = std::make_shared<UndirectedGraph>(order);
        if (order == 1) {
            return g;
        }

        std::vector<Graph::OrderType> level(order);
        for (Graph::OrderType i = 0; i < diameter; ++i) {
            g->add_edge(i, i + 1);
            level[i] = std::min(i, diameter - i);
        }

        DSU dsu(order, true);
        dsu.unite(0, 1);
        dsu.unite(diameter, diameter - 1);

        auto connect_to_neighbor = [&](int v) {
            auto seg = dsu.get_segment(v);
            // the last iteration
            if (seg.second - seg.first + 1 == order) {
                return;
            }
            if (seg.first != 0) {
                dsu.unite(v, seg.first - 1);
            }
            else {
                dsu.unite(v, seg.second + 1);
            }
        };

        Graph::OrderType made = diameter;
        for (auto i = diameter + 1; i < order; ++i) {
            auto v = random.next(made + 1);
            v = dsu.get_parent(v);
//            auto seg = dsu.get_segment(v);
//            // segment can become too big, in this case it will eat more and more
//            // we need a parameter to control it
//            if (seg.second - seg.first + 1) {
//
//            }

            g->add_edge(v, i);
            level[i] = level[v] - 1;
            // vertex 'v' is full, connect segment to the left or right
            if (g->adjacency_list()[v].size() == max_vertex_degree) {
                connect_to_neighbor(v);
            }
            // vertex 'i' is on it's last level, connect it
            if (level[i] == 0) {
                connect_to_neighbor(i);
            }
        }
        return g;
    }

    GraphPtr Generator::generate_tree_fixed_leaves_number(Graph::OrderType order, Graph::OrderType leaves_number,
            double merge_probability) {

        if (0 > merge_probability || merge_probability > 1) {
            throw std::invalid_argument("generate_tree_fixed_leaves_number error: given 'merge_probability' = " +
            std::to_string(merge_probability) + " should be in range [0, 1]");
        }
        if (order < 1) {
            throw std::invalid_argument("generate_tree_fixed_leaves_number error: given order " + std::to_string(order) +
            " is not positive");
        }
        if (order == 1) {
            return std::make_shared<UndirectedGraph>(1);
        }
        if (order == 2) {
            auto graph = std::make_shared<UndirectedGraph>(2);
            graph->add_edge(0, 1);
            return graph;
        }
        if (!Utils::in_range(2, leaves_number, order - 1)) {
            throw std::invalid_argument("generate_tree_fixed_leaves_number error: given 'leaves_number' = " +
            std::to_string(leaves_number) + " should be in range " + Utils::segment_to_string(2, order - 1));
        }

        auto graph = std::make_shared<UndirectedGraph>(order);
        Graph::OrderType root = 0;

        DSU branch_dsu(leaves_number, true);
        std::vector<Graph::OrderType> confirmed_branches;
        std::vector<Graph::OrderType> branch_head(leaves_number);
        std::iota(branch_head.begin(), branch_head.end(), 0);

        auto next_free_vertex = leaves_number;
        auto merges_cnt = leaves_number - 1;
        auto go_up_cnt = order - leaves_number;

        auto pick_neighbor = [&](Graph::OrderType v) {
            auto seg = branch_dsu.get_segment(v);
            if (seg.first != 0) {
                return branch_dsu.get_parent(seg.first - 1);
            }
            else {
                return branch_dsu.get_parent(seg.second + 1);
            }
        };

        for (Graph::OrderType i = 0; i < order - 1; ++i) {
            bool its_merge_time = random.next() < merge_probability;
            // last iteration must be 'merge'
            if (merges_cnt == 1 && i != order - 2) {
                its_merge_time = false;
            }
            if (its_merge_time && merges_cnt && !confirmed_branches.empty() || !go_up_cnt) {
                --merges_cnt;
                auto branch_idx = confirmed_branches.at(random.next(confirmed_branches.size()));
                branch_idx = branch_dsu.get_parent(branch_idx);
                auto neighbor = pick_neighbor(branch_idx);
                graph->add_edge(branch_head[branch_idx], branch_head[neighbor]);
                branch_dsu.unite(neighbor, branch_idx);
            }
            else {
                --go_up_cnt;
                auto branch_idx = branch_dsu.get_parent(random.next(leaves_number));
                graph->add_edge(branch_head[branch_idx], next_free_vertex);
                if (branch_head[branch_idx] == branch_idx) {
                    confirmed_branches.push_back(branch_idx);
                }
                branch_head[branch_idx] = next_free_vertex;
                ++next_free_vertex;
            }
        }

        return graph;
    }

    GraphComponentsPtr Generator::generate_tree_block(std::shared_ptr<TreeConstraintBlock> constraint_block_ptr) {
        auto graph_type = constraint_block_ptr->get_graph_type();
//        auto order_bounds = block_ptr->get_order_bounds();
        auto component_number_bounds = constraint_block_ptr->template get_constraint<ComponentNumberConstraint>()->bounds();
        auto component_order_bounds = constraint_block_ptr->template get_constraint<ComponentOrderConstraint>()->bounds();
        auto component_diameter_bounds = constraint_block_ptr->template get_constraint<ComponentDiameterConstraint>()->bounds();
        auto component_max_vertex_degree = constraint_block_ptr->get_component_maximum_vertex_degree();

//        auto possible_to_construct = [&](int components_number, int vertices_made = 0) {
//            auto glob_left = order_bounds.first - vertices_made;
//            auto glob_right = order_bounds.second - vertices_made;
//
//            auto comp_left = components_order_bounds.first * components_number;
//            auto comp_right = components_order_bounds.second * components_number;
//
//            return Utils::non_empty_segments_intersection(glob_left, glob_right, comp_left, comp_right);
//        };

        auto components = std::make_shared<GraphComponents>();
        auto component_number = random.next(component_number_bounds);

        for (Graph::OrderType i = 0; i < component_number; ++i) {
            components->add_component(generate_tree(graph_type, component_order_bounds, component_diameter_bounds,
                    component_max_vertex_degree));
        }

        return components;
    }

    // TODO: this is copy-paste from generate_strongly_connected_component
    GraphPtr Generator::generate_two_connected_component(Graph::Type graph_type,
            Constraint::OrderBounds order_bounds, Constraint::SizeBounds size_bounds) {

        auto get_size_bounds = [&](Graph::OrderType order) {
            Constraint::SizeBounds order_to_size_bounds, res;
            order_to_size_bounds.first = order;
            order_to_size_bounds.second = Utils::complete_graph_size(order);
            if (graph_type == Graph::Type::kDirected) {
                order_to_size_bounds.second *= 2;
            }
            if (Utils::non_empty_segments_intersection(order_to_size_bounds, size_bounds)) {
                res = Utils::segments_intersection(order_to_size_bounds, size_bounds);
            }
            else {
                res = Constraint::SizeBounds(-1, -1);
            }
            return res;
        };

        std::vector<Graph::OrderType> suitable_order;
        for (auto order = order_bounds.first; order <= order_bounds.second; ++order) {
            auto current_size_bounds = get_size_bounds(order);
            if (current_size_bounds.first != -1) {
                suitable_order.push_back(order);
            }
        }

        if (suitable_order.empty()) {
            throw std::runtime_error("generate_two_connected_component error: given constraints cannot be satisfied");
        }

        auto order = suitable_order.at(random.next(suitable_order.size()));
        auto size = random.next(get_size_bounds(order));
        return generate_two_connected_component(graph_type, order, size, 3);
    }

    GraphPtr Generator::generate_two_connected_component(Graph::Type graph_type, Graph::OrderType order,
            Graph::SizeType size, Graph::OrderType min_loop_size, double loop_ear_probability) {

        std::vector<Graph::EdgeType> result;
        if (order < min_loop_size || !Utils::in_range(order, size, Utils::complete_graph_size(order))) {
            return std::make_shared<UndirectedGraph>();
        }

        struct edge_hash {
            std::size_t operator()(const Graph::EdgeType &p) const {
                return static_cast<long long>(p.first) * Graph::kMaximumOrder + p.second;
            }
        };

        // TODO: flag whether we allow parallel edges or not
        // check it somehow better ...
        // maybe we'll use Graph to store the edges ...
        // do not allow parallel edges
        std::unordered_set<Graph::EdgeType, edge_hash> used_edges;

        auto circuit_rank = size - order + 1;
        auto a1 = circuit_rank == 1 ? order : random.next(min_loop_size, order);
        for (Graph::OrderType i = 0; i < a1; ++i) {
            result.emplace_back(i, (i + 1) % a1);
            used_edges.insert(Graph::EdgeType(i, (i + 1) % a1));
        }

        int vertices_made = a1;
        int ears_made = 1;

        auto edge_exists = [&](int from, int to) -> bool {
            if (graph_type == Graph::Type::kDirected) {
                return used_edges.count(Graph::EdgeType(from, to));
            }
            return used_edges.count(Graph::EdgeType(from, to)) || used_edges.count(Graph::EdgeType(to, from));
        };

        auto generate_ear = [&](Graph::OrderType n) {
            Graph::OrderType start, finish;
            do {
                start = random.next(0, vertices_made - 1);
                finish = random.next(0, vertices_made - 1);
                if (random.next() < loop_ear_probability) {
                    finish = start;
                }
                else if (start == finish) {
                    finish = start == 0 ? 1 : start - 1;
                }
            } while (n == 0 && edge_exists(start, finish));

            if (n == 0) {
                result.emplace_back(start, finish);
                used_edges.insert({start, finish});
            }
            else {
                result.emplace_back(start, vertices_made++);
                used_edges.insert({start, vertices_made - 1});

                for (int i = 0; i < n - 1; ++i, ++vertices_made) {
                    result.emplace_back(vertices_made - 1, vertices_made);
                    used_edges.insert({vertices_made - 1, vertices_made});
                }

                result.emplace_back(vertices_made - 1, finish);
                used_edges.insert({vertices_made - 1, finish});
            }
        };

        for (; ears_made < circuit_rank - 1; ++ears_made) {
            auto left_bound = Utils::complete_graph_size(vertices_made) == used_edges.size();
            auto ear_inner_size = random.next(left_bound, order - vertices_made);
            generate_ear(ear_inner_size);
        }
        // this condition will be false when we have only 1 ear
        if (ears_made < circuit_rank) {
            generate_ear(order - vertices_made);
        }

        auto graph = Graph::create(order, graph_type);
        graph->add_edges(result);
        return graph;
    }

    GraphComponentsPtr Generator::generate_two_edge_connected_block(std::shared_ptr<TwoEdgeConnectedConstraintBlock> constraint_block_ptr,
            bool check_satisfiability_only) {

        auto graph_type = constraint_block_ptr->get_graph_type();
        auto component_number_bounds = constraint_block_ptr->template get_constraint<ComponentNumberConstraint>()->bounds();
        auto component_order_bounds = constraint_block_ptr->template get_constraint<ComponentOrderConstraint>()->bounds();
        auto component_size_bounds = constraint_block_ptr->template get_constraint<ComponentSizeConstraint>()->bounds();
        auto component_cut_point_bounds = constraint_block_ptr->template get_constraint<ComponentCutPointConstraint>()->bounds();

        Graph::OrderType min_subcomponent_order = 3;

        auto get_order_bounds = [&](Graph::OrderType cut_points, Constraint::OrderBounds &order_bounds) {
            auto min_component_order = min_subcomponent_order * (cut_points + 1) - cut_points;
            order_bounds.first = std::max(component_order_bounds.first, min_component_order);
            order_bounds.second = component_order_bounds.second;
            return order_bounds.first <= order_bounds.second;
        };

        auto get_size_bounds_fixed_order = [&](Graph::OrderType cut_points, Graph::OrderType order) {
            auto max_subcomponent_order = order + cut_points - min_subcomponent_order * cut_points;
            Graph::SizeType left_bound = std::max(order, (cut_points + 1) * min_subcomponent_order);
            Graph::SizeType right_bound = cut_points * min_subcomponent_order + Utils::complete_graph_size(max_subcomponent_order);
            return Constraint::SizeBounds(left_bound, right_bound);
        };

        auto get_size_bounds_range_order = [&](Graph::OrderType cut_points, Constraint::OrderBounds order_bounds,
                Constraint::SizeBounds &size_bounds) {
            size_bounds.first = get_size_bounds_fixed_order(cut_points, order_bounds.first).first;
            size_bounds.second = get_size_bounds_fixed_order(cut_points, order_bounds.second).second;
            if (size_bounds.first > size_bounds.second) {
                throw std::runtime_error("debug component size bounds");
            }
            return Utils::non_empty_segments_intersection(size_bounds, component_size_bounds);
        };

        std::vector<Graph::OrderType> suitable_number_of_cut_points;
        for (auto cut_points = component_cut_point_bounds.first; cut_points <= component_cut_point_bounds.second; ++cut_points) {
            Constraint::OrderBounds order_bounds;
            if (!get_order_bounds(cut_points, order_bounds)) {
                continue;
            }
            Constraint::SizeBounds size_bounds;
            if (get_size_bounds_range_order(cut_points, order_bounds, size_bounds)) {
                suitable_number_of_cut_points.push_back(cut_points);
            }
        }

        if (suitable_number_of_cut_points.empty()) {
            throw std::runtime_error("generate_two_edge_connected_component error: given constraints cannot be satisfied");
        }

        if (check_satisfiability_only) {
            return nullptr;
        }

        auto component_number = random.next(component_number_bounds);
        GraphComponentsPtr components = std::make_shared<GraphComponents>();

        for (Graph::OrderType i = 0; i < component_number; ++i) {
            auto cut_points = suitable_number_of_cut_points.at(random.next(suitable_number_of_cut_points.size()));
            Constraint::OrderBounds order_bounds;
            get_order_bounds(cut_points, order_bounds);
            auto L = order_bounds.first - 1;
            auto R = order_bounds.second;
            while (R - L > 1) {
                auto mid = (L + R) / 2;
                auto size_bounds = get_size_bounds_fixed_order(cut_points, mid);
                if (Utils::non_empty_segments_intersection(size_bounds, component_size_bounds)) {
                    R = mid;
                }
                else {
                    L = mid;
                }
            }
            order_bounds.first = R;
            // ok now order_bounds contains only good orders, pick random from it
            auto order = random.next(order_bounds);

            Constraint::SizeBounds size_bounds;
            get_size_bounds_range_order(cut_points, std::make_pair(order, order), size_bounds);
            size_bounds = Utils::segments_intersection(size_bounds, component_size_bounds);
//            int size = random.next(size_bounds);

            auto component = generate_two_edge_connected_component(graph_type, order, size_bounds, cut_points);
            components->add_component(component);
        }
        return components;
    }

    // we construct (cut_points + 1) 2-vertex-connected components, which correspond to vertices in a tree
    // and connect them in points(cut-points), which correspond to edges in a tree
    // component size can be SMALLER than size_bounds, but not greater
    GraphPtr Generator::generate_two_edge_connected_component(Graph::Type graph_type, Graph::OrderType order,
            Constraint::SizeBounds size_bounds, Graph::OrderType cut_points) {

        Graph::OrderType min_subcomponent_order = 3;

        if (order == 1) {
            return std::make_shared<UndirectedGraph>(1);
        }

        if (order < min_subcomponent_order) {
            throw std::runtime_error("generate_two_edge_connected_component error: cannot generate component with order = "+
            std::to_string(order));
        }

        auto max_vertex_degree = (order + cut_points) / (cut_points + 1);
        // 1 component with order = (cut_points + 1)
        auto tree_block = std::make_shared<TreeConstraintBlock>(Graph::Type::kUndirected,
                Constraint::OrderBounds(1, 1),
                Constraint::OrderBounds(cut_points + 1, cut_points + 1),
                Constraint::OrderBounds(0, cut_points),
                max_vertex_degree);

        auto tree_graph = generate_tree_block(tree_block)->get_component(0);

        std::vector<Graph::OrderType> subcomponents_order(cut_points + 1);
        Constraint::SizeBounds current_size_bounds;
        Graph::OrderType subcomponents_order_sum = 0;
        for (Graph::OrderType i = 0; i <= cut_points; ++i) {
            subcomponents_order[i] = std::max<Graph::OrderType>(min_subcomponent_order, tree_graph->adjacency_list().at(i).size());
            subcomponents_order_sum += subcomponents_order[i];
            current_size_bounds.first += subcomponents_order[i];
            current_size_bounds.second += Utils::complete_graph_size(subcomponents_order[i]);
        }

        auto resid_order = order - subcomponents_order_sum + cut_points;
        for (Graph::OrderType i = 0; i < resid_order; ++i) {
            auto index = random.next(cut_points + 1);
            current_size_bounds.first++;
            current_size_bounds.second -= Utils::complete_graph_size(subcomponents_order[index]);
            subcomponents_order[index]++;
            current_size_bounds.second += Utils::complete_graph_size(subcomponents_order[index]);
        }

        bool pick_minimum_size = current_size_bounds.first > size_bounds.second;
        bool pick_maximum_size = current_size_bounds.second < size_bounds.first;
        GraphComponentsPtr subcomponents = std::make_shared<GraphComponents>();

        for (Graph::OrderType i = 0; i <= cut_points; ++i) {
            Constraint::SizeBounds subcomponent_size_bounds;
            subcomponent_size_bounds.first = subcomponents_order[i];
            subcomponent_size_bounds.second = Utils::complete_graph_size(subcomponents_order[i]);

            current_size_bounds.first -= subcomponent_size_bounds.first;
            current_size_bounds.second -= subcomponent_size_bounds.second;

            if (pick_minimum_size) {
                subcomponent_size_bounds.second = subcomponent_size_bounds.first;
            }
            else if (pick_maximum_size) {
                subcomponent_size_bounds.first = subcomponent_size_bounds.second;
            }
            else {
                subcomponent_size_bounds.first = std::max(subcomponent_size_bounds.first,
                        size_bounds.first - current_size_bounds.second);

                subcomponent_size_bounds.second = std::min(subcomponent_size_bounds.second,
                        size_bounds.second - current_size_bounds.first);

                if (subcomponent_size_bounds.first > subcomponent_size_bounds.second) {
                    throw std::runtime_error("debug this");
                }
            }

            auto subcomponent_size = random.next(subcomponent_size_bounds);
            size_bounds.first -= subcomponent_size;
            size_bounds.second -= subcomponent_size;

            auto subcomponent = generate_two_connected_component(graph_type, subcomponents_order[i], subcomponent_size,
                    min_subcomponent_order);

            subcomponents->add_component(subcomponent);
        }

        GraphPtr graph = std::make_shared<UndirectedGraph>(order);
        connect_components_in_vertices(graph, subcomponents, tree_graph);
        return graph;
    }

    // TODO: rewrite
    GraphPtr Generator::generate_connected_component(Graph::Type graph_type, Constraint::OrderBounds order_bounds,
            Constraint::SizeBounds size_bounds, Constraint::OrderBounds cut_point_bounds, Graph::OrderType bridges) {

        // ALERT COPY-PASTE
        auto initial_order = bridges + 1;
        auto initial_size = bridges;

        auto need_order = std::max<Graph::OrderType>(0, order_bounds.first - initial_order);
        auto need_size = std::max<Graph::SizeType>(0, size_bounds.first - initial_size);

        auto can_order = order_bounds.second - initial_order;
        auto can_size = size_bounds.second - initial_size;

        auto current_order_bounds = Constraint::OrderBounds(need_order, can_order);
        auto current_order_to_size_bounds = Constraint::SizeBounds(need_order, Utils::complete_graph_size(can_order));
        auto current_size_bounds = Constraint::SizeBounds(need_size, can_size);

        std::vector<Graph::OrderType> suitable_initial_cut_points;
        // 'k' is a number of inner vertices in initial tree
        for (Graph::OrderType k = 1; k <= std::min(bridges - 1, cut_point_bounds.second); ++k) {
            auto need_cut_point = std::max(0, cut_point_bounds.first - k);
            if (need_cut_point <= 0) {
                suitable_initial_cut_points.push_back(k);
            }
            else {
                auto to_add = 2 * (need_cut_point - bridges + 1);
                if (to_add <= can_order || to_add <= can_size) {
                    suitable_initial_cut_points.push_back(k);
                }
            }
        }

        if (suitable_initial_cut_points.empty()) {
            throw std::runtime_error("generate_connected_component error: given constraints cannot be satisfied");
        }

        auto inner_vertices = suitable_initial_cut_points.at(random.next(suitable_initial_cut_points.size()));
        auto leaves = bridges + 1 - inner_vertices;
        auto tree = generate_tree_fixed_leaves_number(bridges + 1, leaves, 0.2);//random.next());
        auto need_cut_point = std::max(0, cut_point_bounds.first - inner_vertices);
        auto can_cut_point = cut_point_bounds.second - inner_vertices;

//        std::vector<int> vertices_idx(tree->order());
//        std::iota(vertices_idx.begin(), vertices_idx.end(), 0);
//        std::shuffle(vertices_idx.begin(), vertices_idx.end(), random.rng());
        std::vector<Graph::OrderType> subcomponent_order(tree->order(), 1);
        std::vector<Graph::OrderType> free_order(tree->order(), 0);
        std::vector<Graph::OrderType> subcomponent_bridge_cut_points(tree->order(), 1);
        std::vector<Graph::OrderType> subcomponent_inner_cut_points(tree->order(), 0);

        std::fill(subcomponent_bridge_cut_points.begin(), subcomponent_bridge_cut_points.begin() + leaves, 0);
        Graph::OrderType order_used = 0;
        Graph::SizeType size_used = 0;
        auto prefer_bridge_cut_points = random.next();

        auto additional_cut_point = std::min(can_cut_point, can_order / 2 + bridges - 1 - need_cut_point);
//        auto additional_cut_point = can_order / 2 + bridges - 1 - need_cut_point;
        additional_cut_point = random.next(additional_cut_point);

        for (Graph::OrderType i = 0; i < need_cut_point + additional_cut_point; ++i) {
            auto index = random.next(tree->order());
            auto to_add = 0;
            if (subcomponent_order[index] == 1) {
                to_add = 2;
                if (tree->vertex_degree(index) >= 3 && i + 1 < need_cut_point && random.next() < prefer_bridge_cut_points) {
                    subcomponent_bridge_cut_points[index]++;
                    ++i;
                }
                subcomponent_bridge_cut_points[index]++;
            }
            else {
                if (subcomponent_order[index] < tree->vertex_degree(index) && random.next() < prefer_bridge_cut_points) {
                    to_add = 1;
                    subcomponent_bridge_cut_points[index]++;
                }
                else {
                    to_add = 2;
                }
            }
            subcomponent_order[index] += to_add;
            order_used += to_add;
            size_used += to_add;
        }

        need_order -= order_used;
        can_order -= order_used;
        need_size -= size_used;
        can_size -= size_used;
        can_cut_point -= need_cut_point - additional_cut_point;
        auto additional_order = random.next(order_bounds.second - order_bounds.first);
        order_used = size_used = 0;

        for (Graph::OrderType i = 0; i < need_order + additional_order; ++i) {
            // in case we pick leaf in a tree we'll increase 'cut_points' by 1
            auto left_bound = (can_cut_point ? 0 : leaves);
            auto index = random.next(left_bound, tree->order() - 1);
            auto to_add = 1;
            if (tree->vertex_degree(index) == 1) {
                can_cut_point--;
                subcomponent_bridge_cut_points[index]++;
            }
            if (subcomponent_order[index] == 1) {
                to_add = 2;
            }
            free_order[index] += to_add;

            i += to_add - 1;
            order_used += to_add;
            size_used += to_add;
        }

        // time to decide how much edges there will be ...
        need_size = std::max<Graph::SizeType>(0, need_size - size_used);
        can_size = std::max<Graph::SizeType>(0, can_size - size_used);
        GraphComponentsPtr subcomponents_ptr = std::make_shared<GraphComponents>();
        Graph::OrderType order = 0;

        for (Graph::OrderType i = 0; i < tree->order(); ++i) {
            auto total_order = subcomponent_order[i] + free_order[i];
            Utils::LongPair size_bounds = {total_order, total_order + can_size};
            auto subcomponent = generate_two_edge_connected_component(graph_type, total_order, size_bounds,
                    subcomponent_inner_cut_points[i]);
            subcomponents_ptr->add_component(subcomponent);
            can_size = std::max<Graph::SizeType>(0, can_size - subcomponent->size());
            order += subcomponent->order();
        }

        GraphPtr graph = std::make_shared<UndirectedGraph>(order);
        connect_components_with_edges(graph, subcomponents_ptr, tree);
        return graph;
    }

    void Generator::connect_components_in_vertices_dfs(GraphPtr graph, GraphComponentsPtr components, GraphPtr skeleton,
            std::vector<std::vector<std::pair<int, int>>> &selected_vertices, int &next_free_index,
            int current_component_index, int previous_component_index, int link_vertex) {

        int our_link = previous_component_index == -1 ? -1 : selected_vertices[current_component_index][0].first;
        if (previous_component_index != -1) {
            selected_vertices[current_component_index][0].second = link_vertex;
        }

        auto local_index_to_global = [&](int local_index) {
            if (local_index == our_link) {
                return link_vertex;
            }
            int global_index = next_free_index + local_index;
            if (our_link != -1 && local_index > our_link) {
                --global_index;
            }
            return global_index;
        };

        for (int i = previous_component_index != -1; i < selected_vertices[current_component_index].size(); ++i) {
            selected_vertices[current_component_index][i].second =
                    local_index_to_global(selected_vertices[current_component_index][i].first);
        }

        auto component = components->components().at(current_component_index);
        for (int i = 0; i < component->order(); ++i) {
            for (int j : component->adjacency_list().at(i)) {
                if (i < j) {
                    int ii = local_index_to_global(i);
                    int jj = local_index_to_global(j);
                    graph->add_edge(ii, jj);
                }
            }
        }

        next_free_index += component->order() - (previous_component_index != -1);

        auto skeleton_edges = skeleton->adjacency_list().at(current_component_index);
        int next_free_selected_vertex = (previous_component_index !=- 1);

        for (int i = 0; i < skeleton_edges.size(); ++i) {
            if (skeleton_edges[i] != previous_component_index) {
                connect_components_in_vertices_dfs(graph, components, skeleton, selected_vertices, next_free_index,
                        skeleton_edges[i], current_component_index,
                        selected_vertices[current_component_index][next_free_selected_vertex++].second);
            }
        }
    }

    // 'skeleton' is a tree with components.size() vertices
    void Generator::connect_components_in_vertices(GraphPtr graph, GraphComponentsPtr components, GraphPtr skeleton) {
        std::vector<char> used(graph->order());
//         selected_vertices[i][0] stores current index in vector selected_vertices[i]
        std::vector<std::vector<std::pair<int, int>>> selected_vertices(components->components().size());
        for (int i = 0; i < selected_vertices.size(); ++i) {
            int degree = skeleton->adjacency_list().at(i).size();
            int order = components->components().at(i)->order();
            // generate 'degree' different numbers - (cut-points) common vertices between different components
            int left_bound = 0;
            for (int j = degree; j > 0; --j) {
                int current_index = random.next(left_bound, order - j);
                selected_vertices[i].emplace_back(current_index, current_index);
                left_bound = current_index + 1;
            }
        }
        int next_free_index = 0;
        connect_components_in_vertices_dfs(graph, components, skeleton, selected_vertices, next_free_index, 0, -1, -1);
    }

    void Generator::connect_components_with_edges_dfs(GraphPtr graph, GraphComponentsPtr components, GraphPtr skeleton,
            std::vector<std::vector<int>> &local_to_global_index, int current_component_index, int previous_component_index) {

        auto current_component = components->get_component(current_component_index);
        for (int i = 0; i < current_component->order(); ++i) {
            for (int j : current_component->adjacency_list().at(i)) {
                if (i < j) {
                    auto ii = local_to_global_index[current_component_index][i];
                    auto jj = local_to_global_index[current_component_index][j];
                    graph->add_edge(ii, jj);
                }
            }
        }

        auto skeleton_edges = skeleton->adjacency_list().at(current_component_index);
        for (auto neighbor_component_index : skeleton_edges) {
            if (neighbor_component_index != previous_component_index) {
                int our_vertex_local_index = random.next(components->get_component(current_component_index)->order());
                int neighbor_vertex_local_index = random.next(components->get_component(neighbor_component_index)->order());

                int our_vertex_global_index = local_to_global_index[current_component_index][our_vertex_local_index];
                int neighbor_vertex_global_index = local_to_global_index[neighbor_component_index][neighbor_vertex_local_index];

                graph->add_edge(our_vertex_global_index, neighbor_vertex_global_index);
                connect_components_with_edges_dfs(graph, components, skeleton, local_to_global_index,
                        neighbor_component_index, current_component_index);
            }
        }
    }

    void Generator::connect_components_with_edges(GraphPtr graph, GraphComponentsPtr components, GraphPtr skeleton) {
        std::vector<std::vector<int>> local_to_global_index(components->components().size());
        for (int i = 0; i < components->components().size(); ++i) {
            local_to_global_index[i].resize(components->get_component(i)->order());
        }

        std::vector<int> index_map(graph->order());
        std::iota(index_map.begin(), index_map.end(), 0);
        std::shuffle(index_map.begin(), index_map.end(), random.rng());
        int current_idx = 0;


        for (auto &component_indices : local_to_global_index) {
            for (auto &local_idx : component_indices) {
                local_idx = index_map[current_idx++];
            }
        }
        connect_components_with_edges_dfs(graph, components, skeleton, local_to_global_index, 0, -1);
    }
}