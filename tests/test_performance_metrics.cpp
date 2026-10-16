// NLM Performance Metrics Test
// Phase 2: Real Neural Computation Tests

#include "core/PerformanceMetrics/PerformanceMetrics.hpp"
#include <iostream>
#include <cassert>
#include <cmath>
#include <algorithm>

namespace test_performance_metrics {
    void runAll() {
        std::cout << "Performance Metrics: " << std::endl;
        
        // Test PerformanceMetrics default constructor
        nlm::PerformanceMetrics metrics;
        
        // Test initial values
        assert(metrics.brainInitializationTimeMs == 0.0);
        assert(metrics.totalExecutionTimeMs == 0.0);
        assert(metrics.spikeCount == 0);
        assert(metrics.synapseCount == 0);
        assert(metrics.neuronCount == 0);
        assert(metrics.activeNeuronCount == 0);
        assert(metrics.firingNeuronCount == 0);
        assert(metrics.plasticityEvents == 0);
        assert(metrics.predictionSteps == 0);
        assert(metrics.averageFiringRate == 0.0);
        assert(metrics.spikeDensity == 0.0);
        assert(metrics.synapticWeightMean == 0.0);
        assert(metrics.synapticWeightStdDev == 0.0);
        assert(metrics.excitatoryRatio == 0.0);
        assert(metrics.cpuUsagePercent == 0.0);
        assert(metrics.memoryUsageMB == 0);
        assert(metrics.peakMemoryMB == 0);
        assert(metrics.simulationSteps == 0);
        assert(metrics.startTime == 0);
        assert(metrics.endTime == 0);
        
        std::cout << "  - Default constructor: PASSED" << std::endl;
        
        // Test toJSON output
        std::string json = metrics.toJSON();
        assert(json.find("{\n  \"brainInitializationTimeMs\": 0.0") != std::string::npos);
        assert(json.find("  \"totalExecutionTimeMs\": 0.0") != std::string::npos);
        assert(json.find("  \"spikeCount\": 0") != std::string::npos);
        assert(json.find("  \"neuronCount\": 0") != std::string::npos);
        
        std::cout << "  - JSON output: PASSED" << std::endl;
        
        // Test toHumanReadable output
        std::string human = metrics.toHumanReadable();
        assert(human.find("Performance Metrics Summary") != std::string::npos);
        assert(human.find("Brain Initialization") != std::string::npos);
        assert(human.find("Total Execution Time") != std::string::npos);
        assert(human.find("Spike Count") != std::string::npos);
        assert(human.find("Neuron Count") != std::string::npos);
        
        std::cout << "  - Human-readable output: PASSED" << std::endl;
        
        // Test toCSV output
        std::string csv = metrics.toCSV();
        assert(csv.find("Metric,Value") != std::string::npos);
        assert(csv.find("brain_initialization_time_ms,0.000000") != std::string::npos);
        assert(csv.find("step_execution_time_ms,0.000000") != std::string::npos);
        assert(csv.find("total_execution_time_ms,0.000000") != std::string::npos);
        assert(csv.find("spike_count,0") != std::string::npos);
        assert(csv.find("neuron_count,0") != std::string::npos);
        
        std::cout << "  - CSV output: PASSED" << std::endl;
        
        // Test calculateDerivedMetrics with empty data
        metrics.calculateDerivedMetrics();
        assert(metrics.stepExecutionTimeMs == 0.0);
        assert(metrics.simulationSpeed == 0.0);
        assert(metrics.averageFiringRate == 0.0);
        assert(metrics.spikeDensity == 0.0);
        
        std::cout << "  - Calculate derived metrics: PASSED" << std::endl;
        
        // Test PerformanceMonitor default constructor
        nlm::PerformanceMonitor monitor;
        assert(monitor.getCurrentMetrics().stepTimesMs.empty());
        assert(monitor.getCurrentMetrics().spikeCountsPerStep().empty());
        assert(monitor.getCurrentMetrics().activeNeuronCountsPerStep().empty());
        assert(monitor.getCurrentMetrics().componentTimesMs.empty());
        assert(monitor.getStepTimes().empty());
        assert(monitor.getSpikeCounts().empty());
        assert(monitor.getActiveNeuronCounts().empty());
        
        std::cout << "  - PerformanceMonitor default constructor: PASSED" << std::endl;
        
        // Test PerformanceMonitor recordStep
        monitor.recordStep(10.5, 50, 25);
        monitor.recordStep(15.2, 75, 30);
        monitor.recordStep(8.8, 30, 20);
        
        assert(monitor.getCurrentMetrics().stepTimesMs.size() == 3);
        assert(std::abs(monitor.getCurrentMetrics().stepTimesMs[0] - 10.5) < 0.001);
        assert(std::abs(monitor.getCurrentMetrics().stepTimesMs[1] - 15.2) < 0.001);
        assert(std::abs(monitor.getCurrentMetrics().stepTimesMs[2] - 8.8) < 0.001);
        
        assert(monitor.getCurrentMetrics().spikeCountsPerStep().size() == 3);
        assert(monitor.getCurrentMetrics().spikeCountsPerStep()[0] == 50);
        assert(monitor.getCurrentMetrics().spikeCountsPerStep()[1] == 75);
        assert(monitor.getCurrentMetrics().spikeCountsPerStep()[2] == 30);
        
        assert(monitor.getCurrentMetrics().activeNeuronCountsPerStep().size() == 3);
        assert(monitor.getCurrentMetrics().activeNeuronCountsPerStep()[0] == 25);
        assert(monitor.getCurrentMetrics().activeNeuronCountsPerStep()[1] == 30);
        assert(monitor.getCurrentMetrics().activeNeuronCountsPerStep()[2] == 20);
        
        std::cout << "  - PerformanceMonitor recordStep: PASSED" << std::endl;
        
        // Test PerformanceMonitor recordComponentTime
        monitor.recordComponentTime("brain_initialization", 100.0);
        monitor.recordComponentTime("brain_initialization", 150.0);
        monitor.recordComponentTime("simulation", 50.0);
        monitor.recordComponentTime("plasticity", 25.0);
        
        assert(monitor.getCurrentMetrics().componentTimesMs.find("brain_initialization") != monitor.getCurrentMetrics().componentTimesMs.end());
        assert(std::abs(monitor.getCurrentMetrics().componentTimesMs["brain_initialization"] - 125.0) < 0.001); // (100+150)/2
        
        assert(monitor.getCurrentMetrics().componentTimesMs.find("simulation") != monitor.getCurrentMetrics().componentTimesMs.end());
        assert(std::abs(monitor.getCurrentMetrics().componentTimesMs["simulation"] - 50.0) < 0.001);
        
        assert(monitor.getCurrentMetrics().componentTimesMs.find("plasticity") != monitor.getCurrentMetrics().componentTimesMs.end());
        assert(std::abs(monitor.getCurrentMetrics().componentTimesMs["plasticity"] - 25.0) < 0.001);
        
        std::cout << "  - PerformanceMonitor recordComponentTime: PASSED" << std::endl;
        
        // Test PerformanceMonitor isStable
        nlm::PerformanceMonitor stableMonitor;
        
        // Add stable step times
        for (int i = 0; i < 100; ++i) {
            stableMonitor.recordStep(10.0 + (i % 5), 50, 25);
        }
        
        // Should be stable
        assert(stableMonitor.isStable(50, 0.1));
        
        // Add very unstable step times
        nlm::PerformanceMonitor unstableMonitor;
        for (int i = 0; i < 100; ++i) {
            unstableMonitor.recordStep(static_cast<double>(i), 50, 25);
        }
        
        // Should not be stable
        assert(!unstableMonitor.isStable(50, 0.1));
        
        std::cout << "  - PerformanceMonitor isStable: PASSED" << std::endl;
        
        // Test PerformanceMonitor reset
        nlm::PerformanceMonitor resetMonitor;
        resetMonitor.recordStep(10.0, 50, 25);
        resetMonitor.recordComponentTime("test", 25.0);
        
        assert(!resetMonitor.getStepTimes().empty());
        assert(!resetMonitor.getCurrentMetrics().componentTimesMs.empty());
        
        resetMonitor.reset();
        
        assert(resetMonitor.getStepTimes().empty());
        assert(resetMonitor.getCurrentMetrics().componentTimesMs.empty());
        
        std::cout << "  - PerformanceMonitor reset: PASSED" << std::endl;
        
        std::cout << "Performance Metrics Tests: All PASSED" << std::endl;
    }
}

int main() {
    std::cout << "=== NLM Performance Metrics Tests ===" << std::endl;
    std::cout << "Testing Performance Metrics Infrastructure:" << std::endl;
    std::cout << "  - Metrics collection and storage" << std::endl;
    std::cout << "  - JSON, human-readable, and CSV output formats" << std::endl;
    std::cout << "  - Component timing tracking" << std::endl;
    std::cout << "  - Simulation stability monitoring" << std::endl;
    std::cout << std::endl;
    
    try {
        test_performance_metrics::runAll();
        std::cout << std::endl;
        std::cout << "=== All Performance Metrics Tests PASSED ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== Performance Metrics Tests FAILED ===" << std::endl;
        std::cout << "Exception: " << e.what() << std::endl;
        return 1;
    }
}
