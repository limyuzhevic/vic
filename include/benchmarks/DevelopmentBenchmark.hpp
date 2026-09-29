#pragma once

#include "NLMBenchmark.hpp"
#include "brain/Brain.hpp"

namespace nlm {
namespace benchmarks {

class DevelopmentBenchmark {
public:
    static double benchmarkLearning(std::shared_ptr<Brain> brain, uint64_t steps);
    static double benchmarkDevelopment(std::shared_ptr<Brain> brain, uint64_t steps);
    static double benchmarkMemoryConsolidation(std::shared_ptr<Brain> brain, uint64_t steps);
    static double benchmarkMemoryReplay(std::shared_ptr<Brain> brain, uint64_t steps);
};

} // namespace benchmarks
} // namespace nlm