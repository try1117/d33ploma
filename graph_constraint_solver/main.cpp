#include <bits/stdc++.h>
//#include <boost/graph/graphml.hpp>
using namespace std;
using ll = long long;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
//mt19937 rng(1488);

GraphPtr gen_rand_tree(int n) {
    GraphPtr tree(new Graph(n));
    for (int i = 1; i < n; ++i) {
        int parent = rng() % i;
        tree->add_undirected_edge(i, parent);
    }
    return tree;
}

int sub_iterations;
int sub_colony_multiplications;

GraphPtr gen_rand_graph_bridges(int n, int left_bridges_bound, int right_bridges_bound) {

    auto check_cons = [&](GraphPtr g) {
        auto cnt = g->count_bridges();
        return left_bridges_bound <= cnt.first;
    };
    auto check_win = [&](GraphPtr g) {
        auto cnt = g->count_bridges();
        return left_bridges_bound <= cnt.first && cnt.first <= right_bridges_bound;
    };
    auto go_next = [&](GraphPtr g) {
        g->add_random_edge();
    };

//    return go_with_the_winners(tree, add_random_edge, check_cons, check_win);
    return go_go(n, go_next, check_cons, check_win);
}

int main() {
    auto timeit = [&](function<void()> f) {
        auto start = chrono::high_resolution_clock::now();
        f();
        auto finish = chrono::high_resolution_clock::now();

        chrono::duration<double> res = finish - start;
        return res.count();
    };

    int n = 2000;
    GraphPtr g;
    pair<int, int> bridges_cnt;

    auto tim = timeit([&]() {
        g = gen_rand_graph_bridges(n, 2, 3);
        bridges_cnt = g->count_bridges(true);
    });

    int edges = 0;
    for (int i = 0; i < n; ++i) {
        for (auto child : g->g[i]) {
            if (i <= child) {
                ++edges;
//                printf("g.add_edge(%d, %d, color='%s')\n", i, child, (bridges_list.count({i, child}) ? "red" : "blue"));
            }
        }
    }

    cout << "Bridg, nonlv: " << bridges_cnt.first << ", " << bridges_cnt.second << endl;
    cout << "Edges       : " << edges << endl;
    cout << endl;
    cout << "Time        : " << tim << endl;
}
