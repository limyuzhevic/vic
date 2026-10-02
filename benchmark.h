// Performance Benchmark Suite for NLM Project
// Measures key performance metrics before/after optimizations

#include <chrono>
#include <iostream>
#include <vector>
#include <random>
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Config/Config.hpp"

namespace nlm {
namespace perf {

class BenchmarkSuite {
public:
    struct BenchmarkResults {
        double neuronStepTimeUs;
        double synapseStepTimeUs;
        double configGetTimeUs;
        double totalTimeMs;
        size_t neuronsProcessed;
        size_t synapsesProcessed;
        size_t configOperations;
    };
    
    static BenchmarkResults runNeuronsBenchmark(size_t neuronCount, size_t steps) {
        BenchmarkResults results;
        results.neuronsProcessed = neuronCount;
        results.synapsesProcessed = 0;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Create neurons
        std::vector<Neuron> neurons;
        neurons.reserve(neuronCount);
        for (size_t i = 0; i < neuronCount; ++i) {
            neurons.emplace_back(NeuronId(i));
            neurons[i].setMembranePotential(-70.0f);
            neurons[i].setThreshold(-55.0f);
            neurons[i].setRestingPotential(-70.0f);
            neurons[i].setResetPotential(-75.0f);
            neurons[i].setFiringState(FiringState::Resting);
            neurons[i].setRefractoryPeriod(2);
        }
        
        // Benchmark stepLIF operations
        auto stepStart = std::chrono::high_resolution_clock::now();
        for (size_t step = 0; step < steps; ++step) {
            for (auto& neuron : neurons) {
                neuron.stepLIF(static_cast<Timestamp>(step), 0.001f);
            }
        }
        auto stepEnd = std::chrono::high_resolution_clock::now();
        
        results.neuronStepTimeUs = 
            static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(stepEnd - stepStart).count()) / (neuronCount * steps) * 1000.0;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        results.totalTimeMs = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count());
        
        return results;
    }
    
    static BenchmarkResults runSynapsesBenchmark(size_t synapseCount, size_t steps) {
        BenchmarkResults results;
        results.neuronsProcessed = 0;
        results.synapsesProcessed = synapseCount;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Create synapses
        std::vector<Synapse> synapses;
        synapses.reserve(synapseCount);
        for (size_t i = 0; i < synapseCount; ++i) {
            SynapseId source(i % 100);
            SynapseId dest((i + 1) % 100);
            synapses.emplace_back(source, dest);
            synapses[i].setWeight(0.1f * ((i % 2 == 0) ? 1.0f : -1.0f));
            synapses[i].setType((i % 2 == 0) ? SynapseType::Excitatory : SynapseType::Inhibitory);
        }
        
        // Benchmark synapse step operations
        auto stepStart = std::chrono::high_resolution_clock::now();
        for (size_t step = 0; step < steps; ++step) {
            for (auto& synapse : synapses) {
                synapse.step(static_cast<Timestamp>(step));
            }
        }
        auto stepEnd = std::chrono::high_resolution_clock::now();
        
        results.synapseStepTimeUs = 
            static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(stepEnd - stepStart).count()) / (synapseCount * steps) * 1000.0;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        results.totalTimeMs = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count());
        
        return results;
    }
    
    static BenchmarkResults runConfigBenchmark(size_t operationCount) {
        BenchmarkResults results;
        results.neuronsProcessed = 0;
        results.synapsesProcessed = 0;
        results.configOperations = operationCount;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Create and populate config
        Config config;
        for (size_t i = 0; i < operationCount; ++i) {
            std::string key = "param_" + std::to_string(i);
            config.set(key, 42.0); // Set integer values for testing
        }
        
        // Benchmark config get operations
        auto getStart = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < operationCount; ++i) {
            std::string key = "param_" + std::to_string(i);
            auto val = config.get<double>(key);
            (void)val; // Avoid unused variable warning
        }
        auto getEnd = std::chrono::high_resolution_clock::now();
        
        results.configGetTimeUs = 
            static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(getEnd - getStart).count()) / operationCount * 1000.0;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        results.totalTimeMs = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count());
        
        return results;
    }
    
    static void printResults(const BenchmarkResults& results) {
        std::cout << "=== NLM Performance Benchmark Results ===" << std::endl;
        std::cout << "Neuron step time: " << results.neuronStepTimeUs << " us per step" << std::endl;
        std::cout << "Synapse step time: " << results.synapseStepTimeUs << " us per step" << std::endl;
        std::cout << "Config get time: " << results.configGetTimeUs << " us per operation" << std::endl;
        std::cout << "Total benchmark time: " << results.totalTimeMs << " ms" << std::endl;
        std::cout << "Neurons processed: " << results.neuronsProcessed << std::endl;
        std::cout << "Synapses processed: " << results.synapsesProcessed << std::endl;
        std::cout << "Config operations: " << results.configOperations << std::endl;
        std::cout << "==========================================" << std::endl;
    }
};

} // namespace perf
} // namespace nlm
