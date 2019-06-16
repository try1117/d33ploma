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

        ConstrainedGraphPtr constrained_graph;
        auto run_time = graph_constraint_solver::Utils::timeit([&]() {
            auto generator = graph_constraint_solver::Generator();
            constrained_graph = generator.generate(creator_block->get_constraint_block_ptr());
        });
        auto graph = constrained_graph->components_ptr()->get_component(0);

        std::pair<int, int> bridges_number;
        std::vector<std::pair<int, int>> bridges_list;
        GraphAlgorithms::find_bridges(graph, bridges_number, bridges_list);
        std::set<std::pair<int, int>> bridges_set(bridges_list.begin(), bridges_list.end());

        int cut_points_number;
        std::vector<int> cut_points_list;
        GraphAlgorithms::find_cut_points(graph, cut_points_number, cut_points_list);

        std::cout << "Graph order : " << graph->order() << std::endl;
        std::cout << "Graph size  : " << graph->size() << std::endl;
        std::cout << "Bridg, nonlv: " << bridges_number.first << ", " << bridges_number.second << std::endl;
        std::cout << "Cut points  : " << cut_points_list.size() << std::endl;
        std::cout << std::endl;

//        std::set<std::pair<int, int>> edges;
//        for (int i = 0; i < graph->order(); ++i) {
//            for (auto child : graph->adjacency_list()[i]) {
//            if (i <= child) {
//                if (edges.count({i, child})) {
//                    std::cout << "parallel" << std::endl;
//                }
//                edges.insert({i, child});
//                printf("g.add_edge(%d, %d, color='%s')\n", i, child, bridges_set.count({i, child}) ? "red" : "blue");
////                std::cout << i + 1 << " " << child + 1 << "\n";
//            }
//            }
//        }
//
//        std::cout << std::endl;
//        for (auto v : cut_points_list) {
//            std::cout << v << ", ";
//        }
    }
}