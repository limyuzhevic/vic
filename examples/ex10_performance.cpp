// ex10_performance.cpp - NLM Performance Testing Example
// Demonstrates NLM performance analysis, benchmarking, and optimization

#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/experiments/Experiment.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <chrono>
#include <fstream>
#include <algorithm>
#include <iomanip>

// Performance analysis and benchmarking framework
class PerformanceAnalyzer {
public:
    struct BenchmarkResult {
        double simulationTime;
        double initializationTime;
        double stepTime;
        double plasticityTime;
        double memoryTime;
        double spikeTime;
        uint64_t totalSpikes;
        uint64_t spikeEvents;
        size_t memoryUsage;
        double averageFiringRate;
        double stepsPerSecond;
        double performanceScore;
    };
    
    struct ScalabilityResult {
        std::vector<size_t> neuronCounts;
        std::vector<double> initializationTimes;
        std::vector<double> stepTimes;
        std::vector<double> stepsPerSecond;
        std::vector<double> memoryUsages;
        std::vector<double> firingRates;
    };
    
    struct OptimizationResult {
        std::string optimizationName;
        double speedupFactor;
        double memoryReduction;
        double timeReduction;
        bool isViable;
        std::string notes;
    };
    
    PerformanceAnalyzer() {
        setupBenchmarkSuite();
        initializeMetrics();
    }
    
    void setupBenchmarkSuite() {
        // Configure benchmark parameters
        benchmarkConfig_.neuronCounts = {100, 500, 1000, 2000, 5000, 10000};
        benchmarkConfig_.timesteps = {100, 500, 1000};
        benchmarkConfig_.optimizationLevels = {0, 1, 2, 3};
        benchmarkConfig_.repetitions = 3;
        
        // Set performance thresholds
        thresholds_.maxInitializationTime = 60.0;     // 60 seconds
        thresholds_.maxStepTime = 0.1;                  // 100ms per step
        thresholds_.minStepsPerSecond = 100.0;         // 100 SPS minimum
        thresholds_.maxMemoryUsageMB = 1024.0;         // 1GB maximum
        thresholds_.targetPerformanceScore = 1.0;     // Reference score
    }
    
    void initializeMetrics() {
        metrics_.totalBenchmarks = 0;
        metrics_.successfulBenchmarks = 0;
        metrics_.failedBenchmarks = 0;
        metrics_.totalTimeSaved = 0.0;
    }
    
    BenchmarkResult runBenchmark(size_t neuronCount, int timestepCount) {
        BenchmarkResult result;
        result.simulationTime = 0.0;
        result.initializationTime = 0.0;
        result.stepTime = 0.0;
        result.plasticityTime = 0.0;
        result.memoryTime = 0.0;
        result.spikeTime = 0.0;
        result.totalSpikes = 0;
        result.spikeEvents = 0;
        result.memoryUsage = 0;
        result.averageFiringRate = 0.0;
        result.stepsPerSecond = 0.0;
        result.performanceScore = 0.0;
        
        try {
            // Create configuration for benchmark
            auto config = std::make_shared<Config>();
            config->set("brain.maxNeurons", neuronCount);
            config->set("brain.learningRate", 0.01);
            config->set("brain.simulation.maxSteps", timestepCount);
            
            // Enable performance monitoring
            config->set("brain.performance.enableMonitoring", true);
            config->set("brain.performance.enableProfiling", true);
            
            // Create and initialize brain
            auto startTime = std::chrono::high_resolution_clock::now();
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw std::runtime_error("Brain initialization failed");
            }
            auto endTime = std::chrono::high_resolution_clock::now();
            
            result.initializationTime = std::chrono::duration<double>(endTime - startTime).count();
            
            // Start simulation timing
            startTime = std::chrono::high_resolution_clock::now();
            
            // Run simulation
            collectBenchmarkData(brain, timestepCount, result);
            
            endTime = std::chrono::high_resolution_clock::now();
            result.simulationTime = std::chrono::duration<double>(endTime - startTime).count();
            
            // Calculate performance metrics
            calculatePerformanceMetrics(result, neuronCount, timestepCount);
            
            metrics_.successfulBenchmarks++;
            
        } catch (const std::exception& e) {
            std::cerr << "Benchmark failed: " << e.what() << std::endl;
            metrics_.failedBenchmarks++;
        }
        
        return result;
    }
    
    ScalabilityResult runScalabilityTest() {
        ScalabilityResult result;
        
        for (size_t neuronCount : benchmarkConfig_.neuronCounts) {
            result.neuronCounts.push_back(neuronCount);
            
            // Run benchmark with increased timestep count for larger networks
            int timestepCount = std::min(100, (int)neuronCount / 10);
            
            BenchmarkResult benchResult = runBenchmark(neuronCount, timestepCount);
            
            result.initializationTimes.push_back(benchResult.initializationTime);
            result.stepTimes.push_back(benchResult.stepTime);
            result.stepsPerSecond.push_back(benchResult.stepsPerSecond);
            result.memoryUsages.push_back(benchResult.memoryUsage / 1024 / 1024); // MB
            result.firingRates.push_back(benchResult.averageFiringRate);
        }
        
        return result;
    }
    
    OptimizationResult runOptimizationTest(const std::string& optimizationName,
                                           std::function<void(Config&)> applyOptimization) {
        OptimizationResult result;
        result.optimizationName = optimizationName;
        result.speedupFactor = 1.0;
        result.memoryReduction = 0.0;
        result.timeReduction = 0.0;
        result.isViable = true;
        result.notes = "";
        
        try {
            // Baseline performance
            BenchmarkResult baseline = runBenchmark(1000, 100);
            
            // Apply optimization
            auto config = std::make_shared<Config>();
            config->set("brain.maxNeurons", 1000);
            applyOptimization(config);
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw std::runtime_error("Optimization brain initialization failed");
            }
            
            // Measure optimized performance
            BenchmarkResult optimized = runBenchmark(1000, 100);
            
            // Calculate improvements
            if (baseline.stepTime > 0.0) {
                result.speedupFactor = baseline.stepTime / optimized.stepTime;
            }
            
            if (baseline.memoryUsage > 0) {
                result.memoryReduction = (baseline.memoryUsage - optimized.memoryUsage) / (double)baseline.memoryUsage;
            }
            
            if (baseline.simulationTime > 0.0) {
                result.timeReduction = (baseline.simulationTime - optimized.simulationTime) / baseline.simulationTime;
            }
            
            result.isViable = (result.speedupFactor > 0.9) && 
                            (result.memoryReduction >= 0.0) && 
                            (result.timeReduction >= 0.0);
            
            result.notes = result.isViable ? 
                "Optimization is viable and recommended" : 
                "Optimization did not meet performance requirements";
                
        } catch (const std::exception& e) {
            result.isViable = false;
            result.notes = std::string("Optimization failed: ") + e.what();
        }
        
        return result;
    }
    
    void exportBenchmarkResults(const std::string& filename, 
                                const std::vector<BenchmarkResult>& results) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for writing: " << filename << std::endl;
            return;
        }
        
        file << "NLM Performance Benchmark Results" << std::endl;
        file << "================================" << std::endl << std::endl;
        
        file << "Neuron Count | Init Time (s) | Step Time (ms) | Steps/sec | " << std::endl;
        file << "-------------|----------------|----------------|----------|" << std::endl;
        
        for (const auto& result : results) {
            file << std::setw(11) << result.neuronCount << " | "
                 << std::setw(15) << std::fixed << std::setprecision(3) << result.initializationTime << " | "
                 << std::setw(14) << std::fixed << std::setprecision(3) << result.stepTime * 1000 << " | "
                 << std::setw(10) << std::fixed << std::setprecision(1) << result.stepsPerSecond << " |" << std::endl;
        }
        
        file << std::endl;
        file << "Performance Analysis:" << std::endl;
        file << "-----------------" << std::endl;
        
        for (size_t i = 0; i < results.size(); ++i) {
            file << "Neurons: " << results[i].neuronCount << std::endl;
            file << "  Initialization: " << results[i].initializationTime << " seconds" << std::endl;
            file << "  Step time: " << results[i].stepTime * 1000 << " ms" << std::endl;
            file << "  Steps per second: " << results[i].stepsPerSecond << std::endl;
            file << "  Memory usage: " << results[i].memoryUsage / 1024 / 1024 << " MB" << std::endl;
            file << "  Firing rate: " << results[i].averageFiringRate << " Hz" << std::endl;
            file << "  Performance score: " << results[i].performanceScore << std::endl;
            file << "  Total spikes: " << results[i].totalSpikes << std::endl;
            file << std::endl;
        }
        
        file.close();
    }
    
    void generatePerformanceReport(const std::string& filename,
                                    const ScalabilityResult& scalability,
                                    const std::vector<OptimizationResult>& optimizations) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for writing: " << filename << std::endl;
            return;
        }
        
        file << "NLM Performance Analysis Report" << std::endl;
        file << "================================" << std::endl << std::endl;
        
        file << "Scalability Analysis:" << std::endl;
        file << "------------------" << std::endl;
        
        file << "Neurons | Init Time (s) | Step Time (ms) | Steps/sec | Memory (MB) | Firing Rate (Hz)" << std::endl;
        file << "-------|----------------|----------------|----------|-------------|----------------" << std::endl;
        
        for (size_t i = 0; i < scalability.neuronCounts.size(); ++i) {
            file << std::setw(7) << scalability.neuronCounts[i] << " | "
                 << std::setw(15) << std::fixed << std::setprecision(3) << scalability.initializationTimes[i] << " | "
                 << std::setw(14) << std::fixed << std::setprecision(3) << scalability.stepTimes[i] * 1000 << " | "
                 << std::setw(10) << std::fixed << std::setprecision(1) << scalability.stepsPerSecond[i] << " | "
                 << std::setw(13) << std::fixed << std::setprecision(1) << scalability.memoryUsages[i] << " | "
                 << std::setw(14) << std::fixed << std::setprecision(1) << scalability.firingRates[i] << " |" << std::endl;
        }
        
        file << std::endl;
        file << "Optimization Results:" << std::endl;
        file << "--------------------" << std::endl;
        
        for (const auto& opt : optimizations) {
            file << "Optimization: " << opt.optimizationName << std::endl;
            file << "  Speedup factor: " << opt.speedupFactor << std::endl;
            file << "  Memory reduction: " << (opt.memoryReduction * 100) << "%" << std::endl;
            file << "  Time reduction: " << (opt.timeReduction * 100) << "%" << std::endl;
            file << "  Viable: " << (opt.isViable ? "Yes" : "No") << std::endl;
            file << "  Notes: " << opt.notes << std::endl;
            file << std::endl;
        }
        
        file.close();
    }
    
private:
    struct BenchmarkConfig {
        std::vector<size_t> neuronCounts;
        std::vector<int> timesteps;
        std::vector<int> optimizationLevels;
        int repetitions;
    };
    
    struct PerformanceThresholds {
        double maxInitializationTime;
        double maxStepTime;
        double minStepsPerSecond;
        double maxMemoryUsageMB;
        double targetPerformanceScore;
    };
    
    struct BenchmarkMetrics {
        int totalBenchmarks;
        int successfulBenchmarks;
        int failedBenchmarks;
        double totalTimeSaved;
    };
    
    BenchmarkConfig benchmarkConfig_;
    PerformanceThresholds thresholds_;
    BenchmarkMetrics metrics_;
    
    void collectBenchmarkData(std::shared_ptr<Brain> brain, int timestepCount, BenchmarkResult& result) {
        std::vector<double> stepTimes;
        uint64_t totalSpikes = 0;
        uint64_t spikeEvents = 0;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < timestepCount; ++step) {
            auto stepStart = std::chrono::high_resolution_clock::now();
            
            // Profile each component
            auto plasticityStart = std::chrono::high_resolution_clock::now();
            brain->updatePlasticity();
            auto plasticityEnd = std::chrono::high_resolution_clock::now();
            
            auto spikeStart = std::chrono::high_resolution_clock::now();
            brain->step(step);
            auto spikeEnd = std::chrono::high_resolution_clock::now();
            
            auto memoryStart = std::chrono::high_resolution_clock::now();
            // Access memory systems for memory profiling
            auto workingMemory = brain->getWorkingMemory();
            auto episodicMemory = brain->getEpisodicMemory();
            auto associativeMemory = brain->getAssociativeMemory();
            auto memoryEnd = std::chrono::high_resolution_clock::now();
            
            // Calculate timing
            double plasticityTime = std::chrono::duration<double>(plasticityEnd - plasticityStart).count();
            double spikeTime = std::chrono::duration<double>(spikeEnd - spikeStart).count();
            double memoryTime = std::chrono::duration<double>(memoryEnd - memoryStart).count();
            
            stepTimes.push_back(plasticityTime + spikeTime + memoryTime);
            totalSpikes += brain->getTotalSpikeCount();
            spikeEvents += brain->getPendingSpikeEventCount();
            
            if (step % 50 == 0) {
                std::cout << "  Step " << step << ": "
                          << "Plas: " << plasticityTime * 1000 << "ms, "
                          << "Spike: " << spikeTime * 1000 << "ms, "
                          << "Mem: " << memoryTime * 1000 << "ms, "
                          << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
            }
            
            auto stepEnd = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(stepEnd - stepStart).count();
            if (elapsed > 0.1) {  // Log if step takes too long
                std::cout << "Warning: Step " << step << " took " << elapsed * 1000 << "ms" << std::endl;
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.simulationTime = std::chrono::duration<double>(endTime - startTime).count();
        result.totalSpikes = totalSpikes;
        result.spikeEvents = spikeEvents;
        
        if (!stepTimes.empty()) {
            result.stepTime = std::accumulate(stepTimes.begin(), stepTimes.end(), 0.0) / stepTimes.size();
            double totalTime = std::accumulate(stepTimes.begin(), stepTimes.end(), 0.0);
            result.stepsPerSecond = timestepCount / totalTime;
        }
        
        // Profile memory usage
        result.memoryUsage = calculateMemoryUsage(brain);
    }
    
    void calculatePerformanceMetrics(BenchmarkResult& result, size_t neuronCount, int timestepCount) {
        // Calculate performance score based on multiple metrics
        double score = 0.0;
        
        // Step time component (lower is better)
        double stepTimeScore = std::min(1.0, 0.1 / (result.stepTime + 0.001));
        score += stepTimeScore * 0.4;
        
        // Steps per second component (higher is better)
        double spsScore = std::min(1.0, std::min(result.stepsPerSecond / 1000.0, 1.0));
        score += spsScore * 0.3;
        
        // Memory efficiency component (lower is better)
        double memoryScore = std::max(0.0, 1.0 - (result.memoryUsage / (1024 * 1024 * 10.0)));  // <10MB is good
        score += memoryScore * 0.2;
        
        // Firing rate component (reasonable range)
        double firingScore = 1.0 - std::abs(result.averageFiringRate - 10.0) / 50.0;
        firingScore = std::max(0.0, std::min(firingScore, 1.0));
        score += firingScore * 0.1;
        
        result.performanceScore = score;
    }
    
    size_t calculateMemoryUsage(std::shared_ptr<Brain> brain) {
        // Estimate memory usage based on brain components
        size_t baseMemory = 1024 * 1024;  // 1MB base
        size_t neuronMemory = brain->getTotalNeuronCount() * sizeof(float) * 4;  // ~4 bytes per neuron
        size_t synapseMemory = brain->getTotalSynapseCount() * sizeof(float) * 2;  // ~2 bytes per synapse
        size_t memorySystemMemory = 1024 * 100;  // ~100KB for memory systems
        
        return baseMemory + neuronMemory + synapseMemory + memorySystemMemory;
    }
};

int main() {
    // Initialize logging
    nlm::Logger::getInstance().setLevel(nlm::LogLevel::INFO);
    
    std::cout << "=== NLM Performance Testing Suite ===" << std::endl;
    std::cout << "Comprehensive performance analysis and optimization testing" << std::endl;
    
    PerformanceAnalyzer analyzer;
    
    // Run scalability test
    std::cout << "\n--- Running Scalability Test ---" << std::endl;
    ScalabilityResult scalability = analyzer.runScalabilityTest();
    
    std::cout << "Scalability test completed for " << scalability.neuronCounts.size() << " network sizes" << std::endl;
    
    // Run optimization tests
    std::cout << "\n--- Running Optimization Tests ---" << std::endl;
    
    std::vector<OptimizationResult> optimizations;
    
    // Memory pool optimization
    optimizations.push_back(analyzer.runOptimizationTest(
        "Memory Pools",
        [](Config& config) {
            config->set("brain.performance.useMemoryPools", true);
            config->set("brain.memory.workingMemory.capacity", 500);
        }
    ));
    
    // SIMD optimization
    optimizations.push_back(analyzer.runOptimizationTest(
        "SIMD Vectorization",
        [](Config& config) {
            config->set("brain.performance.enableSIMD", true);
            config->set("brain.performance.simdLevel", "AVX2");
        }
    ));
    
    // Activity-based optimization
    optimizations.push_back(analyzer.runOptimizationTest(
        "Activity-Based Updates",
        [](Config& config) {
            config->set("brain.performance.enableActivityBasedUpdates", true);
            config->set("brain.performance.activityThreshold", 0.1);
        }
    ));
    
    // Hierarchical processing optimization
    optimizations.push_back(analyzer.runOptimizationTest(
        "Hierarchical Processing",
        [](Config& config) {
            config->set("brain.performance.enableHierarchicalProcessing", true);
            config->set("brain.performance.numProcessorGroups", 4);
        }
    ));
    
    // Export benchmark results
    std::vector<BenchmarkResult> benchmarks;
    for (size_t neuronCount : analyzer.benchmarkConfig_.neuronCounts) {
        if (neuronCount <= 1000) {  // Limit for demonstration
            benchmarks.push_back(analyzer.runBenchmark(neuronCount, 100));
        }
    }
    
    analyzer.exportBenchmarkResults("benchmark_results.csv", benchmarks);
    
    analyzer.generatePerformanceReport("performance_analysis_report.txt", scalability, optimizations);
    
    // Display summary
    std::cout << "\n=== Performance Test Summary ===" << std::endl;
    std::cout << "Total benchmarks run: " << analyzer.metrics_.totalBenchmarks << std::endl;
    std::cout << "Successful benchmarks: " << analyzer.metrics_.successfulBenchmarks << std::endl;
    std::cout << "Failed benchmarks: " << analyzer.metrics_.failedBenchmarks << std::endl;
    
    std::cout << "\nOptimization Results:" << std::endl;
    for (const auto& opt : optimizations) {
        std::cout << "  " << opt.optimizationName << ": ";
        if (opt.isViable) {
            std::cout << "SPEEDUP x" << std::fixed << std::setprecision(2) << opt.speedupFactor << " ";
            std::cout << "MEMORY REDUCTION " << (opt.memoryReduction * 100) << "% ";
            std::cout << "TIME REDUCTION " << (opt.timeReduction * 100) << "%";
        } else {
            std::cout << "NOT VIABLE";
        }
        std::cout << " - " << opt.notes << std::endl;
    }
    
    std::cout << "\n=== Performance Analysis Complete ===" << std::endl;
    std::cout << "Performance testing completed successfully!" << std::endl;
    std::cout << "Benchmark results saved to: benchmark_results.csv" << std::endl;
    std::cout << "Detailed report saved to: performance_analysis_report.txt" << std::endl;
    
    // Performance recommendations
    std::cout << "\n=== Performance Recommendations ===" << std::endl;
    std::cout << "1. Use memory pools for allocation-heavy operations" << std::endl;
    std::cout << "2. Enable SIMD optimization for large networks" << std::endl;
    std::cout << "3. Implement activity-based updates for efficiency" << std::endl;
    std::cout << "4. Use hierarchical processing for better cache locality" << std::endl;
    std::cout << "5. Monitor and adjust time step based on activity" << std::endl;
    std::cout << "6. Implement spike compression for large networks" << std::endl;
    std::cout << "7. Use predictive memory access patterns" << std::endl;
    
    return 0;
}