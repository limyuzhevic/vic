// NLM Performance Monitoring Demo
// Demonstrates the new performance monitoring and profiling capabilities

#include "src/core/PerformanceMetrics/PerformanceMetrics.hpp"
#include <iostream>
#include <chrono>
#include <thread>

int main() {
    std::cout << "=== NLM Performance Monitoring Demo ===" << std::endl;
    std::cout << "This demo shows the new performance monitoring capabilities." << std::endl;
    std::cout << std::endl;
    
    // Create a performance monitor
    nlm::PerformanceMonitor monitor;
    
    // Start monitoring
    monitor.start();
    
    std::cout << "Performance monitoring started." << std::endl;
    std::cout << std::endl;
    
    // Simulate some work
    std::cout << "Simulating neural computation..." << std::endl;
    
    // Record some steps with varying performance
    for (int step = 0; step < 10; ++step) {
        // Simulate work with varying time
        double stepTime = 10.0 + (step % 5) * 2.5;
        int spikes = 50 + (step % 3) * 20;
        int activeNeurons = 25 + (step % 2) * 15;
        
        monitor.recordStep(stepTime, spikes, activeNeurons);
        
        // Record component times
        monitor.recordComponentTime("brain_initialization", 5.0 + step);
        monitor.recordComponentTime("simulation", stepTime * 0.8);
        monitor.recordComponentTime("plasticity", stepTime * 0.2);
        
        // Record plasticity and prediction times
        if (step % 3 == 0) {
            monitor.recordPlasticityTime(stepTime * 0.5, spikes / 10);
        }
        
        if (step % 4 == 0) {
            monitor.recordPredictionTime(stepTime * 0.3);
        }
        
        std::cout << "  Step " << step << ": " << stepTime << "ms, "
                  << spikes << " spikes, " << activeNeurons << " active neurons" << std::endl;
        
        // Simulate some work
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // Stop monitoring
    monitor.stop();
    
    std::cout << std::endl;
    std::cout << "Performance monitoring stopped." << std::endl;
    std::cout << std::endl;
    
    // Get performance metrics
    auto metrics = monitor.getCurrentMetrics();
    
    std::cout << "=== Performance Metrics Summary ===" << std::endl;
    std::cout << std::endl;
    
    // Show human-readable summary
    std::cout << metrics.toHumanReadable() << std::endl;
    
    // Show JSON output
    std::cout << "=== JSON Output ===" << std::endl;
    std::cout << metrics.toJSON() << std::endl;
    
    // Show CSV output
    std::cout << "=== CSV Output ===" << std::endl;
    std::cout << metrics.toCSV() << std::endl;
    
    // Check stability
    if (monitor.isStable(5, 0.5)) {
        std::cout << "Performance is stable." << std::endl;
    } else {
        std::cout << "Performance is unstable." << std::endl;
    }
    
    // Test output functions (these would normally write to files)
    std::cout << "=== Output File Generation ===" << std::endl;
    std::cout << "To generate output files, use the CLI options:" << std::endl;
    std::cout << "  --perf-metrics    Enable performance metrics output" << std::endl;
    std::cout << "  --profile         Enable detailed profiling" << std::endl;
    std::cout << "  --benchmark       Run performance benchmarking" << std::endl;
    std::cout << "  --compare-configs Compare different configurations" << std::endl;
    std::cout << std::endl;
    std::cout << "Output files would be generated as:" << std::endl;
    std::cout << "  metrics.json          (JSON format)" << std::endl;
    std::cout << "  metrics.human.txt     (Human-readable)" << std::endl;
    std::cout << "  metrics.csv           (CSV format for analysis)" << std::endl;
    std::cout << "  profile.json          (Profile data)" << std::endl;
    std::cout << "  comparison.json       (Configuration comparison)" << std::endl;
    
    return 0;
}
