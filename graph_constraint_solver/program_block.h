#ifndef GRAPH_CONSTRAINT_SOLVER_PROGRAM_BLOCK_H
#define GRAPH_CONSTRAINT_SOLVER_PROGRAM_BLOCK_H

#include <string>
#include <vector>

#include "constraint_block.h"

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

    protected:
        Type type_;
        Identificator id_;
    };

    class InputBlock : public ProgramBlock {
    public:
        using Argument = std::string;
        using Arguments = std::vector<Argument>;
        InputBlock(Identificator id, Arguments arguments);

    private:
        Arguments arguments_;
        // TODO: map argument to value
    };

    class OutputBlock : public ProgramBlock {
    public:
        struct Format {
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

            Format(Structure structure = kDefaultStructure, Indexation indexation = kDefaultIndexation);
        };

        OutputBlock(Identificator id, Identificator graph_id, Format format);

    private:
        Identificator graph_id_;
        Format format_;
    };

    class CreatorBlock : public ProgramBlock {
    public:
        CreatorBlock(Identificator id, ConstraintBlockPtr constraint_block_ptr);
        ConstraintBlockPtr get_constraint_block_ptr();

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

#endif //GRAPH_CONSTRAINT_SOLVER_PROGRAM_BLOCK_H
