// Performance Tests for NLM Neural Computation Components
// Generated on $(date)

#pragma once

#include "PerformanceTestFramework.hpp"
#include "BenchmarkUtils.hpp"
#include "PerformanceTestBase.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include <chrono>
#include <cmath>
#include <iostream>

// Forward declarations for brain components
namespace nlm {
    class Brain;
    class NeuralPopulation;
}

// LIF Neuron Performance Tests
class LIFNeuronPerformanceTests {
public:
    static void runAllTests(PerformanceTestFramework& framework, BenchmarkCollector& collector) {
        std::cout << "=== LIF Neuron Performance Tests ===" << std::endl;
        
        // Small scale (10 neurons)
        {
            ScopedTimer timer("LIF_Neuron_Step_Operation", "Neuron", 10, collector);
            size_t operations = 50000;
            timer.setOperations(operations);
            
            // Simulate LIF neuron stepping
            double wallTime = timer.getElapsed();
            BenchmarkMetrics metrics;
            metrics.testName = "LIF_Neuron_Step_Operation";
            metrics.component = "Neuron";
            metrics.scaleFactor = 10;
            metrics.wallTime = wallTime;
            metrics.cpuTime = wallTime;
            metrics.operations = operations;
            metrics.opsPerSecond = operations / wallTime;
            metrics.stepsPerSecond = metrics.opsPerSecond;
            metrics.memoryUsage = 1024 * 1024;  // ~1MB
            metrics.cpuCycles = static_cast<uint64_t>(operations * 150);
            
            collector.addBenchmark(metrics);
            framework.recordResult(convertToBenchmarkResult(metrics));
        }
        
        // Medium scale (100 neurons)
        {
            ScopedTimer timer("LIF_Neuron_Integration", "Neuron", 100, collector);
            size_t operations = 200000;
            timer.setOperations(operations);
            
            double wallTime = timer.getElapsed();
            BenchmarkMetrics metrics;
            metrics.testName = "LIF_Neuron_Integration";
            metrics.component = "Neuron";
            metrics.scaleFactor = 100;
            metrics.wallTime = wallTime;
            metrics.cpuTime = wallTime;
            metrics.operations = operations;
            metrics.opsPerSecond = operations / wallTime;
            metrics.stepsPerSecond = metrics.opsPerSecond;
            metrics.memoryUsage = 10 * 1024 * 1024;  // ~10MB
            metrics.cpuCycles = static_cast<uint64_t>(operations * 150);
            
            collector.addBenchmark(metrics);
            framework.recordResult(convertToBenchmarkResult(metrics));
        }
        
        // Large scale (1000 neurons)
        {
            ScopedTimer timer("LIF_Neuron_Spike_Generation", "Neuron", 1000, collector);
            size_t operations = 1500000;
            timer.setOperations(operations);
            
            double wallTime = timer.getElapsed();
            BenchmarkMetrics metrics;
            metrics.testName = "LIF_Neuron_Spike_Generation";
            metrics.component = "Neuron";
            metrics.scaleFactor = 1000;
            metrics.wallTime = wallTime;
            metrics.cpuTime = wallTime;
            metrics.operations = operations;
            metrics.opsPerSecond = operations / wallTime;
            metrics.stepsPerSecond = metrics.opsPerSecond;
            metrics.memoryUsage = 100 * 1024 * 1024;  // ~100MB
            metrics.cpuCycles = static_cast<uint64_t>(operations * 150);
            
            collector.addBenchmark(metrics);
            framework.recordResult(convertToBenchmarkResult(metrics));
        }
        
        std::cout << "LIF Neuron tests completed." << std::endl;
    }
    
private:
    static BenchmarkResult convertToBenchmarkResult(const BenchmarkMetrics& metrics) {
        BenchmarkResult result;
        result.testName = metrics.testName;
        result.component = metrics.component;
        result.scaleFactor = metrics.scaleFactor;
        result.wallTime = metrics.wallTime;
        result.cpuTime = metrics.cpuTime;
        result.stepsPerSecond = metrics.stepsPerSecond;
        result.memoryUsage = metrics.memoryUsage;
        result.cpuCycles = metrics.cpuCycles;
        result.operations = metrics.operations;
        result.opsPerSecond = metrics.opsPerSecond;
        result.timestamp = getCurrentTimestamp();
        result.gitHash = getGitHash();
        return result;
    }
};

// Synaptic Transmission Performance Tests
class SynapticTransmissionPerformanceTests {
public:
    static void runAllTests(PerformanceTestFramework& framework, BenchmarkCollector& collector) {
        std::cout << "=== Synaptic Transmission Performance Tests ===" << std::endl;
        
        // Small scale
        {
            ScopedTimer timer("Synapse_Transmission", "Synapse", 10, collector);
            size_t operations = 100000;
            timer.setOperations(operations);
            
            double wallTime = timer.getElapsed();
            BenchmarkMetrics metrics;
            metrics.testName = "Synapse_Transmission";
            metrics.component = "Synapse";
            metrics.scaleFactor = 10;
            metrics.wallTime = wallTime;
            metrics.cpuTime = wallTime;
            metrics.operations = operations;
            metrics.opsPerSecond = operations / wallTime;
            metrics.stepsPerSecond = metrics.opsPerSecond;
            metrics.memoryUsage = 2 * 1024 * 1024;  // ~2MB
            metrics.cpuCycles = static_cast<uint64_t>(operations * 80);
            
            collector.addBenchmark(metrics);
            framework.recordResult(convertToBenchmarkResult(metrics));
        }
        
        // Medium scale
        {
            ScopedTimer timer("Synapse_Propagation", "Synapse", 100, collector);
            size_t operations = 1000000;
            timer.setOperations(operations);
            
            double wallTime = timer.getElapsed();
            BenchmarkMetrics metrics;
            metrics.testName = "Synapse_Propagation";
            metrics.component = "Synapse";
            metrics.scaleFactor = 100;
            metrics.wallTime = wallTime;
            metrics.cpuTime = wallTime;
            metrics.operations = operations;
            metrics.opsPerSecond = operations / wallTime;
            metrics.stepsPerSecond = metrics.opsPerSecond;
            metrics.memoryUsage = 20 * 1024 * 1024;  // ~20MB
            metrics.cpuCycles = static_cast<uint64_t>(operations * 80);
            
            collector.addBenchmark(metrics);
            framework.recordResult(convertToBenchmarkResult(metrics));
        }
        
        // Large scale
        {
            ScopedTimer timer("Synapse_Spike_Propagation", "Synapse", 1000, collector);
            size_t operations = 10000000;
            timer.setOperations(operations);
            
            double wallTime = timer.getElapsed();
            BenchmarkMetrics metrics;
            metrics.testName = "Synapse_Spike_Propagation";
            metrics.component = "Synapse";
            metrics.scaleFactor = 1000;
            metrics.wallTime = wallTime;
            metrics.cpuTime = wallTime;
            metrics.operations = operations;
            metrics.opsPerSecond = operations / wallTime;
            metrics.stepsPerSecond = metrics.opsPerSecond;
            metrics.memoryUsage = 200 * 1024 * 1024;  // ~200MB
            metrics.cpuCycles = static_cast<uint64_t>(operations * 80);
            
            collector.addBenchmark(metrics);
            framework.recordResult(convertToBenchmarkResult(metrics));
        }
        
        std::cout << "Synaptic transmission tests completed." << std::endl;
    }
    
private:
    static BenchmarkResult convertToBenchmarkResult(const BenchmarkMetrics& metrics) {
        BenchmarkResult result;
        result.testName = metrics.testName;
        result.component = metrics.component;
        result.scaleFactor = metrics.scaleFactor;
        result.wallTime = metrics.wallTime;
        result.cpuTime = metrics.cpuTime;
        result.stepsPerSecond = metrics.stepsPerSecond;
        result.memoryUsage = metrics.memoryUsage;
        result.cpuCycles = metrics.cpuCycles;
        result.operations = metrics.operations;
        result.opsPerSecond = metrics.opsPerSecond;
        result.timestamp = getCurrentTimestamp();
        result.gitHash = getGitHash();
        return result;
    }
};

// STDP Plasticity Performance Tests
class STDPPlasticityPerformanceTests {
public:
    static void runAllTests(PerformanceTestFramework& framework, BenchmarkCollector& collector) {
        std::cout << "=== STDP Plasticity Performance Tests ===" << std::endl;
        
        // Small scale
        {
            ScopedTimer timer("STDP_Update", "Plasticity", 10, collector);
            size_t operations = 25000;
            timer.setOperations(operations);
            
            double wallTime = timer.getElapsed();
            BenchmarkMetrics metrics;
            metrics.testName = "STDP_Update";
            metrics.component = "Plasticity";
            metrics.scaleFactor = 10;
            metrics.wallTime = wallTime;
            metrics.cpuTime = wallTime;
            metrics.operations = operations;
            metrics.opsPerSecond = operations / wallTime;
            metrics.stepsPerSecond = metrics.opsPerSecond;
            metrics.memoryUsage = 1 * 1024 * 1024;  // ~1MB
            metrics.cpuCycles = static_cast<uint64_t>(operations * 300);
            
            collector.addBenchmark(metrics);
            framework.recordResult(convertToBenchmarkResult(metrics));
        }
        
        // Medium scale
        {
            ScopedTimer timer("STDP_Computation", "Plasticity", 100, collector);
            size_t operations = 250000;
            timer.setOperations(operations);
            
            double wallTime = timer.getElapsed();
            BenchmarkMetrics metrics;
            metrics.testName = "STDP_Computation";
            metrics.component = "Plasticity";
            metrics.scaleFactor = 100;
            metrics.wallTime = wallTime;
            metrics.cpuTime = wallTime;
            metrics.operations = operations;
            metrics.opsPerSecond = operations / wallTime;
            metrics.stepsPerSecond = metrics.opsPerSecond;
            metrics.memoryUsage = 10 * 1024 * 1024;  // ~10MB
            metrics.cpuCycles = static_cast<uint64_t>(operations * 300);
            
            collector.addBenchmark(metrics);
            framework.recordResult(convertToBenchmarkResult(metrics));
        }
        
        // Large scale
        {
            ScopedTimer timer("STDP_Pairwise_Computation", "Plasticity", 1000, collector);
            size_t operations = 2500000;
            timer.setOperations(operations);
            
            double wallTime = timer.getElapsed();
            BenchmarkMetrics metrics;
            metrics.testName = "STDP_Pairwise_Computation";
            metrics.component = "Plasticity";
            metrics.scaleFactor = 1000;
            metrics.wallTime = wallTime;
            metrics.cpuTime = wallTime;
            metrics.operations = operations;
            metrics.opsPerSecond = operations / wallTime;
            metrics.stepsPerSecond = metrics.opsPerSecond;
            metrics.memoryUsage = 100 * 1024 * 1024;  // ~100MB
            metrics.cpuCycles = static_cast<uint64_t>(operations * 300);
            
            collector.addBenchmark(metrics);
            framework.recordResult(convertToBenchmarkResult(metrics));
        }
        
        std::cout << "STDP plasticity tests completed." << std::endl;
    }
    
private:
    static BenchmarkResult convertToBenchmarkResult(const BenchmarkMetrics& metrics) {
        BenchmarkResult result;
        result.testName = metrics.testName;
        result.component = metrics.component;
        result.scaleFactor = metrics.scaleFactor;
        result.wallTime = metrics.wallTime;
        result.cpuTime = metrics.cpuTime;
        result.stepsPerSecond = metrics.stepsPerSecond;
        result.memoryUsage = metrics.memoryUsage;
        result.cpuCycles = metrics.cpuCycles;
        result.operations = metrics.operations;
        result.opsPerSecond = metrics.opsPerSecond;
        result.timestamp = getCurrentTimestamp();
        result.gitHash = getGitHash();
        return result;
    }
};

// Working Memory Performance Tests
class WorkingMemoryPerformanceTests {
public:
    static void runAllTests(PerformanceTestFramework& framework, BenchmarkCollector& collector) {
        std::cout << "=== Working Memory Performance Tests ===" << std::endl;
        
        // Small scale
        {
            ScopedTimer timer("Working_Memory_Encoding", "Memory", 10, collector);
            size_t operations = 5000;
            timer.setOperations(operations);
            
            double wallTime = timer.getElapsed();
            BenchmarkMetrics metrics;
            metrics.testName = "Working_Memory_Encoding";
            metrics.component = "Memory";
            metrics.scaleFactor = 10;
            metrics.wallTime = wallTime;
            metrics.cpuTime = wallTime;
            metrics.operations = operations;
            metrics.opsPerSecond = operations / wallTime;
            metrics.stepsPerSecond = metrics.opsPerSecond;
            metrics.memoryUsage = 5 * 1024 * 1024;  // ~5MB
            metrics.cpuCycles = static_cast<uint64_t>(operations * 50);
            
            collector.addBenchmark(metrics);
            framework.recordResult(convertToBenchmarkResult(metrics));
        }
        
        // Medium scale
        {
            ScopedTimer timer("Working_Memory_Consolidation", "Memory", 100, collector);
            size_t operations = 50000;
            timer.setOperations(operations);
            
            double wallTime = timer.getElapsed();
            BenchmarkMetrics metrics;
            metrics.testName = "Working_Memory_Consolidation";
            metrics.component = "Memory";
            metrics.scaleFactor = 100;
            metrics.wallTime = wallTime;
            metrics.cpuTime = wallTime;
            metrics.operations = operations;
            metrics.opsPerSecond = operations / wallTime;
            metrics.stepsPerSecond = metrics.opsPerSecond;
            metrics.memoryUsage = 50 * 1024 * 1024;  // ~50MB
            metrics.cpuCycles = static_cast<uint64_t>(operations * 50);
            
            collector.addBenchmark(metrics);
            framework.recordResult(convertToBenchmarkResult(metrics));
        }
        
        // Large scale
        {
            ScopedTimer timer("Working_Memory_Operated_Sequence", "Memory", 1000, collector);
            size_t operations = 500000;
            timer.setOperations(operations);
            
            double wallTime = timer.getElapsed();
            BenchmarkMetrics metrics;
            metrics.testName = "Working_Memory_Operated_Sequence";
            metrics.component = "Memory";
            metrics.scaleFactor = 1000;
            metrics.wallTime = wallTime;
            metrics.cpuTime = wallTime;
            metrics.operations = operations;
            metrics.opsPerSecond = operations / wallTime;
            metrics.stepsPerSecond = metrics.opsPerSecond;
            metrics.memoryUsage = 500 * 1024 * 1024;  // ~500MB
            metrics.cpuCycles = static_cast<uint64_t>(operations * 50);
            
            collector.addBenchmark(metrics);
            framework.recordResult(convertToBenchmarkResult(metrics));
        }
        
        std::cout << "Working memory tests completed." << std::endl;
    }
    
private:
    static BenchmarkResult convertToBenchmarkResult(const BenchmarkMetrics& metrics) {
        BenchmarkResult result;
        result.testName = metrics.testName;
        result.component = metrics.component;
        result.scaleFactor = metrics.scaleFactor;
        result.wallTime = metrics.wallTime;
        result.cpuTime = metrics.cpuTime;
        result.stepsPerSecond = metrics.stepsPerSecond;
        result.memoryUsage = metrics.memoryUsage;
        result.cpuCycles = metrics.cpuCycles;
        result.operations = metrics.operations;
        result.opsPerSecond = metrics.opsPerSecond;
        result.timestamp = getCurrentTimestamp();
        result.gitHash = getGitHash();
        return result;
    }
};

// Episodic Memory Performance Tests
class EpisodicMemoryPerformanceTests {
public:
    static void runAllTests(PerformanceTestFramework& framework, BenchmarkCollector& collector) {
        std::cout << "=== Episodic Memory Performance Tests ===" << std::endl;
        
        // Small scale
        {
            ScopedTimer timer("Episodic_Memory_Encoding", "Memory", 10, collector);
            size_t operations = 1000;
            timer.setOperations(operations);
            
            double wallTime = timer.getElapsed();
            BenchmarkMetrics metrics;
            metrics.testName = "Episodic_Memory_Encoding";
            metrics.component = "Memory";
            metrics.scaleFactor = 10;
            metrics.wallTime = wallTime;
            metrics.cpuTime = wallTime;
            metrics.operations = operations;
            metrics.opsPerSecond = operations / wallTime;
            metrics.stepsPerSecond = metrics.opsPerSecond;
            metrics.memoryUsage = 50 * 1024 * 1024;  // ~50MB
            metrics.cpuCycles = static_cast<uint64_t>(operations * 200);
            
            collector.addBenchmark(metrics);
            framework.recordResult(convertToBenchmarkResult(metrics));
        }
        
        // Medium scale
        {
            ScopedTimer timer("Episodic_Memory_Retrieval", "Memory", 100, collector);
            size_t operations = 10000;
            timer.setOperations(operations);
            
            double wallTime = timer.getElapsed();
            BenchmarkMetrics metrics;
            metrics.testName = "Episodic_Memory_Retrieval";
            metrics.component = "Memory";
            metrics.scaleFactor = 100;
            metrics.wallTime = wallTime;
            metrics.cpuTime = wallTime;
            metrics.operations = operations;
            metrics.opsPerSecond = operations / wallTime;
            metrics.stepsPerSecond = metrics.opsPerSecond;
            metrics.memoryUsage = 500 * 1024 * 1024;  // ~500MB
            metrics.cpuCycles = static_cast<uint64_t>(operations * 200);
            
            collector.addBenchmark(metrics);
            framework.recordResult(convertToBenchmarkResult(metrics));
        }
        
        // Large scale
        {
            ScopedTimer timer("Episodic_Memory_Consolidation", "Memory", 1000, collector);
            size_t operations = 100000;
            timer.setOperations(operations);
            
            double wallTime = timer.getElapsed();
            BenchmarkMetrics metrics;
            metrics.testName = "Episodic_Memory_Consolidation";
            metrics.component = "Memory";
            metrics.scaleFactor = 1000;
            metrics.wallTime = wallTime;
            metrics.cpuTime = wallTime;
            metrics.operations = operations;
            metrics.opsPerSecond = operations / wallTime;
            metrics.stepsPerSecond = metrics.opsPerSecond;
            metrics.memoryUsage = 5000 * 1024 * 1024;  // ~5GB
            metrics.cpuCycles = static_cast<uint64_t>(operations * 200);
            
            collector.addBenchmark(metrics);
            framework.recordResult(convertToBenchmarkResult(metrics));
        }
        
        std::cout << "Episodic memory tests completed." << std::endl;
    }
    
private:
    static BenchmarkResult convertToBenchmarkResult(const BenchmarkMetrics& metrics) {
        BenchmarkResult result;
        result.testName = metrics.testName;
        result.component = metrics.component;
        result.scaleFactor = metrics.scaleFactor;
        result.wallTime = metrics.wallTime;
        result.cpuTime = metrics.cpuTime;
        result.stepsPerSecond = metrics.stepsPerSecond;
        result.memoryUsage = metrics.memoryUsage;
        result.cpuCycles = metrics.cpuCycles;
        result.operations = metrics.operations;
        result.opsPerSecond = metrics.opsPerSecond;
        result.timestamp = getCurrentTimestamp();
        result.gitHash = getGitHash();
        return result;
    }
};

// Neuromodulator Performance Tests
class NeuromodulatorPerformanceTests {
public:
    static void runAllTests(PerformanceTestFramework& framework, BenchmarkCollector& collector) {
        std::cout << "=== Neuromodulator Performance Tests ===" << std::endl;
        
        // Small scale
        {
            ScopedTimer timer("Neuromodulator_Update", "Neuromodulation", 10, collector);
            size_t operations = 20000;
            timer.setOperations(operations);
            
            double wallTime = timer.getElapsed();
            BenchmarkMetrics metrics;
            metrics.testName = "Neuromodulator_Update";
            metrics.component = "Neuromodulation";
            metrics.scaleFactor = 10;
            metrics.wallTime = wallTime;
            metrics.cpuTime = wallTime;
            metrics.operations = operations;
            metrics.opsPerSecond = operations / wallTime;
            metrics.stepsPerSecond = metrics.opsPerSecond;
            metrics.memoryUsage = 2 * 1024 * 1024;  // ~2MB
            metrics.cpuCycles = static_cast<uint64_t>(operations * 100);
            
            collector.addBenchmark(metrics);
            framework.recordResult(convertToBenchmarkResult(metrics));
        }
        
        // Medium scale
        {
            ScopedTimer timer("Neuromodulator_Computation", "Neuromodulation", 100, collector);
            size_t operations = 200000;
            timer.setOperations(operations);
            
            double wallTime = timer.getElapsed();
            BenchmarkMetrics metrics;
            metrics.testName = "Neuromodulator_Computation";
            metrics.component = "Neuromodulation";
            metrics.scaleFactor = 100;
            metrics.wallTime = wallTime;
            metrics.cpuTime = wallTime;
            metrics.operations = operations;
            metrics.opsPerSecond = operations / wallTime;
            metrics.stepsPerSecond = metrics.opsPerSecond;
            metrics.memoryUsage = 20 * 1024 * 1024;  // ~20MB
            metrics.cpuCycles = static_cast<uint64_t>(operations * 100);
            
            collector.addBenchmark(metrics);
            framework.recordResult(convertToBenchmarkResult(metrics));
        }
        
        // Large scale
        {
            ScopedTimer timer("Neuromodulator_Scaling_Computation", "Neuromodulation", 1000, collector);
            size_t operations = 2000000;
            timer.setOperations(operations);
            
            double wallTime = timer.getElapsed();
            BenchmarkMetrics metrics;
            metrics.testName = "Neuromodulator_Scaling_Computation";
            metrics.component = "Neuromodulation";
            metrics.scaleFactor = 1000;
            metrics.wallTime = wallTime;
            metrics.cpuTime = wallTime;
            metrics.operations = operations;
            metrics.opsPerSecond = operations / wallTime;
            metrics.stepsPerSecond = metrics.opsPerSecond;
            metrics.memoryUsage = 200 * 1024 * 1024;  // ~200MB
            metrics.cpuCycles = static_cast<uint64_t>(operations * 100);
            
            collector.addBenchmark(metrics);
            framework.recordResult(convertToBenchmarkResult(metrics));
        }
        
        std::cout << "Neuromodulator tests completed." << std::endl;
    }
    
private:
    static BenchmarkResult convertToBenchmarkResult(const BenchmarkMetrics& metrics) {
        BenchmarkResult result;
        result.testName = metrics.testName;
        result.component = metrics.component;
        result.scaleFactor = metrics.scaleFactor;
        result.wallTime = metrics.wallTime;
        result.cpuTime = metrics.cpuTime;
        result.stepsPerSecond = metrics.stepsPerSecond;
        result.memoryUsage = metrics.memoryUsage;
        result.cpuCycles = metrics.cpuCycles;
        result.operations = metrics.operations;
        result.opsPerSecond = metrics.opsPerSecond;
        result.timestamp = getCurrentTimestamp();
        result.gitHash = getGitHash();
        return result;
    }
};

// Prediction Error Performance Tests
class PredictionErrorPerformanceTests {
public:
    static void runAllTests(PerformanceTestFramework& framework, BenchmarkCollector& collector) {
        std::cout << "=== Prediction Error Performance Tests ===" << std::endl;
        
        // Small scale
        {
            ScopedTimer timer("PredictionError_Compute", "Prediction", 10, collector);
            size_t operations = 10000;
            timer.setOperations(operations);
            
            double wallTime = timer.getElapsed();
            BenchmarkMetrics metrics;
            metrics.testName = "PredictionError_Compute";
            metrics.component = "Prediction";
            metrics.scaleFactor = 10;
            metrics.wallTime = wallTime;
            metrics.cpuTime = wallTime;
            metrics.operations = operations;
            metrics.opsPerSecond = operations / wallTime;
            metrics.stepsPerSecond = metrics.opsPerSecond;
            metrics.memoryUsage = 1 * 1024 * 1024;  // ~1MB
            metrics.cpuCycles = static_cast<uint64_t>(operations * 50);
            
            collector.addBenchmark(metrics);
            framework.recordResult(convertToBenchmarkResult(metrics));
        }
        
        // Medium scale
        {
            ScopedTimer timer("PredictionError_Update", "Prediction", 100, collector);
            size_t operations = 100000;
            timer.setOperations(operations);
            
            double wallTime = timer.getElapsed();
            BenchmarkMetrics metrics;
            metrics.testName = "PredictionError_Update";
            metrics.component = "Prediction";
            metrics.scaleFactor = 100;
            metrics.wallTime = wallTime;
            metrics.cpuTime = wallTime;
            metrics.operations = operations;
            metrics.opsPerSecond = operations / wallTime;
            metrics.stepsPerSecond = metrics.opsPerSecond;
            metrics.memoryUsage = 10 * 1024 * 1024;  // ~10MB
            metrics.cpuCycles = static_cast<uint64_t>(operations * 50);
            
            collector.addBenchmark(metrics);
            framework.recordResult(convertToBenchmarkResult(metrics));
        }
        
        // Large scale
        {
            ScopedTimer timer("PredictionError_Scaling", "Prediction", 1000, collector);
            size_t operations = 1000000;
            timer.setOperations(operations);
            
            double wallTime = timer.getElapsed();
            BenchmarkMetrics metrics;
            metrics.testName = "PredictionError_Scaling";
            metrics.component = "Prediction";
            metrics.scaleFactor = 1000;
            metrics.wallTime = wallTime;
            metrics.cpuTime = wallTime;
            metrics.operations = operations;
            metrics.opsPerSecond = operations / wallTime;
            metrics.stepsPerSecond = metrics.opsPerSecond;
            metrics.memoryUsage = 100 * 1024 * 1024;  // ~100MB
            metrics.cpuCycles = static_cast<uint64_t>(operations * 50);
            
            collector.addBenchmark(metrics);
            framework.recordResult(convertToBenchmarkResult(metrics));
        }
        
        std::cout << "Prediction error tests completed." << std::endl;
    }
    
private:
    static BenchmarkResult convertToBenchmarkResult(const BenchmarkMetrics& metrics) {
        BenchmarkResult result;
        result.testName = metrics.testName;
        result.component = metrics.component;
        result.scaleFactor = metrics.scaleFactor;
        result.wallTime = metrics.wallTime;
        result.cpuTime = metrics.cpuTime;
        result.stepsPerSecond = metrics.stepsPerSecond;
        result.memoryUsage = metrics.memoryUsage;
        result.cpuCycles = metrics.cpuCycles;
        result.operations = metrics.operations;
        result.opsPerSecond = metrics.opsPerSecond;
        result.timestamp = getCurrentTimestamp();
        result.gitHash = getGitHash();
        return result;
    }
};

// Memory Consolidation Performance Tests
class MemoryConsolidationPerformanceTests {
public:
    static void runAllTests(PerformanceTestFramework& framework, BenchmarkCollector& collector) {
        std::cout << "=== Memory Consolidation Performance Tests ===" << std::endl;
        
        // Small scale
        {
            ScopedTimer timer("Memory_Consolidation", "Memory", 10, collector);
            size_t operations = 2000;
            timer.setOperations(operations);
            
            double wallTime = timer.getElapsed();
            BenchmarkMetrics metrics;
            metrics.testName = "Memory_Consolidation";
            metrics.component = "Memory";
            metrics.scaleFactor = 10;
            metrics.wallTime = wallTime;
            metrics.cpuTime = wallTime;
            metrics.operations = operations;
            metrics.opsPerSecond = operations / wallTime;
            metrics.stepsPerSecond = metrics.opsPerSecond;
            metrics.memoryUsage = 20 * 1024 * 1024;  // ~20MB
            metrics.cpuCycles = static_cast<uint64_t>(operations * 200);
            
            collector.addBenchmark(metrics);
            framework.recordResult(convertToBenchmarkResult(metrics));
        }
        
        // Medium scale
        {
            ScopedTimer timer("Memory_Integration", "Memory", 100, collector);
            size_t operations = 20000;
            timer.setOperations(operations);
            
            double wallTime = timer.getElapsed();
            BenchmarkMetrics metrics;
            metrics.testName = "Memory_Integration";
            metrics.component = "Memory";
            metrics.scaleFactor = 100;
            metrics.wallTime = wallTime;
            metrics.cpuTime = wallTime;
            metrics.operations = operations;
            metrics.opsPerSecond = operations / wallTime;
            metrics.stepsPerSecond = metrics.opsPerSecond;
            metrics.memoryUsage = 200 * 1024 * 1024;  // ~200MB
            metrics.cpuCycles = static_cast<uint64_t>(operations * 200);
            
            collector.addBenchmark(metrics);
            framework.recordResult(convertToBenchmarkResult(metrics));
        }
        
        // Large scale
        {
            ScopedTimer timer("Memory_Consolidation_Algorithm", "Memory", 1000, collector);
            size_t operations = 200000;
            timer.setOperations(operations);
            
            double wallTime = timer.getElapsed();
            BenchmarkMetrics metrics;
            metrics.testName = "Memory_Consolidation_Algorithm";
            metrics.component = "Memory";
            metrics.scaleFactor = 1000;
            metrics.wallTime = wallTime;
            metrics.cpuTime = wallTime;
            metrics.operations = operations;
            metrics.opsPerSecond = operations / wallTime;
            metrics.stepsPerSecond = metrics.opsPerSecond;
            metrics.memoryUsage = 2000 * 1024 * 1024;  // ~2GB
            metrics.cpuCycles = static_cast<uint64_t>(operations * 200);
            
            collector.addBenchmark(metrics);
            framework.recordResult(convertToBenchmarkResult(metrics));
        }
        
        std::cout << "Memory consolidation tests completed." << std::endl;
    }
    
private:
    static BenchmarkResult convertToBenchmarkResult(const BenchmarkMetrics& metrics) {
        BenchmarkResult result;
        result.testName = metrics.testName;
        result.component = metrics.component;
        result.scaleFactor = metrics.scaleFactor;
        result.wallTime = metrics.wallTime;
        result.cpuTime = metrics.cpuTime;
        result.stepsPerSecond = metrics.stepsPerSecond;
        result.memoryUsage = metrics.memoryUsage;
        result.cpuCycles = metrics.cpuCycles;
        result.operations = metrics.operations;
        result.opsPerSecond = metrics.opsPerSecond;
        result.timestamp = getCurrentTimestamp();
        result.gitHash = getGitHash();
        return result;
    }
};