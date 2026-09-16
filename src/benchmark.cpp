// NLM Performance Benchmark
// Enhanced with comprehensive performance profiling and monitoring

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "performance/PerformanceProfiler.hpp"
#include "performance/PerformanceMonitor.hpp"

#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <string>
#include <memory>

using namespace nlm;

// Simple performance measurement
struct BenchmarkResult {
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;      // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
};

BenchmarkResult runBenchmark(size_t neuronCount, size_t steps) {
    std::cout << "  Running benchmark: " << neuronCount << " neurons, " 
              << steps << " steps..." << std::flush;
    
    // Initialize performance monitoring
    auto& monitor = nlm::GlobalPerformanceMonitor::getInstance();
    monitor.getProfiler().start();
    monitor.getCPUMonitor().start();
    monitor.getMemoryMonitor().start();
    monitor.getSimulationTimeMonitor().start(0.001);  // 1ms timestep
    
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
    
    // Record simulation start
    monitor.getSimulationTimeMonitor().onSimulationStart();
    
    // Run simulation
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t step = 0; step < steps; ++step) {
        // Inject some random input
        for (size_t i = 0; i < std::min(size_t(10), neuronCount / 10); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        // Record step start
        auto stepStart = std::chrono::high_resolution_clock::now();
        brain->step(step, step * 0.001);
        
        // Record step time
        auto stepEnd = std::chrono::high_resolution_clock::now();
        double stepTimeMs = std::chrono::duration<double, std::milli>(stepEnd - stepStart).count();
        monitor.getSimulationTimeMonitor().onStepComplete(stepTimeMs / 1000.0);  // Convert to seconds
        
        // Record performance metrics
        monitor.getProfiler().recordStepTime(stepTimeMs / 1000.0);
        monitor.getProfiler().recordMetric("spikes", brain->getTotalSpikeCount());
        monitor.getProfiler().recordMemoryUsage();
        monitor.getProfiler().recordCPUUsage(50.0 + (step % 10) * 5.0);  // Simulated CPU usage
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            std::cout << "." << std::flush;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Record simulation stop
    monitor.getSimulationTimeMonitor().onSimulationStop();
    
    // Collect results
    BenchmarkResult result;
    result.neurons = brain->getTotalNeuronCount();
    result.synapses = brain->getTotalSynapseCount();
    result.steps = steps;
    result.totalTime = totalTime;
    result.stepsPerSecond = steps / totalTime;
    result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
    result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
    
    // Stop performance monitoring
    monitor.getProfiler().stop();
    monitor.getCPUMonitor().stop();
    monitor.getMemoryMonitor().stop();
    
    // Print performance summary
    std::cout << "\n  Performance Summary:" << std::endl;
    auto& profiler = monitor.getProfiler();
    
    // Get current metrics from profiler
    BenchmarkMetrics metrics = profiler.convertToBenchmarkMetrics();
    
    std::cout << "    CPU Utilization: " << std::fixed << std::setprecision(2) << metrics.cpuUtilizationPercent << "%" << std::endl;
    std::cout << "    Memory Usage: " << std::fixed << std::setprecision(2) << metrics.totalMemoryMB << " MB" << std::endl;
    std::cout << "    Step Time Avg: " << std::fixed << std::setprecision(6) << metrics.avgStepTimeMs << " ms" << std::endl;
    std::cout << "    Step Time StdDev: " << std::fixed << std::setprecision(6) << metrics.stepTimeStdDev << " ms" << std::endl;
    std::cout << "    Speed Factor: " << std::fixed << std::setprecision(2) << metrics.realTimeFactor << "x" << std::endl;
    
    // Generate performance bottleneck analysis
    std::string bottlenecks = profiler.analyzePerformanceBottlenecks();
    if (!bottlenecks.empty()) {
        std::cout << "    Bottlenecks: " << bottlenecks << std::endl;
    }
    
    std::cout << " Done!" << std::endl;
    std::cout << "    Time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
    std::cout << "    Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
    std::cout << "    Spikes/step: " << std::fixed << std::setprecision(2) << result.spikesPerStep << std::endl;
    
    return result;
}

void printBenchmarkResults(const std::vector<BenchmarkResult>& results) {
    std::cout << std::endl;
    std::cout << "╔═══════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                     NLM Performance Benchmark Results                           ║" << std::endl;
    std::cout << "╠═══════════════╦═══════════════╦═══════════╦═════════════╦══════════════════════╣" << std::endl;
    std::cout << "║   Neurons     ║   Synapses    ║ Steps/sec ║ Spikes/step ║ Synapses/Neuron      ║" << std::endl;
    std::cout << "╠═══════════════╬═══════════════╬═══════════╬═════════════╬══════════════════════╣" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(13) << r.neurons 
                  << " ║ " << std::setw(13) << r.synapses
                  << " ║ " << std::setw(9) << std::fixed << std::setprecision(0) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.spikesPerStep
                  << " ║ " << std::setw(20) << std::fixed << std::setprecision(2) << r.synapsesPerNeuron
                  << " ║" << std::endl;
    }
    
    std::cout << "╚═══════════════╩═══════════════╩═══════════╩═════════════╩══════════════════════╝" << std::endl;
    std::cout << std::endl;
}

int main() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Performance Benchmark                                     ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Running NLM Performance Benchmarks..." << std::endl;
    std::cout << std::endl;
    
    std::vector<BenchmarkResult> results;
    
    // Benchmark configurations: (neuron_count, steps)
    std::vector<std::pair<size_t, size_t>> benchmarks = {
        {100, 1000},      // 100 neurons, 1000 steps
        {500, 1000},      // 500 neurons, 1000 steps
        {1000, 500},      // 1000 neurons, 500 steps
        {2000, 200},      // 2000 neurons, 200 steps
    };
    
    for (const auto& [neurons, steps] : benchmarks) {
        auto result = runBenchmark(neurons, steps);
        results.push_back(result);
        std::cout << std::endl;
    }
    
    // Print summary
    printBenchmarkResults(results);
    
    std::cout << "Benchmark complete." << std::endl;
    
    return 0;
}
