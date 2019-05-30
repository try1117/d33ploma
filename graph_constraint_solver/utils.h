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
        static bool non_empty_segments_intersection(long long l1, long long r1, long long l2, long long r2);

        static void assert_segment_inside(long long outer_left, long long outer_right,
                long long inner_left, long long inner_right, std::string exception_prefix);

        static void assert_value_inside(long long outer_left, long long outer_right,
                long long inner_value, std::string exception_text);

        static std::string segment_to_string(long long left_bound, long long right_bound);
    };

    // TODO: maybe use boost ???
    // left[i], right[i] means whole segment is of color left[i]
    // works only if we unite consecutive elements
    // we use this in tree generation
    class DSU {
    public:
        DSU(int n, bool only_consecutive_unions = false);
        int get_parent(int v);
        bool unite(int follower, int leader);
        std::pair<int, int> get_segment(int v);

    private:
        int n_;
        bool only_consecutive_unions_;
        std::vector<int> parent_, size_, left_, right_;
    };
}

#endif //GRAPH_CONSTRAINT_SOLVER_UTILS_H
