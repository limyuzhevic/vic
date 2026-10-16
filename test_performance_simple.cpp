// Simple test for PerformanceMetrics functionality
#include "src/core/PerformanceMetrics/PerformanceMetrics.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing PerformanceMetrics basic functionality..." << std::endl;
    
    // Test PerformanceMetrics default constructor
    nlm::PerformanceMetrics metrics;
    
    assert(metrics.brainInitializationTimeMs == 0.0);
    assert(metrics.totalExecutionTimeMs == 0.0);
    assert(metrics.spikeCount == 0);
    assert(metrics.synapseCount == 0);
    assert(metrics.neuronCount == 0);
    assert(metrics.activeNeuronCount == 0);
    assert(metrics.firingNeuronCount == 0);
    assert(metrics.plasticityEvents == 0);
    assert(metrics.predictionSteps == 0);
    
    std::cout << "Default constructor: PASSED" << std::endl;
    
    // Test PerformanceMonitor default constructor
    nlm::PerformanceMonitor monitor;
    
    assert(monitor.getCurrentMetrics().stepTimesMs.empty());
    assert(monitor.getCurrentMetrics().spikeCountsPerStep().empty());
    assert(monitor.getCurrentMetrics().activeNeuronCountsPerStep().empty());
    assert(monitor.getCurrentMetrics().componentTimesMs.empty());
    
    std::cout << "PerformanceMonitor default constructor: PASSED" << std::endl;
    
    // Test PerformanceMonitor recordStep
    monitor.recordStep(10.5, 50, 25);
    monitor.recordStep(15.2, 75, 30);
    
    assert(monitor.getCurrentMetrics().stepTimesMs.size() == 2);
    assert(std::abs(monitor.getCurrentMetrics().stepTimesMs[0] - 10.5) < 0.001);
    assert(std::abs(monitor.getCurrentMetrics().stepTimesMs[1] - 15.2) < 0.001);
    
    assert(monitor.getCurrentMetrics().spikeCountsPerStep().size() == 2);
    assert(monitor.getCurrentMetrics().spikeCountsPerStep()[0] == 50);
    assert(monitor.getCurrentMetrics().spikeCountsPerStep()[1] == 75);
    
    assert(monitor.getCurrentMetrics().activeNeuronCountsPerStep().size() == 2);
    assert(monitor.getCurrentMetrics().activeNeuronCountsPerStep()[0] == 25);
    assert(monitor.getCurrentMetrics().activeNeuronCountsPerStep()[1] == 30);
    
    std::cout << "PerformanceMonitor recordStep: PASSED" << std::endl;
    
    // Test PerformanceMonitor recordComponentTime
    monitor.recordComponentTime("brain_initialization", 100.0);
    monitor.recordComponentTime("brain_initialization", 150.0);
    monitor.recordComponentTime("simulation", 50.0);
    
    assert(monitor.getCurrentMetrics().componentTimesMs.find("brain_initialization") != monitor.getCurrentMetrics().componentTimesMs.end());
    assert(std::abs(monitor.getCurrentMetrics().componentTimesMs["brain_initialization"] - 125.0) < 0.001); // (100+150)/2
    
    assert(monitor.getCurrentMetrics().componentTimesMs.find("simulation") != monitor.getCurrentMetrics().componentTimesMs.end());
    assert(std::abs(monitor.getCurrentMetrics().componentTimesMs["simulation"] - 50.0) < 0.001);
    
    std::cout << "PerformanceMonitor recordComponentTime: PASSED" << std::endl;
    
    // Test PerformanceMonitor isStable
    nlm::PerformanceMonitor stableMonitor;
    
    // Add stable step times
    for (int i = 0; i < 50; ++i) {
        stableMonitor.recordStep(10.0 + (i % 3), 50, 25);
    }
    
    // Should be stable
    assert(stableMonitor.isStable(25, 0.2));
    
    // Add very unstable step times
    nlm::PerformanceMonitor unstableMonitor;
    for (int i = 0; i < 50; ++i) {
        unstableMonitor.recordStep(static_cast<double>(i), 50, 25);
    }
    
    // Should not be stable
    assert(!unstableMonitor.isStable(25, 0.2));
    
    std::cout << "PerformanceMonitor isStable: PASSED" << std::endl;
    
    // Test PerformanceMonitor reset
    nlm::PerformanceMonitor resetMonitor;
    resetMonitor.recordStep(10.0, 50, 25);
    resetMonitor.recordComponentTime("test", 25.0);
    
    assert(!resetMonitor.getStepTimes().empty());
    assert(!resetMonitor.getCurrentMetrics().componentTimesMs.empty());
    
    resetMonitor.reset();
    
    assert(resetMonitor.getStepTimes().empty());
    assert(resetMonitor.getCurrentMetrics().componentTimesMs.empty());
    
    std::cout << "PerformanceMonitor reset: PASSED" << std::endl;
    
    std::cout << "" << std::endl;
    std::cout << "=== All Performance Metrics Tests PASSED ===" << std::endl;
    return 0;
}
