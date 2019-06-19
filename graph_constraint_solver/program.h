#ifndef GRAPH_CONSTRAINT_SOLVER_PROGRAM_H
#define GRAPH_CONSTRAINT_SOLVER_PROGRAM_H

#include "parser.h"
#include "program_block.h"

namespace graph_constraint_solver {
    class Program {
    public:
        Program(Parser::JSONFile &json_file, InputBlock::Arguments arguments);
    };
}

#endif //GRAPH_CONSTRAINT_SOLVER_PROGRAM_H
