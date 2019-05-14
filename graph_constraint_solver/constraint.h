#ifndef GRAPH_CONSTRAINT_SOLVER_CONSTRAINT_H
#define GRAPH_CONSTRAINT_SOLVER_CONSTRAINT_H

#include <functional>
#include <memory>
#include <set>

#include "graph.h"

namespace graph_constraint_solver {

    enum ConstraintSatisfactionVerdict : unsigned char {
            kImpossible = 0,
            kPossible = 1,
            kOK = 2,
    };

    class Constraint {
    public:
        virtual void add_directed_edge(int, int) = 0;
        virtual ConstraintSatisfactionVerdict check() = 0;
        virtual ConstraintSatisfactionVerdict check(GraphPtr graph_ptr) = 0;
        virtual void bind_graph(GraphPtr graph_ptr);

    protected:
        GraphPtr graph_ptr_;
    };

    class BridgeConstraint : public Constraint {
    public:
        BridgeConstraint(int l_bound, int r_bound);
        void bind_graph(GraphPtr graph_ptr) override;
        void add_directed_edge(int from, int to) override;
        ConstraintSatisfactionVerdict check() override;
        ConstraintSatisfactionVerdict check(GraphPtr g) override;

    private:
        int l_bound, r_bound;

        // can make this variables static
        std::vector<int> tin, fup;
        int timer;
        std::set<std::pair<int, int>> bridges_list;

        int _count_bridges(GraphPtr graph_ptr, int v, int pr, std::pair<int, int> &res, bool save_bridges = false);
        std::pair<int, int> count_bridges(GraphPtr graph_ptr, bool save_bridges = false);
    };

    using ConstraintPtr = std::shared_ptr<Constraint>;

    class ConstraintList : public Constraint {
    public:
//        ConstraintList() = default;
        void parse_JSON(std::string filepath) {}
        void parse_XML(std::string filepath) {}
        void add_constraint(ConstraintPtr constraint);
        const std::vector<ConstraintPtr> constraints();
        void bind_graph(GraphPtr graph_ptr) override;

        void add_directed_edge(int, int) override;
        ConstraintSatisfactionVerdict check() override;
        ConstraintSatisfactionVerdict check(GraphPtr graph_ptr) override;

    private:
        std::vector<ConstraintPtr> constraints_;
    };

    using ConstraintListPtr = std::shared_ptr<ConstraintList>;
}

#endif //GRAPH_CONSTRAINT_SOLVER_CONSTRAINT_H
