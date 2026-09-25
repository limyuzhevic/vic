// Benchmark.hpp
// Performance benchmarking for NLM

#pragma once

#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

class Benchmark {
public:
    struct Result {
        std::string name;
        double timeMs;
        size_t iterations;
        double averageTimeMs;
    };
    
    Benchmark();
    ~Benchmark();
    
    static Result benchmark(std::function<void()> func, size_t iterations);
    static void saveResults(const std::vector<Result>& results, const std::string& filename);
    
private:
    static std::string getCurrentTimestamp();
};

}} // namespace nlm