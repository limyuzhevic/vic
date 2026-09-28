// NLM Config Command Implementation

#include "cli/NLMConfigCommand.hpp"
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>

namespace nlm {

bool NLMConfigCommand::listTemplates() {
    std::cout << "Available Configuration Templates:" << std::endl;
    std::cout << "  brain_phase2        - Phase 2 neural computation template" << std::endl;
    std::cout << "  learning_standard   - Standard learning configuration" << std::endl;
    std::cout << "  performance_fast    - Performance-optimized template" << std::endl;
    std::cout << "  visualization_full  - Full visualization support" << std::endl;
    std::cout << "  research_advanced   - Advanced research configuration" << std::endl;
    return true;
}

bool NLMConfigCommand::applyTemplate(const std::string& templateName) {
    std::cout << "Applying template: " << templateName << std::endl;
    
    // Load template configuration
    auto config = std::make_shared<Config>();
    
    if (templateName == "brain_phase2") {
        // Apply Phase 2 template
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::User);
        config->set("simulation_timestep", 0.001, ConfigSource::User);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::User);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::User);
        config->set("connection_probability", 0.15f, ConfigSource::User);
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::User);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::User);
        config->set("stdp_tau", 20.0f, ConfigSource::User);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::User);
        config->set("pruning_rate", 0.00001f, ConfigSource::User);
        
        std::cout << "✓ Applied Phase 2 neural computation template" << std::endl;
        std::cout << "  - 500 neurons with 1 region" << std::endl;
        std::cout << "  - STDP parameters configured" << std::endl;
        std::cout << "  - Structural plasticity enabled" << std::endl;
        
    } else if (templateName == "learning_standard") {
        // Apply standard learning template
        config->set("random_seed", static_cast<int64_t>(123), ConfigSource::User);
        config->set("simulation_timestep", 0.001, ConfigSource::User);
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::User);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::User);
        config->set("connection_probability", 0.1f, ConfigSource::User);
        config->set("stdp_ltp_weight", 0.015f, ConfigSource::User);
        config->set("stdp_ltd_weight", 0.01f, ConfigSource::User);
        
        std::cout << "✓ Applied standard learning template" << std::endl;
        std::cout << "  - 1000 neurons with 2 regions" << std::endl;
        std::cout << "  - Standard learning parameters" << std::endl;
        
    } else if (templateName == "performance_fast") {
        // Apply performance template
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::User);
        config->set("simulation_timestep", 0.002, ConfigSource::User);
        config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::User);
        config->set("region_count", static_cast<int64_t>(4), ConfigSource::User);
        config->set("connection_probability", 0.05f, ConfigSource::User);
        config->set("enable_optimization", true, ConfigSource::User);
        config->set("use_memory_pools", true, ConfigSource::User);
        
        std::cout << "✓ Applied performance-optimized template" << std::endl;
        std::cout << "  - 2000 neurons with 4 regions" << std::endl;
        std::cout << "  - Larger timestep for faster simulation" << std::endl;
        std::cout << "  - Optimizations enabled" << std::endl;
        
    } else {
        std::cerr << "Error: Unknown template: " << templateName << std::endl;
        return false;
    }
    
    // Save the configured template
    std::string filename = templateName + ".cfg";
    if (config->saveToFile(filename)) {
        std::cout << "  Saved to: " << filename << std::endl;
    }
    
    return true;
}

bool NLMConfigCommand::validateConfig(const std::string& configFile) {
    std::cout << "Validating configuration file: " << configFile << std::endl;
    
    auto config = std::make_shared<Config>();
    
    if (!config->loadFromFile(configFile)) {
        std::cerr << "Error: Failed to load configuration file: " << configFile << std::endl;
        return false;
    }
    
    std::cout << "Configuration validation:" << std::endl;
    
    // Check required parameters
    bool valid = true;
    
    // Check simulation timestep
    double timestep = config->getOr<double>("simulation_timestep", 0.0);
    if (timestep <= 0.0 || timestep > 1.0) {
        std::cerr << "  ✗ Invalid simulation_timestep: " << timestep << std::endl;
        valid = false;
    } else {
        std::cout << "  ✓ simulation_timestep: " << timestep << "s" << std::endl;
    }
    
    // Check neuron count
    int64_t neuronCount = config->getOr<int64_t>("neuron_count", -1);
    if (neuronCount <= 0) {
        std::cerr << "  ✗ Invalid neuron_count: " << neuronCount << std::endl;
        valid = false;
    } else {
        std::cout << "  ✓ neuron_count: " << neuronCount << std::endl;
    }
    
    // Check region count
    int64_t regionCount = config->getOr<int64_t>("region_count", 1);
    if (regionCount <= 0) {
        std::cerr << "  ✗ Invalid region_count: " << regionCount << std::endl;
        valid = false;
    } else {
        std::cout << "  ✓ region_count: " << regionCount << std::endl;
    }
    
    // Check plasticity parameters if present
    if (config->hasValue("stdp_ltp_weight")) {
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.0f);
        if (ltpWeight < 0.0f || ltpWeight > 1.0f) {
            std::cerr << "  ✗ Invalid stdp_ltp_weight: " << ltpWeight << std::endl;
            valid = false;
        } else {
            std::cout << "  ✓ stdp_ltp_weight: " << ltpWeight << std::endl;
        }
    }
    
    if (valid) {
        std::cout << "✓ Configuration file is valid" << std::endl;
    } else {
        std::cout << "✗ Configuration file has validation errors" << std::endl;
    }
    
    return valid;
}

bool NLMConfigCommand::exportConfig(const std::string& outputFile) {
    std::cout << "Exporting current configuration to: " << outputFile << std::endl;
    
    // Create export configuration
    nlohmann::json exportConfig;
    exportConfig["format_version"] = "1.0";
    exportConfig["creation_date"] = getCurrentTimestamp();
    exportConfig["format"] = "json";
    
    // Add brain configuration
    nlohmann::json brainConfig;
    brainConfig["neuron_count"] = 500; // Would get from actual brain config
    brainConfig["region_count"] = 1;
    brainConfig["connection_probability"] = 0.15f;
    
    // Add simulation configuration
    nlohmann::json simConfig;
    simConfig["timestep"] = 0.001;
    simConfig["random_seed"] = 42;
    
    exportConfig["brain"] = brainConfig;
    exportConfig["simulation"] = simConfig;
    
    // Write to file
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot create export file: " << outputFile << std::endl;
        return false;
    }
    
    file << exportConfig.dump(2);
    file.close();
    
    std::cout << "✓ Configuration exported to: " << outputFile << std::endl;
    return true;
}

bool NLMConfigCommand::importConfig(const std::string& inputFile) {
    std::cout << "Importing configuration from: " << inputFile << std::endl;
    
    // Load JSON configuration
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open input file: " << inputFile << std::endl;
        return false;
    }
    
    nlohmann::json importConfig;
    try {
        file >> importConfig;
    } catch (const std::exception& e) {
        std::cerr << "Error: Failed to parse JSON file: " << e.what() << std::endl;
        return false;
    }
    
    // Validate import format
    if (!importConfig.contains("format_version") || 
        !importConfig.contains("brain") || 
        !importConfig.contains("simulation")) {
        std::cerr << "Error: Invalid configuration format" << std::endl;
        return false;
    }
    
    // Apply configuration to current config
    auto config = std::make_shared<Config>();
    
    // Apply brain configuration
    if (importConfig["brain"].contains("neuron_count")) {
        config->set("neuron_count", importConfig["brain"]["neuron_count"].get<int64_t>(), ConfigSource::User);
    }
    if (importConfig["brain"].contains("region_count")) {
        config->set("region_count", importConfig["brain"]["region_count"].get<int64_t>(), ConfigSource::User);
    }
    if (importConfig["brain"].contains("connection_probability")) {
        config->set("connection_probability", importConfig["brain"]["connection_probability"].get<float>(), ConfigSource::User);
    }
    
    // Apply simulation configuration
    if (importConfig["simulation"].contains("timestep")) {
        config->set("simulation_timestep", importConfig["simulation"]["timestep"].get<double>(), ConfigSource::User);
    }
    if (importConfig["simulation"].contains("random_seed")) {
        config->set("random_seed", importConfig["simulation"]["random_seed"].get<int64_t>(), ConfigSource::User);
    }
    
    // Save imported configuration
    std::string filename = "imported_" + getTimestamp() + ".cfg";
    if (config->saveToFile(filename)) {
        std::cout << "✓ Configuration imported and saved to: " << filename << std::endl;
    }
    
    return true;
}

bool NLMConfigCommand::generateTemplate(const std::string& outputFile) {
    std::cout << "Generating new configuration template: " << outputFile << std::endl;
    
    // Create default configuration
    auto config = std::make_shared<Config>();
    
    // Set default values for a new brain
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    config->set("enable_optimization", true, ConfigSource::Default);
    
    // Save generated template
    if (config->saveToFile(outputFile)) {
        std::cout << "✓ Configuration template generated: " << outputFile << std::endl;
        return true;
    } else {
        std::cerr << "Error: Failed to save configuration template" << std::endl;
        return false;
    }
}

bool NLMConfigCommand::showConfigInfo(const std::string& configFile) {
    std::cout << "Configuration Information for: " << configFile << std::endl;
    
    auto config = std::make_shared<Config>();
    if (!config->loadFromFile(configFile)) {
        std::cerr << "Error: Failed to load configuration file" << std::endl;
        return false;
    }
    
    std::cout << "Configuration File: " << configFile << std::endl;
    std::cout << "Created: " << getFileCreationTime(configFile) << std::endl;
    
    // Display configuration values
    std::cout << "Configuration Values:" << std::endl;
    
    std::vector<std::string> keys = {
        "random_seed", "simulation_timestep", "neuron_count", "region_count",
        "connection_probability", "stdp_ltp_weight", "stdp_ltd_weight",
        "stdp_tau", "synaptogenesis_rate", "pruning_rate", "enable_optimization"
    };
    
    for (const auto& key : keys) {
        if (config->hasValue(key)) {
            if (key == "simulation_timestep" || key == "stdp_tau") {
                double value = config->getOr<double>(key, 0.0);
                std::cout << "  " << key << ": " << value << std::endl;
            } else if (key == "connection_probability" || 
                       key == "stdp_ltp_weight" || 
                       key == "stdp_ltd_weight" ||
                       key == "synaptogenesis_rate" ||
                       key == "pruning_rate") {
                float value = config->getOr<float>(key, 0.0f);
                std::cout << "  " << key << ": " << value << std::endl;
            } else {
                int64_t value = config->getOr<int64_t>(key, 0);
                std::cout << "  " << key << ": " << value << std::endl;
            }
        }
    }
    
    return true;
}

std::string NLMConfigCommand::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S");
    return ss.str();
}

std::string NLMConfigCommand::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string NLMConfigCommand::getFileCreationTime(const std::string& filename) {
    // In a real implementation, we would get the actual file creation time
    // For now, return current time
    return getCurrentTimestamp();
}

} // namespace nlm
