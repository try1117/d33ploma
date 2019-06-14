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
        void parse(JSONFile &json_file);
        // ZALEPA:
        std::shared_ptr<CreatorBlock> get_creator_block();

    private:
        enum class Token;
        static const std::unordered_map<Token, String> token_to_name_;

        static const std::unordered_map<String, ProgramBlock::Type> name_to_program_block_type_;
        ProgramBlock::Type name_to_program_block_type(String name);
        ProgramBlock::Type parse_block_type(nlohmann::json &object, bool remove_field = true);

        static const std::unordered_map<String, ConstraintBlock::ComponentType> name_to_component_type_;
        ConstraintBlock::ComponentType name_to_component_type(String name);
        ConstraintBlock::ComponentType parse_component_type(nlohmann::json &object, bool remove_field = true);

        std::shared_ptr<InputBlock> parse_input_block(nlohmann::json object);
        std::shared_ptr<OutputBlock> parse_output_block(nlohmann::json object);
        std::shared_ptr<CreatorBlock> parse_creator_block(nlohmann::json object);

        std::shared_ptr<InputBlock> input_block_;
        // ZALEPA:
        std::shared_ptr<CreatorBlock> creator_block_;
        std::shared_ptr<OutputBlock> output_block_;
        ProgramBlock::Identificator current_block_id_;

        enum class Token {
            kBlockType,
            kComponentType,
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
