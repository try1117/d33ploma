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
    // in the future think about using boost::graph or something like that
    class Graph {
    public:
        using OrderType = int;
        using SizeType = long long;
        using EdgeType = std::pair<OrderType, OrderType>;

        static const OrderType kMaximumOrder = static_cast<OrderType>(3e6);
        static const SizeType kMaximumSize = static_cast<SizeType>(3e6);

        enum class Type : unsigned char {
            kDirected,
            kUndirected,
        };

        static GraphPtr create(OrderType order, Type type);
        Graph(OrderType order = 0, Type type = Type::kUndirected);

        Type type();
        // number of vertices
        OrderType order();
        // number of edges
        SizeType size();
        const std::vector<std::vector<OrderType>>& adjacency_list();
        bool empty();
        OrderType vertex_degree(OrderType index);

        virtual GraphPtr clone() = 0;
        virtual void add_edge(OrderType from, OrderType to) = 0;
        void add_edge(EdgeType e);

        void add_edges(const std::vector<EdgeType> &edges);
        void append_graph(GraphPtr other, size_t shift);
        void shuffle();

    protected:
        Type type_;
        OrderType order_;
        SizeType size_;
        std::vector<std::vector<OrderType>> adjacency_list_;
//        std::vector<std::vector<bool>> ma_;
//        std::set<std::pair<int, int>> ma_;
    };

    class UndirectedGraph : public Graph {
    public:
        UndirectedGraph(OrderType order = 0);
        GraphPtr clone() override;
        void add_edge(OrderType from, OrderType to) override;
    };

    class DirectedGraph : public Graph {
    public:
        DirectedGraph(OrderType order = 0);
        GraphPtr clone() override;
        void add_edge(OrderType from, OrderType to) override;
    };
}

#ifdef GRAPH_CONSTRAINT_SOLVER_SINGLE_HEADER
#include "graph.cpp"
#endif

#endif //GRAPH_CONSTRAINT_SOLVER_GRAPH_H
