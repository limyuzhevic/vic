#include "advanced/SimulationComparison.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <sstream>

namespace nlm {

void SimulationComparator::compareConfigurations(const std::string& configList) {
    std::cout << "Comparing multiple configurations..." << std::endl;
    
    std::vector<std::string> configs = parseConfigList(configList);
    
    if (configs.size() < 2) {
        std::cout << "⚠ Need at least 2 configurations to compare" << std::endl;
        return;
    }
    
    std::vector<ComparisonResult> results;
    
    // Run comparison
    for (const auto& configPath : configs) {
        std::cout << "Running: " << configPath << std::endl;
        ComparisonResult result = runSingleSimulation(configPath);
        result.configName = configPath;
        results.push_back(result);
        std::cout << "  ✓ Completed" << std::endl;
    }
    
    // Print comparison table
    printComparisonResults(results);
}

ComparisonResult SimulationComparator::runSingleSimulation(const std::string& configPath) {
    ComparisonResult result;
    result.configName = configPath;
    
    // Load configuration
    auto config = std::make_shared<Config>();
    if (!config->loadFromFile(configPath)) {
        std::cout << "✗ Failed to load: " << configPath << std::endl;
        return result;
    }
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cout << "✗ Failed to initialize brain for: " << configPath << std::endl;
        return result;
    }
    
    // Run simulation
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        brain->step(step, step * 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(endTime - startTime).count();
    
    // Collect results
    result.neuronCount = brain->getTotalNeuronCount();
    result.synapseCount = brain->getTotalSynapseCount();
    result.totalTime = elapsed;
    result.stepsPerSecond = 1000.0 / elapsed;
    result.avgFiringRate = static_cast<double>(brain->getTotalSpikeCount()) / 1000.0;
    result.totalSpikes = brain->getTotalSpikeCount();
    
    return result;
}

void SimulationComparator::printComparisonResults(const std::vector<ComparisonResult>& results) {
    if (results.empty()) {
        return;
    }
    
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                Multi-Simulation Comparison Results                           ║" << std::endl;
    std::cout << "╠═════════════════╦════════════╦════════════╦═══════════╦════════════╦═══════════╦═════════╣" << std::endl;
    std::cout << "║ Config Name    ║  Neurons   ║  Synapses  ║ Steps/sec  ║ Firing Rate║  Total Spikes║ Time (s) ║" << std::endl;
    std::cout << "╠═════════════════╬════════════╬════════════╬═══════════╬════════════╬═══════════╬═════════╣" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(15) << r.configName.substr(0, 15)
                  << " ║ " << std::setw(10) << r.neuronCount
                  << " ║ " << std::setw(10) << r.synapseCount
                  << " ║ " << std::setw(10) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(3) << r.avgFiringRate
                  << " ║ " << std::setw(11) << r.totalSpikes
                  << " ║ " << std::setw(9) << std::fixed << std::setprecision(3) << r.totalTime << " ║" << std::endl;
    }
    
    std::cout << "╚═════════════════╩════════════╩════════════╩═══════════╩════════════╩═══════════╩═════════╝" << std::endl;
}

std::vector<std::string> SimulationComparator::parseConfigList(const std::string& configList) {
    std::vector<std::string> configs;
    size_t pos = 0;
    while (pos < configList.length()) {
        size_t comma = configList.find(',', pos);
        if (comma == std::string::npos) {
            configs.push_back(configList.substr(pos));
            break;
        }
        configs.push_back(configList.substr(pos, comma - pos));
        pos = comma + 1;
    }
    return configs;
}

} // namespace nlm