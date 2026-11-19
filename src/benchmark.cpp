// NLM Performance Benchmark
//
// This executable provides comprehensive performance benchmarking for the NLM framework.
// It measures key performance metrics and helps identify bottlenecks.

#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>

using namespace nlm;

// Benchmark configuration
struct BenchmarkConfig {
    uint64_t neuronCount = 1000;
    uint64_t maxSteps = 5000;
    uint64_t warmupSteps = 500;
    uint64_t measurementSteps = 4500;
    std::string benchmarkName;
};

// Performance metrics
struct BenchmarkResults {
    std::string benchmarkName;
    double wallClockTime;           // Total wall-clock time in seconds
    double cpuTime;                 // CPU time in seconds
    double realTimePerStep;         // Real-time per step in seconds
    double stepsPerSecond;          // Processing speed in steps/second
    double avgFiringRate;           // Average firing rate (Hz)
    double avgSynapseWeight;        // Average synaptic weight
    double totalSpikeCount;         // Total number of spikes
    double memoryUsage;             // Memory usage in MB
    std::vector<double> stepTimes;  // Time per step for analysis
    
    BenchmarkResults() : wallClockTime(0.0), cpuTime(0.0), realTimePerStep(0.0),
                        stepsPerSecond(0.0), avgFiringRate(0.0), avgSynapseWeight(0.0),
                        totalSpikeCount(0.0), memoryUsage(0.0) {}
};

// Performance counter utility
class PerformanceTimer {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point cpuStartTime;
    
public:
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
        cpuStartTime = std::chrono::high_resolution_clock::now();
    }
    
    double elapsedWallClock() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
        return duration.count() * 1e-9;  // Convert to seconds
    }
    
    double elapsedCpu() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - cpuStartTime);
        return duration.count() * 1e-9;  // Convert to seconds
    }
};

// Memory usage measurement
class MemoryUsage {
public:
    static double getMemoryUsage() {
        // Simple memory estimation - could be improved with platform-specific calls
        // This returns an approximate value in MB
        return 0.0;  // TODO: Implement proper memory measurement
    }
};

// Brain initialization with specific configuration
std::shared_ptr<Brain> createBenchmarkBrain(const BenchmarkConfig& config) {
    auto brainConfig = std::make_shared<Config>();
    
    // Set configuration for benchmark
    brainConfig->set("neuron_count", static_cast<int64_t>(config.neuronCount));
    brainConfig->set("region_count", static_cast<int64_t>(config.regionCount));
    brainConfig->set("connection_probability", config.connectionProbability);
    brainConfig->set("simulation_timestep", 0.001);
    brainConfig->set("random_seed", static_cast<int64_t>(42));  // For reproducibility
    
    // Set optimal parameters for benchmarking
    brainConfig->set("stdp_ltp_weight", 0.01f);
    brainConfig->set("stdp_ltd_weight", 0.012f);
    brainConfig->set("synaptogenesis_rate", 0.00005f);
    brainConfig->set("pruning_rate", 0.000005f);
    
    auto brain = std::make_shared<Brain>(brainConfig);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain for benchmark!" << std::endl;
        return nullptr;
    }
    
    return brain;
}

// Simple world for benchmarking
std::shared_ptr<SimpleWorld> createBenchmarkWorld() {
    auto world = std::make_shared<SimpleWorld>();
    world->configure(20, 20, 8, 8);  // Reasonable size for benchmarking
    world->reset();
    return world;
}

// Agent initialization for benchmarking
std::shared_ptr<AgentBrain> createBenchmarkAgent(std::shared_ptr<Brain> brain, 
                                                 std::shared_ptr<SimpleWorld> world) {
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    // Enable subsystems for realistic benchmarking
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    return agent;
}

// Single-step simulation
void simulateStep(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                  std::shared_ptr<SimpleWorld> world, uint64_t step) {
    // Get sensory input
    SensoryPercept percept = world->getSensoryPercept();
    
    // Process sensory input
    agent->processSensoryInput(percept);
    
    // Brain step
    brain->step(step, step * 0.001);
    
    // Get motor command
    MotorCommand cmd = agent->decodeMotorCommand();
    
    // Apply action to world
    ActionResult actionResult = world->applyMotorCommand(cmd, step * 0.001);
    
    // Apply reward modulation
    agent->applyRewardModulation(actionResult.reward, 0.0f);
    
    // Update development
    agent->updateDevelopment(0.001);
}

// Complete benchmark run
BenchmarkResults runBenchmark(const BenchmarkConfig& config) {
    BenchmarkResults results;
    results.benchmarkName = config.benchmarkName;
    
    PerformanceTimer timer;
    timer.start();
    
    // Create benchmark components
    auto brain = createBenchmarkBrain(config);
    if (!brain) {
        std::cerr << "Failed to create benchmark brain!" << std::endl;
        return results;
    }
    
    auto world = createBenchmarkWorld();
    auto agent = createBenchmarkAgent(brain, world);
    
    double cpuStartTime = timer.elapsedCpu();
    
    // Warmup phase
    std::cout << "Warmup phase: " << config.warmupSteps << " steps..." << std::endl;
    for (uint64_t step = 0; step < config.warmupSteps; ++step) {
        simulateStep(brain, agent, world, step);
        
        if (step % 500 == 0) {
            double progress = static_cast<double>(step) / config.warmupSteps * 100.0;
            std::cout << "  Warmup progress: " << progress << "%" << std::endl;
        }
    }
    
    // Measurement phase
    std::cout << "Measurement phase: " << config.measurementSteps << " steps..." << std::endl;
    
    for (uint64_t step = config.warmupSteps; step < config.maxSteps; ++step) {
        auto stepStart = std::chrono::high_resolution_clock::now();
        
        simulateStep(brain, agent, world, step);
        
        auto stepEnd = std::chrono::high_resolution_clock::now();
        auto stepDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(stepEnd - stepStart);
        double stepTime = stepDuration.count() * 1e-9;  // Convert to seconds
        
        results.stepTimes.push_back(stepTime);
        
        if ((step - config.warmupSteps) % 1000 == 0) {
            double progress = static_cast<double>(step - config.warmupSteps) / config.measurementSteps * 100.0;
            std::cout << "  Measurement progress: " << progress << "%" << std::endl;
        }
    }
    
    double cpuEndTime = timer.elapsedCpu();
    results.cpuTime = cpuEndTime - cpuStartTime;
    results.wallClockTime = timer.elapsedWallClock();
    
    // Calculate metrics
    results.totalSpikeCount = brain->getTotalSpikeCount();
    results.avgFiringRate = brain->getAverageFiringRate();
    
    // Calculate average synaptic weight
    float totalWeight = 0.0f;
    size_t synapseCount = 0;
    for (const auto& region : brain->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            totalWeight += syn->getWeight();
            synapseCount++;
        }
    }
    
    if (synapseCount > 0) {
        results.avgSynapseWeight = totalWeight / synapseCount;
    }
    
    // Calculate timing metrics
    if (!results.stepTimes.empty()) {
        double totalStepTime = 0.0;
        double minStepTime = results.stepTimes[0];
        double maxStepTime = results.stepTimes[0];
        
        for (double stepTime : results.stepTimes) {
            totalStepTime += stepTime;
            minStepTime = std::min(minStepTime, stepTime);
            maxStepTime = std::max(maxStepTime, stepTime);
        }
        
        results.realTimePerStep = totalStepTime / results.stepTimes.size();
        results.stepsPerSecond = results.stepTimes.size() / totalStepTime;
    }
    
    results.memoryUsage = MemoryUsage::getMemoryUsage();
    
    return results;
}

// Print benchmark results
void printBenchmarkResults(const BenchmarkResults& results) {
    std::cout << "=== NLM Performance Benchmark Results ===" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Benchmark: " << results.benchmarkName << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Timing Performance:" << std::endl;
    std::cout << "  Total wall clock time: " << std::fixed << std::setprecision(3) 
              << results.wallClockTime << " seconds" << std::endl;
    std::cout << "  Total CPU time: " << std::fixed << std::setprecision(3) 
              << results.cpuTime << " seconds" << std::endl;
    std::cout << "  CPU efficiency: " << std::fixed << std::setprecision(1)
              << (results.cpuTime / results.wallClockTime * 100.0) << "%" << std::endl;
    std::cout << "  Steps per second: " << std::fixed << std::setprecision(1)
              << results.stepsPerSecond << std::endl;
    std::cout << "  Real-time per step: " << std::fixed << std::setprecision(6)
              << results.realTimePerStep << " seconds" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Neural Performance:" << std::endl;
    std::cout << "  Average firing rate: " << std::fixed << std::setprecision(1)
              << results.avgFiringRate << " Hz" << std::endl;
    std::cout << "  Total spikes: " << std::fixed << std::setprecision(0)
              << results.totalSpikeCount << std::endl;
    std::cout << "  Average synaptic weight: " << std::fixed << std::setprecision(6)
              << results.avgSynapseWeight << std::endl;
    std::cout << std::endl;
    
    std::cout << "Resource Usage:" << std::endl;
    std::cout << "  Memory usage: " << std::fixed << std::setprecision(1)
              << results.memoryUsage << " MB" << std::endl;
    std::cout << std::endl;
    
    // Statistical analysis of step times
    if (!results.stepTimes.empty()) {
        std::cout << "Step Time Statistics:" << std::endl;
        double minStepTime = *std::min_element(results.stepTimes.begin(), results.stepTimes.end());
        double maxStepTime = *std::max_element(results.stepTimes.begin(), results.stepTimes.end());
        double avgStepTime = std::accumulate(results.stepTimes.begin(), results.stepTimes.end(), 0.0) / results.stepTimes.size();
        double stdDevStepTime = 0.0;
        
        for (double stepTime : results.stepTimes) {
            stdDevStepTime += (stepTime - avgStepTime) * (stepTime - avgStepTime);
        }
        stdDevStepTime = std::sqrt(stdDevStepTime / results.stepTimes.size());
        
        std::cout << "  Min step time: " << std::fixed << std::setprecision(6)
                  << minStepTime << " seconds" << std::endl;
        std::cout << "  Max step time: " << std::fixed << std::setprecision(6)
                  << maxStepTime << " seconds" << std::endl;
        std::cout << "  Average step time: " << std::fixed << std::setprecision(6)
                  << avgStepTime << " seconds" << std::endl;
        std::cout << "  Std deviation: " << std::fixed << std::setprecision(6)
                  << stdDevStepTime << " seconds" << std::endl;
        std::cout << "  95th percentile: " << std::fixed << std::setprecision(6)
                  << percentile(95, results.stepTimes) << " seconds" << std::endl;
        std::cout << std::endl;
    }
}

// Helper function for percentile calculation
double percentile(int percent, const std::vector<double>& data) {
    if (data.empty()) return 0.0;
    
    std::vector<double> sorted = data;
    std::sort(sorted.begin(), sorted.end());
    
    int index = (percent * sorted.size() - 1) / 100;
    if (index < 0) index = 0;
    if (index >= sorted.size()) index = sorted.size() - 1;
    
    return sorted[index];
}

// Parse command line arguments
BenchmarkConfig parseBenchmarkArguments(int argc, char* argv[]) {
    BenchmarkConfig config;
    config.benchmarkName = "Default Benchmark";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.substr(0, 9) == "--neurons=") {
            config.neuronCount = std::stoull(arg.substr(9));
        } else if (arg.substr(0, 8) == "--steps=") {
            config.maxSteps = std::stoull(arg.substr(8));
        } else if (arg.substr(0, 9) == "--warmup=") {
            config.warmupSteps = std::stoull(arg.substr(9));
        } else if (arg.substr(0, 13) == "--benchmark=") {
            config.benchmarkName = arg.substr(13);
        } else if (arg == "--help" || arg == "-h") {
            printBenchmarkHelp();
            exit(0);
        }
    }
    
    return config;
}

void printBenchmarkHelp() {
    std::cout << "NLM Performance Benchmark" << std::endl;
    std::cout << "========================" << std::endl;
    std::cout << "Usage: nlm_benchmark [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --neurons=<N>     Number of neurons (default: 1000)" << std::endl;
    std::cout << "  --steps=<N>       Total simulation steps (default: 5000)" << std::endl;
    std::cout << "  --warmup=<N>      Warmup steps (default: 500)" << std::endl;
    std::cout << "  --benchmark=<name> Benchmark name (default: Default Benchmark)" << std::endl;
    std::cout << "  --help, -h        Show this help message" << std::endl;
}

int main(int argc, char* argv[]) {
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Parse arguments
    BenchmarkConfig config = parseBenchmarkArguments(argc, argv);
    
    std::cout << "=== NLM Performance Benchmark ===" << std::endl;
    std::cout << "Benchmark: " << config.benchmarkName << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << std::endl;
    
    // Run benchmark
    BenchmarkResults results = runBenchmark(config);
    
    // Print results
    printBenchmarkResults(results);
    
    std::cout << "=== Benchmark Complete ===" << std::endl;
    
    return 0;
}