#include "graph_printer.h"

#include <iostream>
#include <fstream>
#include <algorithm>

#include "graph_algorithms.h"
#include "utils.h"

namespace graph_constraint_solver {

    const GraphPrinter::OutputFormat::Filepath GraphPrinter::OutputFormat::kDefaultFilepath = "";

    GraphPrinter::OutputFormat::OutputFormat(Structure structure, Indexation indexation, Filepath filepath)
        : structure(structure), indexation(indexation), filepath(filepath) {

    }

    std::ostream& GraphPrinter::output() {
        return output_file_.badbit ? std::cout : output_file_;
    }

    GraphPrinter::GraphPrinter(GraphPtr graph, OutputFormat &output_format, bool debug) {
        if (!output_format.filepath.empty()) {
            output_file_.exceptions(std::ofstream::failbit | std::ofstream::badbit);
            output_file_.open(output_format.filepath, std::ofstream::out);
            if (!output_file_) {
                throw std::runtime_error("Can't open file " + output_format.filepath);
            }
        }
        std::ios_base::sync_with_stdio(false);
//        std::cin.tie(0);

        if (!debug) {
            if (graph->type() == Graph::Type::kUndirected) {
                print_undirected(graph, output_format);
            }
            else if (graph->type() == Graph::Type::kDirected) {
                print_directed(graph, output_format);
            }
        }
        else {
            if (graph->type() == Graph::Type::kUndirected) {
                print_undirected_debug(graph, output_format);
            }
            else if (graph->type() == Graph::Type::kDirected) {
                print_directed_debug(graph, output_format);
            }
        }
    }

    void GraphPrinter::print_undirected(GraphPtr graph, OutputFormat &output_format) {
        if (graph->empty()) return;
        size_t add_to_index = output_format.indexation == GraphPrinter::OutputFormat::Indexation::kOneBased;
        // now only print adj_list

        //  TODO: OutputFormat parameter to specify whether we need to print 'order' and 'size'
        output() << graph->order() << " " << graph->size() << "\n";
//        output() << graph->order() << " " << graph->size() << "\n";

        for (int i = 0; i < graph->order(); ++i) {
            for (auto child : graph->adjacency_list().at(i)) {
                if (i > child) {
                    continue;
                }
                output() << i + add_to_index << " " << child + add_to_index << "\n";
            }
        }
    }

    void GraphPrinter::print_directed(GraphPtr graph, OutputFormat &output_format) {
        if (graph->empty()) return;
        size_t add_to_index = output_format.indexation == GraphPrinter::OutputFormat::Indexation::kOneBased;
        // now only print adj_list

        //  TODO: OutputFormat parameter to specify whether we need to print 'order' and 'size'
        output() << graph->order() << " " << graph->size() << "\n";

        for (int i = 0; i < graph->order(); ++i) {
            for (auto child : graph->adjacency_list().at(i)) {
                output() << i + add_to_index << " " << child + add_to_index << "\n";
            }
        }
    }

    void GraphPrinter::print_undirected_debug(GraphPtr graph, OutputFormat &output_format) {

        size_t add_to_index = output_format.indexation == GraphPrinter::OutputFormat::Indexation::kOneBased;

        std::pair<int, int> bridges_number;
        std::vector<std::pair<int, int>> bridges_list;
        GraphAlgorithms::find_bridges(graph, bridges_number, bridges_list);
        std::set<std::pair<int, int>> bridges_set(bridges_list.begin(), bridges_list.end());

        int cut_points_number = 0;
        std::vector<int> cut_points_list;
        GraphAlgorithms::find_cut_points(graph, cut_points_number, cut_points_list);

        std::cout << "Graph order : " << graph->order() << std::endl;
        std::cout << "Graph size  : " << graph->size() << std::endl;
        std::cout << "Bridg, nonlv: " << bridges_number.first << ", " << bridges_number.second << std::endl;
        std::cout << "Cut points  : " << cut_points_list.size() << std::endl;
        std::cout << std::endl;

        std::set<std::pair<int, int>> edges;
        for (int i = 0; i < graph->order(); ++i) {
            for (auto child : graph->adjacency_list()[i]) {
                if (i <= child) {
                    if (edges.count({i, child})) {
                        std::cout << "parallel" << std::endl;
                    }
                    edges.insert({i, child});
                    std::cout << "g.add_edge(" << i + add_to_index << ", " << child + add_to_index
                        << ", color='" << (bridges_set.count({i, child}) ? "red" : "blue") << "')\n";
                }
            }
        }

        std::cout << "\nCut-points:\n";
        sort(cut_points_list.begin(), cut_points_list.end());
        for (auto v : cut_points_list) {
            std::cout << v + add_to_index << ", ";
        }
    }

    void GraphPrinter::print_directed_debug(GraphPtr graph, OutputFormat &output_format) {
        size_t add_to_index = output_format.indexation == GraphPrinter::OutputFormat::Indexation::kOneBased;
        std::cout << "Graph order : " << graph->order() << std::endl;
        std::cout << "Graph size  : " << graph->size() << std::endl;
        std::cout << std::endl;

        std::set<std::pair<int, int>> edges;
        for (int i = 0; i < graph->order(); ++i) {
            for (auto child : graph->adjacency_list()[i]) {
                if (edges.count({i, child})) {
                    std::cout << "parallel" << std::endl;
                }
                edges.insert({i, child});
                std::cout << "g.add_edge(" << i + add_to_index << ", " << child + add_to_index << ", color='blue')\n";
            }
        }
//        print_directed(graph, output_format);
    }

}