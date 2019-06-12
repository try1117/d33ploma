#ifndef GRAPH_CONSTRAINT_SOLVER_CONSTRAINT_H
#define GRAPH_CONSTRAINT_SOLVER_CONSTRAINT_H

#include <functional>
#include <memory>
#include <set>
#include <map>
#include <string>

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

        // TODO: component constraints duplicate ordinary ones
        // maybe we should do something about it
        enum class Type : unsigned char {
            kNone,
            kGraphType,
            kOrder,
            kSize,
            kBridge,
            kCutPoint,

            kComponentNumber,
            kComponentOrder,
            kComponentSize,
            kComponentCutPoint,
            kComponentBridge,
            kComponentDiameter,
            kComponentVertexMaxDegree,

//            kTreeBroadness,
//            kVertexMaxDegree,
        };

        using OrderBounds = std::pair<Graph::OrderType, Graph::OrderType>;
        using SizeBounds = std::pair<Graph::SizeType, Graph::SizeType>;

        template<typename T> struct TypeToEnumIdMap {
            static const Type type_id;
            Type operator()() {
                return type_id;
            }
        };

        explicit Constraint(Type type);
        virtual ConstraintPtr clone() = 0;

        Type type();
        const std::string type_name();
        static const std::string type_to_name(Type type);

        GraphPtr graph_ptr();
        virtual void bind_graph(GraphPtr graph_ptr);

        // TODO: return Edge type instead of std::pair<int, int>
//        virtual void add_edge(int from, int to);
//        virtual std::pair<int, int> recommend_edge();

        virtual SatisfactionVerdict check() = 0;
        template <typename T>
        static SatisfactionVerdict lies_in_between(T left_bound, T value, T right_bound, bool increasing = true) {
            if (value > right_bound) {
                if (increasing) return Constraint::SatisfactionVerdict::kImpossible;
                return Constraint::SatisfactionVerdict::kPossible;
            }
            if (value < left_bound) {
                if (increasing) return Constraint::SatisfactionVerdict::kPossible;
                return Constraint::SatisfactionVerdict::kImpossible;
            }
            return Constraint::SatisfactionVerdict::kOK;
        }

    protected:
        GraphPtr graph_ptr_;
        Type type_;
        static const std::unordered_map<Type, std::string> type_to_name_;
    };

    template <typename T, Constraint::Type constraint_type>
    class SingleValueConstraint : public Constraint {
    public:
        SingleValueConstraint(T value)
            : Constraint(constraint_type), value_(value) {
        }

        T value() {
            return value_;
        }

    protected:
        T value_;
    };

    class GraphTypeConstraint : public SingleValueConstraint<Graph::Type, Constraint::Type::kGraphType> {
    public:
        using SingleValueConstraint::SingleValueConstraint;
        ConstraintPtr clone() override;
        SatisfactionVerdict check() override;
    };

    class ComponentVertexMaxDegreeConstraint : public SingleValueConstraint<Graph::OrderType, Constraint::Type::kComponentVertexMaxDegree> {
    public:
        using SingleValueConstraint::SingleValueConstraint;
        ConstraintPtr clone() override;
        SatisfactionVerdict check() override;
    };

    template <typename T, Constraint::Type constraint_type>
    class BoundedValueConstraint : public Constraint {
    public:
        BoundedValueConstraint(T left_bound, T right_bound)
                : Constraint(constraint_type), left_bound_(left_bound), right_bound_(right_bound) {

            if (left_bound_ > right_bound_) {
                throw std::invalid_argument("BoundedValueConstraint: left bound cannot be greater than right bound");
            }
        }

        BoundedValueConstraint(T left_bound)
                : BoundedValueConstraint(left_bound, left_bound) {
        }

        BoundedValueConstraint(std::pair<T, T> bounds)
            : BoundedValueConstraint(bounds.first, bounds.second) {
        }

        virtual T value() = 0;

        void set_bounds(T left_bound, T right_bound) {
            left_bound_ = left_bound;
            right_bound_ = right_bound;
        }

        void set_bounds(std::pair<T, T> bounds) {
            set_bounds(bounds.first, bounds.second);
        }

        std::pair<T, T> bounds() {
            return std::pair(left_bound_, right_bound_);
        }

        SatisfactionVerdict check() override {
            return lies_in_between<T>(left_bound_, value(), right_bound_);
        }

    protected:
        T left_bound_;
        T right_bound_;
    };

    class OrderConstraint : public BoundedValueConstraint<Graph::OrderType, Constraint::Type::kOrder> {
    public:
        using BoundedValueConstraint::BoundedValueConstraint;
        ConstraintPtr clone() override;
        Graph::OrderType value() override;
    };

    class SizeConstraint : public BoundedValueConstraint<Graph::SizeType, Constraint::Type::kSize> {
    public:
        using BoundedValueConstraint::BoundedValueConstraint;
        ConstraintPtr clone() override;
        Graph::SizeType value() override;
    };

    class ComponentNumberConstraint : public BoundedValueConstraint<Graph::OrderType, Constraint::Type::kComponentNumber> {
    public:
        using BoundedValueConstraint::BoundedValueConstraint;
        ConstraintPtr clone() override;
        Graph::OrderType value() override;
    };

    class ComponentOrderConstraint : public BoundedValueConstraint<Graph::OrderType, Constraint::Type::kComponentOrder> {
    public:
        using BoundedValueConstraint::BoundedValueConstraint;
        ConstraintPtr clone() override;
        Graph::OrderType value() override;
        SatisfactionVerdict check() override;
    };

    class ComponentSizeConstraint : public BoundedValueConstraint<Graph::SizeType, Constraint::Type::kComponentSize> {
    public:
        using BoundedValueConstraint::BoundedValueConstraint;
        ConstraintPtr clone() override;
        Graph::SizeType value() override;
        SatisfactionVerdict check() override;
    };

    class ComponentCutPointConstraint : public BoundedValueConstraint<Graph::OrderType, Constraint::Type::kComponentCutPoint> {
    public:
        using BoundedValueConstraint::BoundedValueConstraint;
        ConstraintPtr clone() override;
        Graph::OrderType value() override;
        SatisfactionVerdict check() override;
    };

    class ComponentBridgeConstraint : public BoundedValueConstraint<Graph::SizeType, Constraint::Type::kComponentBridge> {
    public:
        using BoundedValueConstraint::BoundedValueConstraint;
        ConstraintPtr clone() override;
        Graph::SizeType value() override;
        SatisfactionVerdict check() override;
    };

    class ComponentDiameterConstraint : public BoundedValueConstraint<Graph::SizeType, Constraint::Type::kComponentDiameter> {
        using BoundedValueConstraint::BoundedValueConstraint;
        ConstraintPtr clone() override;
        Graph::SizeType value() override;
        SatisfactionVerdict check() override;
    };
}

#endif //GRAPH_CONSTRAINT_SOLVER_CONSTRAINT_H
