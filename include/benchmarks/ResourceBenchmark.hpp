#pragma once

#include "NLMBenchmark.hpp"
#include "brain/Brain.hpp"

namespace nlm {
namespace benchmarks {

class ResourceBenchmark {
public:
    static double benchmarkMemoryEfficiency(std::shared_ptr<Brain> brain, uint64_t steps);
    static size_t benchmarkMemoryFootprint(std::shared_ptr<Brain> brain);
    static double benchmarkCPUEfficiency(std::shared_ptr<Brain> brain, uint64_t steps);
    static double benchmarkCPUUtilization(std::shared_ptr<Brain> brain, uint64_t steps);
    static double benchmarkCheckpointEfficiency(std::shared_ptr<Brain> brain, const std::string& path);
    static double benchmarkMemoryLoadEfficiency(std::shared_ptr<Brain> brain, const std::string& path);
};

} // namespace benchmarks
} // namespace nlm