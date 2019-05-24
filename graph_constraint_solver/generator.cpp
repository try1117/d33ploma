#include "generator.h"

#include <vector>
#include <memory>
#include <iostream>
#include <utility>

#include "utils.h"

namespace graph_constraint_solver {

    ConstrainedGraphPtr Generator::generate(ConstraintBlockPtr constraint_list_ptr) {
        // TODO: default values
        auto graph_type = constraint_list_ptr->template get_constraint<GraphTypeConstraint>(Constraint::Type::kGraphType)->graph_type();
        auto order = constraint_list_ptr->template get_constraint<OrderConstraint>(Constraint::Type::kOrder)->bounds();
        auto size = constraint_list_ptr->template get_constraint<SizeConstraint>(Constraint::Type::kSize)->bounds();
        auto components_number = constraint_list_ptr->template get_constraint<ComponentsNumberConstraint>(Constraint::Type::kComponentsNumber)->bounds();
        return generate_components(graph_type, order, size, components_number, constraint_list_ptr);
//        if (constraint_list_)
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

    // Генерация компонент как отдельных графов вызывает сложности с глобальными ограничениями ...
    // Надо подумать как лучше этого избежать

    // Ok now we have 4 constraints:
    // Order, size, components_number and bridges_number

    // Possible way to generate such graph:
    // well need to write single_component_generation first
    // where BridgeConstraint would recommend us to build tree, then 2-connected components

    // GENERATE 2-CONNECTED GRAPH FOR NOW
    ConstrainedGraphPtr Generator::generate_components(Graph::Type graph_type, std::pair<int, int> order_bounds,
            std::pair<int, int> size_bounds, std::pair<int, int> components_number_bounds,
            ConstraintBlockPtr constraint_list_ptr) {

        for (int iter = 0; iter < 100; ++iter) {
            auto order = random.next(order_bounds);
            auto components_number = random.next(components_number_bounds);
            auto size = random.next(size_bounds);
            auto generated_edges = generate_2connected_graph(order, size, components_number);
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

    ConstrainedGraphPtr Generator::generate_tree(int order, ConstraintBlockPtr constraint_list_ptr) {
        bool remove_tree_constraint = false;
        if (!constraint_list_ptr->has_constraint(Constraint::Type::kTree)) {
            constraint_list_ptr->add_constraint(std::make_shared<TreeConstraint>(random.next(-10, 10)));
            remove_tree_constraint = true;
        }
        constraint_list_ptr->add_goal_constraint(Constraint::Type::kTree);

        auto empty_graph_generator = [&]() -> ConstrainedGraphPtr {
            return std::make_shared<ConstrainedGraph>(constraint_list_ptr, std::make_shared<Graph>(order));
        };
        auto go_build_tree = [](ConstrainedGraphPtr g) {
            auto edge = g->constraint_list_ptr()->template get_constraint<TreeConstraint>(Constraint::Type::kTree)->recommend_edge();
            g->add_edge(edge.first, edge.second);
        };

        auto tree = go_with_the_winners(empty_graph_generator, go_build_tree);
        if (remove_tree_constraint) {
            tree->constraint_list_ptr()->remove_constraint(Constraint::Type::kTree);
        }
        tree->constraint_list_ptr()->remove_goal_constraint(Constraint::Type::kTree);
        return tree;
    }

    ConstrainedGraphPtr Generator::generate_single_component(ConstraintBlockPtr constraint_list_ptr) {

        // TODO: use not only left_bound but full range
        auto order = constraint_list_ptr->template get_constraint<OrderConstraint>(Constraint::Type::kOrder)->bounds().first;

        auto tree_generator = [&]() -> ConstrainedGraphPtr {
            return generate_tree(order, constraint_list_ptr);
        };
        auto go_build_residue = [](ConstrainedGraphPtr g) {
            g->add_random_edge();
        };
        auto result = go_with_the_winners(tree_generator, go_build_residue, true);
        return result;
    }

    std::vector<std::pair<int, int>> Generator::generate_2connected_graph(int order, int size, int components_number) {
        int minimal_order = 3;
        auto get_large_component_order = [&](int order, int components_number) {
            return order - minimal_order * (components_number - 1);
        };

        auto get_edges_bounds = [&](int order, int components_number) {
            int min_size = order;
            int large_component = get_large_component_order(order, components_number);
            long long max_size = 1LL * large_component * (large_component - 1) / 2 + 3 * (components_number - 1);
            return std::make_pair(min_size, max_size);
        };

        auto possible_to_construct = [&](int order, int size, int components_number) {
            if (!order || !components_number) {
                return !order && !size && !components_number;
            }
            if (order < minimal_order * components_number) {
                return false;
            }
            auto bounds = get_edges_bounds(order, components_number);
            return bounds.first <= size && size <= bounds.second;
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
            int prev_component_order = minimal_order - 1;
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
                component_size = random.next(size_left_bound, size_right_bound);
                do {
                    if (possible_to_construct(next_order, current_size - component_size, components_number - i - 1)) {
                        possible = true;
                        break;
                    }
                    prev_component_size = component_size;
                    component_size = random.next(prev_component_size + 1, size_right_bound);
                } while (prev_component_size != size_right_bound);

                if (possible) {
                    break;
                }
                prev_component_order = component_order;

            } while (prev_component_order != large_component);

            // TODO: yeah i think this condition will always be false, but maybe ...
            if (prev_component_order == large_component) {
                throw std::exception();
            }

            auto component = generate_2connected_component(component_order, component_size);
            int shift = order - current_order;
            for (auto &edge : component) {
                result.emplace_back(edge.first + shift, edge.second + shift);
            }

            current_order -= component_order;
            current_size -= component_size;
        }
        return result;
    }

    std::vector<std::pair<int, int>> Generator::generate_2connected_component(int order, int size) {
        std::vector<std::pair<int, int>> result;
        if (order < 3 || size < order || size > 1LL * order * (order - 1) / 2) {
            return result;
        }

        int circuit_rank = size - order + 1;
        int a1 = circuit_rank == 1 ? order : random.next(3, order);
        for (int i = 0; i < a1; ++i) {
            result.emplace_back(i, (i + 1) % a1);
        }

        int vertices_made = a1;
        int ears_made = 1;

        auto generate_ear = [&](int n) {
            int start = random.next(0, a1 - 1);
            int finish = random.next(0, vertices_made - 1);
            if (start == finish) {
                finish = start == 0 ? 1 : start - 1;
            }
            if (n == 0) {
                result.emplace_back(start, finish);
            }
            else {
                result.emplace_back(start, vertices_made++);
                for (int i = 0; i < n - 1; ++i, ++vertices_made) {
                    result.emplace_back(vertices_made - 1, vertices_made);
                }
                result.emplace_back(vertices_made - 1, finish);
            }
        };

        for (; ears_made < circuit_rank - 1; ++ears_made) {
            int ear_inner_size = random.next(0, order - vertices_made);
            generate_ear(ear_inner_size);
        }
        if (ears_made < circuit_rank) {
            generate_ear(order - vertices_made);
        }
        return result;
    }
}