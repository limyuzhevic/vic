#pragma once

/**
 * Performance benchmarks framework
 */
#include <functional>
#include <string>
#include <vector>

namespace nlm {
namespace testing {

struct BenchmarkResult {
    std::string name;
    double duration;
    size_t iterations;
    std::string unit;
};

class BenchmarkSuite {
public:
    void registerBenchmark(std::function<void()> func, const std::string& name);
    void runBenchmarks();
    std::vector<BenchmarkResult> getResults() const;
};

} // namespace testing
} // namespace nlm