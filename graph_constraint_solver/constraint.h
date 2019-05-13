#ifndef GRAPH_CONSTRAINT_SOLVER_CONSTRAINT_H
#define GRAPH_CONSTRAINT_SOLVER_CONSTRAINT_H

#include <functional>
#include <memory>
#include <set>

#include "graph.h"

namespace graph_constraint_solver {

    enum ConstraintSatisfactionVerdict : unsigned char {
            kFail,
            kNotYet,
            kOK,
    };

    class Constraint {
    public:
        virtual void add_directed_edge(int, int) = 0;
        virtual ConstraintSatisfactionVerdict check() = 0;
        virtual ConstraintSatisfactionVerdict check(GraphPtr g) = 0;
    };

    class BridgeConstraint : public Constraint {
    public:
        BridgeConstraint(int l_bound, int r_bound, GraphPtr graph_ptr);
        void add_directed_edge(int from, int to) override;
        ConstraintSatisfactionVerdict check() override;
        ConstraintSatisfactionVerdict check(GraphPtr g) override;

    private:
        int l_bound, r_bound;
        GraphPtr graph_ptr;

        // can make this variables static
        std::vector<int> tin, fup;
        int timer;
        std::set<std::pair<int, int>> bridges_list;

        int _count_bridges(GraphPtr graph_ptr, int v, int pr, std::pair<int, int> &res, bool save_bridges = false);
        std::pair<int, int> count_bridges(GraphPtr graph_ptr, bool save_bridges = false);
    };

    using ConstraintPtr = std::shared_ptr<Constraint>;
}

#endif //GRAPH_CONSTRAINT_SOLVER_CONSTRAINT_H
