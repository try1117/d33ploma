#include "program.h"

//#include <iostream>
//#include <fstream>

#include "generator.h"
#include "graph_algorithms.h"

namespace graph_constraint_solver {
    Program::Program(Parser::JSONFile &json_file, InputBlock::Arguments arguments) {
        Parser parser;
        parser.parse(json_file, arguments);
        auto output_blocks = parser.get_output_blocks();

        auto run_time = graph_constraint_solver::Utils::timeit([&]() {
            for (auto &output_block : output_blocks) {
                // TODO: GraphPrinter enum for this debug mode
                output_block->print_graph();
            }
        });
    }
}