#include "PerformanceMonitor.hpp"
#include <iostream>
#include <thread>
#include <chrono>

using namespace nlm;

int main() {
    std::cout << "=== Performance Monitor Example ===" << std::endl << std::endl;
    
    // Create a performance monitor
    PerformanceMonitor monitor;
    
    // Start monitoring
    monitor.startMonitoring();
    
    std::cout << "Monitoring started. Simulating neural activity..." << std::endl;
    
    // Simulate some neural activity
    for (int step = 0; step < 50; ++step) {
        // Create performance metrics
        PerformanceMetrics metrics;
        
        // Simulate step-dependent metrics
        metrics.wallClockTime = 0.001;
        metrics.simulationTime = 0.001;
        metrics.firingRate = 100.0f + static_cast<float>(rand() % 200); // 100-300 Hz
        metrics.firingNeuronCount = static_cast<size_t>(metrics.firingRate * 0.01); // 1% of neurons
        metrics.totalSpikeCount = metrics.firingNeuronCount * 10; // 10 spikes per firing neuron
        metrics.workingMemorySize = 10 + static_cast<size_t>(rand() % 20); // 10-30
        metrics.episodicMemoryCount = 5 + static_cast<size_t>(rand() % 10); // 5-15
        metrics.associativeMemorySize = 20 + static_cast<size_t>(rand() % 30); // 20-50
        metrics.dopamineLevel = 0.1f + static_cast<float>(rand() % 900) / 1000.0f; // 0.1-1.0
        metrics.noveltyLevel = static_cast<float>(rand() % 1000) / 1000.0f; // 0.0-1.0
        metrics.curiosityLevel = static_cast<float>(rand() % 1000) / 1000.0f; // 0.0-1.0
        metrics.predictionError = static_cast<float>(rand() % 500) / 1000.0f; // 0.0-0.5
        metrics.learningRate = 0.001f + static_cast<float>(rand() % 999) / 1000000.0f; // 0.001-0.001999
        metrics.plasticityChange = static_cast<float>(rand() % 500) / 1000.0f; // 0.0-0.5
        metrics.reward = static_cast<float>(rand() % 200) / 100.0f - 1.0f; // -1.0 to 1.0
        metrics.energy = 100.0f - static_cast<float>(step) * 0.1f; // Decreasing over time
        metrics.actionCount = 1 + static_cast<uint64_t>(rand() % 5); // 1-6 actions
        
        // Record the metrics
        monitor.recordStep(metrics);
        
        // Print progress every 10 steps
        if (step % 10 == 0) {
            std::cout << "Step " << step << ": Firing Rate = " << metrics.firingRate 
                      << " Hz, Reward = " << metrics.reward << std::endl;
        }
        
        // Simulate step time
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    // Stop monitoring
    monitor.stopMonitoring();
    
    std::cout << std::endl << "=== Performance Summary ===" << std::endl;
    std::cout << monitor.getSummary() << std::endl;
    
    std::cout << "=== Detailed Statistics ===" << std::endl;
    std::cout << monitor.getDetailedStats() << std::endl;
    
    std::cout << "=== Learning Progress ===" << std::endl;
    float learningProgress = monitor.calculateLearningProgress(20);
    std::cout << "Learning Progress (last 20 steps): " << learningProgress << std::endl;
    
    if (learningProgress > 0.0f) {
        std::cout << "Positive learning progress!" << std::endl;
    } else if (learningProgress < 0.0f) {
        std::cout << "Negative learning progress! Consider adjusting parameters." << std::endl;
    } else {
        std::cout << "Learning progress is neutral." << std::endl;
    }
    
    return 0;
}
