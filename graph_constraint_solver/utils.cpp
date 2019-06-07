#include "utils.h"

namespace graph_constraint_solver {
    Random random = Random();

    Random::Random()
//        : rng_(std::chrono::high_resolution_clock().now().time_since_epoch().count()) {
        : rng_(1234) {
    }

    std::mt19937 Random::rng() {
        return rng_;
    }

    double Random::next() {
        return std::uniform_real_distribution<double>(0, 1)(rng_);
    }

    int Random::next(int n) {
        if (n == 0) {
            return 0;
        }
        return next(0, n - 1);
    }

    int Random::next(int l, int r) {
        return std::uniform_int_distribution<>(l, r)(rng_);
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

    bool Utils::in_range(ll left_bound, ll value, ll right_bound) {
        return left_bound <= value && value <= right_bound;
    }

    bool Utils::in_range(LongPair bounds, ll value) {
        return in_range(bounds.first, value, bounds.second);
    }

    Utils::ll Utils::complete_graph_size(ll order) {
        return order * (order - 1) / 2;
    }

    bool Utils::invalid_segment(LongPair segment) {
        return segment.first > segment.second;
    }

    bool Utils::non_empty_segments_intersection(ll l1, ll r1, ll l2, ll r2) {
        if (l1 > l2 || l1 == l2 && r1 > r2) {
            std::swap(l1, l2);
            std::swap(r1, r2);
        }
        return l2 <= r1;
    }

    bool Utils::non_empty_segments_intersection(LongPair p1, LongPair p2) {
        return non_empty_segments_intersection(p1.first, p1.second, p2.first, p2.second);
    }

    Utils::LongPair Utils::segments_intersection(ll l1, ll r1, ll l2, ll r2) {
        if (l1 > l2 || l1 == l2 && r1 > r2) {
            std::swap(l1, l2);
            std::swap(r1, r2);
        }
        return std::make_pair(l2, std::min(r1, r2));
    }

    Utils::LongPair Utils::segments_intersection(LongPair p1, LongPair p2) {
        return segments_intersection(p1.first, p1.second, p2.first, p2.second);
    }

    void Utils::assert_segment_inside(ll outer_left, ll outer_right, ll inner_left, ll inner_right,
            std::string exception_prefix) {

        if (outer_left > inner_left || inner_left > outer_right ||
            outer_left > inner_right || inner_right > outer_right) {
            throw std::invalid_argument(exception_prefix +
            " [" + std::to_string(inner_left) + ", " + std::to_string(inner_right) + "]"
            + " must lie in"
            + " [" + std::to_string(outer_left) + ", " + std::to_string(outer_right) + "]");
        }
    }


    void Utils::assert_value_inside(ll outer_left, ll outer_right, ll inner_value, std::string exception_prefix) {
        if (outer_left > inner_value  || inner_value > outer_right) {
            throw std::invalid_argument(exception_prefix + std::to_string(inner_value)
            + " must lie in"
            + " [" + std::to_string(outer_left) + ", " + std::to_string(outer_right) + "]");
        }
    }

    std::string Utils::segment_to_string(ll left_bound, ll right_bound) {
        return "[" + std::to_string(left_bound) + ", " + std::to_string(right_bound) + "]";
    }

    DSU::DSU(int n, bool only_consecutive_unions) :
        n_(n), only_consecutive_unions_(only_consecutive_unions),
        parent_(n), size_(n), left_(n), right_(n) {

        for (int i = 0; i < n; ++i) {
            parent_[i] = i;
            size_[i] = 1;
            left_[i] = i;
            right_[i] = i;
        }
    }

    int DSU::get_parent(int v) {
        return parent_[v] = v == parent_[v] ? v : get_parent(parent_[v]);
    }

    bool DSU::unite(int follower, int leader) {
        follower = get_parent(follower);
        leader = get_parent(leader);
        if (follower == leader) {
            return false;
        }
        if (only_consecutive_unions_) {
            if (left_[follower] - right_[leader] > 1 && left_[leader] - right_[follower] > 1) {
                throw std::invalid_argument("DSU error: union of non-consecutive elements, given elements: "
                + std::to_string(follower) + " -> segment " + Utils::segment_to_string(left_[follower], right_[follower])
                + " and element " + std::to_string(leader) + " -> segment " + Utils::segment_to_string(left_[leader], right_[leader]));
            }
        }
        if (!only_consecutive_unions_ && size_[follower] > size_[leader]) {
            std::swap(follower, leader);
        }
        parent_[follower] = leader;
        size_[leader] += size_[follower];
        left_[leader] = std::min(left_[leader], left_[follower]);
        right_[leader] = std::max(right_[leader], right_[follower]);
        return true;
    }

    std::pair<int, int> DSU::get_segment(int v) {
        v = get_parent(v);
        return {left_[v], right_[v]};
    }
}