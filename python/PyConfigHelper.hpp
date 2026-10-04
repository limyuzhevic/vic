// Helper classes for enhanced NLM Python bindings
// These provide additional functionality and utilities for Python users

#pragma once

#include "../src/core/Config/Config.hpp"
#include <string>
#include <sstream>

namespace nlm {

class PyConfigHelper {
public:
    // Load configuration from JSON string
    static std::shared_ptr<Config> loadFromJsonString(const std::string& json_str) {
        auto config = std::make_shared<Config>();
        // For now, we'll use a simple implementation
        // In a real implementation, you'd use a JSON parser like nlohmann::json
        return config;
    }
    
    // Get configuration schema as JSON string
    static std::string getSchema() {
        return R"({
            "type": "object",
            "properties": {
                "brain": {
                    "type": "object",
                    "properties": {
                        "neuron_count": {"type": "integer", "minimum": 1},
                        "region_count": {"type": "integer", "minimum": 1},
                        "connection_probability": {"type": "number", "minimum": 0.0, "maximum": 1.0},
                        "synapse_density": {"type": "number", "minimum": 0.0, "maximum": 1.0},
                        "v_thresh": {"type": "number"},
                        "v_rest": {"type": "number"},
                        "v_reset": {"type": "number"}
                    }
                },
                "plasticity": {
                    "type": "object",
                    "properties": {
                        "stdp": {
                            "type": "object",
                            "properties": {
                                "enable": {"type": "boolean"},
                                "learning_rate": {"type": "number"}
                            }
                        }
                    }
                }
            }
        })";
    }
    
    // Validate configuration
    static bool validateConfig(const std::shared_ptr<Config>& config) {
        if (!config) return false;
        
        // Check required keys
        if (config->getKeys().empty()) return false;
        
        // Validate specific constraints
        auto neuron_count = config->getOr("brain.neuron_count", 0);
        if (neuron_count <= 0) return false;
        
        auto connection_probability = config->getOr("brain.connection_probability", 0.0);
        if (connection_probability < 0.0 || connection_probability > 1.0) return false;
        
        return true;
    }
    
    // Get configuration summary with formatted output
    static std::string getFormattedSummary(const std::shared_ptr<Config>& config) {
        if (!config) return "Invalid configuration";
        
        std::string summary = "Configuration Summary:\n";
        auto keys = config->getKeys();
        
        for (const auto& key : keys) {
            auto value = config->get<std::string>(key);
            summary += "  " + key + ": " + value + "\n";
        }
        
        return summary;
    }
};

} // namespace nlm
