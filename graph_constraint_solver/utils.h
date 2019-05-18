#ifndef GRAPH_CONSTRAINT_SOLVER_UTILS_H
#define GRAPH_CONSTRAINT_SOLVER_UTILS_H

#include <memory>
#include <functional>
#include <random>
#include <chrono>

namespace graph_constraint_solver {

    // TODO: may be just use testlib.h for random stuff
    class Random {
    public:
        Random();
        double next();
        int next(int n);
        int next(int l, int r);
        // function from testlib
        int wnext(int n, int weight);
    private:
        std::mt19937 rng;
    };

    extern Random random;

    double timeit(std::function<void()> f);
}

#endif //GRAPH_CONSTRAINT_SOLVER_UTILS_H
