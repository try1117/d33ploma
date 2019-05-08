#include <bits/stdc++.h>
//#include <boost/graph/graphml.hpp>
using namespace std;
using ll = long long;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
//mt19937 rng(1488);

const int MAXN = 1e4 + 10;
int timer;
int tin[MAXN], fup[MAXN];
set<pair<int, int>> bridges_list;

int edge_pick_iterations;

class Graph {
public:
    int n;
    vector<vector<int>> g;
    vector<vector<bool>> ma;

    Graph(int n = 0) : n(n), g(n, vector<int>()), ma(n, vector<bool>(n))
    {
    }
//
//    Graph(const Graph &arg) {
//
//    }

    bool empty() {
        return n == 0;
    }

    void add_undirected_edge(int u, int v) {
        add_directed_edge(u, v);
        add_directed_edge(v, u);
    }

    void add_directed_edge(int u, int v) {
        g[u].emplace_back(v);
        ma[u][v] = true;
    }

    void add_random_edge() {
        int u = rng() % n;
        while (g[u].size() == n) u = rng() % n, ++edge_pick_iterations;

        int v = rng() % n;
        while (ma[u][v]) v = rng() % n, ++edge_pick_iterations;

        add_undirected_edge(u, v);
    }

    int _count_bridges(int v, int pr, int &res, bool save_bridges = false) {
        tin[v] = fup[v] = ++timer;
        for (auto child : g[v]) {
            if (child != pr) {
                if (!tin[child]) {
                    _count_bridges(child, v, res, save_bridges);
                    if (fup[child] > tin[v]) {
                        ++res;
                        if (save_bridges) {
                            bridges_list.emplace(min(v, child), max(v, child));
                        }
                    }
                    fup[v] = min(fup[v], fup[child]);
                }
                else {
                    fup[v] = min(fup[v], tin[child]);
                }
            }
        }
    }

    int count_bridges(bool save_bridges = false) {
        timer = 0;
        fill(tin, tin + n, 0);
        fill(fup, fup + n, 0);
        int res = 0;
        _count_bridges(0, -1, res, save_bridges);
        return res;
    }
};

using GraphPtr = shared_ptr<Graph>;

//using Graph = vector<vector<int>>;
//using Graph = vector<set<int>>;

using GoNext = function<void(GraphPtr)>;
using CheckConstraints = function<bool(GraphPtr)>;
using CheckWin = function<bool(GraphPtr)>;

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

GraphPtr go_with_the_winners(GraphPtr start, GoNext go_next, CheckConstraints check_cons,
                          CheckWin check_win, int sz = 20, int mul = 2, int max_iter = 10000) {

    vector<GraphPtr> graphs(sz, make_shared<Graph>(*start));
    int threshold = sz / mul;
    for (int iter = 0; iter < max_iter && !graphs.empty(); ++iter) {
        ++sub_iterations;
        vector<GraphPtr> graphs_nxt;
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

    return make_shared<Graph>();
}

GraphPtr go_go(int n, GoNext go_next, CheckConstraints check_cons,
            CheckWin check_win, int sz = 10, int mul = 2, int max_iter = 10000) {

    for (int i = 0; i < 100; ++i) {
        auto start = gen_rand_tree(n);
        auto res = go_with_the_winners(start, go_next, check_cons, check_win, sz, mul, max_iter);
        if (!res->empty()) {
            cout << "Main        : " << i + 1 << endl;
            cout << "Sub avg     : " << fixed << 1.0 * sub_iterations / (i + 1) << endl;
            cout << "Edgepick avg: " << fixed << 1.0 * edge_pick_iterations / sub_iterations << endl;
            cout << "Edgepick tot: " << edge_pick_iterations << endl;
            cout << "Colony mul: " << sub_colony_multiplications << endl;
            return res;
        }
    }
    return make_shared<Graph>();
}

GraphPtr gen_rand_graph_bridges(int n, int left_bridges_bound, int right_bridges_bound) {

    auto check_cons = [&](GraphPtr g) {
        int cnt = g->count_bridges();
        return left_bridges_bound <= cnt;
    };
    auto check_win = [&](GraphPtr g) {
        int cnt = g->count_bridges();
        return left_bridges_bound <= cnt && cnt <= right_bridges_bound;
    };
    auto go_next = [&](GraphPtr g) {
        g->add_random_edge();
    };

//    return go_with_the_winners(tree, add_random_edge, check_cons, check_win);
    return go_go(n, go_next, check_cons, check_win);
}

signed main() {
#ifndef ONLINE_JUDGE
    freopen("input.txt", "r", stdin);
#endif
//    ios_base::sync_with_stdio(0); cin.tie(0);

    auto timeit = [&](function<void()> f) {
        auto start = chrono::high_resolution_clock::now();
        f();
        auto finish = chrono::high_resolution_clock::now();

        chrono::duration<double, std::milli> res = finish - start;
        return res.count();
    };

    int n = 2000;
    GraphPtr g;
    auto tim = timeit([&]() {
        g = gen_rand_graph_bridges(n, 3, 5);
        int bridges_cnt = g->count_bridges();
        cout << bridges_cnt << endl;
    });

    cout << tim;

//    for (int i = 0; i < n; ++i) {
//        for (auto child : g->g[i]) {
//            if (i <= child) {
//                printf("g.add_edge(%d, %d, color='%s')\n", i, child, (bridges_list.count({i, child}) ? "red" : "blue"));
//            }
//        }
//    }
}
