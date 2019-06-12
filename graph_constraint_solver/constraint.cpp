#include "constraint.h"

#include "utils.h"

namespace graph_constraint_solver {

    // Constraint

    const std::unordered_map<Constraint::Type, std::string> Constraint::type_to_name_({
        {Type::kNone, "None"},
        {Type::kGraphType, "Graph-type"},
        {Type::kOrder, "Order"},
        {Type::kSize, "Size"},
        {Type::kComponentNumber, "Number-of-components"},
        {Type::kComponentOrder, "Components-order"},
        {Type::kComponentDiameter, "Components-diameter"},
//        {Type::kTreeBroadness, "Tree-broadness"},
        {Type::kComponentVertexMaxDegree, "Components-vertex-maximum-degree"},
        {Type::kBridge, "Bridge"},
        {Type::kCutPoint, "CutPoint"},
    });

//    template<typename T>
//    const Constraint::Type Constraint::TypeToEnumIdMap<T>::type_id = Type::kNone;

    // TODO: register?
    template<>
    const Constraint::Type Constraint::TypeToEnumIdMap<GraphTypeConstraint>::type_id = Type::kGraphType;
    template<>
    const Constraint::Type Constraint::TypeToEnumIdMap<OrderConstraint>::type_id = Type::kOrder;
    template<>
    const Constraint::Type Constraint::TypeToEnumIdMap<SizeConstraint>::type_id = Type::kSize;
    template<>
    const Constraint::Type Constraint::TypeToEnumIdMap<ComponentNumberConstraint>::type_id = Type::kComponentNumber;
    template<>
    const Constraint::Type Constraint::TypeToEnumIdMap<ComponentOrderConstraint>::type_id = Type::kComponentOrder;
    template<>
    const Constraint::Type Constraint::TypeToEnumIdMap<ComponentSizeConstraint>::type_id = Type::kComponentSize;
    template<>
    const Constraint::Type Constraint::TypeToEnumIdMap<ComponentDiameterConstraint>::type_id = Type::kComponentDiameter;
    template<>
    const Constraint::Type Constraint::TypeToEnumIdMap<ComponentVertexMaxDegreeConstraint>::type_id = Type::kComponentVertexMaxDegree;
    template<>
    const Constraint::Type Constraint::TypeToEnumIdMap<ComponentCutPointConstraint>::type_id = Type::kComponentCutPoint;
    template<>
    const Constraint::Type Constraint::TypeToEnumIdMap<ComponentBridgeConstraint>::type_id = Type::kComponentBridge;

    Constraint::Constraint(Type type)
        : type_(type), graph_ptr_(nullptr) {

    }

    Constraint::Type Constraint::type() {
        return type_;
    }

    const std::string Constraint::type_name() {
        return Constraint::type_to_name_.at(type_);
    }

    const std::string Constraint::type_to_name(Type type) {
        return Constraint::type_to_name_.at(type);
    }

    GraphPtr Constraint::graph_ptr() {
        return graph_ptr_;
    }

    void Constraint::bind_graph(GraphPtr graph_ptr) {
        graph_ptr_ = graph_ptr;
    }

//    void Constraint::add_edge(int from, int to) {
//
//    }
//
//    std::pair<int, int> Constraint::recommend_edge() {
//        return graph_ptr_->generate_random_edge();
//    }

    // GraphTypeConstraint

    ConstraintPtr GraphTypeConstraint::clone() {
        return std::make_shared<GraphTypeConstraint>(*this);
    }

    Constraint::SatisfactionVerdict GraphTypeConstraint::check() {
        if (graph_ptr_->type() == value_) return SatisfactionVerdict::kOK;
        return SatisfactionVerdict::kImpossible;
    }

    // ComponentVertexMaxDegreeConstraint

    ConstraintPtr ComponentVertexMaxDegreeConstraint::clone() {
        return std::make_shared<ComponentVertexMaxDegreeConstraint>(*this);
    }

    Constraint::SatisfactionVerdict ComponentVertexMaxDegreeConstraint::check() {
        // TODO: check graph ...
        return SatisfactionVerdict::kOK;
    }

    // OrderConstraint

    ConstraintPtr OrderConstraint::clone() {
        return std::make_shared<OrderConstraint>(*this);
    }

    Graph::OrderType OrderConstraint::value() {
        return graph_ptr_->order();
    }

    // SizeConstraint

    ConstraintPtr SizeConstraint::clone() {
        return std::make_shared<SizeConstraint>(*this);
    }

    Graph::SizeType SizeConstraint::value() {
        return graph_ptr_->size();
    }

    // ComponentNumberConstraint

    ConstraintPtr ComponentNumberConstraint::clone() {
        return std::make_shared<ComponentNumberConstraint>(*this);
    }

    Graph::OrderType ComponentNumberConstraint::value() {
        // TODO: count components_number
        Graph::OrderType components_number = 0;
        return components_number;
    }

    // ComponentOrderConstraint

    ConstraintPtr ComponentOrderConstraint::clone() {
        return std::make_shared<ComponentOrderConstraint>(*this);
    }

    Graph::OrderType ComponentOrderConstraint::value() {
        // TODO: count min/max component order
        return 0;
    }

    Constraint::SatisfactionVerdict ComponentOrderConstraint::check() {
        return Constraint::SatisfactionVerdict::kOK;
    }

    // ComponentSizeConstraint

    ConstraintPtr ComponentSizeConstraint::clone() {
        return std::make_shared<ComponentSizeConstraint>(*this);
    }

    Graph::SizeType ComponentSizeConstraint::value() {
        // TODO: count min/max component size
    }

    Constraint::SatisfactionVerdict ComponentSizeConstraint::check() {
        return Constraint::SatisfactionVerdict::kOK;
    }

    // ComponentCutPointConstraint

    ConstraintPtr ComponentCutPointConstraint::clone() {
        return std::make_shared<ComponentCutPointConstraint>(*this);
    }

    int ComponentCutPointConstraint::value() {
        // TODO: count min/max component size
        return 0;
    }

    Constraint::SatisfactionVerdict ComponentCutPointConstraint::check() {
        return Constraint::SatisfactionVerdict::kOK;
    }

    // ComponentBridgeConstraint

    ConstraintPtr ComponentBridgeConstraint::clone() {
        return std::make_shared<ComponentBridgeConstraint>(*this);
    }

    Graph::SizeType ComponentBridgeConstraint::value() {
        // TODO: this
        return 0;
    }

    Constraint::SatisfactionVerdict ComponentBridgeConstraint::check() {
        return Constraint::SatisfactionVerdict::kOK;
    }

    // ComponentDiameterConstraint

    ConstraintPtr ComponentDiameterConstraint::clone() {
        return std::make_shared<ComponentDiameterConstraint>(*this);
    }

    Graph::SizeType ComponentDiameterConstraint::value() {
        // TODO: count diameter
        return 0;
    }

    Constraint::SatisfactionVerdict ComponentDiameterConstraint::check() {
        return Constraint::SatisfactionVerdict::kOK;
    }
}
