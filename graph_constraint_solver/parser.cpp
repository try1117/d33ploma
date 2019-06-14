#include "parser.h"

#include <fstream>

#include "program_block.h"
#include "constraint.h"

namespace graph_constraint_solver {

    const std::unordered_map<Parser::String, ProgramBlock::Type> Parser::name_to_program_block_type_ = {
            {"input", ProgramBlock::Type::kInput},
            {"output", ProgramBlock::Type::kOutput},
            {"creator", ProgramBlock::Type::kCreator},
            {"orientator", ProgramBlock::Type::kOrientator},
            {"combinator", ProgramBlock::Type::kCombinator},
    };

    const std::unordered_map<Parser::String, ConstraintBlock::ComponentType> Parser::name_to_component_type_ = {
            {"connected", ConstraintBlock::ComponentType::kConnected},

            {"two-vertex-connected", ConstraintBlock::ComponentType::kTwoConnected},
            {"two-connected", ConstraintBlock::ComponentType::kTwoConnected},
            {"2-connected", ConstraintBlock::ComponentType::kTwoConnected},
            {"2-vertex-connected", ConstraintBlock::ComponentType::kTwoConnected},
            {"2connected", ConstraintBlock::ComponentType::kTwoConnected},

            {"two-edge-connected", ConstraintBlock::ComponentType::kTwoEdgeConnected},
            {"2-edge-connected", ConstraintBlock::ComponentType::kTwoEdgeConnected},
            {"2econnected", ConstraintBlock::ComponentType::kTwoEdgeConnected},

            {"strongly-connected", ConstraintBlock::ComponentType::kStronglyConnected},
            {"strong-connected", ConstraintBlock::ComponentType::kStronglyConnected},
            {"strong-connect", ConstraintBlock::ComponentType::kStronglyConnected},
            {"strong", ConstraintBlock::ComponentType::kStronglyConnected},

            {"tree", ConstraintBlock::ComponentType::kTree},
    };

    const std::unordered_map<Parser::Token, Parser::String> Parser::token_to_name_ = {
            {Token::kBlockType, "type"},
            {Token::kComponentType, "component-type"},
    };


    ProgramBlock::Type Parser::name_to_program_block_type(String name) {
        if (!name_to_program_block_type_.count(name)) {
            throw std::runtime_error("Parser error: expected type name but got '" + name + "'");
        }
        return name_to_program_block_type_.at(name);
    }

    // TODO: template this functions ??? (i'll do on the third time)

    ProgramBlock::Type Parser::parse_block_type(nlohmann::json &object, bool remove_field) {
        auto token_name = token_to_name_.at(Token::kBlockType);
        if (!object.count(token_name)) {
            throw std::runtime_error("Parser error: specify '" + token_name + "' for block '" + current_block_id_ + "'");
        }
        auto block_type = name_to_program_block_type(object[token_name]);
        if (remove_field) {
            object.erase(token_name);
        }
        return block_type;
    }

    ConstraintBlock::ComponentType Parser::name_to_component_type(String name) {
        if (!name_to_component_type_.count(name)) {
            throw std::runtime_error("Parser error: expected component-type name but got '" + name + "'");
        }
        return name_to_component_type_.at(name);
    }

    ConstraintBlock::ComponentType Parser::parse_component_type(nlohmann::json &object, bool remove_field) {
        auto token_name = token_to_name_.at(Token::kComponentType);
        if (!object.count(token_name)) {
            throw std::runtime_error("Parser error: specify '" + token_name + "' for block '" + current_block_id_ + "'");
        }
        auto component_type = name_to_component_type(object[token_name]);
        if (remove_field) {
            object.erase(token_name);
        }
        if (object.count(token_name)) {
            throw std::exception();
        }
        return component_type;
    }

    void Parser::parse(JSONFile &json_file) {
        if (!json_file) {
            throw std::runtime_error("Parser error: bad file");
        }
        std::string text((std::istreambuf_iterator<char>(json_file)),
                std::istreambuf_iterator<char>());

        auto input_id = ProgramBlock::Identificator("input");
        auto output_id = ProgramBlock::Identificator("output");

        auto json_object = nlohmann::json::parse(text);
        for (auto &element : json_object.items()) {
            auto id = element.key();
            auto value = element.value();
            current_block_id_ = id;
            std::cout << "Parse " + id << std::endl;

            if (id == input_id) {
                input_block_ = parse_input_block(value);
            }
            else if (id == output_id) {
                auto output_block = parse_output_block(value);
            }
            else {
                // TODO: check that id is unique
                auto block_type = parse_block_type(value);
                if (block_type == ProgramBlock::Type::kInput) {
                    input_block_ = parse_input_block(value);
                }
                else if (block_type == ProgramBlock::Type::kOutput) {
                    output_block_ = parse_output_block(value);
                }
                else if (block_type == ProgramBlock::Type::kCreator) {
                    creator_block_ = parse_creator_block(value);
                }
                else {
                    throw std::runtime_error("Parser error: '" + id + " has unsupported block type" );
                }
            }
        }
    }

    std::shared_ptr<CreatorBlock> Parser::get_creator_block() {
        return creator_block_;
    }

    std::shared_ptr<InputBlock> Parser::parse_input_block(nlohmann::json object) {
        if (input_block_ != nullptr) {
            throw std::runtime_error("Parser error: more than one input block");
        }
        return nullptr;
    }

    std::shared_ptr<OutputBlock> Parser::parse_output_block(nlohmann::json object) {
        return nullptr;
    }

    std::shared_ptr<CreatorBlock> Parser::parse_creator_block(nlohmann::json object) {
        auto component_type = parse_component_type(object);
        auto constraint_block_ptr = ConstraintBlock::create(component_type);
        impl::CreatorBlockParser(constraint_block_ptr, object);
        return std::make_shared<CreatorBlock>(current_block_id_, constraint_block_ptr);
    }

    namespace impl {
        const std::unordered_map<Parser::String, Constraint::Type> CreatorBlockParser::name_to_constraint_type_ = {
                {"graph-type", Constraint::Type::kGraphType},
                {"total-order", Constraint::Type::kOrder},
                {"total-size", Constraint::Type::kSize},
                {"total-bridges", Constraint::Type::kBridge},
                {"total-cutpoints", Constraint::Type::kCutPoint},

                {"component-number", Constraint::Type::kComponentNumber},
                {"components-number", Constraint::Type::kComponentNumber},

                {"order", Constraint::Type::kComponentOrder},
                {"v", Constraint::Type::kComponentOrder},
                {"vertex", Constraint::Type::kComponentOrder},
                {"vertices", Constraint::Type::kComponentOrder},

                {"size", Constraint::Type::kComponentSize},
                {"e", Constraint::Type::kComponentSize},
                {"edge", Constraint::Type::kComponentSize},
                {"edges", Constraint::Type::kComponentSize},

                {"cut-point", Constraint::Type::kComponentCutPoint},
                {"cut-points", Constraint::Type::kComponentCutPoint},
                {"cutpoint", Constraint::Type::kComponentCutPoint},
                {"cutpoints", Constraint::Type::kComponentCutPoint},

                {"bridge", Constraint::Type::kComponentBridge},
                {"bridges", Constraint::Type::kComponentBridge},

                {"diameter", Constraint::Type::kComponentDiameter},

                {"vertex-maximum-degree", Constraint::Type::kComponentVertexMaxDegree},
                {"vertex-max-degree", Constraint::Type::kComponentVertexMaxDegree},
                {"maximum-degree", Constraint::Type::kComponentVertexMaxDegree},
                {"max-degree", Constraint::Type::kComponentVertexMaxDegree},
        };

        const std::unordered_map<Parser::String, Graph::Type> CreatorBlockParser::name_graph_type_ = {
                {"undirected", Graph::Type::kUndirected},
                {"undir", Graph::Type::kUndirected},
                {"directed", Graph::Type::kDirected},
                {"dir", Graph::Type::kDirected},
        };

        CreatorBlockParser::CreatorBlockParser(ConstraintBlockPtr constraint_block_ptr, nlohmann::json object) {
            auto constraints = parse_constraints(object);
            constraint_block_ptr->add_constraints(constraints);
        }

        std::vector<ConstraintPtr> CreatorBlockParser::parse_constraints(nlohmann::json object) {
            std::vector<ConstraintPtr> result;
            for (auto &element : object.items()) {
                result.push_back(parse_constraint(element.key(), element.value()));
            }
            return result;
        }

        Constraint::Type CreatorBlockParser::parse_constraint_type(Parser::String name) {
            if (!name_to_constraint_type_.count(name)) {
                throw std::runtime_error("Parser error: expected constraint-type but got '" + name + "'");
            }
            return name_to_constraint_type_.at(name);
        }

        Graph::Type CreatorBlockParser::parse_graph_type(Parser::String name) {
            if (!name_graph_type_.count(name)) {
                throw std::runtime_error("Parser error: expected graph-type but got '" + name + "'");
            }
            return name_graph_type_.at(name);
        }

        ConstraintPtr CreatorBlockParser::parse_constraint(Parser::String key, nlohmann::json value) {
            auto constraint_type = parse_constraint_type(key);
            if (constraint_type == Constraint::Type::kGraphType) {
                auto graph_type = parse_graph_type(value);
                return std::make_shared<GraphTypeConstraint>(graph_type);
            }
            else if (constraint_type == Constraint::Type::kComponentVertexMaxDegree) {
                if (!value.is_number_integer()) {
                    throw std::runtime_error("Parser error: '" + key + "' field expected a single integer");
                }
                return std::make_shared<ComponentVertexMaxDegreeConstraint>(static_cast<Graph::OrderType>(value));
            }
                // BoundedConstraints
            else {
                try {
                    if (value.is_number_integer()) {
                        auto number = static_cast<long long>(value);
                        return Constraint::create_bounded_ptr(constraint_type, number, number);
                    }
                    else if (value.is_array()) {
                        // TODO: replace 'long long' with ???
                        auto numbers = static_cast<std::vector<long long>>(value);
                        if (numbers.size() == 0 || numbers.size() > 2) {
                            throw;
                        }
                        long long left_bound = numbers[0];
                        long long right_bound = numbers.size() == 2 ? numbers[1] : left_bound;
                        return Constraint::create_bounded_ptr(constraint_type, left_bound, right_bound);
                    }
                    else {
                        throw;
                    }
                }
                catch (...) {
                    throw std::runtime_error("Parser error: '" + key + "' field expected one or two integers");
                }
            }
        }
    }
}