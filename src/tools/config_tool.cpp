// Configuration tool implementation
//
// This file provides the implementation of configuration CLI tools

#include "config_tool.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

// Configuration tool implementations
bool ConfigTool::validateConfig(const Config& config) const {
    return validateConfigImpl(config);
}

bool ConfigTool::exportConfig(const std::string& outputFile, const Config& config,
                             const std::string& format) const {
    return exportConfigImpl(outputFile, config, format);
}

bool ConfigTool::mergeConfigs(const std::vector<std::string>& configFiles, Config& resultConfig) const {
    return mergeConfigsImpl(configFiles, resultConfig);
}

bool ConfigTool::compareConfigs(const Config& config1, const Config& config2) const {
    return compareConfigsImpl(config1, config2);
}

bool ConfigTool::applyPreset(Config& config, const std::string& presetName) const {
    // Convert string to preset enum
    ConfigPreset preset;
    if (presetName == "research") {
        preset = ConfigPreset::RESEARCH;
    } else if (presetName == "performance") {
        preset = ConfigPreset::PERFORMANCE;
    } else if (presetName == "education") {
        preset = ConfigPreset::EDUCATION;
    } else if (presetName == "balanced") {
        preset = ConfigPreset::BALANCED;
    } else if (presetName == "custom") {
        preset = ConfigPreset::CUSTOM;
    } else {
        printError("Unknown preset: " + presetName);
        return false;
    }
    
    return config.applyPreset(preset);
}

std::vector<std::string> ConfigTool::getAvailablePresets() const {
    return {"research", "performance", "education", "balanced", "custom"};
}

bool ConfigTool::validateConfigAgainstSchema(const Config& config) const {
    // Basic validation - check for required fields
    auto randomSeed = config.get<int64_t>("random_seed");
    auto logLevel = config.get<std::string>("log_level");
    auto neuronCount = config.get<int64_t>("neuron_count");
    
    if (!randomSeed) {
        printError("Missing required field: random_seed");
        return false;
    }
    
    if (!logLevel) {
        printError("Missing required field: log_level");
        return false;
    }
    
    if (!neuronCount) {
        printError("Missing required field: neuron_count");
        return false;
    }
    
    // Validate ranges
    if (*randomSeed < 0 || *randomSeed > INT64_MAX) {
        printError("Invalid random_seed value: " + std::to_string(*randomSeed));
        return false;
    }
    
    if (*neuronCount <= 0 || *neuronCount > 1000000) {
        printError("Invalid neuron_count value: " + std::to_string(*neuronCount));
        return false;
    }
    
    return true;
}

bool ConfigTool::validateConfigValues(const Config& config) const {
    // Validate configuration values are within acceptable ranges
    auto neuronCount = config.getOr<int64_t>("neuron_count", 0);
    auto regionCount = config.getOr<int64_t>("region_count", 0);
    auto connectionProb = config.getOr<float>("connection_probability", 0.0f);
    auto stdpLTP = config.getOr<float>("stdp_ltp_weight", 0.0f);
    auto stdpLTD = config.getOr<float>("stdp_ltd_weight", 0.0f);
    
    if (neuronCount <= 0) {
        printError("neuron_count must be positive");
        return false;
    }
    
    if (regionCount <= 0) {
        printError("region_count must be positive");
        return false;
    }
    
    if (connectionProb < 0.0f || connectionProb > 1.0f) {
        printError("connection_probability must be between 0.0 and 1.0");
        return false;
    }
    
    if (stdpLTP <= 0.0f) {
        printError("stdp_ltp_weight must be positive");
        return false;
    }
    
    if (stdpLTD <= 0.0f) {
        printError("stdp_ltd_weight must be positive");
        return false;
    }
    
    return true;
}

// Configuration-specific implementations
bool ConfigTool::validateConfigImpl(const Config& config) const {
    return validateConfigAgainstSchema(config) && validateConfigValues(config);
}

bool ConfigTool::exportConfigImpl(const std::string& outputFile, const Config& config,
                                 const std::string& format) const {
    nlohmann::json jsonConfig;
    config.saveToJson(jsonConfig);
    
    // TODO: Implement different export formats
    return writeJsonToFile(jsonConfig, outputFile);
}

bool ConfigTool::mergeConfigsImpl(const std::vector<std::string>& configFiles, Config& resultConfig) const {
    resultConfig.clear();
    
    for (const auto& file : configFiles) {
        Config tempConfig;
        if (loadConfig(file, tempConfig)) {
            // Merge tempConfig into resultConfig
            // TODO: Implement merge logic
            printInfo("Loaded configuration from: " + file);
        } else {
            printError("Failed to load configuration from: " + file);
            return false;
        }
    }
    
    return true;
}

bool ConfigTool::compareConfigsImpl(const Config& config1, const Config& config2) const {
    // TODO: Implement comparison logic
    printInfo("Comparing configurations...");
    return true;
}

} // namespace nlm