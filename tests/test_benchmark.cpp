// Comprehensive Benchmark Tests for NLM Phase 6
// Performance benchmarks for different network sizes and configurations

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Config/ConfigFactory.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <string>
#include <memory>

namespace test_benchmark {

// Benchmark result structure
struct BenchmarkResult {
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;      // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
    double memoryUsageMB;
};

BenchmarkResult runBenchmark(size_t neuronCount, size_t steps) {
    std::cout << "  Running benchmark: " << neuronCount << " neurons, " 
              << steps << " steps..." << std::flush;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, nlm::ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, nlm::ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Run simulation
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t step = 0; step < steps; ++step) {
        // Inject some random input
        for (size_t i = 0; i < std::min(size_t(10), neuronCount / 10); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Collect results
    BenchmarkResult result;
    result.neurons = brain->getTotalNeuronCount();
    result.synapses = brain->getTotalSynapseCount();
    result.steps = steps;
    result.totalTime = totalTime;
    result.stepsPerSecond = steps / totalTime;
    result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
    result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
    
    auto stats = brain->getConfig()->getStats();
    result.memoryUsageMB = stats.memoryUsageMB;
    
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

void testSmallNetworkBenchmark() {
    std::cout << "=== Small Network Benchmark ===" << std::endl;
    
    std::vector<BenchmarkResult> results;
    
    // Benchmark configurations: (neuron_count, steps)
    std::vector<std::pair<size_t, size_t>> benchmarks = {
        {100, 1000},      // 100 neurons, 1000 steps
        {200, 500},      // 200 neurons, 500 steps
        {500, 200},      // 500 neurons, 200 steps
    };
    
    for (const auto& [neurons, steps] : benchmarks) {
        auto result = runBenchmark(neurons, steps);
        results.push_back(result);
        std::cout << std::endl;
    }
    
    printBenchmarkResults(results);
    std::cout << "Small network benchmark complete." << std::endl << std::endl;
}

void testMediumNetworkBenchmark() {
    std::cout << "=== Medium Network Benchmark ===" << std::endl;
    
    std::vector<BenchmarkResult> results;
    
    std::vector<std::pair<size_t, size_t>> benchmarks = {
        {1000, 500},      // 1000 neurons, 500 steps
        {1500, 300},     // 1500 neurons, 300 steps
        {2000, 200},     // 2000 neurons, 200 steps
    };
    
    for (const auto& [neurons, steps] : benchmarks) {
        auto result = runBenchmark(neurons, steps);
        results.push_back(result);
        std::cout << std::endl;
    }
    
    printBenchmarkResults(results);
    std::cout << "Medium network benchmark complete." << std::endl << std::endl;
}

void testLargeNetworkBenchmark() {
    std::cout << "=== Large Network Benchmark ===" << std::endl;
    
    std::vector<BenchmarkResult> results;
    
    std::vector<std::pair<size_t, size_t>> benchmarks = {
        {5000, 100},     // 5000 neurons, 100 steps
        {10000, 50},    // 10000 neurons, 50 steps
        {15000, 30},    // 15000 neurons, 30 steps
    };
    
    for (const auto& [neurons, steps] : benchmarks) {
        auto result = runBenchmark(neurons, steps);
        results.push_back(result);
        std::cout << std::endl;
    }
    
    printBenchmarkResults(results);
    std::cout << "Large network benchmark complete." << std::endl << std::endl;
}

void testScalabilityBenchmark() {
    std::cout << "=== Scalability Benchmark ===" << std::endl;
    
    std::vector<BenchmarkResult> results;
    
    // Fixed steps for scalability testing
    const size_t fixedSteps = 200;
    
    std::vector<size_t> neuronCounts = {100, 500, 1000, 2000, 5000};
    
    for (size_t neurons : neuronCounts) {
        auto result = runBenchmark(neurons, fixedSteps);
        results.push_back(result);
        std::cout << std::endl;
    }
    
    printBenchmarkResults(results);
    std::cout << "Scalability benchmark complete." << std::endl << std::endl;
}

void testStressBenchmark() {
    std::cout << "=== Stress Benchmark ===" << std::endl;
    
    std::vector<BenchmarkResult> results;
    
    // High load stress test
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2f, nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    std::cout << "  Running stress test: 10000 neurons, 500 steps with high input..." << std::flush;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t step = 0; step < 500; ++step) {
        // High input stress
        for (size_t i = 0; i < 50; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
            brain->injectCurrentToNeurons(NeuronType::Excitatory, 10.0f);
        }
        
        brain->step(step, step * 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    BenchmarkResult result;
    result.neurons = brain->getTotalNeuronCount();
    result.synapses = brain->getTotalSynapseCount();
    result.steps = 500;
    result.totalTime = totalTime;
    result.stepsPerSecond = 500 / totalTime;
    result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / 500;
    result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
    
    auto stats = brain->getConfig()->getStats();
    result.memoryUsageMB = stats.memoryUsageMB;
    
    std::cout << " Done!" << std::endl;
    std::cout << "    Time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
    std::cout << "    Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
    std::cout << "    Spikes/step: " << std::fixed << std::setprecision(2) << result.spikesPerStep << std::endl;
    
    results.push_back(result);
    std::cout << std::endl;
    
    printBenchmarkResults(results);
    std::cout << "Stress benchmark complete." << std::endl << std::endl;
}

void runAll() {
    std::cout << "\n" << std::string(80, '═') << std::endl;
    std::cout << "  NLM PHASE 6 COMPREHENSIVE BENCHMARK TESTS" << std::endl;
    std::cout << "  Performance Testing for Different Network Scales" << std::endl;
    std::cout << std::string(80, '═') << std::endl;
    
    testSmallNetworkBenchmark();
    testMediumNetworkBenchmark();
    testLargeNetworkBenchmark();
    testScalabilityBenchmark();
    testStressBenchmark();
    
    std::cout << "All benchmark tests completed successfully!" << std::endl;
}

} // namespace test_benchmark
