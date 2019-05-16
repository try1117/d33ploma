#include "generator.h"

#include <vector>
#include <memory>

namespace graph_constraint_solver {

    ConstrainedGraphPtr Generator::generate(ConstraintListPtr constraint_list_ptr) {
        auto order = constraint_list_ptr->template get_constraint<OrderConstraint>(kOrder)->order();
        return single_component_generator(order, constraint_list_ptr);
    }

//    int sub_iterations;
//    int sub_colony_multiplications;

    ConstrainedGraphPtr Generator::go_with_the_winners(ConstrainedGraphPtr start_graph_ptr, GoNext go_next,
            int colony_size, int growth_rate, int outer_iterations, int inner_iterations) {

        if (start_graph_ptr->check() == kOK) {
            return start_graph_ptr;
        }

        int threshold = colony_size / growth_rate;

        for (int outer = 0; outer < outer_iterations; ++outer) {
            std::vector<ConstrainedGraphPtr> graphs;
            for (int i = 0; i < colony_size; ++i) {
                // TODO: copy constructor (as we have pointers in ConstrainedGraph)
                graphs.push_back(std::make_shared<ConstrainedGraph>(*start_graph_ptr));
            }

            for (int inner = 0; inner < inner_iterations && !graphs.empty(); ++inner) {
//            ++sub_iterations;
                std::vector<ConstrainedGraphPtr> graphs_nxt;
                for (auto &g : graphs) {
                    go_next(g);
                    auto constraint_verdict = g->check();
                    if (constraint_verdict == kOK) {
                        if (g->graph_ptr()->size() > 30) {
                            g->check();
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

    ConstrainedGraphPtr Generator::single_component_generator(int order, ConstraintListPtr constraint_list_ptr) {

        bool remove_tree_constraint = false;
        if (!constraint_list_ptr->has_constraint(kTree)) {
            constraint_list_ptr->add_constraint(std::make_shared<TreeConstraint>());
            remove_tree_constraint = true;
        }
        constraint_list_ptr->add_goal_constraint(kTree);

        auto go_build_tree = [](ConstrainedGraphPtr g) {
            auto edge = g->constraint_list_ptr()->constraints().at(kTree)->recommend_undirected_edge();
            g->add_undirected_edge(edge.first, edge.second);
        };

        ConstrainedGraphPtr empty_graph = std::make_shared<ConstrainedGraph>(constraint_list_ptr, std::make_shared<Graph>(order));

        auto tree = go_with_the_winners(empty_graph, go_build_tree);
        if (remove_tree_constraint) {
            constraint_list_ptr->remove_constraint(kTree);
        }
        constraint_list_ptr->remove_goal_constraint(kTree);

        auto go_build_residue = [](ConstrainedGraphPtr g) {
            g->add_random_undirected_edge();
        };
        auto result = go_with_the_winners(tree, go_build_residue);
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