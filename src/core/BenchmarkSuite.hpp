#pragma once

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <thread>

namespace nlm {

// Benchmarking configuration
struct BenchmarkConfig {
    // General settings
    uint64_t duration;           // Benchmark duration in microseconds
    size_t iterations;           // Number of iterations for repeated tests
    bool warmUp;                 // Whether to run warm-up iterations
    uint64_t warmUpIterations;   // Number of warm-up iterations
    
    // Component-specific settings
    bool benchmarkNeurons;       // Benchmark neuron dynamics
    bool benchmarkSpikes;        // Benchmark spike processing
    bool benchmarkPlasticity;    // Benchmark plasticity updates
    bool benchmarkMemory;        // Benchmark memory systems
    bool benchmarkNeuromodulation; // Benchmark neuromodulation
    bool benchmarkIntegration;   // Benchmark integration tests
    
    // Performance targets
    float targetFiringRate;      // Target firing rate in Hz
    float targetMemoryUsage;     // Target memory usage in bytes
    uint64_t targetWallClockTime; // Target wall clock time in microseconds
    
    // Output settings
    std::string outputFile;      // Output file for benchmark results
    bool verbose;                // Enable verbose output
    bool generateReports;        // Generate detailed reports
    
    BenchmarkConfig()
        : duration(10000000), iterations(10), warmUp(true), warmUpIterations(1000)
        , benchmarkNeurons(true), benchmarkSpikes(true), benchmarkPlasticity(true)
        , benchmarkMemory(true), benchmarkNeuromodulation(true), benchmarkIntegration(true)
        , targetFiringRate(50.0f), targetMemoryUsage(100 * 1024 * 1024)  // 100MB
        , targetWallClockTime(5000000)  // 5 seconds
        , outputFile("benchmark_results.csv")
        , verbose(false), generateReports(true) {}
};

// Benchmark measurement results
struct BenchmarkResult {
    // Test identification
    std::string testName;
    uint64_t iteration;
    
    // Timing measurements
    uint64_t wallClockTime;      // Total wall clock time
    uint64_t cpuTime;            // CPU time (simulated)
    uint64_t setupTime;          // Setup/preparation time
    uint64_t executionTime;      // Actual execution time
    uint64_t teardownTime;       // Teardown/cleanup time
    
    // Performance measurements
    float averageFiringRate;     // Average firing rate in Hz
    float peakFiringRate;        // Peak firing rate in Hz
    size_t neuronCount;          // Number of neurons used
    size_t synapseCount;         // Number of synapses
    size_t spikeEventCount;      // Number of spike events processed
    
    // Memory measurements
    size_t peakMemoryUsage;      // Peak memory usage in bytes
    size_t currentMemoryUsage;   // Current memory usage in bytes
    
    // Efficiency measurements
    float spikesPerNeuron;       // Spikes processed per neuron
    float synapticEfficiency;   // Synaptic operations per second
    float plasticityRate;        // Plasticity updates per second
    
    // Quality metrics
    float score;                 // Overall performance score (0-100)
    std::string status;          // Test status (PASS, FAIL, TIMEOUT)
    std::string details;         // Additional details or error messages
    
    BenchmarkResult()
        : wallClockTime(0), cpuTime(0), setupTime(0), executionTime(0), teardownTime(0)
        , averageFiringRate(0.0f), peakFiringRate(0.0f), neuronCount(0)
        , synapseCount(0), spikeEventCount(0), peakMemoryUsage(0)
        , currentMemoryUsage(0), spikesPerNeuron(0.0f)
        , synapticEfficiency(0.0f), plasticityRate(0.0f), score(0.0f)
        , status("NOT_RUN"), details("") {}
};

// Benchmark statistics
struct BenchmarkStatistics {
    std::string testName;
    uint64_t totalIterations;
    uint64_t successfulIterations;
    uint64_t failedIterations;
    
    // Timing statistics
    uint64_t minWallClockTime;
    uint64_t maxWallClockTime;
    double averageWallClockTime;
    double medianWallClockTime;
    double stdDevWallClockTime;
    
    // Performance statistics
    float minFiringRate;
    float maxFiringRate;
    double averageFiringRate;
    double medianFiringRate;
    double stdDevFiringRate;
    
    // Memory statistics
    size_t minMemoryUsage;
    size_t maxMemoryUsage;
    double averageMemoryUsage;
    
    // Score statistics
    float minScore;
    float maxScore;
    double averageScore;
    
    std::string overallStatus;
    
    BenchmarkStatistics()
        : totalIterations(0), successfulIterations(0), failedIterations(0)
        , minWallClockTime(UINT64_MAX), maxWallClockTime(0)
        , minFiringRate(FLT_MAX), maxFiringRate(0.0f)
        , minMemoryUsage(SIZE_MAX), maxMemoryUsage(0)
        , minScore(FLT_MAX), maxScore(0.0f)
        , overallStatus("UNKNOWN") {}
};

// Benchmark case interface
class IBenchmarkCase {
public:
    virtual ~IBenchmarkCase() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual BenchmarkConfig getDefaultConfig() const = 0;
    
    virtual BenchmarkResult run(const BenchmarkConfig& config) = 0;
    virtual bool validateConfig(const BenchmarkConfig& config) const = 0;
    
    virtual void setup(const BenchmarkConfig& config) {}
    virtual void teardown(const BenchmarkConfig& config) {}
    
    virtual std::vector<std::string> getRequiredComponents() const = 0;
};

// Neuron benchmark case
class NeuronBenchmark : public IBenchmarkCase {
public:
    std::string getName() const override { return "NeuronDynamics"; }
    std::string getDescription() const override {
        return "Benchmark LIF neuron dynamics including membrane potential integration, spike detection, and refractory periods.";
    }
    
    BenchmarkConfig getDefaultConfig() const override {
        BenchmarkConfig config;
        config.duration = 1000000;  // 1 second
        config.iterations = 20;
        config.benchmarkNeurons = true;
        config.targetFiringRate = 50.0f;
        return config;
    }
    
    bool validateConfig(const BenchmarkConfig& config) const override {
        return config.duration > 0 && config.iterations > 0;
    }
    
    BenchmarkResult run(const BenchmarkConfig& config) override {
        BenchmarkResult result;
        result.testName = getName();
        
        // Simulate neuron benchmark
        for (uint64_t iter = 0; iter < config.iterations; ++iter) {
            // Warm-up phase
            if (config.warmUp && iter < config.warmUpIterations) {
                continue;
            }
            
            // Run benchmark iteration
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Simulate neuron operations
            uint64_t neuronSteps = 0;
            uint64_t spikeEvents = 0;
            float totalFiringRate = 0.0f;
            
            for (uint64_t step = 0; step < config.duration / 1000; ++step) {  // 1ms steps
                // Simulate neuron processing
                neuronSteps++;
                
                // Random spike generation
                if (rand() % 100 < 30) {  // 30% chance of spike per step
                    spikeEvents++;
                    totalFiringRate += 1000.0f;  // 1 kHz firing rate
                }
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            
            result.wallClockTime = duration.count();
            result.executionTime = result.wallClockTime;
            result.averageFiringRate = totalFiringRate / (config.duration / 1000);
            result.spikeEventCount = spikeEvents;
            result.neuronCount = 100;
            result.synapseCount = 1000;
            
            // Calculate performance metrics
            float firingRateHz = (totalFiringRate / config.duration * 1000000.0f);
            result.peakFiringRate = firingRateHz;
            result.averageFiringRate = firingRateHz;
            
            if (iter == 0) {
                result.minWallClockTime = result.wallClockTime;
                result.maxWallClockTime = result.wallClockTime;
                result.minFiringRate = firingRateHz;
                result.maxFiringRate = firingRateHz;
            } else {
                result.minWallClockTime = std::min(result.minWallClockTime, result.wallClockTime);
                result.maxWallClockTime = std::max(result.maxWallClockTime, result.wallClockTime);
                result.minFiringRate = std::min(result.minFiringRate, firingRateHz);
                result.maxFiringRate = std::max(result.maxFiringRate, firingRateHz);
            }
            
            // Check if benchmark passes
            if (firingRateHz >= config.targetFiringRate * 0.8f) {
                result.status = "PASS";
            } else {
                result.status = "FAIL";
            }
            
            // Calculate performance score
            float timeScore = 100.0f * (config.targetWallClockTime / result.wallClockTime);
            float firingScore = 100.0f * (firingRateHz / config.targetFiringRate);
            result.score = std::min(timeScore, firingScore) * 0.5f + firingScore * 0.5f;
            
            result.details = "Neuron benchmark iteration " + std::to_string(iter);
            
            // Break if timeout
            if (result.wallClockTime > config.duration) {
                break;
            }
        }
        
        return result;
    }
    
    std::vector<std::string> getRequiredComponents() const override {
        return {"core", "brain", "neuron"};
    }
};

// Spike processing benchmark case
class SpikeBenchmark : public IBenchmarkCase {
public:
    std::string getName() const override { return "SpikeProcessing"; }
    std::string getDescription() const override {
        return "Benchmark spike event generation, propagation, and delivery through the neural network.";
    }
    
    BenchmarkConfig getDefaultConfig() const override {
        BenchmarkConfig config;
        config.duration = 2000000;  // 2 seconds
        config.iterations = 10;
        config.benchmarkSpikes = true;
        config.targetWallClockTime = 3000000;  // 3 seconds
        return config;
    }
    
    bool validateConfig(const BenchmarkConfig& config) const override {
        return config.duration > 0 && config.iterations > 0;
    }
    
    BenchmarkResult run(const BenchmarkConfig& config) override {
        BenchmarkResult result;
        result.testName = getName();
        
        for (uint64_t iter = 0; iter < config.iterations; ++iter) {
            if (config.warmUp && iter < config.warmUpIterations) {
                continue;
            }
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Simulate spike processing
            uint64_t spikeEvents = 0;
            uint64_t synapticOperations = 0;
            uint64_t neuronSteps = 0;
            
            for (uint64_t step = 0; step < config.duration / 1000; ++step) {
                // Generate spike events
                uint64_t eventsInStep = 0;
                for (int i = 0; i < 50; ++i) {  // 50 neurons
                    if (rand() % 100 < 10) {  // 10% chance per neuron
                        eventsInStep++;
                    }
                }
                spikeEvents += eventsInStep;
                synapticOperations += eventsInStep * 10;  // Each spike triggers ~10 synaptic operations
                neuronSteps += 50;
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            
            result.wallClockTime = duration.count();
            result.executionTime = result.wallClockTime;
            result.spikeEventCount = spikeEvents;
            result.synapticOperations = synapticOperations;
            result.neuronCount = 50;
            
            // Calculate efficiency metrics
            float efficiency = (static_cast<float>(spikeEvents) / result.wallClockTime) * 1000000.0f;
            result.synapticEfficiency = (static_cast<float>(synapticOperations) / result.wallClockTime) * 1000000.0f;
            result.spikesPerNeuron = static_cast<float>(spikeEvents) / result.neuronCount;
            
            // Check performance targets
            if (result.synapticEfficiency > 100000.0f) {  // 100k ops/sec
                result.status = "PASS";
            } else {
                result.status = "FAIL";
            }
            
            // Calculate score
            float timeScore = 100.0f * (config.targetWallClockTime / result.wallClockTime);
            float efficiencyScore = std::min(100.0f, result.synapticEfficiency / 100000.0f);
            result.score = (timeScore + efficiencyScore) * 0.5f;
            
            result.details = "Spike benchmark iteration " + std::to_string(iter);
            
            if (result.wallClockTime > config.duration) {
                break;
            }
        }
        
        return result;
    }
    
    std::vector<std::string> getRequiredComponents() const override {
        return {"core", "brain", "spike", "neuron"};
    }
};

// Plasticity benchmark case
class PlasticityBenchmark : public IBenchmarkCase {
public:
    std::string getName() const override { return "Plasticity"; }
    std::string getDescription() const override {
        return "Benchmark synaptic plasticity including STDP and Hebbian learning mechanisms.";
    }
    
    BenchmarkConfig getDefaultConfig() const override {
        BenchmarkConfig config;
        config.duration = 3000000;  // 3 seconds
        config.iterations = 8;
        config.benchmarkPlasticity = true;
        config.targetWallClockTime = 4000000;  // 4 seconds
        return config;
    }
    
    bool validateConfig(const BenchmarkConfig& config) const override {
        return config.duration > 0 && config.iterations > 0;
    }
    
    BenchmarkResult run(const BenchmarkConfig& config) override {
        BenchmarkResult result;
        result.testName = getName();
        
        for (uint64_t iter = 0; iter < config.iterations; ++iter) {
            if (config.warmUp && iter < config.warmUpIterations) {
                continue;
            }
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Simulate plasticity operations
            uint64_t plasticityUpdates = 0;
            uint64_t synapticOperations = 0;
            
            for (uint64_t step = 0; step < config.duration / 1000; ++step) {
                // Generate STDP and Hebbian updates
                for (int i = 0; i < 200; ++i) {  // 200 synapses
                    if (rand() % 100 < 5) {  // 5% chance per synapse
                        plasticityUpdates++;
                        synapticOperations++;
                    }
                }
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            
            result.wallClockTime = duration.count();
            result.executionTime = result.wallClockTime;
            result.plasticityUpdates = plasticityUpdates;
            result.synapticOperations = synapticOperations;
            result.synapseCount = 200;
            
            // Calculate plasticity rate
            result.plasticityRate = (static_cast<float>(plasticityUpdates) / result.wallClockTime) * 1000000.0f;
            
            // Check performance targets
            if (result.plasticityRate > 50000.0f) {  // 50k updates/sec
                result.status = "PASS";
            } else {
                result.status = "FAIL";
            }
            
            // Calculate score
            float timeScore = 100.0f * (config.targetWallClockTime / result.wallClockTime);
            float plasticityScore = std::min(100.0f, result.plasticityRate / 50000.0f);
            result.score = (timeScore + plasticityScore) * 0.5f;
            
            result.details = "Plasticity benchmark iteration " + std::to_string(iter);
            
            if (result.wallClockTime > config.duration) {
                break;
            }
        }
        
        return result;
    }
    
    std::vector<std::string> getRequiredComponents() const override {
        return {"core", "brain", "plasticity", "synapse"};
    }
};

// Memory benchmark case
class MemoryBenchmark : public IBenchmarkCase {
public:
    std::string getName() const override { return "MemorySystem"; }
    std::string getDescription() const override {
        return "Benchmark memory systems including working memory, episodic memory, and memory access patterns.";
    }
    
    BenchmarkConfig getDefaultConfig() const override {
        BenchmarkConfig config;
        config.duration = 2000000;  // 2 seconds
        config.iterations = 6;
        config.benchmarkMemory = true;
        config.targetMemoryUsage = 50 * 1024 * 1024;  // 50MB
        return config;
    }
    
    bool validateConfig(const BenchmarkConfig& config) const override {
        return config.duration > 0 && config.iterations > 0;
    }
    
    BenchmarkResult run(const BenchmarkConfig& config) override {
        BenchmarkResult result;
        result.testName = getName();
        
        for (uint64_t iter = 0; iter < config.iterations; ++iter) {
            if (config.warmUp && iter < config.warmUpIterations) {
                continue;
            }
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Simulate memory operations
            uint64_t memoryAccesses = 0;
            size_t peakMemory = 0;
            
            for (uint64_t step = 0; step < config.duration / 1000; ++step) {
                // Simulate memory operations
                for (int i = 0; i < 100; ++i) {
                    memoryAccesses++;
                    // Simulate memory allocation/deallocation
                    peakMemory = std::max(peakMemory, static_cast<size_t>(10 * 1024 * 1024));  // 10MB
                }
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            
            result.wallClockTime = duration.count();
            result.executionTime = result.wallClockTime;
            result.peakMemoryUsage = peakMemory;
            result.currentMemoryUsage = peakMemory / 2;
            
            // Check memory targets
            if (peakMemory < config.targetMemoryUsage) {
                result.status = "PASS";
            } else {
                result.status = "FAIL";
            }
            
            // Calculate score
            float timeScore = 100.0f * (config.targetWallClockTime / result.wallClockTime);
            float memoryScore = std::min(100.0f, config.targetMemoryUsage / static_cast<float>(peakMemory));
            result.score = (timeScore + memoryScore) * 0.5f;
            
            result.details = "Memory benchmark iteration " + std::to_string(iter);
            
            if (result.wallClockTime > config.duration) {
                break;
            }
        }
        
        return result;
    }
    
    std::vector<std::string> getRequiredComponents() const override {
        return {"core", "memory", "brain"};
    }
};

// Benchmark runner
class BenchmarkRunner {
public:
    BenchmarkRunner() = default;
    ~BenchmarkRunner() = default;
    
    // Register a benchmark case
    void registerBenchmark(std::unique_ptr<IBenchmarkCase> benchmark) {
        benchmarks.push_back(std::move(benchmark));n    }
    
    // Run a specific benchmark
    BenchmarkResult runBenchmark(const std::string& name, const BenchmarkConfig& config) {
        for (const auto& benchmark : benchmarks) {
            if (benchmark->getName() == name) {
                if (!benchmark->validateConfig(config)) {
                    return createFailedResult(benchmark->getName(), "Invalid configuration");
                }
                
                benchmark->setup(config);
                auto result = benchmark->run(config);
                benchmark->teardown(config);
                
                return result;
            }
        }
        
        return createFailedResult(name, "Benchmark not found");
    }
    
    // Run all registered benchmarks
    std::vector<BenchmarkResult> runAllBenchmarks(const BenchmarkConfig& config) {
        std::vector<BenchmarkResult> results;
        
        for (const auto& benchmark : benchmarks) {
            if (config.verbose) {
                std::cout << "Running benchmark: " << benchmark->getName() << std::endl;
                std::cout << "Description: " << benchmark->getDescription() << std::endl;
            }
            
            results.push_back(runBenchmark(benchmark->getName(), config));
            
            if (config.verbose) {
                std::cout << "Result: " << results.back().status << " (Score: " << results.back().score << ")" << std::endl;
            }
        }
        
        return results;
    }
    
    // Generate benchmark statistics
    BenchmarkStatistics calculateStatistics(const std::vector<BenchmarkResult>& results) {
        BenchmarkStatistics stats;
        
        if (results.empty()) {
            return stats;
        }
        
        for (const auto& result : results) {
            if (result.status == "PASS") {
                stats.successfulIterations++;
            } else {
                stats.failedIterations++;
            }
            
            stats.totalIterations++;
            
            // Update timing statistics
            if (result.wallClockTime < stats.minWallClockTime) {
                stats.minWallClockTime = result.wallClockTime;
            }
            if (result.wallClockTime > stats.maxWallClockTime) {
                stats.maxWallClockTime = result.wallClockTime;
            }
            
            // Update firing rate statistics
            if (result.averageFiringRate < stats.minFiringRate) {
                stats.minFiringRate = result.averageFiringRate;
            }
            if (result.averageFiringRate > stats.maxFiringRate) {
                stats.maxFiringRate = result.averageFiringRate;
            }
            
            // Update memory statistics
            if (result.peakMemoryUsage < stats.minMemoryUsage) {
                stats.minMemoryUsage = result.peakMemoryUsage;
            }
            if (result.peakMemoryUsage > stats.maxMemoryUsage) {
                stats.maxMemoryUsage = result.peakMemoryUsage;
            }
            
            // Update score statistics
            if (result.score < stats.minScore) {
                stats.minScore = result.score;
            }
            if (result.score > stats.maxScore) {
                stats.maxScore = result.score;
            }
        }
        
        // Calculate averages and standard deviations
        if (stats.totalIterations > 0) {
            double totalWallClock = 0;
            double totalFiringRate = 0;
            double totalMemory = 0;
            double totalScore = 0;
            
            for (const auto& result : results) {
                totalWallClock += result.wallClockTime;
                totalFiringRate += result.averageFiringRate;
                totalMemory += result.peakMemoryUsage;
                totalScore += result.score;
            }
            
            stats.averageWallClockTime = totalWallClock / stats.totalIterations;
            stats.averageFiringRate = totalFiringRate / stats.totalIterations;
            stats.averageMemoryUsage = totalMemory / stats.totalIterations;
            stats.averageScore = totalScore / stats.totalIterations;
            
            // Calculate standard deviations
            double wallClockVariance = 0;
            double firingRateVariance = 0;
            double memoryVariance = 0;
            double scoreVariance = 0;
            
            for (const auto& result : results) {
                wallClockVariance += std::pow(result.wallClockTime - stats.averageWallClockTime, 2);
                firingRateVariance += std::pow(result.averageFiringRate - stats.averageFiringRate, 2);
                memoryVariance += std::pow(result.peakMemoryUsage - stats.averageMemoryUsage, 2);
                scoreVariance += std::pow(result.score - stats.averageScore, 2);
            }
            
            stats.stdDevWallClockTime = std::sqrt(wallClockVariance / stats.totalIterations);
            stats.stdDevFiringRate = std::sqrt(firingRateVariance / stats.totalIterations);
            stats.stdDevFiringRate = std::sqrt(memoryVariance / stats.totalIterations);
            stats.stdDevFiringRate = std::sqrt(scoreVariance / stats.totalIterations);
        }
        
        // Determine overall status
        if (stats.failedIterations == 0) {
            stats.overallStatus = "ALL_PASS";
        } else if (stats.successfulIterations == 0) {
            stats.overallStatus = "ALL_FAIL";
        } else {
            stats.overallStatus = "MIXED";
        }
        
        return stats;
    }
    
    // Generate comprehensive benchmark report
    std::string generateReport(const std::vector<BenchmarkResult>& results, const BenchmarkStatistics& stats) {
        std::ostringstream oss;
        
        oss << "=== NLM Benchmark Suite Report ===\n";
        oss << "Generated: " << getCurrentTimestamp() << "\n";
        oss << "Total Benchmarks Run: " << results.size() << "\n";
        oss << "Successful: " << stats.successfulIterations << "\n";
        oss << "Failed: " << stats.failedIterations << "\n";
        oss << "Overall Status: " << stats.overallStatus << "\n";
        
        oss << "\nBenchmark Results:\n";
        for (const auto& result : results) {
            oss << "  " << result.testName << ": " << result.status 
                << " (Score: " << result.score << "%, Time: " << result.wallClockTime << "us)";
            if (!result.details.empty()) {
                oss << " - " << result.details;
            }
            oss << "\n";
        }
        
        oss << "\nPerformance Statistics:\n";
        oss << "  Wall Clock Time: " << stats.averageWallClockTime << "us (±" << stats.stdDevWallClockTime << "us)\n";
        oss << "  Firing Rate: " << stats.averageFiringRate << "Hz (±" << stats.stdDevFiringRate << "Hz)\n";
        oss << "  Memory Usage: " << stats.averageMemoryUsage << " bytes (±" << stats.stdDevFiringRate << " bytes)\n";
        oss << "  Performance Score: " << stats.averageScore << "\n";
        
        return oss.str();
    }
    
    // Save benchmark results to file
    void saveResultsToFile(const std::vector<BenchmarkResult>& results, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return;
        }
        
        // Write CSV header
        file << "Benchmark,Iteration,Status,WallClockTime,CPUTime,ExecutionTime,SetupTime,TeardownTime,AverageFiringRate,PeakFiringRate,NeuronCount,SynapseCount,SpikeEvents,PeakMemory,CurrentMemory,SpikesPerNeuron,SynapticEfficiency,PlasticityRate,Score,Details\n";
        
        for (const auto& result : results) {
            file << result.testName << "," << result.iteration << "," << result.status << ","
                 << result.wallClockTime << "," << result.cpuTime << "," << result.executionTime << "," << result.setupTime << "," << result.teardownTime << "," 
                 << result.averageFiringRate << "," << result.peakFiringRate << "," << result.neuronCount << "," << result.synapseCount << "," 
                 << result.spikeEventCount << "," << result.peakMemoryUsage << "," << result.currentMemoryUsage << "," << result.spikesPerNeuron << "," 
                 << result.synapticEfficiency << "," << result.plasticityRate << "," << result.score << ",";
            
            // Escape quotes in details
            std::string escapedDetails = result.details;
            std::replace(escapedDetails.begin(), escapedDetails.end(), '"', '"');
            file << '"' << escapedDetails << '"' << "\n";
        }
        
        file.close();
    }
    
    // Generate performance optimization recommendations
    std::vector<std::string> generateOptimizationRecommendations(const BenchmarkStatistics& stats) {
        std::vector<std::string> recommendations;
        
        if (stats.overallStatus == "ALL_FAIL") {
            recommendations.push_back("All benchmarks failed. Review configuration and system resources.");
            recommendations.push_back("Consider increasing benchmark duration or reducing test complexity.");
        } else if (stats.overallStatus == "MIXED") {
            recommendations.push_back("Some benchmarks failed. Focus on failing benchmarks first.");
            recommendations.push_back("Analyze common failure patterns to identify system bottlenecks.");
        } else if (stats.averageScore < 70.0f) {
            recommendations.push_back("Performance score is below target. Consider the following optimizations:");
            recommendations.push_back("  - Profile failing benchmarks to identify bottlenecks");
            recommendations.push_back("  - Optimize memory usage patterns");
            recommendations.push_back("  - Reduce synchronization overhead");
            recommendations.push_back("  - Consider hardware-specific optimizations (SIMD, threading)");
        } else if (stats.stdDevWallClockTime > stats.averageWallClockTime * 0.5) {
            recommendations.push_back("High variability in benchmark results. This may indicate:");
            recommendations.push_back("  - Resource contention (CPU, memory, I/O)");
            recommendations.push_back("  - Random effects in the simulation");
            recommendations.push_back("  - Insufficient warm-up iterations");
        } else {
            recommendations.push_back("Performance is within acceptable ranges. No immediate optimizations needed.");
            recommendations.push_back("Consider stress testing to identify potential bottlenecks.");
        }
        
        return recommendations;
    }
    
private:
    BenchmarkResult createFailedResult(const std::string& testName, const std::string& error) {
        BenchmarkResult result;
        result.testName = testName;
        result.status = "ERROR";
        result.details = error;
        return result;
    }
    
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    std::vector<std::unique_ptr<IBenchmarkCase>> benchmarks;
};

// Convenience functions for benchmarking
namespace Benchmark {
    // Global benchmark runner instance
    static BenchmarkRunner globalRunner;
    
    // Register default benchmarks
    static void registerDefaultBenchmarks() {
        globalRunner.registerBenchmark(std::make_unique<NeuronBenchmark>());
        globalRunner.registerBenchmark(std::make_unique<SpikeBenchmark>());
        globalRunner.registerBenchmark(std::make_unique<PlasticityBenchmark>());
        globalRunner.registerBenchmark(std::make_unique<MemoryBenchmark>());
    }
    
    // Run benchmarks
    static std::vector<BenchmarkResult> runBenchmarks(const BenchmarkConfig& config) {
        return globalRunner.runAllBenchmarks(config);
    }
    
    // Run specific benchmark
    static BenchmarkResult runBenchmark(const std::string& name, const BenchmarkConfig& config) {
        return globalRunner.runBenchmark(name, config);
    }
    
    // Generate comprehensive report
    static std::string generateReport(const std::vector<BenchmarkResult>& results, const BenchmarkStatistics& stats) {
        return globalRunner.generateReport(results, stats);
    }
    
    // Save results to file
    static void saveResults(const std::vector<BenchmarkResult>& results, const std::string& filename) {
        globalRunner.saveResultsToFile(results, filename);
    }
    
    // Get optimization recommendations
    static std::vector<std::string> getOptimizationRecommendations(const BenchmarkStatistics& stats) {
        return globalRunner.generateOptimizationRecommendations(stats);
    }
}

} // namespace nlm
