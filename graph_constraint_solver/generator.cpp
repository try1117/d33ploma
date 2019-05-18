#include "generator.h"

#include <vector>
#include <memory>
#include <iostream>

#include "utils.h"

namespace graph_constraint_solver {

    ConstrainedGraphPtr Generator::generate(ConstraintListPtr constraint_list_ptr) {
        auto order = constraint_list_ptr->template get_constraint<OrderConstraint>(kOrder)->order();
        return generate_single_component(order, constraint_list_ptr);
    }

    ConstrainedGraphPtr Generator::go_with_the_winners(GraphGenerator initial_graph_generator, GoNext go_next, bool to_print,
            int colony_size, int growth_rate, int outer_iterations, int inner_iterations) {

        int threshold = colony_size / growth_rate;

        int total_inner_iterations = 0;
        int total_inner_true_iterations = 0;
        int total_colony_multiplications = 0;

        for (int outer = 0; outer < outer_iterations; ++outer) {

            auto initial_graph_ptr = initial_graph_generator();
            if (initial_graph_ptr->check() == kOK) {
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
                    if (constraint_verdict == kOK) {
                        if (to_print) {
                            std::cout << "Outer           : " << outer + 1 << std::endl;
                            std::cout << "Inner last      : " << inner + 1 << std::endl;
                            std::cout << "Inner avg       : " << 1.0 * total_inner_iterations / (outer + 1) << std::endl;
                            std::cout << "Inner total     : " << total_inner_iterations << std::endl;
                            std::cout << "Inner total true: " << total_inner_true_iterations << std::endl;
                            std::cout << std::endl;
                        }
                        return g;
                    }
                    if (constraint_verdict == kPossible) {
                        graphs_nxt.push_back(g);
                    }
                }

                graphs.clear();
                if (graphs_nxt.size() <= threshold) {
//                ++sub_colony_multiplications;
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

    ConstrainedGraphPtr Generator::generate_tree(int order, ConstraintListPtr constraint_list_ptr) {
        bool remove_tree_constraint = false;
        if (!constraint_list_ptr->has_constraint(kTree)) {
            constraint_list_ptr->add_constraint(std::make_shared<TreeConstraint>(random.next(-10, 10)));
            remove_tree_constraint = true;
        }
        constraint_list_ptr->add_goal_constraint(kTree);

        auto empty_graph_generator = [&]() -> ConstrainedGraphPtr {
            return std::make_shared<ConstrainedGraph>(constraint_list_ptr, std::make_shared<Graph>(order));
        };
        auto go_build_tree = [](ConstrainedGraphPtr g) {
            auto edge = g->constraint_list_ptr()->template get_constraint<TreeConstraint>(kTree)->recommend_undirected_edge();
            g->add_undirected_edge(edge.first, edge.second);
        };

        auto tree = go_with_the_winners(empty_graph_generator, go_build_tree);
        if (remove_tree_constraint) {
            tree->constraint_list_ptr()->remove_constraint(kTree);
        }
        tree->constraint_list_ptr()->remove_goal_constraint(kTree);
        return tree;
    }

    ConstrainedGraphPtr Generator::generate_single_component(int order, ConstraintListPtr constraint_list_ptr) {

        auto tree_generator = [&]() -> ConstrainedGraphPtr {
            return generate_tree(order, constraint_list_ptr);
        };
        auto go_build_residue = [](ConstrainedGraphPtr g) {
            g->add_random_undirected_edge();
        };
        auto result = go_with_the_winners(tree_generator, go_build_residue, true);
        return result;

//        for (int i = 0; i < 100; ++i) {
//            auto start = gen_rand_tree(n);
//            auto res = go_with_the_winners(start, go_next, constraint_list_ptr, sz, mul, max_iter);
//            if (!res->empty()) {
////                cout << "Main        : " << i + 1 << endl;
////                cout << "Sub avg     : " << fixed << 1.0 * sub_iterations / (i + 1) << endl;
////            cout << "Edgepick avg: " << fixed << 1.0 * edge_pick_iterations / sub_iterations << endl;
////                cout << "Edgepick tot: " << edge_pick_iterations << endl;
////                cout << "Colony size : " << sz << endl;
////                cout << "Colony mul  : " << sub_colony_multiplications << endl;
//                return res;
//            }
//        }
//        return std::make_shared<Graph>();
    }
}