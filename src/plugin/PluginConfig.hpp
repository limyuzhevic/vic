#pragma once

#include "PluginBase.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <iomanip>

namespace nlm {

class PluginConfig {
public:
    // Load configuration from JSON file
    static bool loadFromFile(const std::string& filepath, 
                            std::unordered_map<std::string, std::string>& config) {
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            nlohmann::json jsonConfig;
            file >> jsonConfig;
            
            for (auto it = jsonConfig.begin(); it != jsonConfig.end(); ++it) {
                config[it.key()] = it.value().dump();
            }
            
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    // Save configuration to JSON file
    static bool saveToFile(const std::string& filepath,
                          const std::unordered_map<std::string, std::string>& config) {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            nlohmann::json jsonConfig;
            
            for (const auto& pair : config) {
                // Try to parse as JSON, otherwise store as string
                try {
                    nlohmann::json value = nlohmann::json::parse(pair.second);
                    jsonConfig[pair.first] = value;
                } catch (...) {
                    jsonConfig[pair.first] = pair.second;
                }
            }
            
            file << std::setw(2) << jsonConfig << std::endl;
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    // Convert configuration to JSON string
    static std::string toJSON(const std::unordered_map<std::string, std::string>& config) {
        std::stringstream ss;
        ss << std::setw(2) << nlohmann::json(config);
        return ss.str();
    }
    
    // Parse configuration from JSON string
    static bool fromJSON(const std::string& jsonStr, 
                        std::unordered_map<std::string, std::string>& config) {
        try {
            nlohmann::json jsonConfig = nlohmann::json::parse(jsonStr);
            
            for (auto it = jsonConfig.begin(); it != jsonConfig.end(); ++it) {
                config[it.key()] = it.value().dump();
            }
            
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    // Configuration preset management
    static bool loadPreset(const std::string& presetName,
                          std::unordered_map<std::string, std::string>& config) {
        // Try to load from presets directory
        std::string filepath = "presets/" + presetName + ".json";
        return loadFromFile(filepath, config);
    }
    
    static std::vector<std::string> getAvailablePresets() {
        std::vector<std::string> presets;
        // This would scan the presets directory
        // For now, return common preset names
        return {"default", "high_performance", "balanced", "accurate"};
    }
    
    // Configuration validation
    static bool validateConfig(const std::unordered_map<std::string, std::string>& config,
                              const std::string& pluginType) {
        // Basic validation - can be extended for specific plugin types
        for (const auto& pair : config) {
            if (pair.first.empty() || pair.second.empty()) {
                return false;
            }
            
            // Type-specific validation
            if (pluginType == "neuron") {
                if (pair.first == "neuron_leak_conductance") {
                    try {
                        float value = std::stof(pair.second);
                        if (value < 0.0f || value > 100.0f) return false;
                    } catch (...) {
                        return false;
                    }
                }
            } else if (pluginType == "synapse") {
                if (pair.first == "synapse_initial_weight") {
                    try {
                        float value = std::stof(pair.second);
                        if (value < -10.0f || value > 10.0f) return false;
                    } catch (...) {
                        return false;
                    }
                }
            }
        }
        
        return true;
    }
    
    // Configuration merging
    static std::unordered_map<std::string, std::string> mergeConfigs(
        const std::unordered_map<std::string, std::string>& base,
        const std::unordered_map<std::string, std::string>& override) {
        
        std::unordered_map<std::string, std::string> merged = base;
        
        for (const auto& pair : override) {
            merged[pair.first] = pair.second;
        }
        
        return merged;
    }
    
    // Configuration diffing
    static std::unordered_map<std::string, std::pair<std::string, std::string>> 
    diffConfigs(const std::unordered_map<std::string, std::string>& oldConfig,
                const std::unordered_map<std::string, std::string>& newConfig) {
        
        std::unordered_map<std::string, std::pair<std::string, std::string>> changes;
        
        // Check for new or modified keys
        for (const auto& pair : newConfig) {
            auto it = oldConfig.find(pair.first);
            if (it == oldConfig.end() || it->second != pair.second) {
                changes[pair.first] = std::make_pair(
                    it != oldConfig.end() ? it->second : "",
                    pair.second
                );
            }
        }
        
        // Check for deleted keys
        for (const auto& pair : oldConfig) {
            auto it = newConfig.find(pair.first);
            if (it == newConfig.end()) {
                changes[pair.first] = std::make_pair(pair.second, "");
            }
        }
        
        return changes;
    }
};

} // namespace nlm