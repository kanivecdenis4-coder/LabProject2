#pragma once

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <thread>
#include <utility>
#include <vector>

#if __has_include(<execution>)
#include <execution>
#define LAB_HAS_EXECUTION 1
#else
#define LAB_HAS_EXECUTION 0
#endif

namespace algo {

    
    inline bool should_replace(int x) { return x % 7 == 0 || x % 11 == 0; }

    constexpr int kNewValue = -1;

    
    inline void replace_plain(std::vector<int>& v) {
        std::replace_if(v.begin(), v.end(), should_replace, kNewValue);
    }

#if LAB_HAS_EXECUTION
    
    template <class Policy>
    void replace_with_policy(std::vector<int>& v, Policy&& policy) {
        std::replace_if(std::forward<Policy>(policy), v.begin(), v.end(), should_replace, kNewValue);
    }
#endif

    
    template <class It, class Pred, class T>
    void parallel_replace_if(It first, It last, Pred pred, const T& new_value, std::size_t k) {
        const auto n = static_cast<std::size_t>(std::distance(first, last));
        if (n == 0) return;
        if (k == 0) k = 1;
        if (k > n) k = n;

        const std::size_t base = n / k;
        const std::size_t rest = n % k;

        std::vector<std::pair<It, It>> parts;
        parts.reserve(k);
        It cur = first;
        for (std::size_t i = 0; i < k; ++i) {
            It next = cur + static_cast<std::ptrdiff_t>(base + (i < rest ? 1 : 0));
            parts.emplace_back(cur, next);
            cur = next;
        }

        std::vector<std::thread> threads;
        threads.reserve(k - 1);
        for (std::size_t i = 1; i < k; ++i) {
            threads.emplace_back([&parts, i, &pred, &new_value] {
                std::replace_if(parts[i].first, parts[i].second, pred, new_value);
                });
        }
        
        std::replace_if(parts[0].first, parts[0].second, pred, new_value);
        for (auto& t : threads) t.join();
    }

    inline void replace_own(std::vector<int>& v, std::size_t k) {
        parallel_replace_if(v.begin(), v.end(), should_replace, kNewValue, k);
    }

}