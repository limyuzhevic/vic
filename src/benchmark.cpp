// NLM Performance Benchmark
// Measures simulation performance for different network sizes

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
    
    // Detailed analysis
    std::cout << "Detailed Analysis:" << std::endl;
    std::cout << "─" * 60 << std::endl;
    
    for (const auto& r : results) {
        std::cout << "Network Size: " << r.neurons << " neurons, " << r.synapses << " synapses" << std::endl;
        std::cout << "  Simulation: " << r.steps << " steps in " << std::fixed << std::setprecision(2) << r.totalTime << " seconds" << std::endl;
        std::cout << "  Performance: " << std::fixed << std::setprecision(0) << r.stepsPerSecond << " steps/sec" << std::endl;
        std::cout << "  Activity: " << std::fixed << std::setprecision(2) << r.spikesPerStep << " spikes per step" << std::endl;
        std::cout << "  Connectivity: " << std::fixed << std::setprecision(2) << r.synapsesPerNeuron << " synapses per neuron" << std::endl;
        std::cout << "  Firing: " << std::fixed << std::setprecision(2) << r.avgFiringRate << " avg firing rate, " 
                  << r.activeNeurons << " active neurons" << std::endl;
        std::cout << "  Membrane potential: avg=" << std::fixed << std::setprecision(2) << r.avgMembranePotential
                  << ", max=" << std::fixed << std::setprecision(2) << r.maxMembranePotential
                  << ", min=" << std::fixed << std::setprecision(2) << r.minMembranePotential << std::endl;
        std::cout << "  Time distribution: " << std::fixed << std::setprecision(1) << (r.connectivityTime.count() / r.totalTime * 100) << "% connectivity, "
                  << (r.plasticTime.count() / r.totalTime * 100) << "% plasticity, "
                  << (r.memoryTime.count() / r.totalTime * 100) << "% memory" << std::endl;
        std::cout << "─" * 60 << std::endl;
    }
}

void printStressTestResults(const BenchmarkResult& result, const std::string& testName) {
    std::cout << std::endl;
    std::cout << "╔═══════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    STRESS TEST RESULTS: " << testName << "                      ║" << std::endl;
    std::cout << "╠═══════════════╦═══════════════╦═══════════╦═════════════╦══════════════════════╣" << std::endl;
    std::cout << "║   Neurons     ║   Synapses    ║ Steps/sec ║ Spikes/step ║ Synapses/Neuron      ║" << std::endl;
    std::cout << "╠═══════════════╬═══════════════╬═══════════╬═════════════╬══════════════════════╣" << std::endl;
    
    std::cout << "║ " << std::setw(13) << result.neurons 
              << " ║ " << std::setw(13) << result.synapses
              << " ║ " << std::setw(9) << std::fixed << std::setprecision(0) << result.stepsPerSecond
              << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << result.spikesPerStep
              << " ║ " << std::setw(20) << std::fixed << std::setprecision(2) << result.synapsesPerNeuron
              << " ║" << std::endl;
    
    std::cout << "╚═══════════════╩═══════════════╩═══════════╩═════════════╩══════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Stress Test Analysis:" << std::endl;
    std::cout << "─" * 60 << std::endl;
    std::cout << "Test: " << testName << std::endl;
    std::cout << "Network size: " << result.neurons << " neurons, " << result.synapses << " synapses (" << std::fixed << std::setprecision(2) << (result.synapsesPerNeuron * 100) << "% density)" << std::endl;
    std::cout << "Simulation load: " << result.steps << " steps in " << std::fixed << std::setprecision(2) << result.totalTime << " seconds" << std::endl;
    std::cout << "Performance: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << " steps/sec, "
              << std::fixed << std::setprecision(1) << (result.spikesPerStep * 100) << "% spike rate per step" << std::endl;
    std::cout << "Resource utilization: " << std::fixed << std::setprecision(1) << (result.activeNeurons * 100.0 / result.neurons) << "% neurons active, "
              << result.avgFiringRate << " avg firing rate" << std::endl;
    
    // Stress indicators
    if (result.stepsPerSecond < 1000) {
        std::cout << "⚠️  WARNING: Low performance (< 1000 steps/sec)" << std::endl;
    }
    if (result.spikesPerStep > 10.0) {
        std::cout << "⚠️  WARNING: High spike rate (> 10 per step) - possible seizure" << std::endl;
    }
    if (result.synapsesPerNeuron > 5.0) {
        std::cout << "⚠️  WARNING: High connectivity (> 5 synapses/neuron) - potential instability" << std::endl;
    }
    if (result.activeNeurons > result.neurons * 0.8) {
        std::cout << "⚠️  WARNING: High activity (> 80% active neurons) - potential runaway excitation" << std::endl;
    }
    
    std::cout << "✓ Stress test complete" << std::endl;
    std::cout << "─" * 60 << std::endl;
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
