#include "program_block.h"

namespace graph_constraint_solver {
    ProgramBlock::ProgramBlock(Type type, Identificator id)
        : type_(type), id_(id) {

    }

    InputBlock::InputBlock(Identificator id, Arguments arguments)
        : ProgramBlock(Type::kInput, id), arguments_(arguments) {

    }

    OutputBlock::Format::Format(Structure structure, Indexation indexation)
        : structure(structure), indexation(indexation) {

    }


    OutputBlock::OutputBlock(Identificator id, Identificator graph_id, Format format)
        : ProgramBlock(Type::kOutput, id), graph_id_(graph_id), format_(format) {

    }


    CreatorBlock::CreatorBlock(Identificator id, ConstraintBlockPtr constraint_block_ptr)
        : ProgramBlock(Type::kCreator, id), constraint_block_ptr_(constraint_block_ptr) {

    }

    ConstraintBlockPtr CreatorBlock::get_constraint_block_ptr() {
        return constraint_block_ptr_;
    }
}