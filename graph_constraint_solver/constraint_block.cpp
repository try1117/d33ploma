#include "constraint_block.h"

#include "constraint.h"

namespace graph_constraint_solver {

    // ConstraintBlock

    const std::unordered_map<ConstraintBlock::ComponentType, std::string> ConstraintBlock::component_type_to_name_({
        {ComponentType::kConnected, "Connected"},
        {ComponentType::kTwoConnected, "TwoConnected"},
        {ComponentType::kTwoEdgeConnected, "TwoEdgeConnected"},
        {ComponentType::kTree, "Tree"},
        {ComponentType::kRing, "Ring"},
    });

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

    void ConstraintBlock::add_edge(int from, int to) {
        for (auto &c : constraints_) {
            c.second->add_edge(from, to);
        }
    }

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
            for (auto imporant : important_constraints_) {
                res = std::min(res, constraints_[imporant]->check());
            }
            return res;
        }
        return SatisfactionVerdict::kPossible;
    }

    Graph::Type ConstraintBlock::get_graph_type() {
        return get_constraint<GraphTypeConstraint>(Constraint::Type::kGraphType)->value();
    }

    std::pair<int, int> ConstraintBlock::get_order_bounds() {
        return get_constraint<OrderConstraint>(Constraint::Type::kOrder)->bounds();
    }

    std::pair<int, int> ConstraintBlock::get_size_bounds() {
        return get_constraint<SizeConstraint>(Constraint::Type::kSize)->bounds();
    }

    std::pair<int, int> ConstraintBlock::get_components_number_bounds() {
        return get_constraint<ComponentNumberConstraint>(Constraint::Type::kComponentNumber)->bounds();
    }

    const std::set<Constraint::Type> combine_constraint_types(std::initializer_list<Constraint::Type> constraint_types) {
        std::vector<Constraint::Type> default_types({
            Constraint::Type::kGraphType,
            Constraint::Type::kOrder,
            Constraint::Type::kSize,
            Constraint::Type::kComponentNumber,
            Constraint::Type::kComponentOrder,
        });
        default_types.insert(default_types.end(), constraint_types);
        return std::set<Constraint::Type>(default_types.begin(), default_types.end());
    }

    // ConnectedBlock

    const std::set<Constraint::Type> connected_block_types_ = combine_constraint_types({
        Constraint::Type::kBridge,
        Constraint::Type::kCutPoint,
    });

    const std::set<std::pair<Constraint::Type, Constraint::Type>> connected_block_restrictions_;

    ConnectedBlock::ConnectedBlock()
        : ConstraintBlock(ComponentType::kConnected, connected_block_types_, connected_block_restrictions_) {

    }

    // TwoConnectedBlock

    const std::set<Constraint::Type> two_connected_block_types({
        Constraint::Type::kGraphType,
        Constraint::Type::kOrder,
        Constraint::Type::kSize,
        Constraint::Type::kComponentNumber,
        Constraint::Type::kComponentOrder,
    });
    const std::set<std::pair<Constraint::Type, Constraint::Type>> two_connected_block_restrictions_;

    TwoConnectedBlock::TwoConnectedBlock()
            : ConstraintBlock(ComponentType::kTwoConnected, two_connected_block_types, two_connected_block_restrictions_) {

    }

    // TwoEdgeConnectedBlock

    const std::set<Constraint::Type> two_edge_connected_block_types = combine_constraint_types({
        Constraint::Type::kGraphType,
        Constraint::Type::kComponentNumber,
        Constraint::Type::kComponentOrder,
        Constraint::Type::kComponentSize,
        Constraint::Type::kComponentCutPoint,
    });
    const std::set<std::pair<Constraint::Type, Constraint::Type>> two_edge_connected_block_restrictions_;

    TwoEdgeConnectedBlock::TwoEdgeConnectedBlock()
        : ConstraintBlock(ComponentType::kTwoEdgeConnected, two_edge_connected_block_types, two_edge_connected_block_restrictions_) {

    }

    // TreeBlock

    const std::set<Constraint::Type> tree_block_types({
        Constraint::Type::kGraphType,
        Constraint::Type::kOrder,
//        Constraint::Type::kComponentNumber,
//        Constraint::Type::kComponentOrder,
        Constraint::Type::kDiameter,
        Constraint::Type::kTreeBroadness,
        Constraint::Type::kVertexMaxDegree,
    });

    const std::set<std::pair<Constraint::Type, Constraint::Type>> tree_block_restrictions_({
        {Constraint::Type::kTreeBroadness, Constraint::Type::kDiameter},
    });

    TreeBlock::TreeBlock()
            : ConstraintBlock(ComponentType::kTree, tree_block_types, tree_block_restrictions_) {

    }

    int TreeBlock::get_maximum_vertex_degree() {
        return get_constraint<VertexMaxDegreeConstraint>(Type::kVertexMaxDegree)->value();
    }

    std::pair<int, int> TreeBlock::get_diameter_bounds() {
        return get_constraint<DiameterConstraint>(Type::kDiameter)->bounds();
    }
}