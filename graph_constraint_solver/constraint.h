#ifndef GRAPH_CONSTRAINT_SOLVER_CONSTRAINT_H
#define GRAPH_CONSTRAINT_SOLVER_CONSTRAINT_H

#include <functional>
#include <memory>
#include <set>
#include <map>

#include "graph.h"

namespace graph_constraint_solver {

    class Constraint;
    using ConstraintPtr = std::shared_ptr<Constraint>;

    // TODO: store version and last Constraint::SatisfactionVerdict
    class Constraint {
    public:
        enum class SatisfactionVerdict : unsigned char {
            kImpossible = 0,
            kPossible = 1,
            kOK = 2,
        };

        enum class Type : unsigned char {
            kNone,
            kGraphType,
            kOrder,
            kSize,
            kComponentsNumber,
            kTree,
            kBridge,
        };

        explicit Constraint(Type type);
        virtual ConstraintPtr clone() = 0;

        Type type();
        GraphPtr graph_ptr();
        virtual void bind_graph(GraphPtr graph_ptr);

        // TODO: return Edge type instead of std::pair<int, int>
        virtual void add_edge(int from, int to);
        virtual std::pair<int, int> recommend_edge();

        virtual SatisfactionVerdict check() = 0;

    protected:
        GraphPtr graph_ptr_;
        Type type_;
    };

    class GraphTypeConstraint : public Constraint {
    public:
        explicit GraphTypeConstraint(Graph::Type graph_type);
        ConstraintPtr clone() override;
        Graph::Type graph_type();
        SatisfactionVerdict check() override;

    private:
        Graph::Type graph_type_;
    };

    class OrderConstraint : public Constraint {
    public:
        explicit OrderConstraint(int order);
        ConstraintPtr clone() override;
        int order();
        SatisfactionVerdict check();

    private:
        int order_;
    };

    class SizeConstraint : public Constraint {
    public:
        SizeConstraint(int left_bound, int right_bound = -1);
        ConstraintPtr clone() override;
        SatisfactionVerdict check();

    private:
        int left_bound_;
        int right_bound_;
    };

    class ComponentsNumberConstraint : public Constraint {
    public:
        ComponentsNumberConstraint(int left_bound, int right_bound = -1);
        ConstraintPtr clone() override;
        SatisfactionVerdict check();
        std::pair<int, int> bounds();

    private:
        int left_bound_;
        int right_bound_;
    };

    // TODO: real tree constraint (dsu or something)
    class TreeConstraint : public Constraint {
    public:
        explicit TreeConstraint(int weight = 0);
        ConstraintPtr clone() override;
//        void bind_graph(GraphPtr graph_ptr);

        void add_edge(int from, int to) override;
        std::pair<int, int> recommend_edge() override;

        SatisfactionVerdict check() override;

    private:
        int weight_;
        int latest_connected_vertex_;
    };

    class BridgeConstraint : public Constraint {
    public:
        BridgeConstraint(int left_bound, int right_bound);
        ConstraintPtr clone() override;
        void bind_graph(GraphPtr graph_ptr) override;
//        void add_edge(int from, int to) override;
        SatisfactionVerdict check();

        std::pair<int, int> count_bridges(GraphPtr graph_ptr, bool save_bridges = false);
        std::set<std::pair<int, int>> get_bridges_list();

    private:
        int left_bound_, right_bound_;
        std::vector<int> tin, fup;
        int timer;
        std::set<std::pair<int, int>> bridges_list_;

        int _count_bridges(GraphPtr graph_ptr, int v, int pr, std::pair<int, int> &res, bool save_bridges = false);
    };

    class ConstraintList : public Constraint {
    public:
        ConstraintList();
        ConstraintList(ConstraintList &other);
        ConstraintPtr clone() override;

        void bind_graph(GraphPtr graph_ptr) override;

        std::map<Type, ConstraintPtr> constraints();
        bool has_constraint(Type constraint_type);

        void add_constraint(ConstraintPtr constraint_ptr);
        void remove_constraint(Type constraint_type);

        template <typename T>
        std::shared_ptr<T> get_constraint(Type constraint_type) {
            if (has_constraint(constraint_type)) {
                return std::static_pointer_cast<T>(constraints_.at(constraint_type));
            }
            return nullptr;
        }

        void add_goal_constraint(Type constraint_type);
        void remove_goal_constraint(Type constraint_type);

        void add_edge(int, int) override;

        SatisfactionVerdict check();

        // TODO: parse
        void parse_JSON(std::string filepath) {}
        void parse_XML(std::string filepath) {}

    private:
        // TODO: maybe we need std::map<Constraint::Type, std::vector<ConstraintPtr>>
        // not sure now whether we'll have multiple constraints of the same type
        std::map<Type, ConstraintPtr> constraints_;
        std::set<Type> important_constraints_;

        SatisfactionVerdict check_goals();
    };

    using ConstraintListPtr = std::shared_ptr<ConstraintList>;
}

#endif //GRAPH_CONSTRAINT_SOLVER_CONSTRAINT_H
