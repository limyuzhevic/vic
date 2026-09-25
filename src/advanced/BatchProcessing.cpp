#include "advanced/BatchProcessing.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "advanced/AdvancedCommands.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace nlm {

void BatchProcessor::processBatch(const std::string& configFile) {
    std::cout << "Processing batch configuration file: " << configFile << std::endl;
    
    std::ifstream file(configFile);
    if (!file.is_open()) {
        std::cout << "ERROR: Cannot open batch file: " << configFile << std::endl;
        return;
    }
    
    std::string line;
    std::vector<std::string> configPaths;
    while (std::getline(file, line)) {
        line = AdvancedCommands::trim(line);
        if (!line.empty() && line[0] != '#') {
            configPaths.push_back(line);
        }
    }
    
    std::cout << "Found " << configPaths.size() << " configurations to process:" << std::endl;
    for (const auto& path : configPaths) {
        std::cout << "  - " << path << std::endl;
    }
    
    // Process each config
    for (size_t i = 0; i < configPaths.size(); ++i) {
        std::cout << std::endl << "=== Processing Configuration " << (i + 1) 
                  << "/" << configPaths.size() << " ===" << std::endl;
        
        runSimulationFromConfig(configPaths[i], i + 1, configPaths.size());
        
        if (i < configPaths.size() - 1) {
            std::cout << std::endl << "=== Waiting for next configuration ===" << std::endl;
        }
    }
    
    std::cout << std::endl << "=== Batch processing complete ===" << std::endl;
}

bool BatchProcessor::loadConfigFromFile(const std::string& filepath, std::shared_ptr<Config>& config) {
    config = std::make_shared<Config>();
    if (config->loadFromFile(filepath)) {
        std::cout << "✓ Loaded configuration from: " << filepath << std::endl;
        return true;
    } else {
        std::cout << "✗ Failed to load configuration: " << filepath << std::endl;
        return false;
    }
}

void BatchProcessor::runSimulationFromConfig(const std::string& configPath, size_t index, size_t total) {
    std::cout << "Running simulation with config: " << configPath << std::endl;
    
    auto config = std::make_shared<Config>();
    if (!loadConfigFromFile(configPath, config)) {
        return;
    }
    
    // Apply default settings
    config->set("random_seed", static_cast<int64_t>(42 + index), ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cout << "✗ Failed to initialize brain" << std::endl;
        return;
    }
    
    std::cout << "✓ Brain initialized successfully" << std::endl;
    
    // Run plasticity experiment
    runPlasticityExperiment(brain);
    
    std::cout << "✓ Simulation completed for config " << index << "/" << total << std::endl;
}

} // namespace nlm