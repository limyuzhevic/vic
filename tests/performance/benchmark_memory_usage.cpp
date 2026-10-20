// Memory usage benchmark for NLM
// Tests memory consumption patterns and efficiency

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>
#include <string>

namespace test_memory_usage {

struct MemoryUsageResult {
    int neuron_count;
    size_t memory_usage_kb;
    double memory_efficiency;
    double memory_per_neuron_kb;
};

class MemoryUsageTester {
public:
    std::vector<MemoryUsageResult> runMemoryUsageTests() {
        std::cout << "=== Memory Usage Testing ===" << std::endl;
        
        std::vector<int> neuron_counts = {100, 500, 1000, 2000, 5000};
        std::vector<MemoryUsageResult> results;
        
        for (int neuron_count : neuron_counts) {
            std::cout << "\nTesting memory usage with " << neuron_count << " neurons..." << std::endl;
            
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(neuron_count), nlm::ConfigSource::Default);
            
            nlm::Brain brain(config);
            assert(brain.initialize());
            
            PerformanceMetrics metrics;
            nlm::SimulationStep step = 0;
            
            // Run simulation to get memory usage
            for (int i = 0; i < 100; ++i) {
                brain.step(step + i);
            }
            
            metrics.end();
            
            MemoryUsageResult result;
            result.neuron_count = neuron_count;
            result.memory_usage_kb = metrics.memory_usage_kb;
            result.memory_efficiency = 1.0 / (1.0 + result.memory_usage_kb / 1000.0); // Efficiency metric
            result.memory_per_neuron_kb = static_cast<double>(result.memory_usage_kb) / neuron_count;
            
            results.push_back(result);
            
            std::cout << "  Memory usage: " << result.memory_usage_kb << " KB" << std::endl;
            std::cout << "  Memory per neuron: " << result.memory_per_neuron_kb << " KB" << std::endl;
            std::cout << "  Memory efficiency: " << result.memory_efficiency << std::endl;
        }
        
        return results;
    }
    
    void generateMemoryUsageReport(const std::vector<MemoryUsageResult>& results) {
        std::cout << "\n=== Memory Usage Benchmark Results ===" << std::endl;
        std::cout << std::left << std::setw(15) << "Neurons" 
                 << std::setw(20) << "Memory (KB)" 
                 << std::setw(25) << "Memory/Neuron (KB)" 
                 << std::setw(20) << "Efficiency" << std::endl;
        std::cout << std::string(85, '-') << std::endl;
        
        for (const auto& result : results) {
            std::cout << std::left << std::setw(15) << result.neuron_count
                     << std::setw(20) << result.memory_usage_kb
                     << std::setw(25) << std::fixed << std::setprecision(2) << result.memory_per_neuron_kb
                     << std::setw(20) << std::fixed << std::setprecision(4) << result.memory_efficiency << std::endl;
        }
    }
};

void runMemoryUsageBenchmarks() {
    MemoryUsageTester tester;
    auto results = tester.runMemoryUsageTests();
    tester.generateMemoryUsageReport(results);
}

} // namespace test_memory_usage
