#include "generator.h"

#include <vector>
#include <memory>

namespace graph_constraint_solver {

    GraphPtr Generator::go_with_the_winners(GraphPtr start, GoNext go_next, ConstraintPtr constraint,
            int sz, int mul, int max_iter = 1000) {

        std::vector<GraphPtr> graphs;
        for (int i = 0; i < sz; ++i) {
            graphs.push_back(std::make_shared<Graph>(*start));
        }

        int threshold = sz / mul;
        for (int iter = 0; iter < max_iter && !graphs.empty(); ++iter) {
            ++sub_iterations;
            std::vector<GraphPtr> graphs_nxt;
            for (auto &g : graphs) {
                go_next(g);
                if (check_cons(g)) {
                    if (check_win(g)) return g;
                    graphs_nxt.push_back(g);
                }
            }

            graphs.clear();
            if (graphs_nxt.size() <= threshold) {
                ++sub_colony_multiplications;
                for (auto &g: graphs_nxt) {
                    for (int i = 0; i < mul; ++i) {
                        graphs.push_back(g);
                    }
                }
            }
            else {
                graphs = graphs_nxt;
            }
        }

        return std::make_shared<Graph>();
    }

    GraphPtr Generator::go_go(int n, GoNext go_next, ConstraintPtr constraint,
            int sz, int mul, int max_iter) {

        for (int i = 0; i < 100; ++i) {
            auto start = gen_rand_tree(n);
            auto res = go_with_the_winners(start, go_next, check_cons, check_win, sz, mul, max_iter);
            if (!res->empty()) {
                cout << "Main        : " << i + 1 << endl;
                cout << "Sub avg     : " << fixed << 1.0 * sub_iterations / (i + 1) << endl;
//            cout << "Edgepick avg: " << fixed << 1.0 * edge_pick_iterations / sub_iterations << endl;
                cout << "Edgepick tot: " << edge_pick_iterations << endl;
                cout << "Colony size : " << sz << endl;
                cout << "Colony mul  : " << sub_colony_multiplications << endl;
                return res;
            }
        }
        return make_shared<Graph>();
    }
}