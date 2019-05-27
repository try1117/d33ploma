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
        int next(std::pair<int, int> bounds);
        // function from testlib
        int wnext(int n, int weight);
    private:
        std::mt19937 rng;
    };

    extern Random random;

    class Utils {
    public:
        static double timeit(std::function<void()> f);
        static bool in_range(long long left_bound, long long value, long long right_bound);
        static bool in_range(std::pair<long long, long long> bounds, long long value);
        static long long complete_graph_size(long long order);
    };
}

#endif //GRAPH_CONSTRAINT_SOLVER_UTILS_H
