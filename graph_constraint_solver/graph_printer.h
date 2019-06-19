#ifndef GRAPH_CONSTRAINT_SOLVER_GRAPH_PRINTER_H
#define GRAPH_CONSTRAINT_SOLVER_GRAPH_PRINTER_H

#include <iostream>
#include <fstream>

#include "graph.h"

namespace graph_constraint_solver {
    class GraphPrinter {
    public:
        struct OutputFormat {
            enum class Structure {
                kEdgeList,
                kAdjMatrix,
                kParentArray,
            };

            enum class Indexation {
                kZeroBased,
                kOneBased,
            };

            using Filepath = std::string;

            static const Structure kDefaultStructure = Structure::kEdgeList;
            static const Indexation kDefaultIndexation = Indexation::kOneBased;
            static const Filepath kDefaultFilepath;

            Structure structure;
            Indexation indexation;
            Filepath filepath;

            OutputFormat(Structure structure = kDefaultStructure,
                    Indexation indexation = kDefaultIndexation,
                    Filepath filepath = kDefaultFilepath);
        };

        GraphPrinter(GraphPtr graph, OutputFormat &output_format, bool debug);

    private:
        std::ofstream output_file;
        std::ostream& output() {
            return output_file ? output_file : std::cout;
        }

        void print_undirected(GraphPtr graph, OutputFormat &output_format);
        void print_directed(GraphPtr graph, OutputFormat &ouptut_format);

        void print_undirected_debug(GraphPtr graph, OutputFormat &output_format);
        void print_directed_debug(GraphPtr graph, OutputFormat &ouptut_format);
    };
}

#endif //GRAPH_CONSTRAINT_SOLVER_GRAPH_PRINTER_H
