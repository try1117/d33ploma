#include "program_block.h"

#include "generator.h"

namespace graph_constraint_solver {
    ProgramBlock::ProgramBlock(Type type, Identificator id)
        : type_(type), id_(id) {

    }

    ProgramBlock::Type ProgramBlock::type() {
        return type_;
    }

    InputBlock::InputBlock(Identificator id, Arguments arguments)
        : ProgramBlock(Type::kInput, id), arguments_(arguments) {

    }

    GraphComponentsPtr InputBlock::generate_graph() {
        return nullptr;
    }

    GraphComponentsPtr OutputBlock::generate_graph() {
        return graph_program_block_ptr_->generate_graph();
    }

    void OutputBlock::print_graph(bool debug) {
        auto graph = generate_graph();
        graph->print(format_, debug);
    }

    OutputBlock::OutputBlock(Identificator id, Identificator graph_id, GraphPrinter::OutputFormat format,
            ProgramBlockPtr graph_program_block_ptr)
        : ProgramBlock(Type::kOutput, id), graph_id_(graph_id), format_(format),
        graph_program_block_ptr_(graph_program_block_ptr)  {

    }


    CreatorBlock::CreatorBlock(Identificator id, ConstraintBlockPtr constraint_block_ptr)
        : ProgramBlock(Type::kCreator, id), constraint_block_ptr_(constraint_block_ptr) {

    }

    ConstraintBlockPtr CreatorBlock::get_constraint_block_ptr() {
        return constraint_block_ptr_;
    }

    GraphComponentsPtr CreatorBlock::generate_graph() {
        Generator generator;
        auto graph = generator.generate(constraint_block_ptr_);
        return graph;
    }

}