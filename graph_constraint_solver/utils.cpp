#include "utils.h"

namespace graph_constraint_solver {
    Random random = Random();

    Random::Random()
//        : rng(std::chrono::high_resolution_clock().now().time_since_epoch().count()) {
        : rng(1234) {
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

    double Utils::timeit(std::function<void()> f) {
        auto start = std::chrono::high_resolution_clock::now();
        f();
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> res = finish - start;
        return res.count();
    }

    bool Utils::in_range(long long left_bound, long long value, long long right_bound) {
        return left_bound <= value && value <= right_bound;
    }

    bool Utils::in_range(std::pair<long long, long long> bounds, long long value) {
        return in_range(bounds.first, value, bounds.second);
    }

    long long Utils::complete_graph_size(long long order) {
        return order * (order - 1) / 2;
    }
}