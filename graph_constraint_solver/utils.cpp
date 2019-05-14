#include "utils.h"

namespace graph_constraint_solver {
    int Random::next(int n) {
        static std::mt19937 rng(123456);
        return rng() % n;
    }

    double timeit(std::function<void()> f) {
        auto start = std::chrono::high_resolution_clock::now();
        f();
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> res = finish - start;
        return res.count();
    };
}