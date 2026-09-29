#pragma once

#include "NLMBenchmark.hpp"
#include "brain/Brain.hpp"

namespace nlm {
namespace benchmarks {

class NeuralBenchmark {
public:
    static double benchmarkSpikeProcessing(std::shared_ptr<Brain> brain, uint64_t steps);
    static double benchmarkPlasticity(std::shared_ptr<Brain> brain, uint64_t steps);
    static double benchmarkWorkingMemory(std::shared_ptr<Brain> brain, uint64_t steps);
    static double benchmarkEpisodicMemory(std::shared_ptr<Brain> brain, uint64_t steps);
    static double benchmarkNeuromodulation(std::shared_ptr<Brain> brain, uint64_t steps);
    static double benchmarkSystemIntegration(std::shared_ptr<Brain> brain, uint64_t steps);
};

} // namespace benchmarks
} // namespace nlm