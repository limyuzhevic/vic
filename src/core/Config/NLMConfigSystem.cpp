// NLM Configuration System - Implementation
// This file provides the implementation for advanced configuration handling,
// validation, and utilities for the NLM (Neural Learning Machine) demos and applications.

#include "core/Config/NLMConfigSystem.hpp"
#include <fstream>
#include <filesystem>
#include <algorithm>

namespace nlm {

// ConfigValidator implementation

void ConfigValidator::addCustomValidator(const std::string& key, std::function<bool(float)> validator) {
    customValidators[key] = validator;
}

// ConfigFileManager implementation

bool ConfigFileManager::saveConfig(const std::shared_ptr<Config>& config, const std::string& filepath) {
    try {
        std::vector<std::string> errors = validator.validateAll(config);
        if (!errors.empty()) {
            std::cerr << "[CONFIG ERROR] Cannot save invalid configuration:" << std::endl;
            for (const auto& error : errors) {
                std::cerr << "  - " << error << std::endl;
            }
            return false;
        }
        
        if (config->saveToFile(filepath)) {
            std::cout << "[CONFIG SUCCESS] Configuration saved to " << filepath << std::endl;
            return true;
        } else {
            std::cerr << "[CONFIG ERROR] Failed to save configuration to " << filepath << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "[CONFIG ERROR] Exception while saving configuration: " << e.what() << std::endl;
        return false;
    }
}

// ConfigPresets implementation

} // namespace nlm
