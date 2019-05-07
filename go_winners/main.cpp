#include <bits/stdc++.h>
//#include <boost/graph/graphml.hpp>
using namespace std;
using ll = long long;

//mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
mt19937 rng(1488);

class Graph {
public:
    int n;
    Graph(int n = 0) : n(n) {}

    virtual void add_directed_edge(int u, int v) = 0;

    void add_undirected_edge(int u, int v) {
        add_directed_edge(u, v);
        add_directed_edge(v, u);
    }
    virtual void add_random_edge() = 0;

//    virtual iterator_traits<int> edges() = 0;
    virtual initializer_list<int> edges() = 0;

    static const int MAXN = 1e4 + 10;
    static int timer;
    static int tin[MAXN], fup[MAXN];
    set<pair<int, int>> bridges_list;

    int _count_bridges(Graph &g, int v, int pr, int &res, bool save_bridges = false) {

        for (int child : )

        tin[v] = fup[v] = ++timer;
        for (auto child : g[v]) {
            if (child != pr) {
                if (!tin[child]) {
                    _count_bridges(g, child, v, res, save_bridges);
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

    int count_bridges(Graph &g, bool save_bridges = false) {
        timer = 0;
        fill(tin, tin + n, 0);
        fill(fup, fup + n, 0);
        int res = 0;
        _count_bridges(g, 0, -1, res, save_bridges);
        return res;
    }
};

class GraphOnSets : public Graph {
public:
    GraphOnSets(int n) : Graph(n), g(n, set<int>()) {};

    void add_directed_edge(int u, int v) override {
        g[u].emplace(v);
    }

    void add_random_edge() {
        int u = rng() % n;
        while (g[u].size() == n) u = rng() % n;

        int v = rng() % n;
        while (g[u].count(v)) v = rng() % n;

        add_undirected_edge(u, v);
    }

    initializer_list<int> edges() {
        return g[0];
    }

//     edges() { return g[0].begin(); }

//    vector<set<int>> edges() { return g; }
private:
    vector<set<int>> g;
};

class GraphOnVectors : public Graph {
public:
    GraphOnVectors(int n) : Graph(n), g(n, vector<int>()), ma(n, vector<bool>(n)) {};

    void add_directed_edge(int u, int v) override {
        g[u].emplace_back(v);
        ma[u][v] = true;
    }

    void add_random_edge() {
        int u = rng() % n;
        while (g[u].size() == n) u = rng() % n;

        int v = rng() % n;
        while (ma[u][v]) v = rng() % n;

        add_undirected_edge(u, v);
    }
private:
    vector<vector<int>> g;
    vector<vector<bool>> ma;
};

//using Graph = vector<vector<int>>;
//using Graph = vector<set<int>>;

using GoNext = function<void(Graph&)>;
using CheckConstraints = function<bool(Graph&)>;
using CheckWin = function<bool(Graph&)>;

template<typename T>
T gen_rand_tree(int n) {
    T tree(n);
    for (int i = 1; i < n; ++i) {
        int parent = rng() % i;
        tree.add_undirected_edge(i, parent);
    }
    return tree;
}

Graph go_with_the_winners(Graph &start, GoNext go_next, CheckConstraints check_cons,
                          CheckWin check_win, int sz = 20, int mul = 2, int max_iter = 10000) {

    vector<Graph> graphs(sz, start);
    int threshold = sz / mul;
    for (int iter = 0; iter < max_iter && !graphs.empty(); ++iter) {
        vector<Graph> graphs_nxt;
        for (auto &g : graphs) {
            go_next(g);
            if (check_cons(g)) {
                if (check_win(g)) return g;
                graphs_nxt.push_back(g);
            }
        }

        graphs.clear();
        if (graphs_nxt.size() <= threshold) {
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

    return Graph();
}

Graph go_go(int n, GoNext go_next, CheckConstraints check_cons,
            CheckWin check_win, int sz = 20, int mul = 2, int max_iter = 10000) {

    for (int i = 0; i < 100; ++i) {
        auto start = gen_rand_tree(n);
        auto res = go_with_the_winners(start, go_next, check_cons, check_win, sz, mul, max_iter);
        if (!res.empty()) return res;
    }
    return Graph();
}

Graph gen_rand_graph_bridges(int n, int left_bridges_bound, int right_bridges_bound) {

    auto check_cons = [&](Graph &g) {
        int cnt = count_bridges(g);
        return left_bridges_bound <= cnt;
    };
    auto check_win = [&](Graph &g) {
        int cnt = count_bridges(g);
        return left_bridges_bound <= cnt && cnt <= right_bridges_bound;
    };

//    return go_with_the_winners(tree, add_random_edge, check_cons, check_win);
    return go_go(n, add_random_edge, check_cons, check_win);
}

signed main() {
#ifndef ONLINE_JUDGE
    freopen("input.txt", "r", stdin);
#endif
    ios_base::sync_with_stdio(0); cin.tie(0);

    int n = 300;
    auto g = gen_rand_graph_bridges(n, 25, 28);
    int bridges_cnt = count_bridges(g, true);
    cout << bridges_cnt;

//    for (int i = 0; i < n; ++i) {
//        for (auto child : g[i]) {
//            if (i <= child) {
//                printf("g.add_edge(%d, %d, color='%s')\n", i, child, (bridges_list.count({i, child}) ? "red" : "blue"));
//            }
//        }
//    }
}
