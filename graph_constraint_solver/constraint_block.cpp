#include "constraint_block.h"

#include "constraint.h"

namespace graph_constraint_solver {

    // ConstraintBlock

    const std::unordered_map<ConstraintBlock::ComponentType, std::string> ConstraintBlock::component_type_to_name_({
        {ComponentType::kConnected, "Connected"},
        {ComponentType::kTwoConnected, "Two-connected"},
        {ComponentType::kTwoEdgeConnected, "Two-edge-connected"},
        {ComponentType::kTree, "Tree"},
        {ComponentType::kRing, "Ring"},
        {ComponentType::kStronglyConnected, "Strongly-connected"}
    });

    ConstraintBlockPtr ConstraintBlock::create(ComponentType component_type) {
        switch (component_type) {
            case ComponentType::kConnected: return std::make_shared<ConnectedConstraintBlock>();
            case ComponentType::kTwoConnected: return std::make_shared<TwoConnectedConstraintBlock>();
            case ComponentType::kTwoEdgeConnected: return std::make_shared<TwoEdgeConnectedConstraintBlock>();
            case ComponentType::kTree: return std::make_shared<TreeConstraintBlock>();
            case ComponentType::kStronglyConnected: return std::make_shared<StronglyConnectedConstraintBlock>();
        }
    }

    ConstraintBlock::ConstraintBlock(ComponentType component_type, const std::set<Constraint::Type> &available_constraint_types,
                                     const std::set<std::pair<Constraint::Type, Constraint::Type>> &restricted_constraint_pairs)
            : Constraint(Type::kNone), component_type_(component_type),
            available_constraint_types_(available_constraint_types),
            restricted_constraint_pairs_(restricted_constraint_pairs) {

    }

    ConstraintPtr ConstraintBlock::clone() {
        return std::make_shared<ConstraintBlock>(*this);
    }

    ConstraintBlock::ConstraintBlock(ConstraintBlock &other)
            : Constraint(Type::kNone), important_constraints_(other.important_constraints_),
            available_constraint_types_(other.available_constraint_types_),
            restricted_constraint_pairs_(other.restricted_constraint_pairs_) {

        for (auto &c : other.constraints_) {
            constraints_[c.first] = c.second->clone();
        }
    }

    bool ConstraintBlock::has_constraint(Type constraint_type) {
        return constraints_.count(constraint_type);
    }

    void ConstraintBlock::add_constraint(ConstraintPtr constraint_ptr) {
        if (!constraint_ptr) {
            throw std::invalid_argument("add_constraint error: invalid pointer");
        }
        if (!available_constraint_types_.count(constraint_ptr->type())) {
            throw std::invalid_argument(component_type_name() + "-component does not allow " + constraint_ptr->type_name() + "-constraint");
        }
        for (auto &constraint : constraints_) {
            if (restricted_constraint_pairs_.count({constraint.second->type(), constraint_ptr->type()}) ||
                    restricted_constraint_pairs_.count({constraint_ptr->type(), constraint.second->type()})) {
                throw std::invalid_argument(component_type_name() + "-component does not allow both constraints '" +
                constraint.second->type_name() + "' and '" + constraint_ptr->type_name() + "' at the same time");
            }
        }
        constraints_[constraint_ptr->type()] = constraint_ptr;
    }

    void ConstraintBlock::add_constraints(std::vector<ConstraintPtr> &constraints) {
        for (auto &constraint : constraints) {
            add_constraint(constraint);
        }
    }

    void ConstraintBlock::remove_constraint(Type constraint_type) {
//        if (has_constraint(constraint_type)) {
        constraints_.erase(constraint_type);
//        }
    }

    void ConstraintBlock::add_goal_constraint(Type constraint_type) {
        important_constraints_.emplace(constraint_type);
    }

    void ConstraintBlock::remove_goal_constraint(Type constraint_type) {
        important_constraints_.erase(constraint_type);
    }

    std::map<Constraint::Type, ConstraintPtr> ConstraintBlock::constraints() {
        return constraints_;
    }

    void ConstraintBlock::bind_graph(GraphPtr graph_ptr) {
        for (auto &c : constraints_) {
            c.second->bind_graph(graph_ptr);
        }
    }

    ConstraintBlockPtr& ConstraintBlock::vertices_block() {
        return vertices_block_;
    }

    ConstraintBlockPtr& ConstraintBlock::edges_block() {
        return edges_block_;
    }

//    void ConstraintBlock::add_edge(int from, int to) {
//        for (auto &c : constraints_) {
//            c.second->add_edge(from, to);
//        }
//    }

    Constraint::SatisfactionVerdict ConstraintBlock::check() {
        auto res = SatisfactionVerdict::kOK;
        for (auto &c : constraints_) {
            res = std::min(res, c.second->check());
        }
        auto res_goals = check_goals();
        if (res_goals == SatisfactionVerdict::kOK && res >= SatisfactionVerdict::kPossible) {
            return SatisfactionVerdict::kOK;
        }
        return res;
    }

    ConstraintBlock::ComponentType ConstraintBlock::component_type() {
        return component_type_;
    }

    const std::string ConstraintBlock::component_type_name() {
        return component_type_to_name_.at(component_type_);
    }

    Constraint::SatisfactionVerdict ConstraintBlock::check_goals() {
        if (!important_constraints_.empty()) {
            auto res = SatisfactionVerdict::kOK;
            for (auto important : important_constraints_) {
                res = std::min(res, constraints_[important]->check());
            }
            return res;
        }
        return SatisfactionVerdict::kPossible;
    }

    Graph::Type ConstraintBlock::get_graph_type() {
        return get_constraint<GraphTypeConstraint>()->value();
    }

    // ConnectedConstraintBlock

    const std::set<Constraint::Type> connected_block_types_({
        Constraint::Type::kGraphType,
        Constraint::Type::kComponentNumber,
        Constraint::Type::kComponentOrder,
        Constraint::Type::kComponentSize,
        Constraint::Type::kComponentCutPoint,
        Constraint::Type::kComponentBridge,
    });
    const std::set<std::pair<Constraint::Type, Constraint::Type>> connected_block_restrictions_;

    ConnectedConstraintBlock::ConnectedConstraintBlock()
        : ConstraintBlock(ComponentType::kConnected, connected_block_types_, connected_block_restrictions_) {

    }

    // TwoConnectedConstraintBlock

    const std::set<Constraint::Type> two_connected_block_types({
        Constraint::Type::kGraphType,
//        Constraint::Type::kOrder,
        Constraint::Type::kComponentNumber,
        Constraint::Type::kComponentOrder,
        Constraint::Type::kComponentSize,
    });
    const std::set<std::pair<Constraint::Type, Constraint::Type>> two_connected_block_restrictions_;

    TwoConnectedConstraintBlock::TwoConnectedConstraintBlock()
            : ConstraintBlock(ComponentType::kTwoConnected, two_connected_block_types, two_connected_block_restrictions_) {

    }

    // TwoEdgeConnectedConstraintBlock

    const std::set<Constraint::Type> two_edge_connected_block_types({
        Constraint::Type::kGraphType,
        Constraint::Type::kComponentNumber,
        Constraint::Type::kComponentOrder,
        Constraint::Type::kComponentSize,
        Constraint::Type::kComponentCutPoint,
    });
    const std::set<std::pair<Constraint::Type, Constraint::Type>> two_edge_connected_block_restrictions_;

    TwoEdgeConnectedConstraintBlock::TwoEdgeConnectedConstraintBlock()
        : ConstraintBlock(ComponentType::kTwoEdgeConnected, two_edge_connected_block_types, two_edge_connected_block_restrictions_) {

    }

    // TreeConstraintBlock

    const std::set<Constraint::Type> tree_block_types({
        Constraint::Type::kGraphType,
//        Constraint::Type::kOrder,
        Constraint::Type::kComponentNumber,
        Constraint::Type::kComponentOrder,
        Constraint::Type::kComponentDiameter,
//        Constraint::Type::kTreeBroadness,
        Constraint::Type::kComponentVertexMaxDegree,
    });

    const std::set<std::pair<Constraint::Type, Constraint::Type>> tree_block_restrictions({
//        {Constraint::Type::kTreeBroadness, Constraint::Type::kDiameter},
    });

    const int TreeConstraintBlock::kMaximumComponentOrder;

    TreeConstraintBlock::TreeConstraintBlock()
        : ConstraintBlock(ComponentType::kTree, tree_block_types, tree_block_restrictions) {

    }

    TreeConstraintBlock::TreeConstraintBlock(Graph::Type graph_type,
            Constraint::OrderBounds component_number,
            Constraint::OrderBounds component_order_bounds,
            Constraint::OrderBounds component_diameter_bounds,
            Graph::OrderType component_max_vertex_degree)
        : TreeConstraintBlock() {

        add_constraint(std::make_shared<GraphTypeConstraint>(graph_type));
        add_constraint(std::make_shared<ComponentNumberConstraint>(component_number));
        add_constraint(std::make_shared<ComponentOrderConstraint>(component_order_bounds));
        add_constraint(std::make_shared<ComponentDiameterConstraint>(component_diameter_bounds));
        add_constraint(std::make_shared<ComponentVertexMaxDegreeConstraint>(component_max_vertex_degree));
    }

    // TODO: default values for this functions
    Graph::OrderType TreeConstraintBlock::get_component_maximum_vertex_degree() {
        return get_constraint<ComponentVertexMaxDegreeConstraint>()->value();
    }

    Constraint::OrderBounds TreeConstraintBlock::get_component_diameter_bounds() {
        return get_constraint<ComponentDiameterConstraint>()->bounds();
    }

    // StronglyConnectedConstraintBlock

    const std::set<Constraint::Type> strongly_connected_block_types({
        Constraint::Type::kGraphType,
        Constraint::Type::kComponentOrder,
        Constraint::Type::kComponentSize,
        Constraint::Type::kComponentNumber,
    });

    const std::set<std::pair<Constraint::Type, Constraint::Type>> strongly_connected_block_restrictions({
//        {Constraint::Type::kTreeBroadness, Constraint::Type::kDiameter},
    });

    StronglyConnectedConstraintBlock::StronglyConnectedConstraintBlock()
        : ConstraintBlock(ComponentType::kStronglyConnected, strongly_connected_block_types, strongly_connected_block_restrictions) {

    }
}