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

    // Constraint

    Constraint::Constraint(ConstraintType type)
        : type_(type), graph_ptr_(nullptr) {

    }

    ConstraintType Constraint::type() {
        return type_;
    }

    GraphPtr Constraint::graph_ptr() {
        return graph_ptr_;
    }

    void Constraint::bind_graph(GraphPtr graph_ptr) {
        graph_ptr_ = graph_ptr;
    }

    void Constraint::add_directed_edge(int from, int to) {

    }

    void Constraint::add_undirected_edge(int from, int to) {

    }

    std::pair<int, int> Constraint::recommend_directed_edge() {
        return graph_ptr_->generate_random_directed_edge();
    }

    std::pair<int, int> Constraint::recommend_undirected_edge() {
        return graph_ptr_->generate_random_undirected_edge();
    }

    // OrderConstraint

    OrderConstraint::OrderConstraint(int order)
        : Constraint(kOrder), order_(order) {

    }

    ConstraintPtr OrderConstraint::clone() {
        return clone_constraint<OrderConstraint>(*this);
    }

    int OrderConstraint::order() {
        return order_;
    }

    ConstraintSatisfactionVerdict OrderConstraint::check() {
        return check(graph_ptr_);
    }

    ConstraintSatisfactionVerdict OrderConstraint::check(GraphPtr graph_ptr) {
        // TODO: think about possible gradual change of graph_ptr->order()
        if (graph_ptr->order() == order_) return kOK;
        return kImpossible;
    }

    // SizeConstraint

    SizeConstraint::SizeConstraint(int left_bound, int right_bound)
            : Constraint(kSize), left_bound_(left_bound),
            right_bound_(right_bound == -1 ? left_bound : right_bound) {

    }

    ConstraintPtr SizeConstraint::clone() {
        return clone_constraint<SizeConstraint>(*this);
    }

    ConstraintSatisfactionVerdict SizeConstraint::check() {
        return check(graph_ptr_);
    }

    ConstraintSatisfactionVerdict SizeConstraint::check(GraphPtr graph_ptr) {
        if (graph_ptr->size() > right_bound_) return kImpossible;
        if (graph_ptr->size() < left_bound_) return kPossible;
        return kOK;
    }

    // TreeConstraint

    TreeConstraint::TreeConstraint(int weight)
        : Constraint(kTree), weight_(weight), latest_connected_vertex_(0) {

    }

    ConstraintPtr TreeConstraint::clone() {
        return clone_constraint<TreeConstraint>(*this);
    }

    std::pair<int, int> TreeConstraint::recommend_directed_edge() {
        // TODO: check for latest_connected_vertex_ == n - 1
        return {random.wnext(latest_connected_vertex_ + 1, weight_), latest_connected_vertex_ + 1};
    }

    std::pair<int, int> TreeConstraint::recommend_undirected_edge() {
        return recommend_directed_edge();
    }

    void TreeConstraint::add_directed_edge(int from, int to) {
        // TODO: normal version
        latest_connected_vertex_ = std::max(latest_connected_vertex_, std::max(from, to));
    }

    void TreeConstraint::add_undirected_edge(int from, int to) {
        latest_connected_vertex_ = std::max(latest_connected_vertex_, std::max(from, to));
    }

    ConstraintSatisfactionVerdict TreeConstraint::check() {
        if (latest_connected_vertex_ > graph_ptr_->order() - 1) return kImpossible;
        if (latest_connected_vertex_ < graph_ptr_->order() - 1) return kPossible;
        return kOK;
    }

    ConstraintSatisfactionVerdict TreeConstraint::check(GraphPtr g) {
        // TODO: check whether given graph is a tree or not
        return kImpossible;
    }

//    void BridgeConstraint::add_directed_edge(int from, int to) {
//
//    }

    // BridgeConstraint

    BridgeConstraint::BridgeConstraint(int left_bound, int right_bound)
        : Constraint(kBridge), left_bound_(left_bound), right_bound_(right_bound) {

    }

    ConstraintPtr BridgeConstraint::clone() {
        return clone_constraint<BridgeConstraint>(*this);
    }

    void BridgeConstraint::bind_graph(GraphPtr graph_ptr) {
        Constraint::bind_graph(graph_ptr);
        tin.resize(graph_ptr->order());
        fup.resize(graph_ptr->order());
    }

    ConstraintSatisfactionVerdict BridgeConstraint::check() {
        return check(graph_ptr_);
    }

    ConstraintSatisfactionVerdict BridgeConstraint::check(GraphPtr graph_ptr) {
        // TODO: think about it
        // do not count bridges when graph is not even tree
        if (graph_ptr->size() < graph_ptr->order() - 1) {
            return kOK;
        }
        auto t = count_bridges(graph_ptr);
        if (t.first < left_bound_) return kImpossible;
        if (t.first > right_bound_) return kPossible;
        return kOK;
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
        : Constraint(kNone) {

    }

    // do nothing
    ConstraintPtr ConstraintList::clone() {
        return clone_constraint<ConstraintList>(*this);
    }

    ConstraintList::ConstraintList(ConstraintList &other)
        : Constraint(kNone), important_constraints_(other.important_constraints_) {

        for (auto &c : other.constraints_) {
            constraints_[c.first] = c.second->clone();
        }
    }

    bool ConstraintList::has_constraint(ConstraintType constraint_type) {
        return constraints_.count(constraint_type);
    }

    void ConstraintList::add_constraint(ConstraintPtr constraint_ptr) {
        constraints_[constraint_ptr->type()] = constraint_ptr;
    }

    void ConstraintList::remove_constraint(ConstraintType constraint_type) {
//        if (has_constraint(constraint_type)) {
        constraints_.erase(constraint_type);
//        }
    }

    void ConstraintList::add_goal_constraint(ConstraintType constraint_type) {
        important_constraints_.emplace(constraint_type);
    }

    void ConstraintList::remove_goal_constraint(ConstraintType constraint_type) {
        important_constraints_.erase(constraint_type);
    }

    std::map<ConstraintType, ConstraintPtr> ConstraintList::constraints() {
        return constraints_;
    }

    void ConstraintList::bind_graph(GraphPtr graph_ptr) {
        for (auto &c : constraints_) {
            c.second->bind_graph(graph_ptr);
        }
    }

    void ConstraintList::add_directed_edge(int from, int to) {
        for (auto &c : constraints_) {
            c.second->add_directed_edge(from, to);
        }
    }

    void ConstraintList::add_undirected_edge(int from, int to) {
        for (auto &c : constraints_) {
            c.second->add_undirected_edge(from, to);
        }
    }

    ConstraintSatisfactionVerdict ConstraintList::check() {
        ConstraintSatisfactionVerdict res = kOK;
        for (auto &c : constraints_) {
            res = std::min(res, c.second->check());
        }
        auto res_goals = check_goals();
        if (res_goals == kOK && res >= kPossible) {
            return kOK;
        }
        return res;
    }

    ConstraintSatisfactionVerdict ConstraintList::check(GraphPtr graph_ptr) {
        ConstraintSatisfactionVerdict res = kOK;
        for (auto &c : constraints_) {
            res = std::min(res, c.second->check(graph_ptr));
        }
        auto res_goals = check_goals(graph_ptr);
        if (res_goals == kOK && res >= kPossible) {
            return kOK;
        }
        return res;
    }

    ConstraintSatisfactionVerdict ConstraintList::check_goals() {
        if (!important_constraints_.empty()) {
            ConstraintSatisfactionVerdict res = kOK;
            for (auto imporant : important_constraints_) {
                res = std::min(res, constraints_[imporant]->check());
            }
            return res;
        }
        return kPossible;
    }

    ConstraintSatisfactionVerdict ConstraintList::check_goals(GraphPtr graph_ptr) {
        // TODO: think about it
        // may be change arguments in check functions (default = nullptr???)
        // alert copy-paste
        if (!important_constraints_.empty()) {
            ConstraintSatisfactionVerdict res = kOK;
            for (auto imporant : important_constraints_) {
                res = std::min(res, constraints_[imporant]->check(graph_ptr));
            }
            return res;
        }
        return kPossible;
    }
}
