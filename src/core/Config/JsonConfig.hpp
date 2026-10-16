// JSON configuration support for NLM
// Replaces the simple key=value format with proper JSON parsing

#include "core/Config/Config.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

using json = nlohmann::json;

namespace nlm {

// JSON config loader implementation
class JsonConfigLoader {
public:
    static bool loadFromJson(const std::string& filepath, std::vector<ConfigEntry>& entries) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        try {
            json j;
            file >> j;
            
            // Clear existing entries
            entries.clear();
            
            // Parse JSON object
            if (j.is_object()) {
                for (auto it = j.begin(); it != j.end(); ++it) {
                    std::string key = it.key();
                    json value = it.value();
                    
                    // Convert JSON value to ConfigValue
                    ConfigValue configValue = jsonToConfigValue(value);
                    
                    // Add description from JSON if present
                    std::string description = "";
                    if (value.contains("_description")) {
                        description = value["_description"].get<std::string>();
                    }
                    
                    entries.emplace_back(key, configValue, ConfigSource::File, description);
                }
            } else {
                // Try to parse as array
                std::vector<std::string> keys;
                for (size_t i = 0; i < j.size(); ++i) {
                    keys.push_back("item_" + std::to_string(i));
                }
                
                for (size_t i = 0; i < j.size(); ++i) {
                    json value = j[i];
                    ConfigValue configValue = jsonToConfigValue(value);
                    entries.emplace_back(keys[i], configValue, ConfigSource::File);
                }
            }
            
            return true;
        } catch (const json::exception& e) {
            // Log error if available
            return false;
        }
    }
    
    static ConfigValue jsonToConfigValue(const json& j) {
        if (j.is_string()) {
            return j.get<std::string>();
        } else if (j.is_number_integer()) {
            return j.get<int64_t>();
        } else if (j.is_number_unsigned()) {
            return j.get<uint64_t>();
        } else if (j.is_number()) {
            return j.get<double>();
        } else if (j.is_boolean()) {
            return j.get<bool>();
        } else if (j.is_array()) {
            // For arrays, store as string representation
            return j.dump();
        } else if (j.is_object()) {
            // For objects, store as string representation
            return j.dump();
        } else {
            // null or other
            return std::string();
        }
    }
    
    static bool saveToJson(const std::string& filepath, const std::vector<ConfigEntry>& entries) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        json j;
        
        for (const auto& entry : entries) {
            // Convert back from ConfigValue
            json value;
            std::visit([&value](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    value = arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    value = arg;
                } else if constexpr (std::is_same_v<T, uint64_t>) {
                    value = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    value = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    value = arg;
                }
            }, entry.value);
            
            // Add description as metadata
            if (!entry.description.empty()) {
                value["_description"] = entry.description;
            }
            
            j[entry.key] = value;
        }
        
        try {
            file << j.dump(2);  // Pretty print with 2-space indentation
            return true;
        } catch (const json::exception& e) {
            return false;
        }
    }
};

// Enhanced Config class with JSON support
class EnhancedConfig : public Config {
public:
    bool loadFromJsonFile(const std::string& filepath) {
        std::vector<ConfigEntry> entries;
        if (JsonConfigLoader::loadFromJson(filepath, entries)) {
            pImpl->entries = std::move(entries);
            return true;
        }
        return false;
    }
    
    bool saveToJsonFile(const std::string& filepath) const {
        return JsonConfigLoader::saveToJson(filepath, pImpl->entries);
    }
    
    // Merge JSON with existing config
    bool mergeFromJson(const std::string& filepath) {
        std::vector<ConfigEntry> newEntries;
        if (!JsonConfigLoader::loadFromJson(filepath, newEntries)) {
            return false;
        }
        
        // Add new entries or update existing ones
        for (const auto& newEntry : newEntries) {
            auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&newEntry](const ConfigEntry& e) { return e.key == newEntry.key; });
            
            if (it != pImpl->entries.end()) {
                // Update existing entry
                it->value = newEntry.value;
                it->source = ConfigSource::File;  // Mark as file-loaded
            } else {
                // Add new entry
                pImpl->entries.push_back(newEntry);
            }
        }
        
        return true;
    }
    
    // Get configuration as JSON string
    std::string toJsonString() const {
        json j;
        
        for (const auto& entry : pImpl->entries) {
            json value;
            std::visit([&value](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    value = arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    value = arg;
                } else if constexpr (std::is_same_v<T, uint64_t>) {
                    value = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    value = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    value = arg;
                }
            }, entry.value);
            
            if (!entry.description.empty()) {
                value["_description"] = entry.description;
            }
            
            j[entry.key] = value;
        }
        
        return j.dump(2);
    }
    
    // Validate configuration against schema
    bool validateConfig(const json& schema) const {
        try {
            json instance;
            for (const auto& entry : pImpl->entries) {
                json value;
                std::visit([&value](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        value = arg;
                    } else if constexpr (std::is_same_v<T, int64_t>) {
                        value = arg;
                    } else if constexpr (std::is_same_v<T, uint64_t>) {
                        value = arg;
                    } else if constexpr (std::is_same_v<T, double>) {
                        value = arg;
                    } else if constexpr (std::is_same_v<T, bool>) {
                        value = arg;
                    }
                }, entry.value);
                
                instance[entry.key] = value;
            }
            
            // Basic validation: required fields
            if (schema.contains("_required")) {
                json required = schema["_required"];
                for (const auto& item : required) {
                    std::string key = item.get<std::string>();
                    if (!has(key)) {
                        return false;
                    }
                }
            }
            
            return true;
        } catch (const json::exception& e) {
            return false;
        }
    }
};

// Configuration presets for different use cases
class ConfigPresets {
public:
    static std::shared_ptr<Config> createDemoConfig() {
        auto config = std::make_shared<EnhancedConfig>();
        
        // Basic settings for demo
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.2f);
        
        // Short simulation for quick demos
        config->set("simulation_steps", 500);
        config->set("simulation_timestep", 0.001);
        
        // Enable plasticity
        config->set("enable_hebbian", true);
        config->set("enable_stdp", true);
        config->set("enable_structural_plasticity", true);
        
        // Reward settings
        config->set("reward_range", 1.0);
        config->set("enable_reward_modulation", true);
        
        return config;
    }
    
    static std::shared_ptr<Config> createResearchConfig() {
        auto config = std::make_shared<EnhancedConfig>();
        
        // Larger network for research
        config->set("neuron_count", 10000);
        config->set("region_count", 4);
        config->set("connection_probability", 0.05f);
        
        // Longer simulation for analysis
        config->set("simulation_steps", 10000);
        config->set("simulation_timestep", 0.0001);
        
        // Comprehensive plasticity
        config->set("enable_hebbian", true);
        config->set("enable_stdp", true);
        config->set("enable_structural_plasticity", true);
        config->set("enable_reward_modulation", true);
        
        // Advanced neuromodulation
        config->set("enable_curiosity", true);
        config->set("enable_novelty_detection", true);
        config->set("enable_prediction_error", true);
        
        // Development
        config->set("enable_development", true);
        config->set("developmental_stage", "adult");
        
        return config;
    }
    
    static std::shared_ptr<Config> createRealTimeConfig() {
        auto config = std::make_shared<EnhancedConfig>();
        
        // Small network for real-time interaction
        config->set("neuron_count", 50);
        config->set("region_count", 1);
        config->set("connection_probability", 0.3f);
        
        // Fast simulation
        config->set("simulation_steps", 100);
        config->set("simulation_timestep", 0.001);
        
        // Minimal plasticity for stability
        config->set("enable_hebbian", true);
        config->set("enable_stdp", false);  // Disable for stability
        config->set("enable_structural_plasticity", false);
        
        // Focus on responsiveness
        config->set("reward_range", 10.0);
        config->set("enable_reward_modulation", true);
        
        return config;
    }
    
    static std::shared_ptr<Config> createCompetitiveConfig() {
        auto config = std::make_shared<EnhancedConfig>();
        
        // Multiple competing networks
        config->set("neuron_count", 200);
        config->set("region_count", 3);
        config->set("connection_probability", 0.15f);
        
        // Extended training
        config->set("simulation_steps", 5000);
        config->set("simulation_timestep", 0.001);
        
        // Competitive dynamics
        config->set("enable_hebbian", true);
        config->set("enable_stdp", true);
        config->set("enable_structural_plasticity", true);
        
        // Competition settings
        config->set("competition_enabled", true);
        config->set("fitness_threshold", 0.7f);
        config->set("elimination_rate", 0.1f);
        
        return config;
    }
};

} // namespace nlm
