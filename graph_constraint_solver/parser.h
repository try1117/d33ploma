#ifndef GRAPH_CONSTRAINT_SOLVER_PARSER_H
#define GRAPH_CONSTRAINT_SOLVER_PARSER_H

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <unordered_map>

#include "program_block.h"
#include "constraint_block.h"
#include "graph.h"

namespace graph_constraint_solver {

    class Parser {
    public:
        using JSONFile = std::ifstream;
        using String = std::string;
        void parse(JSONFile &json_file, InputBlock::Arguments arguments);

        std::vector<std::shared_ptr<OutputBlock>> get_output_blocks();

    private:
        void throw_exception(std::string message);
        std::unordered_map<ProgramBlock::Identificator, std::shared_ptr<ProgramBlock>> id_to_program_block_ptr_;

        enum class Token;
        static const std::unordered_map<Token, String> token_to_name_;
        static const ProgramBlock::Identificator input_reserved_id_;
        static const ProgramBlock::Identificator output_reserved_id_;

        static const std::unordered_map<String, ProgramBlock::Type> name_to_program_block_type_;
        ProgramBlock::Type name_to_program_block_type(String name);
        ProgramBlock::Type parse_block_type(nlohmann::json &object, bool remove_field = true);

        static const std::unordered_map<String, ConstraintBlock::ComponentType> name_to_component_type_;
        ConstraintBlock::ComponentType name_to_component_type(String name);
        ConstraintBlock::ComponentType parse_component_type(nlohmann::json &object, bool remove_field = true);

        void cut_input_block(nlohmann::json &object, InputBlock::Arguments argument_values);

        static const std::unordered_map<String, OutputBlock::Format::Structure> name_to_output_format_structure_;
        static const std::unordered_map<String, OutputBlock::Format::Indexation> name_to_output_format_indexation_;
        OutputBlock::Format parse_output_format(nlohmann::json object);
        ProgramBlock::Identificator parse_output_graph_id(nlohmann::json object);
        std::shared_ptr<OutputBlock> parse_output_block(nlohmann::json object);

        std::shared_ptr<CreatorBlock> parse_creator_block(nlohmann::json object);

        ProgramBlock::Identificator current_block_id_;
        std::shared_ptr<InputBlock> input_block_;
        // ZALEPA:
        std::shared_ptr<CreatorBlock> creator_block_;
        std::vector<std::shared_ptr<OutputBlock>> output_blocks_;

        enum class Token {
            kBlockType,
            kComponentType,
            kInputArguments,
            kOutputFormat,
            kOutputGraphId,
        };
    };

    namespace impl {
        class CreatorBlockParser {
        public:
            CreatorBlockParser(ConstraintBlockPtr constraint_block_ptr, nlohmann::json object);

        private:
            static const std::unordered_map<Parser::String, Constraint::Type> name_to_constraint_type_;
            static const std::unordered_map<Parser::String, Graph::Type> name_graph_type_;

            std::vector<ConstraintPtr> parse_constraints(nlohmann::json object);
            Constraint::Type parse_constraint_type(Parser::String name);
            Graph::Type parse_graph_type(Parser::String name);
            ConstraintPtr parse_constraint(Parser::String key, nlohmann::json value);
        };
    }
}

#endif //GRAPH_CONSTRAINT_SOLVER_PARSER_H
