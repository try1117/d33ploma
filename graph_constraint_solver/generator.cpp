#include "generator.h"

#include <vector>
#include <memory>
#include <iostream>
#include <utility>

#include "utils.h"

namespace graph_constraint_solver {

    ConstrainedGraphPtr Generator::generate(ConstraintBlockPtr constraint_list_ptr) {
        return generate_block(constraint_list_ptr);
    }

    std::vector<int> generate_n_numbers(int n, int sum, std::function<bool(std::vector<int>&)> check) {
        std::vector<int> result;
        int minimal_size = 1;
        for (int i = 0; i < n - 1; ++i) {
            result.push_back(random.next(minimal_size, sum - minimal_size * (n - i - 1)));
            sum -= result.back();
        }
        result.push_back(sum);
        return result;
    }

    ConstrainedGraphPtr Generator::generate_block(ConstraintBlockPtr constraint_list_ptr) {
        if (constraint_list_ptr->component_type() == ConstraintBlock::ComponentType::kTwoConnected) {
            return generate_two_connected_block(constraint_list_ptr);
        }
        if (constraint_list_ptr->component_type() == ConstraintBlock::ComponentType::kTree) {
            return generate_tree(std::static_pointer_cast<TreeBlock>(constraint_list_ptr));
        }
    }

    ConstrainedGraphPtr Generator::generate_two_connected_block(ConstraintBlockPtr constraint_list_ptr) {
        // TODO: default values
        auto graph_type = constraint_list_ptr->get_graph_type();
        auto order_bounds = constraint_list_ptr->get_order_bounds();
        auto size_bounds = constraint_list_ptr->get_size_bounds();
        auto components_number_bounds = constraint_list_ptr->get_components_number_bounds();

        auto components_order_bounds = constraint_list_ptr->template get_constraint<ComponentsOrderConstraint>(Constraint::Type::kComponentsOrder)->bounds();

        for (int iter = 0; iter < 100; ++iter) {
            auto order = random.next(order_bounds);
            auto components_number = random.next(components_number_bounds);
            auto size = random.next(size_bounds);
            auto generated_edges = generate_2connected_graph(order, size, components_number, components_order_bounds);
            if (!generated_edges.empty()) {
                return std::make_shared<ConstrainedGraph>(constraint_list_ptr, std::make_shared<Graph>(order, graph_type, generated_edges));
            }
        }
        return std::make_shared<ConstrainedGraph>();

//        // TODO: at least some crude check
//        auto check = [](std::vector<int> &orders) {
//            return true;
//        };
//
//        while (true) {
//            auto components_number = random.next(components_number_bounds);
//            auto components_orders = generate_n_numbers(components_number, order, check);
//            ConstrainedGraphPtr result = std::make_shared<ConstrainedGraph>(constraint_list_ptr, std::make_shared<Graph>(order, graph_type));
//            bool success = true;
//            for (auto component_order : components_orders) {
//                ConstraintBlockPtr component_constraint_list = std::static_pointer_cast<ConstraintBlock>(constraint_list_ptr->clone());
//                component_constraint_list->add_constraint(std::make_shared<OrderConstraint>(component_order));
//                component_constraint_list->add_constraint(std::make_shared<ComponentsNumberConstraint>(1));
//                auto component = generate_single_component(component_constraint_list);
//                if (component->empty()) {
//                    success = false;
//                    break;
//                }
//                result->append_graph(component->graph_ptr());
//            }
//            if (success) {
//                break;
//            }
//            return result;
//        }
//        return std::make_shared<ConstrainedGraph>();
    }

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

    ConstrainedGraphPtr Generator::generate_tree(std::shared_ptr<TreeBlock> constraint_block_ptr) {
        auto graph_type = constraint_block_ptr->get_graph_type();
        auto order_bounds = constraint_block_ptr->get_order_bounds();

        // TODO: use TreeBroadness coefficient
        // for now only use diameter
        auto diameter_bounds = constraint_block_ptr->get_diameter_bounds();
        auto max_vertex_degree = constraint_block_ptr->get_maximum_vertex_degree();

        // TODO: maybe check all this bounds in TreeBlock
        Utils::assert_segment_inside(1, static_cast<int>(3e6), order_bounds.first, order_bounds.second,
                "Tree generator: given order bounds");
        Utils::assert_segment_inside(0, static_cast<int>(3e6), diameter_bounds.first, diameter_bounds.second,
                "Tree generator: given diameter bounds");
        Utils::assert_value_inside(0, static_cast<int>(3e6), max_vertex_degree,
                "Tree generator: given maximum vertex degree");

        auto bad = []() {
            throw std::runtime_error("Tree generator: Given constraints cannot be satisfied");
        };

        if (diameter_bounds.first >= order_bounds.second) {
            bad();
        }
        diameter_bounds.second = std::min(diameter_bounds.second, order_bounds.second - 1);

        // TODO: maybe there are some other bad cases
        if (max_vertex_degree <= 1 && diameter_bounds.first > max_vertex_degree) {
            bad();
        }

        // give this guy a graph...
        if (max_vertex_degree == 0) {

        }
        if (max_vertex_degree == 1) {

        }

        long long v = max_vertex_degree;
        long long inf = order_bounds.second;

        // true formula is: (d + 1) + (v - 2) * [ 2 * SUM_{i = 0}^{floor[(d - 1) / 2]}((v - 1) ^ i) + (d % 2 ? 0 : (v - 1) ^ (d / 2) ]
        // we stop calculation when value exceeds 'order_bounds.second'
        // 'd' is diameter
        // 'v' is max_vertex_degree
        // function works in O(log2(order_bounds.second)) as minimal v = 3 gives us powers of (v - 1) = 2
        auto maximum_vertices_number = [&](int d) {
            long long res = d + 1;
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
        int L = diameter_bounds.first;
        int R = diameter_bounds.second;
        auto f = [&](long long diameter) {
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

        auto diameter_left = R;
        auto diameter_right = diameter_bounds.second;
        auto diameter = random.next(diameter_left, diameter_right);
        order_bounds.first = std::max(order_bounds.first, diameter + 1);
        order_bounds.second = std::min<int>(order_bounds.second, maximum_vertices_number(diameter));

        auto tree = generate_tree_fixed_diameter(graph_type, order_bounds, max_vertex_degree, diameter);
        return std::make_shared<ConstrainedGraph>(constraint_block_ptr, tree);
    }

    GraphPtr Generator::generate_tree_fixed_diameter(Graph::Type graph_type, std::pair<int, int> order_bounds,
            int max_vertex_degree, int diameter) {

        std::cout << diameter << std::endl;
        int order = random.next(order_bounds);
        GraphPtr g = std::make_shared<Graph>(order, graph_type);
        std::vector<int> level(order);
        for (int i = 0; i < diameter; ++i) {
            g->add_edge(i, i + 1);
            level[i] = std::min(i, diameter - i);
        }

        DSU dsu(order, true);
        dsu.unite(0, 1);
        dsu.unite(diameter, diameter - 1);

        auto connect_to_neighbor = [&](int v) {
            auto seg = dsu.get_segment(v);
            if (seg.first != 0) {
                dsu.unite(v, seg.first - 1);
            }
            else {
                dsu.unite(v, seg.second + 1);
            }
        };

        int made = diameter;
        for (int i = diameter + 1; i < order; ++i) {
            int v = random.next(made + 1);
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

    ConstrainedGraphPtr Generator::generate_tree_block(ConstraintBlockPtr constraint_block_ptr) {
        auto graph_type = constraint_block_ptr->get_graph_type();
        auto order_bounds = constraint_block_ptr->get_order_bounds();
        auto components_number_bounds = constraint_block_ptr->get_components_number_bounds();
        auto components_order_bounds = constraint_block_ptr->template get_constraint<ComponentsOrderConstraint>(Constraint::Type::kComponentsOrder)->bounds();

        auto possible_to_construct = [&](int components_number, int vertices_made = 0) {
            auto glob_left = order_bounds.first - vertices_made;
            auto glob_right = order_bounds.second - vertices_made;

            auto comp_left = components_order_bounds.first * components_number;
            auto comp_right = components_order_bounds.second * components_number;

            return Utils::non_empty_segments_intersection(glob_left, glob_right, comp_left, comp_right);
        };

        for (int components_number = components_number_bounds.first; components_number <= components_number_bounds.second; ++components_number) {
            if (!possible_to_construct(components_number)) {
                continue;
            }

        }

        throw std::runtime_error("Tree block generator: Given constraints cannot be satisfied");

//        bool remove_tree_constraint = false;
//        if (!constraint_list_ptr->has_constraint(Constraint::Type::kTree)) {
//            constraint_list_ptr->add_constraint(std::make_shared<TreeConstraint>(random.next(-10, 10)));
//            remove_tree_constraint = true;
//        }
//        constraint_list_ptr->add_goal_constraint(Constraint::Type::kTree);
//
//        auto empty_graph_generator = [&]() -> ConstrainedGraphPtr {
//            return std::make_shared<ConstrainedGraph>(constraint_list_ptr, std::make_shared<Graph>(order));
//        };
//        auto go_build_tree = [](ConstrainedGraphPtr g) {
//            auto edge = g->constraint_list_ptr()->template get_constraint<TreeConstraint>(Constraint::Type::kTree)->recommend_edge();
//            g->add_edge(edge.first, edge.second);
//        };
//
//        auto tree = go_with_the_winners(empty_graph_generator, go_build_tree);
//        if (remove_tree_constraint) {
//            tree->constraint_list_ptr()->remove_constraint(Constraint::Type::kTree);
//        }
//        tree->constraint_list_ptr()->remove_goal_constraint(Constraint::Type::kTree);
//        return tree;
    }

    // TODO: rename and rewrite, it's actually build random connected component
    ConstrainedGraphPtr Generator::generate_single_component(ConstraintBlockPtr constraint_list_ptr) {

//        // TODO: use not only left_bound but full range
//        auto order = constraint_list_ptr->template get_constraint<OrderConstraint>(Constraint::Type::kOrder)->bounds().first;
//
//        auto tree_generator = [&]() -> ConstrainedGraphPtr {
//            return generate_tree(order, constraint_list_ptr);
//        };
//        auto go_build_residue = [](ConstrainedGraphPtr g) {
//            g->add_random_edge();
//        };
//        auto result = go_with_the_winners(tree_generator, go_build_residue, true);
//        return result;
    }

    // TODO: create class TwoConnectedGenerator ???
    // actually in the future I'll need some class ComponentGenerator with stuff from this function
    std::vector<std::pair<int, int>> Generator::generate_2connected_graph(int order, int size, int components_number,
                                                                          std::pair<int, int> components_order_bounds) {

        // TODO: call it min_component_order (there probably will be some harder constraints later)
        long long min_order = components_order_bounds.first;
        long long max_order = components_order_bounds.second;

        auto get_large_component_order = [&](int order, int components_number) {
            return std::min(max_order, order - min_order * (components_number - 1));
        };

        auto get_edges_bounds = [&](int order, int components_number) {
            auto min_size = order;
            auto L = get_large_component_order(order, components_number);

            long long L_cnt, resid;
            if (L != min_order) {
                L_cnt = (order - min_order * components_number) / (L - min_order);
                resid = (order - min_order * components_number) % (L - min_order);
            }
            else {
                L_cnt = components_number;
                resid = 0;
            }

            long long max_size = Utils::complete_graph_size(L) * L_cnt;
            if (resid) {
                max_size += Utils::complete_graph_size(min_order + resid);
            }
            max_size += Utils::complete_graph_size(min_order) * (components_number - L_cnt - (resid != 0));

            return std::make_pair(min_size, max_size);
        };

        auto possible_to_construct = [&](int order, int size, int components_number) {
            if (!order || !components_number) {
                return !order && !size && !components_number;
            }
            if (!Utils::in_range(min_order * components_number, order, max_order * components_number)) {
                return false;
            }
            auto bounds = get_edges_bounds(order, components_number);
            return Utils::in_range(bounds, size);
        };

        std::vector<std::pair<int, int>> result;
        if (!possible_to_construct(order, size, components_number)) {
            return result;
        }

        int current_order = order;
        int current_size = size;
        for (int i = 0; i < components_number; ++i) {
            int large_component = get_large_component_order(current_order, components_number - i);
            int component_order;
            int prev_component_order = min_order - 1;
            int component_size;
            int prev_component_size;

            do {
                component_order =
                        i + 1 == components_number ? large_component : random.next(prev_component_order + 1, large_component);

                int next_order = current_order - component_order;

                auto current_edges_bounds = get_edges_bounds(current_order, components_number - i);
                auto next_edges_bounds = get_edges_bounds(next_order, components_number - i - 1);

                int size_left_bound = std::max<long long>(component_order, current_size - next_edges_bounds.second);
                int size_right_bound = std::min<long long>(1LL * component_order * (component_order - 1) / 2,
                                                current_size - next_edges_bounds.first);

                if (size_left_bound > size_right_bound) {
                    prev_component_order = component_order;
                    continue;
                }

                bool possible = false;
                prev_component_size = size_left_bound - 1;
                while (prev_component_size != size_right_bound) {
                    component_size = random.next(prev_component_size + 1, size_right_bound);
                    if (possible_to_construct(next_order, current_size - component_size, components_number - i - 1)) {
                        possible = true;
                        break;
                    }
                    prev_component_size = component_size;
                }

                if (possible) {
                    break;
                }
                prev_component_order = component_order;

            } while (prev_component_order != large_component);

            // TODO: yeah i think this condition will always be false, but maybe ...
            if (prev_component_order == large_component) {
                throw std::exception();
            }

            auto component = generate_2connected_component(component_order, component_size, components_order_bounds);
            int shift = order - current_order;
            for (auto &edge : component) {
                result.emplace_back(edge.first + shift, edge.second + shift);
            }

            current_order -= component_order;
            current_size -= component_size;
        }
        return result;
    }

    std::vector<std::pair<int, int>> Generator::generate_2connected_component(int order, int size,
                                                                              std::pair<int, int> components_order_bounds) {

        // first ear
        auto min_loop_size = 3;

        std::vector<std::pair<int, int>> result;
        // TODO: not necessary check, as we check possibility in the upper function
        if (!Utils::in_range(components_order_bounds, order) || !Utils::in_range(order, size, Utils::complete_graph_size(order))) {
            return result;
        }
        if (order < min_loop_size) {
            return result;
        }

        // TODO: flag whether we allow parallel edges or not
        // check it somehow better ...
        // maybe we'll use Graph to store the edges ...
        // do not allow parallel edges
        std::set<std::pair<int, int>> used_edges;

        int circuit_rank = size - order + 1;
        int a1 = circuit_rank == 1 ? order : random.next(min_loop_size, order);
        for (int i = 0; i < a1; ++i) {
            result.emplace_back(i, (i + 1) % a1);
            used_edges.insert({std::min(i, (i + 1) % a1), std::max(i, (i + 1) % a1)});
        }

        int vertices_made = a1;
        int ears_made = 1;

        auto generate_ear = [&](int n) {
            int start, finish;
            do {
                start = random.next(0, vertices_made - 1);
                finish = random.next(0, vertices_made - 1);
                if (start == finish) {
                    finish = start == 0 ? 1 : start - 1;
                }
            } while (n == 0 && used_edges.count({std::min(start, finish), std::max(start, finish)}));

            if (n == 0) {
                result.emplace_back(start, finish);
                used_edges.insert({std::min(start, finish), std::max(start, finish)});
            }
            else {
                result.emplace_back(start, vertices_made++);
                used_edges.insert({start, vertices_made - 1});

                for (int i = 0; i < n - 1; ++i, ++vertices_made) {
                    result.emplace_back(vertices_made - 1, vertices_made);
                    used_edges.insert({vertices_made - 1, vertices_made});
                }

                result.emplace_back(vertices_made - 1, finish);
                used_edges.insert({finish, vertices_made - 1});
            }
        };

        for (; ears_made < circuit_rank - 1; ++ears_made) {
            int left_bound = Utils::complete_graph_size(vertices_made) == used_edges.size();
            int ear_inner_size = random.next(left_bound, order - vertices_made);
            generate_ear(ear_inner_size);
        }
        // this condition will be false when we have only 1 ear
        if (ears_made < circuit_rank) {
            generate_ear(order - vertices_made);
        }
        return result;
    }
}