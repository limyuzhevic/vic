// NLM Synapse Performance Baseline Test
// Measures current synapse iteration performance

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>

using namespace nlm;

struct BaselineResult {
    size_t neurons;
    size_t synapses;
    double totalTime;      // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
};

BaselineResult measureCurrentPerformance(size_t neuronCount, size_t steps) {
    std::cout << "  Measuring baseline: " << neuronCount << " neurons, " 
              << steps << " steps..." << std::flush;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int>(1), ConfigSource::Default);
    config->set("connection_probability", 0.05f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_time_constant", 20.0f, ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Run simulation and measure time
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t step = 0; step < steps; ++step) {
        // Inject minimal input
        for (size_t i = 0; i < std::min(size_t(5), neuronCount / 50); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Collect results
    BaselineResult result;
    result.neurons = brain->getTotalNeuronCount();
    result.synapses = brain->getTotalSynapseCount();
    result.totalTime = totalTime;
    result.stepsPerSecond = steps / totalTime;
    result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
    result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
    
    std::cout << " Done!" << std::endl;
    std::cout << "    Time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
    std::cout << "    Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
    std::cout << "    Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    
    return result;
}

void printBaselineResults(const std::vector<BaselineResult>& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    NLM Current Performance Baseline                                 ║" << std::endl;
    std::cout << "╠═══════════════╦═══════════════╦═══════════╦═════════════╦════════════════════════╗" << std::endl;
    std::cout << "║   Neurons     ║   Synapses    ║ Steps/sec ║ Spikes/step ║ Synapses/Neuron       ║" << std::endl;
    std::cout << "╠═══════════════╬═══════════════╬═══════════╬═════════════╬════════════════════════╗" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(13) << r.neurons 
                  << " ║ " << std::setw(13) << r.synapses
                  << " ║ " << std::setw(9) << std::fixed << std::setprecision(0) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.spikesPerStep
                  << " ║ " << std::setw(20) << std::fixed << std::setprecision(2) << r.synapsesPerNeuron
                  << " ║" << std::endl;
    }
    
    std::cout << "╚═══════════════╩═══════════════╩═══════════╩═════════════╩════════════════════════╝" << std::endl;
    std::cout << std::endl;
}

int main() {
    std::cout << "NLM Synapse Performance Baseline Measurement" << std::endl;
    std::cout << "=============================================" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::vector<BaselineResult> results;
    
    // Small benchmark configurations for quick testing
    std::vector<std::pair<size_t, size_t>> benchmarks = {
        {100, 100},      // 100 neurons, 100 steps - warm up
        {200, 200},      // 200 neurons, 200 steps
        {500, 500},      // 500 neurons, 500 steps
        {1000, 200},     // 1000 neurons, 200 steps
    };
    
    for (const auto& [neurons, steps] : benchmarks) {
        auto result = measureCurrentPerformance(neurons, steps);
        results.push_back(result);
        std::cout << std::endl;
    }
    
    // Print summary
    printBaselineResults(results);
    
    std::cout << "Baseline measurement complete." << std::endl;
    std::cout << std::endl;
    std::cout << "Key observations for optimization:" << std::endl;
    std::cout << "1. Check for O(N²) complexity in synapse iteration" << std::endl;
    std::cout << "2. Identify scalar operations preventing SIMD" << std::endl;
    std::cout << "3. Analyze memory access patterns" << std::endl;
    std::cout << "4. Measure time spent on plasticity calculations" << std::endl;
    
    return 0;
}
