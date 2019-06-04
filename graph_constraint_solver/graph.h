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
        enum class Type : unsigned char {
            kDirected,
            kUndirected,
        };

        Graph(int order = 0, Type type = Type::kUndirected);
        // TODO: edge type???

        Type type();
        // number of vertices
        int order();
        // number of edges
        int size();
        const std::vector<std::vector<int>>& adjacency_list();
        bool empty();

        virtual GraphPtr clone() = 0;
        virtual void add_edge(int u, int v) = 0;
        void append_graph(GraphPtr other);
        void shuffle();

    protected:
        Type type_;
        int order_;
        int size_;
        std::vector<std::vector<int>> g_;
//        std::vector<std::vector<bool>> ma_;
//        std::set<std::pair<int, int>> ma_;
    };

    class UndirectedGraph : public Graph {
    public:
        UndirectedGraph(int order = 0);
        UndirectedGraph(int order, const std::vector<std::pair<int, int>> &edges);
        GraphPtr clone() override;
        void add_edge(int u, int v) override;
    };

    class DirectedGraph : public Graph {
    public:
        DirectedGraph(int order = 0);
        DirectedGraph(int order, const std::vector<std::pair<int, int>> &edges);
        GraphPtr clone() override;
        void add_edge(int u, int v) override;
    };

    class GraphComponents {
    public:
        GraphComponents();
        GraphComponents(std::vector<GraphPtr> &components);
        std::vector<GraphPtr>& components();
        std::shared_ptr<GraphComponents> clone();
        bool empty();
        void add_component(GraphPtr component_ptr);
        GraphPtr get_component(int index);

    private:
        std::vector<GraphPtr> components_;
    };

    using GraphComponentsPtr = std::shared_ptr<GraphComponents>;
}

#endif //GRAPH_CONSTRAINT_SOLVER_GRAPH_H
