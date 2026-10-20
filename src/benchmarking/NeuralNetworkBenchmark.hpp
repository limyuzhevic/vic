#pragma once

#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <map>
#include <unordered_map>

namespace nlm {
namespace benchmark {

// Performance benchmark results
struct BenchmarkResult {
    std::string testName;
    double executionTime;          // seconds
    size_t memoryUsed;            // bytes
    size_t totalSpikes;           // spikes generated
    float averageFiringRate;      // Hz
    size_t neuronCount;           // number of neurons
    size_t regionCount;           // number of regions
    double stepsPerSecond;        // performance metric
    
    BenchmarkResult(const std::string& name) 
        : testName(name), executionTime(0.0), memoryUsed(0), 
          totalSpikes(0), averageFiringRate(0.0f), 
          neuronCount(0), regionCount(0), stepsPerSecond(0.0) {}
};

// Comprehensive neural network benchmark suite
class NeuralNetworkBenchmark {
public:
    NeuralNetworkBenchmark(std::shared_ptr<Config> config) 
        : config_(config) {}
    
    ~NeuralNetworkBenchmark() = default;
    
    // Run full benchmark suite
    std::vector<BenchmarkResult> runFullBenchmark() {
        std::vector<BenchmarkResult> results;
        
        NLM_LOG_INFO("=== Starting Comprehensive Neural Network Benchmark ===");
        
        // Basic connectivity benchmark
        results.push_back(benchmarkConnectivity());
        
        // Plasticity benchmark
        results.push_back(benchmarkPlasticity());
        
        // STDP benchmark
        results.push_back(benchmarkStdp());
        
        // Memory scaling benchmark
        results.push_back(benchmarkMemoryScaling());
        
        // Performance benchmark
        results.push_back(benchmarkPerformance());
        
        printBenchmarkSummary(results);
        
        return results;
    }
    
    // Run specific benchmark
    BenchmarkResult runBenchmark(const std::string& testName) {
        if (testName == "connectivity") {
            return benchmarkConnectivity();
        } else if (testName == "plasticity") {
            return benchmarkPlasticity();
        } else if (testName == "stdp") {
            return benchmarkStdp();
        } else if (testName == "memory") {
            return benchmarkMemoryScaling();
        } else if (testName == "performance") {
            return benchmarkPerformance();
        } else {
            NLM_LOG_ERROR("Unknown benchmark: {}", testName);
            return BenchmarkResult(testName);
        }
    }
    
    // Export benchmark results
    bool exportResults(const std::vector<BenchmarkResult>& results, 
                      const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open benchmark export file: {}", filepath);
            return false;
        }
        
        file << "NLM Neural Network Benchmark Results\n";
        file << "Generated: " << std::chrono::system_clock::now() << "\n\n";
        
        file << "Benchmark\tTime(s)\tMemory(B)\tSpikes\tFiringRate(Hz)\tSteps/sec\tNeurons\tRegions\n";
        file << "--------\t-------\t----------\t------\t-------------\t--------\t-------\t------\n";
        
        for (const auto& result : results) {
            file << result.testName << "\t"
                 << std::fixed << std::setprecision(3) << result.executionTime << "\t"
                 << result.memoryUsed << "\t"
                 << result.totalSpikes << "\t"
                 << std::fixed << std::setprecision(3) << result.averageFiringRate << "\t"
                 << std::fixed << std::setprecision(3) << result.stepsPerSecond << "\t"
                 << result.neuronCount << "\t"
                 << result.regionCount << "\n";
        }
        
        return true;
    }
    
    // Generate performance report
    std::string generatePerformanceReport(const std::vector<BenchmarkResult>& results) const {
        std::stringstream report;
        
        report << "=== NLM Neural Network Performance Report ===\n\n";
        
        double totalTime = 0.0;
        size_t totalSpikes = 0;
        size_t totalNeurons = 0;
        
        for (const auto& result : results) {
            totalTime += result.executionTime;
            totalSpikes += result.totalSpikes;
            totalNeurons += result.neuronCount;
            
            report << result.testName << ":\n";
            report << "  Execution Time: " << std::fixed << std::setprecision(3) 
                   << result.executionTime << " seconds\n";
            report << "  Memory Used: " << (result.memoryUsed / 1024) << " KB\n";
            report << "  Total Spikes: " << result.totalSpikes << "\n";
            report << "  Average Firing Rate: " << std::fixed << std::setprecision(3) 
                   << result.averageFiringRate << " Hz\n";
            report << "  Performance: " << std::fixed << std::setprecision(3) 
                   << result.stepsPerSecond << " steps/second\n\n";
        }
        
        report << "=== Summary ===\n";
        report << "Total Benchmark Time: " << std::fixed << std::setprecision(3) 
               << totalTime << " seconds\n";
        report << "Overall Spikes Generated: " << totalSpikes << "\n";
        report << "Average Neurons per Test: " << (results.empty() ? 0 : totalNeurons / results.size()) << "\n";
        report << "Average Steps/second: " << (results.empty() ? 0.0 : 
            (totalSpikes / totalTime)) << "\n\n";
        
        return report.str();
    }
    
private:
    BenchmarkResult benchmarkConnectivity() {
        NLM_LOG_INFO("=== Benchmark: Basic Neural Connectivity ===");
        
        auto startTime = std::chrono::high_resolution_clock::now();
        auto brain = std::make_shared<Brain>(config_);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain for connectivity benchmark");
            return BenchmarkResult("connectivity");
        }
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No region found for connectivity benchmark");
            return BenchmarkResult("connectivity");
        }
        
        auto neurons = region->getAllNeurons();
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into neurons
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        // Run benchmark steps
        for (SimulationStep step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        float firingRate = (duration > 0.0) ? (spikes / duration) : 0.0f;
        
        BenchmarkResult result("connectivity");
        result.executionTime = duration;
        result.memoryUsed = getMemoryUsage();
        result.totalSpikes = spikes;
        result.averageFiringRate = firingRate;
        result.neuronCount = brain->getTotalNeuronCount();
        result.regionCount = brain->getRegionCount();
        result.stepsPerSecond = (duration > 0.0) ? (100.0 / duration) : 0.0;
        
        NLM_LOG_INFO("Connectivity benchmark completed in {}s, {} spikes, {} steps/s", 
                    duration, spikes, result.stepsPerSecond);
        
        return result;
    }
    
    BenchmarkResult benchmarkPlasticity() {
        NLM_LOG_INFO("=== Benchmark: Plasticity Learning ===");
        
        auto startTime = std::chrono::high_resolution_clock::now();
        auto brain = std::make_shared<Brain>(config_);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain for plasticity benchmark");
            return BenchmarkResult("plasticity");
        }
        
        LearningExperiment experiment(brain, 42);
        experiment.recordInitialState();
        
        // Enable plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Run plasticity steps
        size_t totalSteps = 1000;
        for (SimulationStep step = 0; step < totalSteps; ++step) {
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            brain->step(step, step * 0.001);
        }
        
        experiment.recordFinalState();
        experiment.computeStatistics();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        
        float firingRate = (duration > 0.0) ? (brain->getTotalSpikeCount() / duration) : 0.0f;
        
        BenchmarkResult result("plasticity");
        result.executionTime = duration;
        result.memoryUsed = getMemoryUsage();
        result.totalSpikes = brain->getTotalSpikeCount();
        result.averageFiringRate = firingRate;
        result.neuronCount = brain->getTotalNeuronCount();
        result.regionCount = brain->getRegionCount();
        result.stepsPerSecond = (duration > 0.0) ? (totalSteps / duration) : 0.0;
        
        NLM_LOG_INFO("Plasticity benchmark completed in {}s, {} spikes, {} steps/s", 
                    duration, brain->getTotalSpikeCount(), result.stepsPerSecond);
        
        return result;
    }
    
    BenchmarkResult benchmarkStdp() {
        NLM_LOG_INFO("=== Benchmark: STDP Verification ===");
        
        auto startTime = std::chrono::high_resolution_clock::now();
        auto brain = std::make_shared<Brain>(config_);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain for STDP benchmark");
            return BenchmarkResult("stdp");
        }
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No region found for STDP benchmark");
            return BenchmarkResult("stdp");
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            NLM_LOG_ERROR("Not enough synapses for STDP benchmark");
            return BenchmarkResult("stdp");
        }
        
        // Record initial weights
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);
        }
        
        // Run STDP trials
        size_t trials = 50;
        for (size_t trial = 0; trial < trials; ++trial) {
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                Neuron* preNeuron = neurons[0];
                Neuron* postNeuron = neurons[1];
                
                preNeuron->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                postNeuron->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        // Calculate total change
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            totalDelta += synapses[i]->getWeight() - beforeWeights[i];
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        
        float firingRate = (duration > 0.0) ? (brain->getTotalSpikeCount() / duration) : 0.0f;
        
        BenchmarkResult result("stdp");
        result.executionTime = duration;
        result.memoryUsed = getMemoryUsage();
        result.totalSpikes = brain->getTotalSpikeCount();
        result.averageFiringRate = firingRate;
        result.neuronCount = brain->getTotalNeuronCount();
        result.regionCount = brain->getRegionCount();
        result.stepsPerSecond = (duration > 0.0) ? (trials * 2 / duration) : 0.0;
        
        NLM_LOG_INFO("STDP benchmark completed in {}s, {} spikes, {} steps/s, delta={}", 
                    duration, brain->getTotalSpikeCount(), result.stepsPerSecond, totalDelta);
        
        return result;
    }
    
    BenchmarkResult benchmarkMemoryScaling() {
        NLM_LOG_INFO("=== Benchmark: Memory Scaling ===");
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Test with different neuron counts
        std::vector<int64_t> neuronCounts = {100, 500, 1000};
        BenchmarkResult result("memory_scaling");
        
        std::vector<double> executionTimes;
        
        for (size_t i = 0; i < neuronCounts.size(); ++i) {
            auto brain = std::make_shared<Brain>(config_);
            config_->set("neuron_count", neuronCounts[i], ConfigSource::Runtime);
            
            if (brain->initialize()) {
                auto endTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration<double>(endTime - startTime).count();
                
                executionTimes.push_back(duration);
                NLM_LOG_INFO("Memory scaling test {} neurons: {}s", neuronCounts[i], duration);
                
                startTime = std::chrono::high_resolution_clock::now();
            }
        }
        
        if (!executionTimes.empty()) {
            double avgTime = std::accumulate(executionTimes.begin(), executionTimes.end(), 0.0) / executionTimes.size();
            result.executionTime = avgTime;
            result.stepsPerSecond = 1.0 / avgTime; // steps per second metric
        }
        
        result.memoryUsed = getMemoryUsage();
        result.neuronCount = *std::max_element(neuronCounts.begin(), neuronCounts.end());
        
        NLM_LOG_INFO("Memory scaling benchmark completed in {}s average", avgTime);
        
        return result;
    }
    
    BenchmarkResult benchmarkPerformance() {
        NLM_LOG_INFO("=== Benchmark: Performance ===");
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run performance intensive test
        auto brain = std::make_shared<Brain>(config_);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain for performance benchmark");
            return BenchmarkResult("performance");
        }
        
        // Run many steps to measure performance
        size_t totalSteps = 10000;
        for (SimulationStep step = 0; step < totalSteps; ++step) {
            // Inject random activity
            for (size_t i = 0; i < 10 && i < brain->getTotalNeuronCount() / 10; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Excitatory, 10.0f);
            }
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        
        float firingRate = (duration > 0.0) ? (brain->getTotalSpikeCount() / duration) : 0.0f;
        
        BenchmarkResult result("performance");
        result.executionTime = duration;
        result.memoryUsed = getMemoryUsage();
        result.totalSpikes = brain->getTotalSpikeCount();
        result.averageFiringRate = firingRate;
        result.neuronCount = brain->getTotalNeuronCount();
        result.regionCount = brain->getRegionCount();
        result.stepsPerSecond = (duration > 0.0) ? (totalSteps / duration) : 0.0;
        
        NLM_LOG_INFO("Performance benchmark completed in {}s, {} spikes, {} steps/s", 
                    duration, brain->getTotalSpikeCount(), result.stepsPerSecond);
        
        return result;
    }
    
    void printBenchmarkSummary(const std::vector<BenchmarkResult>& results) const {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Benchmark Summary ===");
        
        double totalTime = 0.0;
        for (const auto& result : results) {
            totalTime += result.executionTime;
        }
        
        NLM_LOG_INFO("Total benchmark time: {} seconds", totalTime);
        NLM_LOG_INFO("Number of tests: {}", results.size());
        
        // Find best performer
        if (!results.empty()) {
            auto bestResult = std::max_element(results.begin(), results.end(),
                [](const BenchmarkResult& a, const BenchmarkResult& b) {
                    return a.stepsPerSecond < b.stepsPerSecond;
                });
            
            NLM_LOG_INFO("Best performance: {} steps/s ({} benchmark)", 
                        bestResult->stepsPerSecond, bestResult->testName);
        }
    }
    
    size_t getMemoryUsage() const {
        // Simple memory usage estimation
        // In production, use platform-specific memory APIs
        return 0; // TODO: Implement actual memory measurement
    }
    
    std::shared_ptr<Config> config_;
};

} // namespace benchmark
} // namespace nlm