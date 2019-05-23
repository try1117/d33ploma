#include "utils.h"

namespace graph_constraint_solver {
    Random random = Random();

    Random::Random()
//        : rng(std::chrono::high_resolution_clock().now().time_since_epoch().count()) {
        : rng(123456) {
    }

    double Random::next() {
        return std::uniform_real_distribution<double>(0, 1)(rng);
    }

    int Random::next(int n) {
        return next(0, n - 1);
    }

    int Random::next(int l, int r) {
        return std::uniform_int_distribution<>(l, r)(rng);
    }

    int Random::next(std::pair<int, int> bounds) {
        return next(bounds.first, bounds.second);
    }

    int Random::wnext(int n, int weight) {
        if (weight == 0) {
            return next(n);
        }
        double p;
        if (weight > 0) {
            p = std::pow(next(), 1.0 / (weight + 1));
        }
        else {
            p = 1 - std::pow(next(), 1.0 / (-weight + 1)) - 1e-10;
        }
        return int(n * p);
    }

    double timeit(std::function<void()> f) {
        auto start = std::chrono::high_resolution_clock::now();
        f();
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> res = finish - start;
        return res.count();
    };
}