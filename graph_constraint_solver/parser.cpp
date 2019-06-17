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
            {Token::kInputArguments, "arguments"},
            {Token::kOutputFormat, "format"},
            {Token::kOutputGraphId, "graph-id"},
    };

    const ProgramBlock::Identificator Parser::input_reserved_id_("input");
    const ProgramBlock::Identificator Parser::output_reserved_id_("output");

    const std::unordered_map<Parser::String, GraphPrinter::OutputFormat::Structure> Parser::name_to_output_format_structure_ = {
            {"adjacency-list", GraphPrinter::OutputFormat::Structure::kAdjList},
            {"adj-list", GraphPrinter::OutputFormat::Structure::kAdjList},
            {"edge-list", GraphPrinter::OutputFormat::Structure::kAdjList},
            {"edges-list", GraphPrinter::OutputFormat::Structure::kAdjList},
            {"list", GraphPrinter::OutputFormat::Structure::kAdjList},

            {"adjacency-matrix", GraphPrinter::OutputFormat::Structure::kAdjMatrix},
            {"adj-matrix", GraphPrinter::OutputFormat::Structure::kAdjMatrix},
            {"matrix", GraphPrinter::OutputFormat::Structure::kAdjMatrix},

            {"parent-array", GraphPrinter::OutputFormat::Structure::kParentArray},
    };

    const std::unordered_map<Parser::String, GraphPrinter::OutputFormat::Indexation> Parser::name_to_output_format_indexation_ = {
            {"zero-based", GraphPrinter::OutputFormat::Indexation::kZeroBased},
            {"zero", GraphPrinter::OutputFormat::Indexation::kZeroBased},
            {"0-based", GraphPrinter::OutputFormat::Indexation::kZeroBased},
            {"0-base", GraphPrinter::OutputFormat::Indexation::kZeroBased},
            {"0", GraphPrinter::OutputFormat::Indexation::kZeroBased},

            {"one-based", GraphPrinter::OutputFormat::Indexation::kOneBased},
            {"one", GraphPrinter::OutputFormat::Indexation::kOneBased},
            {"1-based", GraphPrinter::OutputFormat::Indexation::kOneBased},
            {"1-base", GraphPrinter::OutputFormat::Indexation::kOneBased},
            {"1", GraphPrinter::OutputFormat::Indexation::kOneBased},
    };

    void Parser::throw_exception(std::string message) {
        throw std::runtime_error("Parser error in block with id '" + current_block_id_ + "': " + message);
    }

    ProgramBlock::Type Parser::name_to_program_block_type(String name) {
        if (!name_to_program_block_type_.count(name)) {
            throw_exception("expected block-type name but got '" + name + "'");
        }
        return name_to_program_block_type_.at(name);
    }

    // TODO: template this functions ??? (i'll do on the third time)

    ProgramBlock::Type Parser::parse_block_type(nlohmann::json &object, bool remove_field) {
        auto token_name = token_to_name_.at(Token::kBlockType);
        if (!object.count(token_name)) {
            throw_exception("'" + token_name + "' is not specified");
        }
        auto block_type = name_to_program_block_type(object[token_name]);
        if (remove_field) {
            object.erase(token_name);
        }
        return block_type;
    }

    ConstraintBlock::ComponentType Parser::name_to_component_type(String name) {
        if (!name_to_component_type_.count(name)) {
            throw_exception("expected component-type name but got '" + name + "'");
        }
        return name_to_component_type_.at(name);
    }

    ConstraintBlock::ComponentType Parser::parse_component_type(nlohmann::json &object, bool remove_field) {
        auto token_name = token_to_name_.at(Token::kComponentType);
        if (!object.count(token_name)) {
            throw_exception("'" + token_name + "' is not specified");
        }
        auto component_type = name_to_component_type(object[token_name]);
        if (remove_field) {
            object.erase(token_name);
        }
        return component_type;
    }

    void Parser::parse(JSONFile &json_file, InputBlock::Arguments arguments) {
        std::string text((std::istreambuf_iterator<char>(json_file)),
                std::istreambuf_iterator<char>());

        auto json_object = nlohmann::json::parse(text);
        cut_input_block(json_object, arguments);

        for (auto &element : json_object.items()) {
            auto id = element.key();
            auto value = element.value();
            current_block_id_ = id;
//            std::cerr << "Parse " + id << std::endl;
            if (id_to_program_block_ptr_.count(id)) {
                throw_exception("non-unique id");
            }

            if (id == output_reserved_id_) {
                output_blocks_.push_back(parse_output_block(value));
            }
            else {
                // TODO: check that id is unique
                auto block_type = parse_block_type(value);
                if (block_type == ProgramBlock::Type::kOutput) {
                    output_blocks_.push_back(parse_output_block(value));
                }
                else if (block_type == ProgramBlock::Type::kCreator) {
                    id_to_program_block_ptr_[id] = std::static_pointer_cast<ProgramBlock>(parse_creator_block(value));
                }
                else {
                    throw_exception("unsupported block-type");
                }
            }
        }
    }

    std::vector<std::shared_ptr<OutputBlock>> Parser::get_output_blocks() {
        return output_blocks_;
    }

    void Parser::cut_input_block(nlohmann::json &object, InputBlock::Arguments arguments_values) {
        auto input_block_id = input_reserved_id_;
        size_t number_of_input_blocks = 0;
        if (object.count(input_reserved_id_)) {
            ++number_of_input_blocks;
        }
        for (auto &element : object.items()) {
            auto id = element.key();
            auto value = element.value();
            if (value.count(token_to_name_.at(Token::kBlockType))) {
                auto block_type = parse_block_type(value);
                if (block_type == ProgramBlock::Type::kInput) {
                    input_block_id = id;
                    ++number_of_input_blocks;
                }
                else if (id == input_reserved_id_) {
                    throw_exception("id '" + input_reserved_id_ + "' is reserved for input-block");
                }
            }
        }
        if (number_of_input_blocks > 1) {
            throw_exception("given more than one input-block");
        }

        if (number_of_input_blocks == 0) {
            return;
        }

        auto input_block_object = object.at(input_block_id);

        // when id = "input", check that BlockType is undefined or equal to "input"
        if (input_block_object.count(token_to_name_.at(Token::kBlockType))) {
            if (parse_block_type(input_block_object) != ProgramBlock::Type::kInput) {
                throw_exception("id '" + input_reserved_id_ + "' is reserved for input-block type");
            }
        }

        InputBlock::Arguments arguments_names;
        auto arguments_token_name = token_to_name_.at(Token::kInputArguments);
        if (input_block_object.count(arguments_token_name)) {
            nlohmann::json arguments_array = input_block_object.at(arguments_token_name);
            try {
                if (!arguments_array.is_array()) {
                    throw std::exception();
                }
                arguments_names = static_cast<InputBlock::Arguments>(arguments_array);
            }
            catch (...) {
                throw_exception("'" + arguments_token_name + "' expected to be an array of strings - example: [\"param1\", \"param2\"]");
            }
        }

        if (arguments_names.size() != arguments_values.size()) {
            throw_exception("expected " + std::to_string(arguments_names.size()) + " arguments but " +
            std::to_string(arguments_values.size()) + " given");
        }

        object.erase(input_block_id);
        std::string dump_string = object.dump();

        for (size_t i = 0; i < arguments_names.size(); ++i) {
            auto name = "\"%" + arguments_names[i] + "\"";
            auto value = arguments_values[i];

            // TODO: BOOST can boost::replace_all ...
            // well this code is definitely an overkill, could just do 'while(find) replace'
            // parser isn't a bottleneck of this program ...
            std::vector<size_t> pattern_positions;
            size_t position = dump_string.find(name, 0);
            while (position != std::string::npos) {
                pattern_positions.push_back(position);
                position = dump_string.find(name, position + 1);
            }

            auto new_dump_string = std::string(dump_string.size() + pattern_positions.size() * (value.size() - name.size()), 0);
            size_t dump_string_position = 0;
            size_t new_dump_string_position = 0;

            for (size_t pattern_index = 0; dump_string_position < dump_string.size(); ) {
                if (pattern_index < pattern_positions.size() && dump_string_position == pattern_positions[pattern_index]) {
                    for (size_t j = 0; j < value.size(); ++j) {
                        new_dump_string[new_dump_string_position + j] = value[j];
                    }
                    dump_string_position += name.size();
                    new_dump_string_position += value.size();
                    pattern_index++;
                }
                else {
                    new_dump_string[new_dump_string_position++] = dump_string[dump_string_position++];
                }
            }

            dump_string = new_dump_string;
        }

        try {
            object = nlohmann::json::parse(dump_string);
        }
        catch (...) {
            throw_exception("bad arguments substitution");
        }
    }

    GraphPrinter::OutputFormat Parser::parse_output_format(nlohmann::json object) {
        auto format_token_name = token_to_name_.at(Token::kOutputFormat);
        if (!object.count(format_token_name)) {
            throw_exception("expected '" + format_token_name + "' field");
        }

        nlohmann::json format_json_array = object.at(format_token_name);
        std::vector<Parser::String> format_array;

        try {
            if (!format_json_array.is_array()) {
                throw std::exception();
            }
            format_array = static_cast<std::vector<Parser::String>>(format_json_array);
        }
        catch (...) {
            throw_exception("'" + format_token_name + "' expected array of strings");
        }

        size_t structure_options_cnt = 0;
        size_t indexation_options_cnt = 0;

        auto structure = GraphPrinter::OutputFormat::kDefaultStructure;
        auto indexation = GraphPrinter::OutputFormat::kDefaultIndexation;

        for (auto &option : format_array) {
            if (name_to_output_format_structure_.count(option)) {
                ++structure_options_cnt;
                structure = name_to_output_format_structure_.at(option);
            }
            else if (name_to_output_format_indexation_.count(option)) {
                ++indexation_options_cnt;
                indexation = name_to_output_format_indexation_.at(option);
            }
            else {
                throw_exception("undefined output-format '" + option + "'");
            }
        }
        return GraphPrinter::OutputFormat(structure, indexation);
    }

    ProgramBlock::Identificator Parser::parse_output_graph_id(nlohmann::json object) {
        auto token_name = token_to_name_.at(Token::kOutputGraphId);
        if (!object.count(token_name)) {
            throw_exception("expected '" + token_name + "' field");
        }
        nlohmann::json json_id = object.at(token_name);
        if (!json_id.is_string()) {
            throw_exception("'" + token_name + "' field expected a string identificator");
        }
        auto id = ProgramBlock::Identificator(json_id);
        if (!id_to_program_block_ptr_.count(id)) {
            throw_exception("unknown " + token_name + " '" + id + "'");
        }
        return id;
    }

    std::shared_ptr<OutputBlock> Parser::parse_output_block(nlohmann::json object) {
        if (object.count(token_to_name_.at(Token::kBlockType))) {
            auto block_type = parse_block_type(object);
            if (block_type != ProgramBlock::Type::kOutput) {
                throw_exception("id '" + output_reserved_id_ + "' is reserved for output-type block");
            }
        }

        auto graph_id = parse_output_graph_id(object);
        auto format = parse_output_format(object);
        return std::make_shared<OutputBlock>(current_block_id_, graph_id, format, id_to_program_block_ptr_[graph_id]);
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
                        throw std::exception();
                    }
                }
                catch (...) {
                    throw std::runtime_error("Parser error: '" + key + "' field expected one or two integers");
                }
            }
        }
    }
}