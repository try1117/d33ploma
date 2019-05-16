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
        kOrder,
        kSize,
        kTree,
        kBridge,
    };

    // TODO: store version and last ConstraintSatisfactionVerdict
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

    class OrderConstraint : public Constraint {
    public:
        explicit OrderConstraint(int order);
        int order();
        ConstraintSatisfactionVerdict check() override;
        ConstraintSatisfactionVerdict check(GraphPtr graph_ptr) override;

    private:
        int order_;
    };

    class SizeConstraint : public Constraint {
    public:
        explicit SizeConstraint(int size);
        int size();
        ConstraintSatisfactionVerdict check() override;
        ConstraintSatisfactionVerdict check(GraphPtr graph_ptr) override;

    private:
        int size_;
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
        ConstraintSatisfactionVerdict check(GraphPtr graph_ptr) override;

    private:
        int latest_connected_vertex_;
    };

    class BridgeConstraint : public Constraint {
    public:
        BridgeConstraint(int left_bound, int right_bound);
        void bind_graph(GraphPtr graph_ptr) override;
//        void add_directed_edge(int from, int to) override;
        ConstraintSatisfactionVerdict check() override;
        ConstraintSatisfactionVerdict check(GraphPtr g) override;

        std::pair<int, int> count_bridges(GraphPtr graph_ptr, bool save_bridges = false);
        std::set<std::pair<int, int>> get_bridges_list();

    private:
        int left_bound_, right_bound_;
        std::vector<int> tin, fup;
        int timer;
        std::set<std::pair<int, int>> bridges_list_;

        int _count_bridges(GraphPtr graph_ptr, int v, int pr, std::pair<int, int> &res, bool save_bridges = false);
    };

    using ConstraintPtr = std::shared_ptr<Constraint>;

    class ConstraintList : public Constraint {
    public:
        ConstraintList();
        void bind_graph(GraphPtr graph_ptr) override;

        std::map<ConstraintType, ConstraintPtr> constraints();
        bool has_constraint(ConstraintType constraint_type);

        void add_constraint(ConstraintPtr constraint_ptr);
        void remove_constraint(ConstraintType constraint_type);

        template <typename T>
        std::shared_ptr<T> get_constraint(ConstraintType constraint_type) {
            if (has_constraint(constraint_type)) {
                return std::static_pointer_cast<T>(constraints_.at(constraint_type));
            }
            return nullptr;
        }

        void add_goal_constraint(ConstraintType constraint_type);
        void remove_goal_constraint(ConstraintType constraint_type);

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
        std::set<ConstraintType> important_constraints_;

        ConstraintSatisfactionVerdict check_goals();
        ConstraintSatisfactionVerdict check_goals(GraphPtr graph_ptr);
    };

    using ConstraintListPtr = std::shared_ptr<ConstraintList>;
}

#endif //GRAPH_CONSTRAINT_SOLVER_CONSTRAINT_H
