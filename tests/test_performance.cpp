// NLM Performance Optimization Test Suite
// Tests for O(N³) neural computation and spike delivery bottlenecks

#include "Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>

using namespace nlm;

class PerformanceAnalyzer {
public:
    static void analyzeNeuralComputationComplexity() {
        std::cout << "=== NLM Neural Computation Complexity Analysis ===" << std::endl;
        
        // Test different network sizes to identify O(N³) behavior
        std::vector<size_t> neuronCounts = {100, 500, 1000, 2000};
        std::vector<double> times;
        
        std::cout << std::left << std::setw(15) << "Neurons" 
                  << std::setw(20) << "Time (s)" 
                  << std::setw(15) << "Time/Neuron" 
                  << std::setw(15) << "Steps/sec" << std::endl;
        std::cout << std::string(65, '=') << std::endl;
        
        for (size_t neurons : neuronCounts) {
            double elapsed = measureNeuralComputationTime(neurons);
            times.push_back(elapsed);
            
            double timePerNeuron = elapsed / neurons;
            double stepsPerSecond = neurons / elapsed; // Simplified metric
            
            std::cout << std::left << std::setw(15) << neurons
                      << std::setw(20) << std::fixed << std::setprecision(3) << elapsed
                      << std::setw(15) << std::fixed << std::setprecision(6) << timePerNeuron
                      << std::setw(15) << std::fixed << std::setprecision(0) << stepsPerSecond << std::endl;
        }
        
        // Analyze growth pattern
        analyzeComplexityGrowth(neuronCounts, times);
    }
    
    static double measureNeuralComputationTime(size_t neuronCount) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Create brain configuration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(std::min<size_t>(1, neuronCount / 100)), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Inject minimal input
        for (size_t i = 0; i < std::min<size_t>(10, neuronCount / 10); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        // Run minimal simulation (100 steps) - just enough to trigger core loops
        for (SimulationStep step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(endTime - startTime).count();
        
        return elapsed;
    }
    
    static void analyzeComplexityGrowth(const std::vector<size_t>& neuronCounts, 
                                        const std::vector<double>& times) {
        std::cout << std::endl;
        std::cout << "Complexity Growth Analysis:" << std::endl;
        
        if (neuronCounts.size() < 2) return;
        
        // Check if it's O(N³) by looking at ratio of time increases
        // For O(N³), doubling neurons should increase time by ~8x
        for (size_t i = 1; i < neuronCounts.size(); ++i) {
            size_t oldNeurons = neuronCounts[i-1];
            size_t newNeurons = neuronCounts[i];
            double oldTime = times[i-1];
            double newTime = times[i];
            
            double neuronsRatio = static_cast<double>(newNeurons) / oldNeurons;
            double timeRatio = newTime / oldTime;
            double expectedN3 = neuronsRatio * neuronsRatio * neuronsRatio;
            double expectedN2 = neuronsRatio * neuronsRatio;
            double expectedN = neuronsRatio;
            
            std::cout << std::left << std::setw(15) << "N=" << oldNeurons << " → N=" << newNeurons
                      << std::setw(10) << "Time ratio: " << std::fixed << std::setprecision(2) << timeRatio
                      << std::setw(15) << "Expected O(N³): " << std::fixed << std::setprecision(2) << expectedN3
                      << std::setw(15) << "Expected O(N²): " << std::fixed << std::setprecision(2) << expectedN2
                      << std::setw(15) << "Expected O(N): " << std::fixed << std::setprecision(2) << expectedN << std::endl;
        }
        
        // Check for O(N³) characteristics
        std::cout << std::endl;
        std::cout << "O(N³) Characteristics Detected:" << std::endl;
        
        double totalDeviation = 0.0;
        size_t o3Matches = 0;
        
        for (size_t i = 1; i < neuronCounts.size(); ++i) {
            size_t oldNeurons = neuronCounts[i-1];
            size_t newNeurons = neuronCounts[i];
            double oldTime = times[i-1];
            double newTime = times[i];
            
            double neuronsRatio = static_cast<double>(newNeurons) / oldNeurons;
            double timeRatio = newTime / oldTime;
            double expectedN3 = neuronsRatio * neuronsRatio * neuronsRatio;
            
            if (std::abs(timeRatio - expectedN3) / expectedN3 < 0.5) {  // 50% tolerance
                o3Matches++;
                std::cout << "  ✓ " << oldNeurons << " → " << newNeurons << " follows O(N³) pattern" << std::endl;
            } else {
                double deviation = std::abs(timeRatio - expectedN3) / expectedN3;
                totalDeviation += deviation;
            }
        }
        
        if (o3Matches > 0) {
            std::cout << "  CONFLICT: Detected O(N³) complexity in neural computation!" << std::endl;
            std::cout << "  Recommended: Implement O(1) spike delivery using neuron index mapping" << std::endl;
        } else {
            std::cout << "  O(N³) characteristics not clearly detected" << std::endl;
        }
    }
    
    static void benchmarkSpikeDelivery() {
        std::cout << "=== NLM Spike Delivery Performance Analysis ===" << std::endl;
        
        // Create a simple brain to test spike delivery
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.05f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create neurons that will fire
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) return;
        
        // Inject current to make neurons fire
        size_t numFireNeurons = std::min<size_t>(10, neurons.size());
        for (size_t i = 0; i < numFireNeurons; ++i) {
            neurons[i]->injectCurrent(60.0f);
        }
        
        // Measure spike delivery time
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run simulation to deliver spikes
        for (SimulationStep step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << "Spike Delivery Performance:" << std::endl;
        std::cout << "  Network size: " << neurons.size() << " neurons" << std::endl;
        std::cout << "  Time for 100 steps: " << std::fixed << std::setprecision(3) << elapsed << "s" << std::endl;
        std::cout << "  Steps per second: " << std::fixed << std::setprecision(0) << 100 / elapsed << std::endl;
        std::cout << "  Total spikes generated: " << brain->getTotalSpikeCount() << std::endl;
        
        // Analyze spike delivery complexity
        size_t totalDelayEvents = brain->getPendingSpikeEventCount();
        std::cout << "  Pending spike events: " << totalDelayEvents << std::endl;
        
        if (totalDelayEvents > 0) {
            std::cout << "  WARNING: Spike delivery may be O(N) or worse" << std::endl;
            std::cout << "  Recommended: Implement O(1) synapse-to-neuron lookup table" << std::endl;
        }
    }
};

void testNeuralComputationComplexity() {
    std::cout << "Testing NLM Neural Computation Complexity..." << std::endl;
    PerformanceAnalyzer::analyzeNeuralComputationComplexity();
}

void testSpikeDeliveryPerformance() {
    std::cout << "Testing NLM Spike Delivery Performance..." << std::endl;
    PerformanceAnalyzer::benchmarkSpikeDelivery();
}

void runAllPerformanceTests() {
    std::cout << "=== NLM Performance Test Suite ===" << std::endl << std::endl;
    
    testNeuralComputationComplexity();
    std::cout << std::endl;
    
    testSpikeDeliveryPerformance();
    std::cout << std::endl;
    
    std::cout << "=== Performance Test Suite Complete ===" << std::endl;
}