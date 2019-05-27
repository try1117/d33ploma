#include "constraint_block.h"

#include "constraint.h"

namespace graph_constraint_solver {

    // ConstraintBlock

    ConstraintBlock::ConstraintBlock(ComponentType component_type)
            : Constraint(Type::kNone), component_type_(component_type) {

    }

    ConstraintPtr ConstraintBlock::clone() {
        return std::make_shared<ConstraintBlock>(*this);
    }

    ConstraintBlock::ConstraintBlock(ConstraintBlock &other)
            : Constraint(Type::kNone), important_constraints_(other.important_constraints_) {

        for (auto &c : other.constraints_) {
            constraints_[c.first] = c.second->clone();
        }
    }

    bool ConstraintBlock::has_constraint(Type constraint_type) {
        return constraints_.count(constraint_type);
    }

    void ConstraintBlock::add_constraint(ConstraintPtr constraint_ptr) {
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
        return get_constraint<GraphTypeConstraint>(Constraint::Type::kGraphType)->graph_type();
    }

    std::pair<int, int> ConstraintBlock::get_order_bounds() {
        return get_constraint<OrderConstraint>(Constraint::Type::kOrder)->bounds();
    }

    std::pair<int, int> ConstraintBlock::get_size_bounds() {
        return get_constraint<SizeConstraint>(Constraint::Type::kSize)->bounds();
    }

    std::pair<int, int> ConstraintBlock::get_components_number_bounds() {
        return get_constraint<ComponentsNumberConstraint>(Constraint::Type::kComponentsNumber)->bounds();
    }

    const std::set<Constraint::Type> combine_constraint_types(std::initializer_list<Constraint::Type> constraint_types) {
        std::vector<Constraint::Type> important({
            Constraint::Type::kGraphType,
            Constraint::Type::kOrder,
            Constraint::Type::kSize,
            Constraint::Type::kComponentsNumber,
            Constraint::Type::kComponentsOrder,
        });
        important.insert(important.end(), constraint_types);
        return std::set<Constraint::Type>(important.begin(), important.end());
    }

    // ConnectedBlock

    const std::set<Constraint::Type> ConnectedBlock::available_constraint_types_ = combine_constraint_types({
        Constraint::Type::kBridge,
        Constraint::Type::kCutPoint,
    });

    ConnectedBlock::ConnectedBlock()
        : ConstraintBlock(ComponentType::kConnected) {

    }

    // ConnectedBlock

    const std::set<Constraint::Type> TwoConnectedBlock::available_constraint_types_ = combine_constraint_types({
    });

    TwoConnectedBlock::TwoConnectedBlock()
            : ConstraintBlock(ComponentType::kTwoConnected) {

    }
}