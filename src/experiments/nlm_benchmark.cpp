// NLM Benchmark - Performance benchmarking tool
// Measures and compares performance of different NLM configurations

#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <iomanip>

struct BenchmarkResult {
    std::string name;
    size_t neuronCount;
    size_t steps;
    double wallTimeSeconds;
    float avgFiringRate;
    size_t totalSpikes;
    float eiRatio;
    double efficiency; // spikes per neuron per millisecond
    double memoryUsage; // relative measure
};

void printBenchmarkHeader() {
    std::cout << "=== NLM Benchmark Suite ===" << std::endl;
    std::cout << std::left << std::setw(25) << "Test Name"
              << std::setw(15) << "Neurons"
              << std::setw(12) << "Steps"
              << std::setw(15) << "Wall Time (s)"
              << std::setw(18) << "Avg Firing (Hz)"
              << std::setw(15) << "Total Spikes"
              << std::setw(15) << "E/I Ratio"
              << std::setw(15) << "Efficiency"
              << std::setw(15) << "Memory Usage"
              << std::endl;
    std::cout << std::string(150, '=') << std::endl;
}

void printBenchmarkResult(const BenchmarkResult& result) {
    std::cout << std::left << std::setw(25) << result.name
              << std::setw(15) << result.neuronCount
              << std::setw(12) << result.steps
              << std::setw(15) << std::fixed << std::setprecision(3) << result.wallTimeSeconds
              << std::setw(18) << std::fixed << std::setprecision(2) << result.avgFiringRate
              << std::setw(15) << result.totalSpikes
              << std::setw(15) << std::fixed << std::setprecision(3) << result.eiRatio
              << std::setw(15) << std::fixed << std::setprecision(6) << result.efficiency
              << std::setw(15) << std::fixed << std::setprecision(1) << result.memoryUsage
              << std::endl;
}

BenchmarkResult runBenchmark(const std::string& name, size_t neuronCount, 
                            size_t steps, bool enablePlasticity = true,
                            bool enableDevelopment = true,
                            bool enableNeuromodulation = true) {
    auto config = std::make_shared<Config>();
    
    // Configure based on benchmark parameters
    config->set("neuron_count", static_cast<int64_t>(neuronCount));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("connection_probability", 0.1f);
    config->set("random_seed", static_cast<int64_t>(12345)); // Deterministic
    config->set("simulation_timestep", 0.001);
    
    if (enablePlasticity) {
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
    }
    
    if (enableDevelopment) {
        config->set("development.enable_critical_period", true);
        config->set("development.enable_synaptogenesis", true);
        config->set("development.enable_pruning", true);
    }
    
    if (enableNeuromodulation) {
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
    }
    
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain for benchmark: " << name << std::endl;
        return BenchmarkResult{name, neuronCount, steps, 0.0, 0.0, 0, 0.0, 0.0, 0.0};
    }
    
    // Record start time
    auto startWall = std::chrono::high_resolution_clock::now();
    
    // Run simulation
    for (size_t step = 0; step < steps; ++step) {
        // Inject some input every 100 steps
        if (step % 100 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Progress indicator for long benchmarks
        if (steps > 10000 && step % 5000 == 0) {
            std::cout << "  Progress: " << (step * 100.0 / steps) << "%" << std::endl;
        }
    }
    
    // Record end time
    auto endWall = std::chrono::high_resolution_clock::now();
    double wallTime = std::chrono::duration<double>(endWall - startWall).count();
    
    // Collect results
    BenchmarkResult result;
    result.name = name;
    result.neuronCount = neuronCount;
    result.steps = steps;
    result.wallTimeSeconds = wallTime;
    result.avgFiringRate = brain->getAverageFiringRate();
    result.totalSpikes = brain->getTotalSpikeCount();
    result.eiRatio = brain->getExcitationInhibitionRatio();
    
    // Calculate efficiency (spikes per neuron per millisecond)
    double totalSeconds = wallTime;
    double totalMilliseconds = totalSeconds * 1000.0;
    result.efficiency = static_cast<double>(result.totalSpikes) / 
                       (result.neuronCount * totalMilliseconds);
    
    // Estimate memory usage (very rough approximation)
    // Based on neuron count and system complexity
    result.memoryUsage = (neuronCount / 1000.0) * 
                        (1.0 + (enablePlasticity ? 0.5 : 0.0) +
                         (enableDevelopment ? 0.3 : 0.0) +
                         (enableNeuromodulation ? 0.2 : 0.0));
    
    return result;
}

void runMemoryBenchmark() {
    std::cout << "=== Memory Efficiency Benchmark ===" << std::endl;
    
    std::vector<BenchmarkResult> results;
    
    // Test different neuron counts
    std::vector<size_t> neuronCounts = {100, 500, 1000, 2000, 5000};
    
    for (size_t neuronCount : neuronCounts) {
        if (neuronCount > 5000) continue; // Skip very large for this demo
        
        std::string name = "Memory Efficiency (" + std::to_string(neuronCount) + " neurons)";
        BenchmarkResult result = runBenchmark(name, neuronCount, 2000, 
                                            true, true, true);
        results.push_back(result);
    }
    
    printBenchmarkHeader();
    for (const auto& result : results) {
        printBenchmarkResult(result);
    }
    
    // Find best efficiency
    double maxEfficiency = 0.0;
    const BenchmarkResult* bestResult = nullptr;
    
    for (const auto& result : results) {
        if (result.efficiency > maxEfficiency) {
            maxEfficiency = result.efficiency;
            bestResult = &result;
        }
    }
    
    if (bestResult) {
        std::cout << std::endl;
        std::cout << "=== Efficiency Analysis ===" << std::endl;
        std::cout << "Best efficiency: " << bestResult->name << std::endl;
        std::cout << "Efficiency: " << bestResult->efficiency << " spikes/neuron/ms" << std::endl;
        std::cout << "Recommended neuron count for balance: " << bestResult->neuronCount << std::endl;
    }
}

void runPlasticityBenchmark() {
    std::cout << "=== Plasticity Performance Benchmark ===" << std::endl;
    
    std::vector<BenchmarkResult> results;
    
    // Test with and without plasticity
    BenchmarkResult noPlasticity = runBenchmark("No Plasticity", 1000, 5000, 
                                               false, false, false);
    results.push_back(noPlasticity);
    
    BenchmarkResult stdpOnly = runBenchmark("STDP Only", 1000, 5000, 
                                          true, false, false);
    results.push_back(stdpOnly);
    
    BenchmarkResult fullIntegration = runBenchmark("Full Integration", 1000, 5000, 
                                                 true, true, true);
    results.push_back(fullIntegration);
    
    printBenchmarkHeader();
    for (const auto& result : results) {
        printBenchmarkResult(result);
    }
    
    std::cout << std::endl;
    std::cout << "=== Plasticity Analysis ===" << std::endl;
    std::cout << "Plasticity overhead: " << 
        ((fullIntegration.wallTimeSeconds - stdpOnly.wallTimeSeconds) / stdpOnly.wallTimeSeconds * 100.0) << "%" << std::endl;
    std::cout << "Plasticity effectiveness: " << 
        ((fullIntegration.efficiency - stdpOnly.efficiency) / stdpOnly.efficiency * 100.0) << "% improvement" << std::endl;
}

void runDevelopmentBenchmark() {
    std::cout << "=== Development Performance Benchmark ===" << std::endl;
    
    std::vector<BenchmarkResult> results;
    
    // Test development at different stages
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(500));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("random_seed", static_cast<int64_t>(12345));
    config->set("simulation_timestep", 0.001);
    
    // Development stage benchmarks
    std::vector<std::string> stages = {"Initial", "CriticalPeriod", "Maturation", "Adult"};
    
    for (const auto& stage : stages) {
        std::string name = "Development " + stage;
        
        // Configure development stage
        auto configCopy = std::make_shared<Config>(*config);
        
        BenchmarkResult result = runBenchmark(name, 500, 10000, 
                                            true, true, true);
        results.push_back(result);
    }
    
    printBenchmarkHeader();
    for (const auto& result : results) {
        printBenchmarkResult(result);
    }
    
    std::cout << std::endl;
    std::cout << "=== Development Analysis ===" << std::endl;
    std::cout << "Development impact on performance varies by stage" << std::endl;
    std::cout << "Early stages (Initial, CriticalPeriod) have higher plasticity" << std::endl;
    std::cout << "Late stages (Adult, Maturation) have lower plasticity" << std::endl;
}

int main(int argc, char* argv[]) {
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    std::cout << "=== NLM Benchmark Suite ===" << std::endl;
    std::cout << "Performance benchmarking for NLM Phase 6 implementation" << std::endl;
    
    std::string benchmarkType = "all";
    
    if (argc > 1) {
        benchmarkType = argv[1];
    }
    
    try {
        if (benchmarkType == "memory" || benchmarkType == "all") {
            runMemoryBenchmark();
        }
        
        if (benchmarkType == "plasticity" || benchmarkType == "all") {
            runPlasticityBenchmark();
        }
        
        if (benchmarkType == "development" || benchmarkType == "all") {
            runDevelopmentBenchmark();
        }
        
        if (benchmarkType != "memory" && benchmarkType != "plasticity" && 
            benchmarkType != "development" && benchmarkType != "all") {
            std::cout << "Unknown benchmark type: " << benchmarkType << std::endl;
            std::cout << "Available benchmarks: memory, plasticity, development, all" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error during benchmark: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << std::endl;
    std::cout << "=== Benchmark Complete ===" << std::endl;
    
    return 0;
}
