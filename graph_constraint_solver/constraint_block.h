#ifndef GRAPH_CONSTRAINT_SOLVER_CONSTRAINT_BLOCK_H
#define GRAPH_CONSTRAINT_SOLVER_CONSTRAINT_BLOCK_H

#include "constraint.h"

namespace graph_constraint_solver {

    class ConstraintBlock;
    using ConstraintBlockPtr = std::shared_ptr<ConstraintBlock>;

    class ConstraintBlock : public Constraint {
    public:
        enum class ComponentType {
            kConnected,
            kTwoConnected,
            kTwoEdgeConnected,
            kStronglyConnected,
            kTree,
            kRing,
        };

        static ConstraintBlockPtr create(ComponentType component_type);

        ConstraintBlock(ComponentType component_type, const std::set<Constraint::Type> &available_constraint_types,
                        const std::set<std::pair<Constraint::Type, Constraint::Type>> &restricted_constraint_pairs);

        ConstraintBlock(ConstraintBlock &other);
        ConstraintPtr clone() override;

        void bind_graph(GraphPtr graph_ptr) override;
        ConstraintBlockPtr& vertices_block();
        ConstraintBlockPtr& edges_block();

        std::map<Type, ConstraintPtr> constraints();
        bool has_constraint(Type constraint_type);

        void add_constraint(ConstraintPtr constraint_ptr);
        void add_constraints(std::vector<ConstraintPtr> &constraints);
        void remove_constraint(Type constraint_type);

        template <typename ConstraintType>
        std::shared_ptr<ConstraintType> get_constraint() {
            auto contraint_type_id = Constraint::TypeToEnumIdMap<ConstraintType>()();
            if (has_constraint(contraint_type_id)) {
                return std::static_pointer_cast<ConstraintType>(constraints_.at(contraint_type_id));
            }
            throw std::runtime_error("Call to non-existing " + Constraint::type_to_name_.at(contraint_type_id) + "-constraint");
        }

        void add_goal_constraint(Type constraint_type);
        void remove_goal_constraint(Type constraint_type);

//        void add_edge(int, int) override;

        SatisfactionVerdict check() override;

        ComponentType component_type();
        const std::string component_type_name();
        Graph::Type get_graph_type();

//        template <Constraint::Type constraint_type>
//        std::pair<int, int> get_constraint_bounds() {
//            if (!constraints_.count(constraint_type)) {
//                throw std::out_of_range("get_constraint_bounds error: unable to access " + Constraint::type_to_name(constraint_type));
//            }
//            return std::static_pointer_cast<BoundedValueConstraint<int, constraint_type>>(constraints_[constraint_type])->bounds();
//        }

        template <typename T>
        std::shared_ptr<T> clone() {
            auto result = std::make_shared<T>();
            for (auto &p : constraints_) {
                if (result->available_constraint_types_.count(p.first)) {
                    result->add_constraint(p.second->clone());
                }
            }
            return result;
        }

    protected:
        // TODO: maybe we need std::map<Constraint::Type, std::vector<ConstraintPtr>>
        // not sure now whether we'll have multiple constraints of the same type
        std::map<Type, ConstraintPtr> constraints_;
        std::set<Type> important_constraints_;
        SatisfactionVerdict check_goals();

        ComponentType component_type_;
        const std::set<Constraint::Type> &available_constraint_types_;
        const std::set<std::pair<Constraint::Type, Constraint::Type>> &restricted_constraint_pairs_;
        static const std::unordered_map<ComponentType, std::string> component_type_to_name_;

        ConstraintBlockPtr vertices_block_, edges_block_;
    };

    class ConnectedConstraintBlock : public ConstraintBlock {
    public:
        ConnectedConstraintBlock();
    };

    class TwoConnectedConstraintBlock : public ConstraintBlock {
    public:
        TwoConnectedConstraintBlock();
    };

    class TwoEdgeConnectedConstraintBlock : public ConstraintBlock {
    public:
        TwoEdgeConnectedConstraintBlock();
    };

    class TreeConstraintBlock : public ConstraintBlock {
    public:
        static const int kMaximumComponentOrder = static_cast<int>(3e6);

        TreeConstraintBlock();
        TreeConstraintBlock(Graph::Type graph_type,
                Constraint::OrderBounds component_number,
                Constraint::OrderBounds component_order_bounds,
                Constraint::OrderBounds component_diameter_bounds = Constraint::OrderBounds(0, kMaximumComponentOrder),
                Graph::OrderType component_max_vertex_degree = kMaximumComponentOrder);

        Graph::OrderType get_component_maximum_vertex_degree();
        Constraint::OrderBounds get_component_diameter_bounds();
    };

    class StronglyConnectedConstraintBlock : public ConstraintBlock {
    public:
        StronglyConnectedConstraintBlock();
    };
}

#ifdef GRAPH_CONSTRAINT_SOLVER_SINGLE_HEADER
#include "constraint_block.cpp"
#endif

#endif //GRAPH_CONSTRAINT_SOLVER_CONSTRAINT_BLOCK_H
