#ifndef GRAPH_CONSTRAINT_SOLVER_UTILS_H
#define GRAPH_CONSTRAINT_SOLVER_UTILS_H

#include <memory>
#include <functional>
#include <random>
#include <chrono>

namespace graph_constraint_solver {

    class Random {
    public:
        static int next(int n);
    private:
        Random() {}
    };

    double timeit(std::function<void()> f);
}

#endif //GRAPH_CONSTRAINT_SOLVER_UTILS_H
