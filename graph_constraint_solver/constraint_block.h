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

        explicit ConstraintBlock(ComponentType component_type);
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
            return nullptr;
        }

        void add_goal_constraint(Type constraint_type);
        void remove_goal_constraint(Type constraint_type);

        void add_edge(int, int) override;

        SatisfactionVerdict check() override;

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

        static const std::set<Constraint::Type> available_constraint_types_;
    };

    using ConstraintBlockPtr = std::shared_ptr<ConstraintBlock>;

    class ConnectedBlock : public ConstraintBlock {
    public:
        ConnectedBlock();

    private:
        static const std::set<Constraint::Type> available_constraint_types_;
    };
}

#endif //GRAPH_CONSTRAINT_SOLVER_CONSTRAINT_BLOCK_H
