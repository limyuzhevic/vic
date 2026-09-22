// Example usage of performance monitoring in NLM
// This file demonstrates how to integrate performance monitoring

#include "../performance/PerformanceAnalyzer.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include <iostream>

void demonstratePerformanceMonitoring() {
    std::cout << "=== NLM Performance Monitoring Demonstration ===\n\n";
    
    // Create a simple brain configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create brain with performance monitoring
    std::cout << "Creating brain...\n";
    auto brain = std::make_unique<nlm::Brain>(config);
    
    if (brain->initialize()) {
        std::cout << "Brain initialized successfully\n";
        
        // Run a performance benchmark
        std::cout << "\nRunning simulation steps (benchmarking)...\n";
        nlm::Timer simulationTimer;
        simulationTimer.start();
        
        for (nlm::SimulationStep step = 0; step < 100; ++step) {
            brain->step(step);
        }
        
        simulationTimer.stop();
        double elapsed = simulationTimer.elapsedSeconds();
        
        std::cout << "Simulation completed in " << elapsed << " seconds\n";
        std::cout << "Average time per step: " << elapsed/100.0 << " seconds\n";
        
        // Demonstrate memory tracking
        nlm::MemoryTracker& memoryTracker = nlm::MemoryTracker::getInstance();
        memoryTracker.recordMemoryUsage(1024 * 1024); // 1MB
        memoryTracker.recordMemoryUsage(2 * 1024 * 1024); // 2MB (new peak)
        
        std::cout << "\nMemory tracking:\n";
        std::cout << "Current memory: " << memoryTracker.getCurrentMemory() << " bytes\n";
        std::cout << "Peak memory: " << memoryTracker.getPeakMemory() << " bytes\n";
        
        // Show performance analysis
        std::cout << "\nPerformance Analysis:\n";
        std::cout << nlm::PerformanceAnalyzer().generateReport();
        
        // Demonstrate timer usage
        std::cout << "\nTimer demonstration:\n";
        nlm::Timer neuronTimer;
        neuronTimer.start();
        
        // Create some neurons (this would normally be done by the brain)
        for (int i = 0; i < 10; ++i) {
            nlm::Neuron neuron(nlm::NeuronId(i));
        }
        
        neuronTimer.stop();
        
        std::cout << "Created 10 neurons in " << neuronTimer.elapsedMilliseconds() 
                  << " milliseconds\n";
        
    } else {
        std::cout << "Failed to initialize brain\n";
    }
}

void demonstratePerformanceMacros() {
    std::cout << "\n=== Performance Macros Demonstration ===\n\n";
    
    // Using performance macros
    NLM_PROFILE("neuron_creation") {
        // This block's execution time will be automatically recorded
        for (int i = 0; i < 100; ++i) {
            nlm::Neuron neuron(nlm::NeuronId(i));
        }
    }
    
    std::cout << "Used NLM_PROFILE macro for automatic timing\n";
    std::cout << "Check performance report for details\n";
}

int main() {
    demonstratePerformanceMonitoring();
    demonstratePerformanceMacros();
    
    return 0;
}
