// Tutorial 6: Performance Optimization
#include "src/bench/Benchmark.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <iostream>
#include <memory>
#include <chrono>

void printPerformanceTips() {
    std::cout << "Performance Optimization Tips" << std::endl;
    std::cout << "=============================" << std::endl;
    std::cout << "1. Use appropriate neuron count for your task" << std::endl;
    std::cout << "2. Optimize connection probability" << std::endl;
    std::cout << "3. Use efficient plasticity rules" << std::endl;
    std::cout << "4. Optimize memory usage" << std::endl;
    std::cout << "5. Use parallel processing" << std::endl;
    std::cout << "6. Optimize configuration settings" << std::endl;
    std::cout << "7. Use efficient algorithms" << std::endl;
    std::cout << "8. Monitor performance regularly" << std::endl;
}

int main() {
    std::cout << "Tutorial 6: Performance Optimization" << std::endl;
    std::cout << "====================================" << std::endl;
    
    printPerformanceTips();
    
    std::cout << std::endl;
    std::cout << "Performance Comparison:" << std::endl;
    std::cout << "=======================" << std::endl;
    
    // Test with different configurations
    struct ConfigTest {
        std::string name;
        int neuronCount;
        int regionCount;
        float connectionProbability;
        int steps;
    };
    
    std::vector<ConfigTest> configs = {
        {"Small Network", 100, 1, 0.1, 1000},
        {"Medium Network", 1000, 2, 0.15, 10000},
        {"Large Network", 5000, 3, 0.2, 50000}
    };
    
    for (const auto& test : configs) {
        std::cout << std::endl;
        std::cout << "Testing " << test.name << ":" << std::endl;
        std::cout << "  Neurons: " << test.neuronCount << std::endl;
        std::cout << "  Regions: " << test.regionCount << std::endl;
        std::cout << "  Connection probability: " << test.connectionProbability << std::endl;
        std::cout << "  Steps: " << test.steps << std::endl;
        
        // Create configuration
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", test.neuronCount);
        config->set("region_count", test.regionCount);
        config->set("connection_probability", test.connectionProbability);
        config->set("max_simulation_steps", test.steps);
        
        // Create brain
        auto brain = std::make_shared<nlm::Brain>(config);
        
        // Initialize brain
        if (!brain->initialize()) {
            std::cerr << "✗ Failed to initialize brain for " << test.name << "!" << std::endl;
            continue;
        }
        
        // Measure performance
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (nlm::SimulationStep step = 0; step < test.steps; ++step) {
            brain->step(step);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << "  Time: " << duration.count() / 1000.0 << " seconds" << std::endl;
        std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Performance optimization tutorial completed successfully!" << std::endl;
    return 0;
}