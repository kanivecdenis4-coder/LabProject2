// Лабораторная работа: дослідження алгоритма replace_if (варант 8).
// Язик: C++20. Компілятор: с++ (GCC) 13 / MSVC 2022 ( C++20/23).
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include "algorithms.hpp"
#include "benchmark.hpp"
#include "sequence_io.hpp"

namespace {

const std::vector<std::size_t> kSizes = {100'000, 1'000'000, 10'000'000};
constexpr int kRepeats = 7;

std::string file_name(const std::string& dir, std::size_t n) {
    return dir + "/seq_" + std::to_string(n) + ".txt";
}

void print_row(const std::string& name, const bench::Result& r) {
    std::cout << "  " << std::left << std::setw(28) << name << std::right << std::setw(10)
              << std::fixed << std::setprecision(3) << r.ms << " ms   "
              << (r.correct ? "OK" : "WRONG RESULT") << '\n';
}

std::vector<std::size_t> thread_counts(std::size_t hw) {
    std::set<std::size_t> s = {1, 2, 3, 4, 6, 8, 12, 16, 32, 64, hw, 2 * hw, 4 * hw};
    return {s.begin(), s.end()};
}

void run_policies(const std::vector<int>& src, const std::vector<int>& ref) {
    std::cout << " [1-2] Library std::replace_if\n";
    print_row("no policy", bench::measure(src, ref, kRepeats, algo::replace_plain));
#if LAB_HAS_EXECUTION
    print_row("execution::seq", bench::measure(src, ref, kRepeats, [](std::vector<int>& v) {
                  algo::replace_with_policy(v, std::execution::seq);
              }));
    print_row("execution::par", bench::measure(src, ref, kRepeats, [](std::vector<int>& v) {
                  algo::replace_with_policy(v, std::execution::par);
              }));
    print_row("execution::par_unseq", bench::measure(src, ref, kRepeats, [](std::vector<int>& v) {
                  algo::replace_with_policy(v, std::execution::par_unseq);
              }));
    print_row("execution::unseq", bench::measure(src, ref, kRepeats, [](std::vector<int>& v) {
                  algo::replace_with_policy(v, std::execution::unseq);
              }));
#else
    std::cout << "  <execution> is not available in this compiler\n";
#endif
}

void run_own(const std::vector<int>& src, const std::vector<int>& ref, std::size_t hw) {
    std::cout << " [3] Own parallel algorithm (K parts = K threads)\n";
    std::cout << "  " << std::left << std::setw(8) << "K" << std::right << std::setw(12) << "time, ms"
              << std::setw(12) << "speedup" << "  check\n";

    const auto base = bench::measure(src, ref, kRepeats, [](std::vector<int>& v) { algo::replace_own(v, 1); });
    std::size_t best_k = 1;
    double best_ms = base.ms;
    for (std::size_t k : thread_counts(hw)) {
        const auto r = bench::measure(src, ref, kRepeats, [k](std::vector<int>& v) { algo::replace_own(v, k); });
        std::cout << "  " << std::left << std::setw(8) << k << std::right << std::setw(12) << std::fixed
                  << std::setprecision(3) << r.ms << std::setw(12) << std::setprecision(2) << base.ms / r.ms
                  << "  " << (r.correct ? "OK" : "WRONG RESULT") << '\n';
        if (r.ms < best_ms) {
            best_ms = r.ms;
            best_k = k;
        }
    }
    std::cout << "  >>> Best K = " << best_k << " (" << std::setprecision(3) << best_ms << " ms), "
              << "hardware threads = " << hw << ", K / threads = " << std::setprecision(2)
              << static_cast<double>(best_k) / static_cast<double>(hw) << "\n";
}

int cmd_generate(const std::string& dir) {
    std::filesystem::create_directories(dir);
    unsigned seed = 2024;
    for (std::size_t n : kSizes) {
        seqio::save(file_name(dir, n), seqio::generate(n, seed++));
        std::cout << "Created " << file_name(dir, n) << '\n';
    }
    return 0;
}

int cmd_run(const std::string& dir, const std::string& label) {
    const std::size_t hw = std::max(1u, std::thread::hardware_concurrency());
#ifdef NDEBUG
    const char* mode = "Release (NDEBUG)";
#else
    const char* mode = "Debug";
#endif
    std::cout << "Variant 8: replace_if | build: " << mode << " | label: " << label
              << " | hardware threads: " << hw << " | repeats: " << kRepeats << " (median)\n";

    for (std::size_t n : kSizes) {
        const auto src = seqio::load(file_name(dir, n));
        auto ref = src;
        algo::replace_plain(ref);  

        std::cout << "\n=== N = " << src.size() << " ===\n";
        run_policies(src, ref);
        run_own(src, ref, hw);
    }
    return 0;
}

}  

int main(int argc, char** argv) {
    try {
        const std::string cmd = argc > 1 ? argv[1] : "run";
        const std::string dir = argc > 2 ? argv[2] : "data";
        const std::string label = argc > 3 ? argv[3] : "-";
        if (cmd == "generate") return cmd_generate(dir);
        if (cmd == "run") return cmd_run(dir, label);
        std::cerr << "Usage: lab generate [dir] | lab run [dir] [label]\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}