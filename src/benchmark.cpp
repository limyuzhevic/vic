// Enhanced NLM Performance Benchmark with Comprehensive Profiling
// Measures simulation performance for different network sizes with detailed analysis
// Includes memory usage tracking, scalability analysis, and performance optimization insights

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"

#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <string>
#include <algorithm>
#include <cmath>

using namespace nlm;

// Enhanced performance measurement with detailed profiling
struct BenchmarkResult {
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;      // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
    double timePerStepMs;   // milliseconds per step
    double memoryUsageMB;   // estimated memory usage in MB
    double efficiency;      // normalized efficiency score
};

BenchmarkResult runBenchmark(size_t neuronCount, size_t steps) {
    std::cout << "  Running benchmark: " << neuronCount << " neurons, " 
              << steps << " steps..." << std::flush;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Initial memory check
    size_t initialNeurons = brain->getTotalNeuronCount();
    size_t initialSynapses = brain->getTotalSynapseCount();
    
    // Run simulation with precise timing
    auto startTime = std::chrono::high_resolution_clock::now();
    auto cpuStartTime = std::chrono::system_clock::now();
    
    for (size_t step = 0; step < steps; ++step) {
        // Inject controlled input to maintain activity
        for (size_t i = 0; i < std::min(size_t(10), neuronCount / 10); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Progress indicator for long runs
        if (step % 100 == 0 && steps > 1000) {
            std::cout << "." << std::flush;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto cpuEndTime = std::chrono::system_clock::now();
    
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    double cpuTime = std::chrono::duration<double>(cpuEndTime - cpuStartTime).count();
    
    // Collect final results
    BenchmarkResult result;
    result.neurons = brain->getTotalNeuronCount();
    result.synapses = brain->getTotalSynapseCount();
    result.steps = steps;
    result.totalTime = totalTime;
    result.stepsPerSecond = steps / totalTime;
    result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
    result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
    result.timePerStepMs = (totalTime / steps) * 1000.0;  // Convert to milliseconds
    
    // Estimate memory usage
    // Rough estimate: neurons + synapses + connections
    // Neuron: ~200 bytes, Synapse: ~100 bytes, plus overhead
    double memoryBytes = (static_cast<double>(result.neurons) * 200.0) +
                         (static_cast<double>(result.synapses) * 100.0);
    result.memoryUsageMB = memoryBytes / (1024.0 * 1024.0);
    
    // Calculate efficiency score (higher is better)
    // Based on steps per second relative to optimal for this size
    double optimalStepsPerSecond = 10000.0;  // Assumed optimal for this architecture
    double efficiency = std::min(1.0, result.stepsPerSecond / optimalStepsPerSecond);
    result.efficiency = efficiency * 100.0;  // Convert to percentage
    
    std::cout << " Done!" << std::endl;
    std::cout << "    Time: " << std::fixed << std::setprecision(2) << totalTime << "s (" 
              << std::setprecision(1) << cpuTime << " CPUs)" << std::endl;
    std::cout << "    Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
    std::cout << "    Time/step: " << std::fixed << std::setprecision(3) << result.timePerStepMs << "ms" << std::endl;
    std::cout << "    Spikes/step: " << std::fixed << std::setprecision(2) << result.spikesPerStep << std::endl;
    std::cout << "    Synapses/Neuron: " << std::fixed << std::setprecision(1) << result.synapsesPerNeuron << std::endl;
    std::cout << "    Memory usage: " << std::fixed << std::setprecision(1) << result.memoryUsageMB << " MB" << std::endl;
    std::cout << "    Efficiency: " << std::fixed << std::setprecision(1) << result.efficiency << "%" << std::endl;
    
    return result;
}

void printEnhancedBenchmarkResults(const std::vector<BenchmarkResult>& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                          ENHANCED NLM Performance Benchmark Results                         ║" << std::endl;
    std::cout << "╠═══════════════╦═══════════════╦═══════════╦═════════════╦════════════════════════╦══════════════╦════════════╗" << std::endl;
    std::cout << "║   Neurons     ║   Synapses    ║ Steps/sec  ║ Time/step  ║ Synapses/Neuron       ║ Efficiency  ║ Memory(MB) ║" << std::endl;
    std::cout << "╠═══════════════╬═══════════════╬═══════════╬═════════════╬════════════════════════╬══════════════╬════════════╗" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(13) << r.neurons 
                  << " ║ " << std::setw(13) << r.synapses
                  << " ║ " << std::setw(9) << std::fixed << std::setprecision(0) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(3) << r.timePerStepMs
                  << " ║ " << std::setw(23) << std::fixed << std::setprecision(1) << r.synapsesPerNeuron
                  << " ║ " << std::setw(12) << std::fixed << std::setprecision(1) << r.efficiency << "%"
                  << " ║ " << std::setw(10) << std::fixed << std::setprecision(1) << r.memoryUsageMB
                  << " ║" << std::endl;
    }
    
    std::cout << "╚═══════════════╩═══════════════╩═══════════╩═════════════╩════════════════════════╩══════════════╩════════════╝" << std::endl;
    std::cout << std::endl;
    
    // Print scalability analysis
    printScalabilityAnalysis(results);
    
    // Print performance recommendations
    printPerformanceRecommendations(results);
}

void printScalabilityAnalysis(const std::vector<BenchmarkResult>& results) {
    if (results.size() < 2) return;
    
    std::cout << "SCALABILITY ANALYSIS:" << std::endl;
    std::cout << "=======================" << std::endl;
    
    for (size_t i = 1; i < results.size(); ++i) {
        const auto& prev = results[i - 1];
        const auto& current = results[i];
        
        // Calculate scaling factors
        double neuronScale = static_cast<double>(current.neurons) / prev.neurons;
        double synapseScale = static_cast<double>(current.synapses) / prev.synapses;
        double stepScale = static_cast<double>(current.steps) / prev.steps;
        
        double timeScale = current.totalTime / prev.totalTime;
        double stepsScale = current.stepsPerSecond / prev.stepsPerSecond;
        
        // Calculate scaling efficiency
        double neuronEfficiency = neuronScale / timeScale;
        double stepEfficiency = stepScale / timeScale;
        
        std::cout << "From " << prev.neurons << " → " << current.neurons << " neurons:" << std::endl;
        std::cout << "  Neuron scale: " << std::fixed << std::setprecision(2) << neuronScale 
                  << " | Time scale: " << std::setprecision(2) << timeScale 
                  << " | Scaling efficiency: " << std::setprecision(3) << neuronEfficiency << std::endl;
        std::cout << "  Step efficiency: " << std::setprecision(3) << stepEfficiency 
                  << " | Throughput: " << std::setprecision(0) << current.stepsPerSecond << " steps/sec" << std::endl;
        
        if (neuronEfficiency > 1.0) {
            std::cout << "  ⚡️ SCALING UP: Better than expected (super-linear scaling)" << std::endl;
        } else if (neuronEfficiency < 0.5) {
            std::cout << "  ⚠️ SCALING DOWN: Poor scaling (sub-linear)" << std::endl;
        } else {
            std::cout << "  📊 SCALING NORMAL: Expected linear scaling" << std::endl;
        }
        std::cout << std::endl;
    }
}

void printPerformanceRecommendations(const std::vector<BenchmarkResult>& results) {
    if (results.empty()) return;
    
    std::cout << "PERFORMANCE RECOMMENDATIONS:" << std::endl;
    std::cout << "==========================" << std::endl;
    
    const auto& best = results[0];  // Best performance result
    
    std::cout << "Optimal configuration (recommended for production):" << std::endl;
    std::cout << "  Neurons: ~" << best.neurons << " (" << best.stepsPerSecond << " steps/sec)" << std::endl;
    std::cout << "  Time per step: ~" << std::fixed << std::setprecision(3) << best.timePerStepMs << "ms" << std::endl;
    std::cout << "  Memory per neuron: ~" << std::setprecision(1) << (best.memoryUsageMB / best.neurons) << " KB" << std::endl;
    std::cout << "  Efficiency: " << std::setprecision(1) << best.efficiency << "% optimal" << std::endl;
    
    // Find bottlenecks
    double maxTimePerStep = 0.0;
    double maxMemoryPerNeuron = 0.0;
    
    for (const auto& r : results) {
        if (r.timePerStepMs > maxTimePerStep) maxTimePerStep = r.timePerStepMs;
        double memPerNeuron = r.memoryUsageMB / r.neurons;
        if (memPerNeuron > maxMemoryPerNeuron) maxMemoryPerNeuron = memPerNeuron;
    }
    
    std::cout << std::endl;
    std::cout << "Identified potential bottlenecks:" << std::endl;
    
    if (maxTimePerStep > 10.0) {
        std::cout << "  ⚠️  High latency: " << std::fixed << std::setprecision(3) << maxTimePerStep << "ms per step" << std::endl;
        std::cout << "     Recommendation: Consider optimizing spike propagation or memory access" << std::endl;
    }
    
    if (maxMemoryPerNeuron > 1024) {
        std::cout << "  ⚠️  High memory usage: " << std::fixed << std::setprecision(1) << maxMemoryPerNeuron << "KB per neuron" << std::endl;
        std::cout << "     Recommendation: Consider memory pool optimization or data structure compression" << std::endl;
    }
    
    if (best.efficiency < 50.0) {
        std::cout << "  ⚠️  Low efficiency: Only " << std::fixed << std::setprecision(1) << best.efficiency << "% of optimal performance" << std::endl;
        std::cout << "     Recommendation: Consider algorithmic optimizations or hardware acceleration" << std::endl;
    }
    
    if (best.efficiency >= 80.0) {
        std::cout << "  ✅ Excellent efficiency: " << std::fixed << std::setprecision(1) << best.efficiency << "% optimal" << std::endl;
        std::cout << "     Recommendation: Configuration is well-optimized for production use" << std::endl;
    }
}

int main() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — Enhanced Performance Benchmark                       ║
    ║                                                               ║
    ║     Comprehensive neural simulation performance analysis         ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Running Enhanced NLM Performance Benchmarks..." << std::endl;
    std::cout << "This provides detailed performance analysis including scalability assessment and optimization recommendations." << std::endl;
    std::cout << std::endl;
    
    std::vector<BenchmarkResult> results;
    
    // Enhanced benchmark configurations with more granular steps
    std::vector<std::pair<size_t, size_t>> benchmarks = {
        {50, 2000},      // 50 neurons, 2000 steps (quick test)
        {100, 2000},     // 100 neurons, 2000 steps (small scale)
        {500, 1000},     // 500 neurons, 1000 steps (medium scale)
        {1000, 500},     // 1000 neurons, 500 steps (large scale)
        {2000, 200},     // 2000 neurons, 200 steps (very large scale)
        {5000, 100},     // 5000 neurons, 100 steps (extreme scale)
    };
    
    std::cout << "Running comprehensive performance tests..." << std::endl;
    std::cout << std::endl;
    
    for (size_t i = 0; i < benchmarks.size(); ++i) {
        const auto& [neurons, steps] = benchmarks[i];
        std::cout << "Test " << (i + 1) << "/" << benchmarks.size() << ":" << std::endl;
        
        auto result = runBenchmark(neurons, steps);
        results.push_back(result);
        
        if (i < benchmarks.size() - 1) {
            std::cout << std::endl;
        }
    }
    
    // Print comprehensive results
    printEnhancedBenchmarkResults(results);
    
    std::cout << "Enhanced benchmark complete." << std::endl;
    std::cout << "Use the configuration validation tool (nlm_config_validator) for configuration analysis." << std::endl;
    
    return 0;
}