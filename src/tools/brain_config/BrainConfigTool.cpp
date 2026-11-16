// BrainConfigTool implementation for NLM

#pragma once

#include "BrainConfigTool.hpp"
#include "../Tools.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <memory>

namespace nlm {

BrainConfigToolImpl::BrainConfigToolImpl() {
    config_ = std::make_shared<Config>();
    NLM_LOG_INFO("BrainConfigToolImpl initialized");
}

bool BrainConfigToolImpl::create(const std::vector<std::pair<std::string, std::string>>& args) {
    try {
        // Set default values
        config_->set("brain.name", "DefaultBrain");
        config_->set("brain.region_count", "1");
        config_->set("brain.neuron_per_region", "100");
        config_->set("brain.synapse_per_neuron", "1000");
        config_->set("simulation.timestep", "0.001");
        config_->set("simulation.max_steps", "1000");
        config_->set("random.seed", "12345");
        config_->set("development.enabled", "true");
        config_->set("neuromodulation.enabled", "true");
        
        // Override with provided arguments
        for (const auto& arg : args) {
            config_->set(arg.first, arg.second);
        }
        
        NLM_LOG_INFO("Brain configuration created");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to create brain config: ") + e.what());
        return false;
    }
}

bool BrainConfigToolImpl::load(const std::string& filepath) {
    try {
        if (!ToolUtils::validateFile(filepath)) {
            NLM_LOG_ERROR("Invalid config file: " + filepath);
            return false;
        }
        
        bool success = config_->loadFromFile(filepath);
        if (!success) {
            NLM_LOG_ERROR("Failed to load config from: " + filepath);
            return false;
        }
        
        currentFile_ = filepath;
        NLM_LOG_INFO("Brain configuration loaded from: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to load brain config: ") + e.what());
        return false;
    }
}

bool BrainConfigToolImpl::save(const std::string& filepath, bool pretty) {
    try {
        if (!ToolUtils::validateFile(filepath, false)) {
            std::string dir = std::filesystem::path(filepath).parent_path();
            if (!ToolUtils::ensureDirectory(dir)) {
                NLM_LOG_ERROR("Cannot create directory: " + dir);
                return false;
            }
        }
        
        bool success = config_->saveToFile(filepath);
        if (!success) {
            NLM_LOG_ERROR("Failed to save config to: " + filepath);
            return false;
        }
        
        currentFile_ = filepath;
        NLM_LOG_INFO("Brain configuration saved to: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to save brain config: ") + e.what());
        return false;
    }
}

bool BrainConfigToolImpl::list(const std::string& pattern) {
    try {
        std::cout << "Brain Configuration Entries:" << std::endl;
        std::cout << "===========================" << std::endl;
        
        auto entries = config_->getAllEntries();
        for (const auto& entry : entries) {
            std::cout << entry.key << " = " << entry.value;
            if (pattern.empty() || ToolUtils::toLowercase(entry.key).find(ToolUtils::toLowercase(pattern)) != std::string::npos) {
                std::cout << " *";
            }
            std::cout << std::endl;
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to list brain config: ") + e.what());
        return false;
    }
}

bool BrainConfigToolImpl::set(const std::string& key, const std::string& value) {
    try {
        config_->set(key, value);
        NLM_LOG_INFO("Set config key: " + key + " = " + value);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to set brain config key: ") + e.what());
        return false;
    }
}

bool BrainConfigToolImpl::get(const std::string& key) {
    try {
        std::string value = config_->get(key, "");
        if (!value.empty()) {
            std::cout << key << " = " << value << std::endl;
        } else {
            NLM_LOG_WARNING("Config key not found: " + key);
        }
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to get brain config key: ") + e.what());
        return false;
    }
}

bool BrainConfigToolImpl::delete(const std::string& key) {
    try {
        config_->remove(key);
        NLM_LOG_INFO("Deleted config key: " + key);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to delete brain config key: ") + e.what());
        return false;
    }
}

bool BrainConfigToolImpl::validate(const std::string& filepath) {
    try {
        // Simple validation for config file
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Config file validation failed: " + filepath);
            return false;
        }
        
        std::string line;
        int lineNumber = 0;
        bool hasValidLines = false;
        
        while (std::getline(file, line)) {
            lineNumber++;
            line = ToolUtils::trim(line);
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Validate key=value format
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                NLM_LOG_WARNING("Invalid format at line " + std::to_string(lineNumber) + ": " + line);
                continue;
            }
            
            std::string key = ToolUtils::trim(line.substr(0, pos));
            std::string value = ToolUtils::trim(line.substr(pos + 1));
            
            if (key.empty() || value.empty()) {
                NLM_LOG_WARNING("Empty key or value at line " + std::to_string(lineNumber));
                continue;
            }
            
            hasValidLines = true;
        }
        
        if (!hasValidLines) {
            NLM_LOG_WARNING("Config file has no valid key=value pairs");
        }
        
        file.close();
        NLM_LOG_INFO("Config validation completed for: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to validate brain config: ") + e.what());
        return false;
    }
}

} // namespace nlm