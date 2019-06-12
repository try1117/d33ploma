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
        std::mt19937 rng();
        double next();
        size_t next(size_t n);
        int next(int n);
        int next(int l, int r);
        int next(std::pair<int, int> bounds);

        long long next(long long n);
        long long next(long long l, long long r);
        long long next(std::pair<long long, long long> bounds);
        // function from testlib
        int wnext(int n, int weight);
    private:
        std::mt19937 rng_;
    };

    extern Random random;

    class Utils {
    public:
        using ll = long long;
        using IntPair = std::pair<int, int>;
        using LongPair = std::pair<long long, long long>;

        static double timeit(std::function<void()> f);
        static bool in_range(ll left_bound, ll value, ll right_bound);
        static bool in_range(LongPair bounds, ll value);
        static ll complete_graph_size(ll order);

        static bool invalid_segment(LongPair segment);
        static bool non_empty_segments_intersection(ll l1, ll r1, ll l2, ll r2);
        static bool non_empty_segments_intersection(LongPair p1, LongPair p2);
        static LongPair segments_intersection(ll l1, ll r1, ll l2, ll r2);
        static LongPair segments_intersection(LongPair p1, LongPair p2);

        static void assert_segment_inside(ll outer_left, ll outer_right, ll inner_left, ll inner_right,
                std::string exception_prefix);

        static void assert_value_inside(ll outer_left, ll outer_right, ll inner_value,
                std::string exception_prefix);

        static std::string segment_to_string(ll left_bound, ll right_bound);
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
