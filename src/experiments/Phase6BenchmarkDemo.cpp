/**
 * Phase 6 Benchmark Demo
 * 
 * This demo runs comprehensive performance benchmarks on the Phase 6 integration
 * to measure performance, scalability, and efficiency of the integrated brain systems.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <numeric>

using namespace nlm;

/**
 * Simple benchmark timer utility
 */
class BenchmarkTimer {
public:
    BenchmarkTimer() : startTime_(std::chrono::high_resolution_clock::now()) {}
    
    ~BenchmarkTimer() = default;
    
    double elapsed() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(endTime - startTime_).count();
    }
    
    void reset() {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
private:
    std::chrono::high_resolution_clock::time_point startTime_;
};

/**
 * Phase 6 Benchmark Demo
 */
class Phase6BenchmarkDemo {
public:
    Phase6BenchmarkDemo() {}
    ~Phase6BenchmarkDemo() {}
    
    void runBenchmarks() {
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 6 BENCHMARK DEMO\n";
        std::cout << "===========================================\n\n";
        
        // Define benchmark configurations
        std::vector<Phase6BenchmarkConfig> configs = getBenchmarkConfigs();
        
        std::cout << "Running benchmark suite with " << configs.size() << " configurations...\n\n";
        
        // Run benchmarks
        std::vector<Phase6BenchmarkResult> results;
        for (size_t i = 0; i < configs.size(); ++i) {
            std::cout << "--- Benchmark Configuration " << (i + 1) << " ---\n";
            
            Phase6BenchmarkConfig config = configs[i];
            Phase6BenchmarkResult result = runSingleBenchmark(config);
            
            results.push_back(result);
            
            printBenchmarkResults(result, "Configuration " + std::to_string(i + 1));
            std::cout << std::endl;
        }
        
        // Print summary
        printBenchmarkSuiteResults(results);
        
        // Generate performance report
        generatePerformanceReport(results);
    }

private:
    std::vector<Phase6BenchmarkConfig> getBenchmarkConfigs() {
        std::vector<Phase6BenchmarkConfig> configs;
        
        // Small configuration for quick testing
        Phase6BenchmarkConfig smallConfig;
        smallConfig.maxSteps = 1000;
        smallConfig.neuronCount = 200;
        smallConfig.regionCount = 1;
        smallConfig.connectionProbability = 0.1f;
        smallConfig.enableCheckpointing = true;
        smallConfig.enableReplay = false;
        smallConfig.enableDevelopment = true;
        configs.push_back(smallConfig);
        
        // Medium configuration for balanced testing
        Phase6BenchmarkConfig mediumConfig;
        mediumConfig.maxSteps = 5000;
        mediumConfig.neuronCount = 500;
        mediumConfig.regionCount = 2;
        mediumConfig.connectionProbability = 0.1f;
        mediumConfig.enableCheckpointing = true;
        mediumConfig.enableReplay = true;
        mediumConfig.enableDevelopment = true;
        configs.push_back(mediumConfig);
        
        // Large configuration for stress testing
        Phase6BenchmarkConfig largeConfig;
        largeConfig.maxSteps = 10000;
        largeConfig.neuronCount = 1000;
        largeConfig.regionCount = 3;
        largeConfig.connectionProbability = 0.1f;
        largeConfig.enableCheckpointing = true;
        largeConfig.enableReplay = true;
        largeConfig.enableDevelopment = true;
        configs.push_back(largeConfig);
        
        return configs;
    }
    
    Phase6BenchmarkResult runSingleBenchmark(const Phase6BenchmarkConfig& config) {
        Phase6BenchmarkResult result;
        
        std::cout << "Benchmark config: " << config.neuronCount << " neurons, " 
                  << config.maxSteps << " steps, checkpointing: " 
                  << (config.enableCheckpointing ? "yes" : "no") << "\n";
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Initialize logging
        Logger::getInstance().setLevel(Logger::Level::Error); // Reduce log output for benchmarks
        
        // Create integration experiment
        Phase6IntegratedExperiment experiment;
        
        // Phase 1: Integration verification (quick check)
        auto verificationStart = std::chrono::high_resolution_clock::now();
        bool integrationOK = experiment.verifyIntegration();
        auto verificationEnd = std::chrono::high_resolution_clock::now();
        
        result.totalWallClockTime = std::chrono::duration<double>(verificationEnd - verificationStart).count();
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            return result;
        }
        
        // Phase 2: Memory integration test
        auto memoryStart = std::chrono::high_resolution_clock::now();
        experiment.testMemoryIntegration();
        auto memoryEnd = std::chrono::high_resolution_clock::now();
        
        // Phase 3: Neuromodulation integration test
        auto neuromodStart = std::chrono::high_resolution_clock::now();
        experiment.testNeuromodulationIntegration();
        auto neuromodEnd = std::chrono::high_resolution_clock::now();
        
        // Phase 4: Checkpoint save/load test
        auto checkpointStart = std::chrono::high_resolution_clock::now();
        if (config.enableCheckpointing) {
            experiment.testCheckpointing();
        }
        auto checkpointEnd = std::chrono::high_resolution_clock::now();
        
        result.checkpointTime = std::chrono::duration<double>(checkpointEnd - checkpointStart).count();
        
        // Phase 5: Full integration experiment
        auto experimentStart = std::chrono::high_resolution_clock::now();
        Phase6Config expConfig;
        expConfig.maxSteps = config.maxSteps;
        expConfig.neuronCount = config.neuronCount;
        expConfig.regionCount = config.regionCount;
        expConfig.connectionProbability = config.connectionProbability;
        expConfig.enableCheckpointing = config.enableCheckpointing;
        expConfig.enableReplay = config.enableReplay;
        expConfig.enableDevelopment = config.enableDevelopment;
        expConfig.checkpointPath = config.checkpointPath;
        
        Phase6IntegrationResult integrationResult = experiment.run(expConfig);
        auto experimentEnd = std::chrono::high_resolution_clock::now();
        
        // Calculate metrics
        double totalExperimentTime = std::chrono::duration<double>(experimentEnd - experimentStart).count();
        result.simulationTime = totalExperimentTime;
        
        // Calculate throughput metrics
        result.stepsPerSecond = config.maxSteps / totalExperimentTime;
        result.spikesPerSecond = integrationResult.totalSpikeCount / totalExperimentTime;
        result.synapticChangesPerSecond = integrationResult.totalSynapticChanges / totalExperimentTime;
        
        // Set benchmark results
        result.totalWallClockTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count();
        result.totalSpikesGenerated = integrationResult.totalSpikeCount;
        result.totalSynapticChanges = integrationResult.totalSynapticChanges;
        result.totalMemoryOperations = integrationResult.totalMemoryOperations;
        result.totalNeuromodulationOperations = integrationResult.totalNeuromodulationOperations;
        
        return result;
    }
    
    void printBenchmarkResults(const Phase6BenchmarkResult& result, const std::string& description) {
        std::cout << description << " Results:\n";
        std::cout << "  Total time:          " << std::fixed << std::setprecision(3) << result.totalWallClockTime << "s\n";
        std::cout << "  Simulation time:     " << std::fixed << std::setprecision(3) << result.simulationTime << "s\n";
        std::cout << "  Checkpoint time:     " << std::fixed << std::setprecision(3) << result.checkpointTime << "s\n";
        std::cout << "  Steps/second:        " << std::fixed << std::setprecision(1) << result.stepsPerSecond << "\n";
        std::cout << "  Spikes/second:       " << std::fixed << std::setprecision(1) << result.spikesPerSecond << "\n";
        std::cout << "  Syn changes/sec:     " << std::fixed << std::setprecision(1) << result.synapticChangesPerSecond << "\n";
        std::cout << "  Total spikes:         " << result.totalSpikesGenerated << "\n";
        std::cout << "  Total synaptic changes: " << result.totalSynapticChanges << "\n";
        std::cout << "  Memory operations:   " << result.totalMemoryOperations << "\n";
        std::cout << "  Neuromod ops:        " << result.totalNeuromodulationOperations << "\n";
        std::cout << "  Peak memory usage:   " << (result.peakMemoryUsage / 1024 / 1024) << "MB\n";
        std::cout << "  Efficiency:          " << std::fixed << std::setprecision(2) << (result.totalSynapticChanges / result.totalWallClockTime) << " changes/sec\n";
    }
    
    void printBenchmarkSuiteResults(const std::vector<Phase6BenchmarkResult>& results) {
        std::cout << "===========================================\n";
        std::cout << "BENCHMARK SUITE SUMMARY\n";
        std::cout << "===========================================\n\n";
        
        std::cout << "Configuration | Neurons | Steps | Time(s) | Steps/sec | Spikes/sec | Efficiency\n";
        std::cout << "-------------|---------|-------|---------|-----------|------------|-----------\n";
        
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            
            // Find the config for this benchmark
            Phase6BenchmarkConfig config = getBenchmarkConfigs()[i];
            
            std::cout << std::setw(12) << std::left << ("Config " + std::to_string(i + 1))
                      << "| " << std::setw(7) << std::right << config.neuronCount
                      << "| " << std::setw(6) << std::right << config.maxSteps
                      << "| " << std::setw(7) << std::right << std::fixed << std::setprecision(3) << result.totalWallClockTime
                      << "| " << std::setw(10) << std::right << std::fixed << std::setprecision(1) << result.stepsPerSecond
                      << "| " << std::setw(9) << std::right << std::fixed << std::setprecision(1) << result.spikesPerSecond
                      << "| " << std::setw(10) << std::right << std::fixed << std::setprecision(2) << (result.totalSynapticChanges / result.totalWallClockTime) << "\n";
        }
        
        // Calculate averages
        double avgStepsPerSecond = 0.0;
        double avgSpikesPerSecond = 0.0;
        double avgEfficiency = 0.0;
        
        for (const auto& result : results) {
            avgStepsPerSecond += result.stepsPerSecond;
            avgSpikesPerSecond += result.spikesPerSecond;
            avgEfficiency += (result.totalSynapticChanges / result.totalWallClockTime);
        }
        
        avgStepsPerSecond /= results.size();
        avgSpikesPerSecond /= results.size();
        avgEfficiency /= results.size();
        
        std::cout << "\nAverages:";
        std::cout << " Steps/sec: " << std::setw(10) << std::right << std::fixed << std::setprecision(1) << avgStepsPerSecond;
        std::cout << "  Spikes/sec: " << std::setw(10) << std::right << std::fixed << std::setprecision(1) << avgSpikesPerSecond;
        std::cout << "  Efficiency: " << std::setw(10) << std::right << std::fixed << std::setprecision(2) << avgEfficiency << "\n";
    }
    
    void generatePerformanceReport(const std::vector<Phase6BenchmarkResult>& results) {
        std::cout << "===========================================\n";
        std::cout << "PERFORMANCE REPORT\n";
        std::cout << "===========================================\n\n";
        
        std::cout << "The NLM Phase 6 benchmark suite provides:\n";
        std::cout << "  • Scalability analysis across different neuron counts\n";
        std::cout << "  • Performance measurements (steps/second, spikes/second)\n";
        std::cout << "  • Efficiency metrics (synaptic changes per second)\n";
        std::cout << "  • Memory and operation profiling\n";
        std::cout << "  • Comparative analysis between benchmark configurations\n\n";
        
        // Find best performing configuration
        if (!results.empty()) {
            auto bestIt = std::max_element(results.begin(), results.end(),
                [](const Phase6BenchmarkResult& a, const Phase6BenchmarkResult& b) {
                    return a.stepsPerSecond < b.stepsPerSecond;
                });
            
            std::cout << "Best performing configuration: " << std::distance(results.begin(), bestIt) + 1 << "\n";
            std::cout << "  Steps/second: " << std::fixed << std::setprecision(1) << bestIt->stepsPerSecond << "\n";
            std::cout << "  Spikes/second: " << std::fixed << std::setprecision(1) << bestIt->spikesPerSecond << "\n";
        }
        
        std::cout << "\nBenchmark report complete.\n";
    }
};

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Benchmark Demo ===" << std::endl;
    std::cout << "Running comprehensive performance benchmarks..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Error);
    
    // Create benchmark demo
    Phase6BenchmarkDemo benchmark;
    benchmark.runBenchmarks();
    
    return 0;
}
