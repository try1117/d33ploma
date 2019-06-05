#ifndef GRAPH_CONSTRAINT_SOLVER_CONSTRAINT_BLOCK_H
#define GRAPH_CONSTRAINT_SOLVER_CONSTRAINT_BLOCK_H

#include "constraint.h"

namespace graph_constraint_solver {

    class ConstraintBlock : public Constraint {
    public:
        enum class ComponentType {
            kConnected,
            kTwoConnected,
            kTwoEdgeConnected,
            kTree,
            kRing,
        };

        ConstraintBlock(ComponentType component_type, const std::set<Constraint::Type> &available_constraint_types,
                        const std::set<std::pair<Constraint::Type, Constraint::Type>> &restricted_constraint_pairs);

        ConstraintBlock(ConstraintBlock &other);
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
            throw std::runtime_error("Call to non-existing " + Constraint::type_to_name_.at(constraint_type) + "-constraint");
        }

        void add_goal_constraint(Type constraint_type);
        void remove_goal_constraint(Type constraint_type);

        void add_edge(int, int) override;

        SatisfactionVerdict check() override;

        ComponentType component_type();
        const std::string component_type_name();
        Graph::Type get_graph_type();

        template <Constraint::Type constraint_type>
        std::pair<int, int> get_constraint_bounds() {
            if (!constraints_.count(constraint_type)) {
                throw std::out_of_range("get_constraint_bounds error: unable to access " + Constraint::type_to_name(constraint_type));
            }
            return std::static_pointer_cast<BoundedValueConstraint<int, constraint_type>>(constraints_[constraint_type])->bounds();
        }

        // TODO: parse
        void parse_JSON(std::string filepath) {}
        void parse_XML(std::string filepath) {}

    protected:
        // TODO: maybe we need std::map<Constraint::Type, std::vector<ConstraintPtr>>
        // not sure now whether we'll have multiple constraints of the same type
        std::map<Type, ConstraintPtr> constraints_;
        std::set<Type> important_constraints_;

        ComponentType component_type_;

        SatisfactionVerdict check_goals();

        const std::set<Constraint::Type> &available_constraint_types_;
        const std::set<std::pair<Constraint::Type, Constraint::Type>> &restricted_constraint_pairs_;
        static const std::unordered_map<ComponentType, std::string> component_type_to_name_;
    };

    using ConstraintBlockPtr = std::shared_ptr<ConstraintBlock>;

    class ConnectedBlock : public ConstraintBlock {
    public:
        ConnectedBlock();
    };

    class TwoConnectedBlock : public ConstraintBlock {
    public:
        TwoConnectedBlock();
    };

    class TwoEdgeConnectedBlock : public ConstraintBlock {
    public:
        TwoEdgeConnectedBlock();
    };

    class TreeBlock : public ConstraintBlock {
    public:
        static const int kMaximumComponentOrder = static_cast<int>(3e6);

        TreeBlock();
        TreeBlock(Graph::Type graph_type, std::pair<int, int> component_number, std::pair<int, int> component_order_bounds,
                std::pair<int, int> component_diameter_bounds = std::pair<int, int>(0, kMaximumComponentOrder),
                        int component_max_vertex_degree = kMaximumComponentOrder);

        int get_component_maximum_vertex_degree();
        std::pair<int, int> get_component_diameter_bounds();
    };


}

#endif //GRAPH_CONSTRAINT_SOLVER_CONSTRAINT_BLOCK_H
