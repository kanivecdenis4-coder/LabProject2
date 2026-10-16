#pragma once

#include <algorithm>
#include <chrono>
#include <vector>

namespace bench {

    struct Result {
        double ms = 0.0;      
        bool correct = false; 
    };

    template <class Fn>
    Result measure(const std::vector<int>& source, const std::vector<int>& reference,
        int repeats, Fn&& fn) {
        std::vector<double> times;
        bool ok = true;
        for (int r = 0; r < repeats; ++r) {
            std::vector<int> work = source;
            const auto t0 = std::chrono::steady_clock::now();
            fn(work);
            const auto t1 = std::chrono::steady_clock::now();
            times.push_back(std::chrono::duration<double, std::milli>(t1 - t0).count());
            ok = ok && (work == reference);
        }
        std::sort(times.begin(), times.end());
        return { times[times.size() / 2], ok };
    }

} 