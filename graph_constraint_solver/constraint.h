#ifndef GRAPH_CONSTRAINT_SOLVER_CONSTRAINT_H
#define GRAPH_CONSTRAINT_SOLVER_CONSTRAINT_H

#include <functional>
#include <memory>
#include <set>
#include <map>

#include "graph.h"

namespace graph_constraint_solver {

    enum ConstraintSatisfactionVerdict : unsigned char {
            kImpossible = 0,
            kPossible = 1,
            kOK = 2,
    };

    enum ConstraintType : unsigned char {
        kNone,
        kTree,
        kBridge,
    };

    class Constraint {
    public:
        explicit Constraint(ConstraintType type);
        ConstraintType type();
        virtual void bind_graph(GraphPtr graph_ptr);

        // TODO: return Edge type instead of std::pair<int, int>
        virtual void add_directed_edge(int from, int to);
        virtual void add_undirected_edge(int from, int to);
        virtual std::pair<int, int> recommend_directed_edge();
        virtual std::pair<int, int> recommend_undirected_edge();

        virtual ConstraintSatisfactionVerdict check() = 0;
        virtual ConstraintSatisfactionVerdict check(GraphPtr graph_ptr) = 0;

    protected:
        GraphPtr graph_ptr_;
        ConstraintType type_;
    };

    class TreeConstraint : public Constraint {
    public:
        TreeConstraint();
//        void bind_graph(GraphPtr graph_ptr);

        void add_directed_edge(int from, int to) override;
        void add_undirected_edge(int from, int to) override;
        std::pair<int, int> recommend_directed_edge() override;
        std::pair<int, int> recommend_undirected_edge() override;

        ConstraintSatisfactionVerdict check() override;
        ConstraintSatisfactionVerdict check(GraphPtr g) override;

    private:
        int latest_connected_vertex_;
    };

    class BridgeConstraint : public Constraint {
    public:
        BridgeConstraint(int left_bound, int right_bound);
//        void bind_graph(GraphPtr graph_ptr) override;
//        void add_directed_edge(int from, int to) override;
        ConstraintSatisfactionVerdict check() override;
        ConstraintSatisfactionVerdict check(GraphPtr g) override;

    private:
        int left_bound_, right_bound_;
        std::vector<int> tin, fup;
        int timer;
        std::set<std::pair<int, int>> bridges_list;

        int _count_bridges(GraphPtr graph_ptr, int v, int pr, std::pair<int, int> &res, bool save_bridges = false);
        std::pair<int, int> count_bridges(GraphPtr graph_ptr, bool save_bridges = false);
    };

    using ConstraintPtr = std::shared_ptr<Constraint>;

    class ConstraintList : public Constraint {
    public:
        ConstraintList();
        std::map<ConstraintType, ConstraintPtr> constraints();
        void bind_graph(GraphPtr graph_ptr);

        void add_constraint(ConstraintPtr constraint_ptr);
        void add_directed_edge(int, int) override;
        void add_undirected_edge(int, int) override;

        ConstraintSatisfactionVerdict check() override;
        ConstraintSatisfactionVerdict check(GraphPtr graph_ptr) override;

        // TODO: parse
        void parse_JSON(std::string filepath) {}
        void parse_XML(std::string filepath) {}

    private:
        // TODO: maybe we need std::map<ConstraintType, std::vector<ConstraintPtr>>
        // not sure now whether we'll have multiple constraints of the same type
        std::map<ConstraintType, ConstraintPtr> constraints_;
    };

    using ConstraintListPtr = std::shared_ptr<ConstraintList>;
}

#endif //GRAPH_CONSTRAINT_SOLVER_CONSTRAINT_H
