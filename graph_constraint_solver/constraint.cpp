#include "constraint.h"

#include "utils.h"

namespace graph_constraint_solver {

    template<typename T>
    std::shared_ptr<T> clone_constraint(T &other) {
//        auto other_t = std::static_pointer_cast<T>(other);
        auto result = std::make_shared<T>(other);
//        result->bind_graph(std::make_shared<Graph>(*other.graph_ptr()));
        return result;
    }

    Constraint::SatisfactionVerdict lies_in_between(int left_bound, int value, int right_bound, bool increasing = true) {
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

    // Constraint

    Constraint::Constraint(Type type)
        : type_(type), graph_ptr_(nullptr) {

    }

    Constraint::Type Constraint::type() {
        return type_;
    }

    GraphPtr Constraint::graph_ptr() {
        return graph_ptr_;
    }

    void Constraint::bind_graph(GraphPtr graph_ptr) {
        graph_ptr_ = graph_ptr;
    }

    void Constraint::add_edge(int from, int to) {

    }

    std::pair<int, int> Constraint::recommend_edge() {
        return graph_ptr_->generate_random_edge();
    }

    // GraphTypeConstraint

    GraphTypeConstraint::GraphTypeConstraint(Graph::Type graph_type)
        : Constraint(Type::kGraphType), graph_type_(graph_type) {

    }

    ConstraintPtr GraphTypeConstraint::clone() {
        return clone_constraint<GraphTypeConstraint>(*this);
    }

    Graph::Type GraphTypeConstraint::graph_type() {
        return graph_type_;
    }

    Constraint::SatisfactionVerdict GraphTypeConstraint::check() {
        if (graph_ptr_->type() == graph_type_) return SatisfactionVerdict::kOK;
        return SatisfactionVerdict::kImpossible;
    }

    // OrderConstraint

    OrderConstraint::OrderConstraint(int order)
        : Constraint(Type::kOrder), order_(order) {

    }

    ConstraintPtr OrderConstraint::clone() {
        return clone_constraint<OrderConstraint>(*this);
    }

    int OrderConstraint::order() {
        return order_;
    }

    Constraint::SatisfactionVerdict OrderConstraint::check() {
        // TODO: think about possible gradual change of graph_ptr->order()
        return lies_in_between(order_, graph_ptr_->order(), order_);
    }

    // SizeConstraint

    SizeConstraint::SizeConstraint(int left_bound, int right_bound)
            : Constraint(Type::kSize), left_bound_(left_bound),
            right_bound_(right_bound == -1 ? left_bound : right_bound) {

    }

    ConstraintPtr SizeConstraint::clone() {
        return clone_constraint<SizeConstraint>(*this);
    }

    Constraint::SatisfactionVerdict SizeConstraint::check() {
        return lies_in_between(left_bound_, graph_ptr_->size(), right_bound_);
    }

    // ComponentsNumberConstraint

    ComponentsNumberConstraint::ComponentsNumberConstraint(int left_bound, int right_bound)
        : Constraint(Type::kComponentsNumber), left_bound_(left_bound),
        right_bound_(right_bound == -1 ? left_bound : right_bound) {

    }

    ConstraintPtr ComponentsNumberConstraint::clone() {
        return clone_constraint<ComponentsNumberConstraint>(*this);
    }

    Constraint::SatisfactionVerdict ComponentsNumberConstraint::check() {
        // TODO: count components_number
        int components_number = 0;
        return lies_in_between(left_bound_, components_number, right_bound_);
    }

    std::pair<int, int> ComponentsNumberConstraint::bounds() {
        return {left_bound_, right_bound_};
    }

    // TreeConstraint

    TreeConstraint::TreeConstraint(int weight)
        : Constraint(Type::kTree), weight_(weight), latest_connected_vertex_(0) {

    }

    ConstraintPtr TreeConstraint::clone() {
        return clone_constraint<TreeConstraint>(*this);
    }

    std::pair<int, int> TreeConstraint::recommend_edge() {
        // TODO: check for latest_connected_vertex_ == n - 1
        return {random.wnext(latest_connected_vertex_ + 1, weight_), latest_connected_vertex_ + 1};
    }

    void TreeConstraint::add_edge(int from, int to) {
        latest_connected_vertex_ = std::max(latest_connected_vertex_, std::max(from, to));
    }

    Constraint::SatisfactionVerdict TreeConstraint::check() {
        auto t = graph_ptr_->order() - 1;
        return lies_in_between(t, latest_connected_vertex_, t);
    }

//    void BridgeConstraint::add_directed_edge(int from, int to) {
//
//    }

    // BridgeConstraint

    BridgeConstraint::BridgeConstraint(int left_bound, int right_bound)
        : Constraint(Type::kBridge), left_bound_(left_bound), right_bound_(right_bound) {

    }

    ConstraintPtr BridgeConstraint::clone() {
        return clone_constraint<BridgeConstraint>(*this);
    }

    void BridgeConstraint::bind_graph(GraphPtr graph_ptr) {
        Constraint::bind_graph(graph_ptr);
        tin.resize(graph_ptr->order());
        fup.resize(graph_ptr->order());
    }

    Constraint::SatisfactionVerdict BridgeConstraint::check() {
        // TODO: think about it
        // do not count bridges when graph is not even tree
        if (graph_ptr_->size() < graph_ptr_->order() - 1) {
            return SatisfactionVerdict::kOK;
        }
        auto t = count_bridges(graph_ptr_);
        return lies_in_between(left_bound_, t.first, right_bound_, false);
    }

    int BridgeConstraint::_count_bridges(GraphPtr graph_ptr, int v, int pr, std::pair<int, int> &res, bool save_bridges) {
        tin[v] = fup[v] = ++timer;
        for (auto child : graph_ptr->adjacency_list()[v]) {
            if (child != pr) {
                if (!tin[child]) {
                    _count_bridges(graph_ptr, child, v, res, save_bridges);
                    if (fup[child] > tin[v]) {
                        ++res.first;
                        if (!graph_ptr->is_leaf(v) && !graph_ptr->is_leaf(child)) {
                            ++res.second;
                        }
                        if (save_bridges) {
                            bridges_list_.emplace(std::min(v, child), std::max(v, child));
                        }
                    }
                    fup[v] = std::min(fup[v], fup[child]);
                }
                else {
                    fup[v] = std::min(fup[v], tin[child]);
                }
            }
        }
    }

    std::pair<int, int> BridgeConstraint::count_bridges(GraphPtr graph_ptr, bool save_bridges) {
        timer = 0;
        fill(tin.begin(), tin.end(), 0);
        fill(fup.begin(), fup.end(), 0);
        std::pair<int, int> res;
        _count_bridges(graph_ptr, 0, -1, res, save_bridges);
        return res;
    }

    std::set<std::pair<int, int>> BridgeConstraint::get_bridges_list() {
        return bridges_list_;
    }

    // ConstraintList

    ConstraintList::ConstraintList()
        : Constraint(Type::kNone) {

    }

    ConstraintPtr ConstraintList::clone() {
        return clone_constraint<ConstraintList>(*this);
    }

    ConstraintList::ConstraintList(ConstraintList &other)
        : Constraint(Type::kNone), important_constraints_(other.important_constraints_) {

        for (auto &c : other.constraints_) {
            constraints_[c.first] = c.second->clone();
        }
    }

    bool ConstraintList::has_constraint(Type constraint_type) {
        return constraints_.count(constraint_type);
    }

    void ConstraintList::add_constraint(ConstraintPtr constraint_ptr) {
        constraints_[constraint_ptr->type()] = constraint_ptr;
    }

    void ConstraintList::remove_constraint(Type constraint_type) {
//        if (has_constraint(constraint_type)) {
        constraints_.erase(constraint_type);
//        }
    }

    void ConstraintList::add_goal_constraint(Type constraint_type) {
        important_constraints_.emplace(constraint_type);
    }

    void ConstraintList::remove_goal_constraint(Type constraint_type) {
        important_constraints_.erase(constraint_type);
    }

    std::map<Constraint::Type, ConstraintPtr> ConstraintList::constraints() {
        return constraints_;
    }

    void ConstraintList::bind_graph(GraphPtr graph_ptr) {
        for (auto &c : constraints_) {
            c.second->bind_graph(graph_ptr);
        }
    }

    void ConstraintList::add_edge(int from, int to) {
        for (auto &c : constraints_) {
            c.second->add_edge(from, to);
        }
    }

    Constraint::SatisfactionVerdict ConstraintList::check() {
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

    Constraint::SatisfactionVerdict ConstraintList::check_goals() {
        if (!important_constraints_.empty()) {
            auto res = SatisfactionVerdict::kOK;
            for (auto imporant : important_constraints_) {
                res = std::min(res, constraints_[imporant]->check());
            }
            return res;
        }
        return SatisfactionVerdict::kPossible;
    }
}
