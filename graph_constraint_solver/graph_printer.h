#ifndef GRAPH_CONSTRAINT_SOLVER_GRAPH_PRINTER_H
#define GRAPH_CONSTRAINT_SOLVER_GRAPH_PRINTER_H

#include "graph.h"

namespace graph_constraint_solver {
    class GraphPrinter {
    public:
        struct OutputFormat {
            enum class Structure {
                kAdjList,
                kAdjMatrix,
                kParentArray,
            };

            enum class Indexation {
                kZeroBased,
                kOneBased,
            };

            static const Structure kDefaultStructure = Structure::kAdjList;
            static const Indexation kDefaultIndexation = Indexation::kOneBased;

            Structure structure;
            Indexation indexation;

            OutputFormat(Structure structure = kDefaultStructure, Indexation indexation = kDefaultIndexation);
        };

        static void print(GraphPtr graph, OutputFormat output_format, bool debug);

    private:
        static void print_undirected(GraphPtr graph, OutputFormat output_format);
        static void print_directed(GraphPtr graph, OutputFormat ouptut_format);

        static void print_undirected_debug(GraphPtr graph, OutputFormat output_format);
        static void print_directed_debug(GraphPtr graph, OutputFormat ouptut_format);
    };
}

#endif //GRAPH_CONSTRAINT_SOLVER_GRAPH_PRINTER_H
