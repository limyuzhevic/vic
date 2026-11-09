#include "NLMUtils.hpp"
#include "nlm/Brain.hpp"
#include "nlm/AgentBrain.hpp"
#include "nlm/SimpleWorld.hpp"
#include "nlm/Config.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <chrono>
#include <cmath>

namespace nlm {

std::shared_ptr<Config> NLMUtils::ConfigUtils::loadConfig(const std::string& filename) {
    auto config = std::make_shared<Config>();
    
    if (!fileExists(filename)) {
        std::cerr << "Config file not found: " << filename << std::endl;
        return config;
    }
    
    try {
        config->loadFromFile(filename);
        std::cout << "Successfully loaded configuration from: " << filename << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error loading config file: " << e.what() << std::endl;
    }
    
    return config;
}

bool NLMUtils::ConfigUtils::saveConfig(const Config& config, const std::string& filename) {
    try {
        config.saveToFile(filename);
        std::cout << "Configuration saved to: " << filename << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving config file: " << e.what() << std::endl;
        return false;
    }
}

std::shared_ptr<Config> NLMUtils::ConfigUtils::mergeConfigs(const Config& base, const std::string& overrideFile) {
    auto mergedConfig = std::make_shared<Config>(base);
    
    if (fileExists(overrideFile)) {
        try {
            mergedConfig->loadFromFile(overrideFile);
            std::cout << "Override configuration loaded from: " << overrideFile << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Warning: Could not load override file: " << e.what() << std::endl;
        }
    }
    
    return mergedConfig;
}

void NLMUtils::ConfigUtils::printConfig(const Config& config) {
    std::cout << "=== NLM Configuration ===" << std::endl;
    auto keys = config.getKeys();
    for (const auto& key : keys) {
        std::cout << key << " = " << config.getGeneric(key).getValue() << std::endl;
    }
}

std::string NLMUtils::ConfigUtils::getConfigSummary(const Config& config) {
    auto keys = config.getKeys();
    std::ostringstream summary;
    summary << "Config with " << keys.size() << " keys:";
    for (const auto& key : keys) {
        summary << " " << key << "=" << config.getGeneric(key).getValue();
    }
    return summary.str();
}