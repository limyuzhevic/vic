// Main benchmark executable
#include "benchmark.h"
#include <iostream>

int main() {
    std::cout << "NLM Performance Benchmark Suite" << std::endl;
    std::cout << "Running benchmarks..." << std::endl;
    
    // Run benchmarks with moderate scale
    const size_t neuronCount = 1000;
    const size_t synapseCount = 5000;
    const size_t steps = 100;
    const size_t configOps = 1000;
    
    std::cout << "\n1. Testing Neuron performance..." << std::endl;
    auto neuronResults = nlm::perf::BenchmarkSuite::runNeuronsBenchmark(neuronCount, steps);
    nlm::perf::BenchmarkSuite::printResults(neuronResults);
    
    std::cout << "\n2. Testing Synapse performance..." << std::endl;
    auto synapseResults = nlm::perf::BenchmarkSuite::runSynapsesBenchmark(synapseCount, steps);
    nlm::perf::BenchmarkSuite::printResults(synapseResults);
    
    std::cout << "\n3. Testing Config performance..." << std::endl;
    auto configResults = nlm::perf::BenchmarkSuite::runConfigBenchmark(configOps);
    nlm::perf::BenchmarkSuite::printResults(configResults);
    
    std::cout << "\nBenchmark completed successfully!" << std::endl;
    return 0;
}
