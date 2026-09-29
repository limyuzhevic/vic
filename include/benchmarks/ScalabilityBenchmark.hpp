#pragma once

#include "NLMBenchmark.hpp"
#include "core/Config/Config.hpp"

namespace nlm {
namespace benchmarks {

class ScalabilityBenchmark {
public:
    struct ScaleTestConfig {
        std::vector<size_t> neuronCounts;
        std::vector<size_t> regionCounts;
        float connectionProbability = 0.1f;
        uint64_t simulationSteps = 1000;
        std::string checkpointPath = "/tmp/nlm_checkpoint.bin";
    };
    
    static ScalabilityMetrics benchmarkScaling(const ScaleTestConfig& config);
    
    static std::vector<double> benchmarkNeuronScaling(const ScaleTestConfig& config);
    static std::vector<double> benchmarkRegionScaling(const ScaleTestConfig& config);
    static std::vector<double> benchmarkConnectionScaling(const ScaleTestConfig& config);
    static std::vector<double> benchmarkMemoryScaling(const ScaleTestConfig& config);
};

} // namespace benchmarks
} // namespace nlm