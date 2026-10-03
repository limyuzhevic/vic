// Benchmark Tests for NLM
// Performance testing and profiling for neural simulation components

#include <iostream>
#include <chrono>
#include <memory>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "brain/Synapse.hpp"
#include "brain/Neuron.hpp"
#include "dynamics/SpikeSystem.hpp"

namespace benchmark {

struct BenchmarkResult {
    std::string name;
    double durationMs;
    size_t operations;
    double opsPerSecond;
    std::string unit;
    
    BenchmarkResult(const std::string& n, double duration, size_t ops, const std::string& u = "ops")
        : name(n), durationMs(duration * 1000), operations(ops), 
          opsPerSecond(ops / (duration > 0 ? duration : 1.0)), unit(u) {}
    
    void print() const {
        std::cout << std::left << std::setw(30) << name 
                  << std::right << std::setw(12) << durationMs << " ms"
                  << std::right << std::setw(15) << opsPerSecond << " " << unit << "/s"
                  << std::endl;
    }
};

class BenchmarkSuite {
public:
    std::vector<BenchmarkResult> results;
    
    void runAll() {
        std::cout << "=== NLM Benchmark Suite ===" << std::endl;
        std::cout << std::left << std::setw(30) << "Test" 
                  << std::right << std::setw(12) << "Duration (ms)"
                  << std::right << std::setw(15) << "Rate (ops/s)" << std::endl;
        std::cout << std::string(65, '-') << std::endl;
        
        runConfigBenchmarks();
        runSpikeSystemBenchmarks();
        runSynapseBenchmarks();
        runPlasticityBenchmarks();
        runBrainBenchmarks();
        
        std::cout << std::string(65, '-') << std::endl;
        std::cout << "Benchmark suite completed." << std::endl;
    }
    
protected:
    void runConfigBenchmarks() {
        auto start = std::chrono::high_resolution_clock::now();
        
        // Benchmark Config operations
        const size_t iterations = 10000;
        std::vector<std::string> keys;
        
        for (size_t i = 0; i < iterations; ++i) {
            nlm::Config config;
            for (size_t j = 0; j < 10; ++j) {
                config.set("test_key_" + std::to_string(j), 
                          static_cast<int64_t>(j * 100), 
                          nlm::ConfigSource::Runtime);
                keys.push_back("test_key_" + std::to_string(j));
            }
            
            for (const auto& key : keys) {
                config.get<int64_t>(key);
            }
            
            config.clear();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        
        results.emplace_back("Config Operations", duration.count(), iterations * 20);
    }
    
    void runSpikeSystemBenchmarks() {
        auto start = std::chrono::high_resolution_clock::now();
        
        const size_t iterations = 1000;
        std::vector<nlm::SpikeEvent> events;
        
        // Create test events
        for (size_t i = 0; i < 100; ++i) {
            events.emplace_back(nlm::NeuronId(i), 1.0, i % 1000);
        }
        
        for (size_t i = 0; i < iterations; ++i) {
            nlm::SpikeSystem spikeSystem;
            
            // Queue spikes
            for (const auto& event : events) {
                spikeSystem.queueSpike(event);
            }
            
            // Process spikes
            spikeSystem.processSpikes(i % 1000);
            
            // Reset for next iteration
            spikeSystem.reset();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        
        results.emplace_back("SpikeSystem Processing", duration.count(), iterations * events.size() * 2);
    }
    
    void runSynapseBenchmarks() {
        auto start = std::chrono::high_resolution_clock::now();
        
        const size_t iterations = 1000;
        const size_t spikesPerIteration = 50;
        
        for (size_t i = 0; i < iterations; ++i) {
            nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
            
            // Generate spike history
            std::vector<nlm::Timestamp> preSpikes, postSpikes;
            for (size_t j = 0; j < spikesPerIteration; ++j) {
                preSpikes.push_back(static_cast<nlm::Timestamp>(j * 10.0));
                postSpikes.push_back(static_cast<nlm::Timestamp>(j * 10.0 + 5.0));
            }
            
            // Record spikes
            for (const auto& spike : preSpikes) {
                synapse.recordPreSpike(spike);
            }
            for (const auto& spike : postSpikes) {
                synapse.recordPostSpike(spike);
            }
            
            // Update synapse
            synapse.step(static_cast<nlm::Timestamp>(i * 1000.0));
            
            // Clear history
            synapse.clearHistory();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        
        results.emplace_back("Synapse Operations", duration.count(), iterations * (spikesPerIteration * 2 + 2));
    }
    
    void runPlasticityBenchmarks() {
        auto start = std::chrono::high_resolution_clock::now();
        
        const size_t iterations = 1000;
        
        for (size_t i = 0; i < iterations; ++i) {
            nlm::STDP stdp;
            nlm::Hebbian hebbian;
            
            // Create test synapse
            nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
            synapse.setWeight(0.5f);
            
            // Generate spike history
            std::vector<nlm::Timestamp> preSpikes, postSpikes;
            for (size_t j = 0; j < 20; ++j) {
                preSpikes.push_back(static_cast<nlm::Timestamp>(j * 10.0));
                postSpikes.push_back(static_cast<nlm::Timestamp>(j * 10.0 + 5.0));
            }
            
            // Update with STDP
            stdp.update(&synapse, preSpikes, postSpikes, 0.001);
            
            // Update with Hebbian
            hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
            
            // Reset synapse
            synapse.clearHistory();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        
        results.emplace_back("Plasticity Operations", duration.count(), iterations * 3);
    }
    
    void runBrainBenchmarks() {
        auto start = std::chrono::high_resolution_clock::now();
        
        const size_t iterations = 10;
        const size_t neurons = 100;
        const size_t regions = 2;
        
        for (size_t i = 0; i < iterations; ++i) {
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(neurons), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(regions), nlm::ConfigSource::Default);
            config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
            
            nlm::Brain brain(config);
            
            if (brain.initialize()) {
                // Run simulation steps
                for (nlm::SimulationStep step = 0; step < 100; ++step) {
                    brain.step(step);
                }
                
                // Reset and reinitialize
                brain.reset();
                brain.initialize();
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        
        results.emplace_back("Brain Simulation", duration.count(), iterations * 100);
    }
};

void runBenchmarkSuite() {
    BenchmarkSuite suite;
    suite.runAll();
    
    std::cout << std::endl;
    std::cout << "=== Summary ===" << std::endl;
    double totalDuration = 0.0;
    size_t totalOperations = 0;
    
    for (const auto& result : suite.results) {
        std::cout << std::left << std::setw(30) << result.name 
                  << std::right << std::setw(12) << result.durationMs << " ms"
                  << std::right << std::setw(15) << result.opsPerSecond << " " << result.unit << "/s"
                  << std::endl;
        totalDuration += result.durationMs / 1000.0;
        totalOperations += result.operations;
    }
    
    std::cout << std::string(65, '-') << std::endl;
    std::cout << std::left << std::setw(30) << "Total" 
              << std::right << std::setw(12) << totalDuration * 1000 << " ms"
              << std::right << std::setw(15) << totalOperations / totalDuration << " " << suite.results[0].unit << "/s"
              << std::endl;
    
    std::cout << std::endl;
    std::cout << "Performance Recommendations:" << std::endl;
    std::cout << "- For large-scale simulations (1M+ neurons): Consider batch processing" << std::endl;
    std::cout << "- For real-time applications: Optimize spike event handling" << std::endl;
    std::cout << "- For memory-constrained environments: Use Config.saveToFile() with sparse representation" << std::endl;
}

} // namespace benchmark

// Main function for standalone benchmark execution
int main() {
    std::cout << "NLM Benchmark Suite" << std::endl;
    std::cout << "====================" << std::endl;
    std::cout << "Running performance benchmarks for neural simulation components." << std::endl;
    std::cout << std::endl;
    
    benchmark::runBenchmarkSuite();
    
    return 0;
}
