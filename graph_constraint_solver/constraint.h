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
        static SatisfactionVerdict lies_in_between(int left_bound, int value, int right_bound, bool increasing = true);

    protected:
        GraphPtr graph_ptr_;
        Type type_;
    };

    template <typename T, Constraint::Type constraint_type>
    class BoundedValueConstraint : public Constraint {
    public:
        BoundedValueConstraint(T left_bound, T right_bound)
                : Constraint(constraint_type), left_bound_(left_bound), right_bound_(right_bound) {
        }

        explicit BoundedValueConstraint(T left_bound)
                : BoundedValueConstraint(left_bound, left_bound) {
        }

        explicit BoundedValueConstraint(std::pair<T, T> bounds)
            : BoundedValueConstraint(bounds.first, bounds.second) {
        }

        virtual T value() = 0;

        std::pair<T, T> bounds() {
            return std::pair(left_bound_, right_bound_);
        }

        SatisfactionVerdict check() override {
            return lies_in_between(left_bound_, value(), right_bound_);
        }

    protected:
        T left_bound_;
        T right_bound_;
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

    class OrderConstraint : public BoundedValueConstraint<int, Constraint::Type::kOrder> {
    public:
        using BoundedValueConstraint::BoundedValueConstraint;
        ConstraintPtr clone() override;
        int value() override;
    };

    class SizeConstraint : public BoundedValueConstraint<int, Constraint::Type::kSize> {
    public:
        using BoundedValueConstraint::BoundedValueConstraint;
        ConstraintPtr clone() override;
        int value() override;
    };

    class ComponentsNumberConstraint : public BoundedValueConstraint<int, Constraint::Type::kComponentsNumber> {
    public:
        using BoundedValueConstraint::BoundedValueConstraint;
        ConstraintPtr clone() override;
        int value() override;
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
}

#endif //GRAPH_CONSTRAINT_SOLVER_CONSTRAINT_H
