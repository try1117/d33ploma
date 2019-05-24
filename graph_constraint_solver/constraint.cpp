#include "constraint.h"

#include <sys/resource.h>

#include "utils.h"

namespace graph_constraint_solver {

    // Constraint

    Constraint::SatisfactionVerdict Constraint::lies_in_between(int left_bound, int value, int right_bound, bool increasing) {
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
        return std::make_shared<GraphTypeConstraint>(*this);
    }

    Graph::Type GraphTypeConstraint::graph_type() {
        return graph_type_;
    }

    Constraint::SatisfactionVerdict GraphTypeConstraint::check() {
        if (graph_ptr_->type() == graph_type_) return SatisfactionVerdict::kOK;
        return SatisfactionVerdict::kImpossible;
    }

    // OrderConstraint

    ConstraintPtr OrderConstraint::clone() {
        return std::make_shared<OrderConstraint>(*this);
    }

    int OrderConstraint::value() {
        return graph_ptr_->order();
    }

    // SizeConstraint

    ConstraintPtr SizeConstraint::clone() {
        return std::make_shared<SizeConstraint>(*this);
    }

    int SizeConstraint::value() {
        return graph_ptr_->size();
    }

    // ComponentsNumberConstraint

    ConstraintPtr ComponentsNumberConstraint::clone() {
        return std::make_shared<ComponentsNumberConstraint>(*this);
    }

    int ComponentsNumberConstraint::value() {
        // TODO: count components_number
        int components_number = 0;
        return components_number;
    }

    // TreeConstraint

    TreeConstraint::TreeConstraint(int weight)
        : Constraint(Type::kTree), weight_(weight), latest_connected_vertex_(0) {

    }

    ConstraintPtr TreeConstraint::clone() {
        return std::make_shared<TreeConstraint>(*this);
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
        return std::make_shared<BridgeConstraint>(*this);
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

    // from stackoverflow
    // https://stackoverflow.com/questions/2275550/change-stack-size-for-a-c-application-in-linux-during-compilation-with-gnu-com/2284691#2284691
    bool increase_stack_size() {
        const rlim_t kStackSize = 64 * 1024 * 1024;
        struct rlimit rl;
        int result;

        result = getrlimit(RLIMIT_STACK, &rl);
        if (result == 0)
        {
            if (rl.rlim_cur < kStackSize)
            {
                rl.rlim_cur = kStackSize;
                result = setrlimit(RLIMIT_STACK, &rl);
                if (result != 0) return false;
            }
        }
        return true;
    }

    std::pair<int, int> BridgeConstraint::count_bridges(GraphPtr graph_ptr, bool save_bridges) {
        timer = 0;
        fill(tin.begin(), tin.end(), 0);
        fill(fup.begin(), fup.end(), 0);
        std::pair<int, int> res;
        increase_stack_size();
        _count_bridges(graph_ptr, 0, -1, res, save_bridges);
        return res;
    }

    std::set<std::pair<int, int>> BridgeConstraint::get_bridges_list() {
        return bridges_list_;
    }
}
