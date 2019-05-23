#ifndef GRAPH_CONSTRAINT_SOLVER_GRAPH_H
#define GRAPH_CONSTRAINT_SOLVER_GRAPH_H

#include <memory>
#include <functional>
#include <vector>
#include <set>

namespace graph_constraint_solver {

    class Graph;
    using GraphPtr = std::shared_ptr<Graph>;

    // TODO: multi-edge and self-loops
    // TODO: directed/undirected graph
    // in future think about using boost::graph or something like that
    // now it is primarily undirected graph
    class Graph {
    public:
        enum class Type : unsigned char {
            kDirected,
            kUndirected,
        };

        Graph(int order = 0, Type type = Type::kUndirected);
        // TODO: edge type???
        Graph(int order, Type type, std::vector<std::pair<int, int>> edges);

        Type type();
        // number of vertices
        int order();
        // number of undirected edges
        int size();
        const std::vector<std::vector<int>>& adjacency_list();

        bool empty();
        bool is_leaf(int v);

        void add_edge(int u, int v);
        std::pair<int, int> generate_random_edge();

        void append_graph(GraphPtr other);

    private:
        void add_undirected_edge(int u, int v);
        void add_directed_edge(int u, int v);

        std::pair<int, int> generate_random_directed_edge();
        std::pair<int, int> generate_random_undirected_edge();

        Type type_;
        int order_;
        int size_;
        // TODO: do i really need this greatest_variable_ever ?
        int greatest_used_vertex_index_;
        std::vector<std::vector<int>> g_;
//        std::vector<std::vector<bool>> ma_;
        std::set<std::pair<int, int>> ma_;
    };
}

#endif //GRAPH_CONSTRAINT_SOLVER_GRAPH_H
