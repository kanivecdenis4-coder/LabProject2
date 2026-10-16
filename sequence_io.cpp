#include "sequence_io.hpp"

#include <fstream>
#include <random>
#include <stdexcept>

namespace seqio {

    std::vector<int> generate(std::size_t n, unsigned seed) {
        std::mt19937 gen(seed);
        std::uniform_int_distribution<int> dist(0, 1'000'000);
        std::vector<int> v(n);
        for (auto& x : v) x = dist(gen);
        return v;
    }

    void save(const std::string& path, const std::vector<int>& data) {
        std::ofstream out(path);
        if (!out) throw std::runtime_error("Cannot open file for writing: " + path);
        out << data.size() << '\n';
        for (std::size_t i = 0; i < data.size(); ++i) {
            out << data[i] << (i + 1 == data.size() ? '\n' : ' ');
        }
    }

    std::vector<int> load(const std::string& path) {
        std::ifstream in(path);
        if (!in) throw std::runtime_error("Cannot open file for reading: " + path);
        std::size_t n = 0;
        if (!(in >> n)) throw std::runtime_error("Bad file format: " + path);
        std::vector<int> v(n);
        for (auto& x : v) {
            if (!(in >> x)) throw std::runtime_error("Unexpected end of file: " + path);
        }
        return v;
    }

}  // namespace seqio