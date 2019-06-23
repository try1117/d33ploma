#ifndef GRAPH_CONSTRAINT_SOLVER_PROGRAM_BLOCK_H
#define GRAPH_CONSTRAINT_SOLVER_PROGRAM_BLOCK_H

#include <string>
#include <vector>

#include "constraint_block.h"
#include "graph_components.h"

namespace graph_constraint_solver {

    class ProgramBlock {
    public:
        enum class Type {
            kInput,
            kOutput,
            kCreator,
            kOrientator,
            kCombinator,
        };

        using Identificator = std::string;

        ProgramBlock(Type type, Identificator id);

        virtual GraphComponentsPtr generate_graph() = 0;

    protected:
        Type type_;
        Identificator id_;
    };

    using ProgramBlockPtr = std::shared_ptr<ProgramBlock>;

    class InputBlock : public ProgramBlock {
    public:
        using Argument = std::string;
        using Arguments = std::vector<Argument>;
        InputBlock(Identificator id, Arguments arguments);
        GraphComponentsPtr generate_graph();

    private:
        Arguments arguments_;
        // TODO: map argument to value
    };

    class OutputBlock : public ProgramBlock {
    public:
        OutputBlock(Identificator id, Identificator graph_id, GraphPrinter::OutputFormat format, ProgramBlockPtr graph_program_block_ptr);
        GraphComponentsPtr generate_graph() override;
        void print_graph(bool debug = false);

    private:
        Identificator graph_id_;
        ProgramBlockPtr graph_program_block_ptr_;
        GraphPrinter::OutputFormat format_;
    };

    class CreatorBlock : public ProgramBlock {
    public:
        CreatorBlock(Identificator id, ConstraintBlockPtr constraint_block_ptr);
        ConstraintBlockPtr get_constraint_block_ptr();
        GraphComponentsPtr generate_graph() override;

    private:
        ConstraintBlockPtr constraint_block_ptr_;
    };

    class OrientatorBlock : public ProgramBlock {
    public:
        // TODO: think about it
        struct Format {

        };

        OrientatorBlock(Identificator id, Identificator graph_id, Format format);

    private:
        Identificator graph_id_;
        Format format_;
    };

    class CombinatorBlock : public ProgramBlock {
    public:
        enum class CombinationType {
            kAppend,
            kConnectMainWithSeconary,
            kConnectMainComponents,
        };

        CombinatorBlock(Identificator id, CombinationType combination_type,
                Identificator main_graph_id, Identificator seconary_graph_id, Identificator frame_graph_id);

    private:
        CombinationType combination_type_;
        Identificator main_graph_id_;
        Identificator secondary_graph_id_;
        Identificator frame_graph_id_;
    };
}

#ifdef GRAPH_CONSTRAINT_SOLVER_SINGLE_HEADER
#include "program_block.cpp"
#endif

#endif //GRAPH_CONSTRAINT_SOLVER_PROGRAM_BLOCK_H
