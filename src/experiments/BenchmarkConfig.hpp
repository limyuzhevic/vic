// NLM Benchmark Configuration
// Standard benchmark settings for performance testing

#ifndef NLM_BENCHMARK_CONFIG_HPP
#define NLM_BENCHMARK_CONFIG_HPP

namespace nlm {

struct BenchmarkConfig {
    size_t neuronCount;
    size_t steps;
    float connectionProbability;
    float stdpLTPWeight;
    float stdpLTDWeight;
    double timestep;
    
    BenchmarkConfig(size_t neurons = 1000, size_t s = 500)
        : neuronCount(neurons), steps(s), connectionProbability(0.1f),
          stdpLTPWeight(0.01f), stdpLTDWeight(0.012f), timestep(0.001) {}
    
    void validate() const {
        if (neuronCount == 0) {
            throw std::invalid_argument("Neuron count must be positive");
        }
        if (steps == 0) {
            throw std::invalid_argument("Steps must be positive");
        }
        if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
            throw std::invalid_argument("Connection probability must be between 0 and 1");
        }
    }
    
    std::string getDescription() const {
        return "Benchmark with " + std::to_string(neuronCount) + 
               " neurons over " + std::to_string(steps) + " steps";
    }
};

} // namespace nlm

#endif // NLM_BENCHMARK_CONFIG_HPP
