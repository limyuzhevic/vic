// Configuration Presets and Settings Management
// Provides pre-defined configuration sets for different use cases
// Includes save/load functionality for configuration persistence
// Simple implementation without external JSON dependencies

#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nlm {

// Simple JSON implementation for configurations (standalone)
class SimpleJson {
public:
    static bool parse(const std::string& jsonStr, std::map<std::string, std::string>& out) {
        size_t pos = 0;
        while (pos < jsonStr.size()) {
            pos = jsonStr.find('"', pos);
            if (pos == std::string::npos) break;
            
            size_t keyEnd = jsonStr.find('"', pos + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = jsonStr.substr(pos + 1, keyEnd - pos - 1);
            pos = keyEnd + 1;
            
            if (pos < jsonStr.size() && jsonStr[pos] == ':') {
                pos++;
                while (pos < jsonStr.size() && (jsonStr[pos] == ' ' || jsonStr[pos] == '\n')) {
                    pos++;
                }
                
                if (pos < jsonStr.size() && jsonStr[pos] == '"') {
                    pos++;
                    size_t valueEnd = jsonStr.find('"', pos);
                    if (valueEnd != std::string::npos) {
                        std::string value = jsonStr.substr(pos, valueEnd - pos);
                        out[key] = value;
                        pos = valueEnd + 1;
                    }
                }
            }
            
            // Skip comma and whitespace
            while (pos < jsonStr.size() && (jsonStr[pos] == ',' || jsonStr[pos] == ' ' || jsonStr[pos] == '\n')) {
                pos++;
            }
        }
        return !out.empty();
    }
    
    static std::string toJson(const std::map<std::string, std::string>& data) {
        std::stringstream ss;
        ss << "{\n";
        bool first = true;
        for (const auto& item : data) {
            if (!first) ss << ",\n";
            ss << "  \"" << item.first << "\": \"" << item.second << "\"";
            first = false;
        }
        ss << "\n}";
        return ss.str();
    }
};

// Configuration presets for different use cases
class ConfigPresets {
public:
    struct Preset {
        std::string name;
        std::string description;
        std::string category; // beginner, expert, research, demo, etc.
        std::map<std::string, std::string> settings;
        std::string useCase;
        double estimatedRuntime; // in seconds
    };
    
    ConfigPresets() {
        initializePresets();
    }
    
    // Get all available presets
    std::vector<Preset> getAllPresets() const {
        return presets_;
    }
    
    // Get presets by category
    std::vector<Preset> getPresetsByCategory(const std::string& category) const {
        std::vector<Preset> result;
        for (const auto& preset : presets_) {
            if (preset.category == category) {
                result.push_back(preset);
            }
        }
        return result;
    }
    
    // Get preset by name
    Preset getPreset(const std::string& name) const {
        for (const auto& preset : presets_) {
            if (preset.name == name) {
                return preset;
            }
        }
        throw std::runtime_error("Preset not found: " + name);
    }
    
    // Apply preset to config
    void applyPreset(std::shared_ptr<Config> config, const std::string& presetName) {
        Preset preset = getPreset(presetName);
        
        // Convert string values to appropriate types
        for (const auto& setting : preset.settings) {
            applySetting(config, setting.first, setting.second);
        }
    }
    
    // Save preset to file
    bool savePreset(const std::string& filename, const std::string& presetName) {
        Preset preset = getPreset(presetName);
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        // Save as simple JSON-like format
        file << "{\n";
        file << "  \"name\": \"" << preset.name << "\",\n";
        file << "  \"description\": \"" << preset.description << "\",\n";
        file << "  \"category\": \"" << preset.category << "\",\n";
        file << "  \"useCase\": \"" << preset.useCase << "\",\n";
        file << "  \"estimatedRuntime\": " << preset.estimatedRuntime << ',\n';
        file << "  \"settings\": {\n";
        
        bool firstSetting = true;
        for (const auto& setting : preset.settings) {
            if (!firstSetting) file << ",\n";
            file << "    \"" << setting.first << "\": \"" << setting.second << "\"";
            firstSetting = false;
        }
        
        file << "\n  }\n";
        file << "}\n";
        
        return true;
    }
    
    // Load preset from file
    bool loadPreset(std::string& presetName, const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        // Simple parsing
        std::map<std::string, std::string> settings;
        if (!SimpleJson::parse(content, settings)) {
            return false;
        }
        
        Preset preset;
        preset.name = getValueFromJson(content, "name");
        preset.description = getValueFromJson(content, "description");
        preset.category = getValueFromJson(content, "category");
        preset.useCase = getValueFromJson(content, "useCase");
        
        try {
            preset.estimatedRuntime = std::stod(getValueFromJson(content, "estimatedRuntime"));
        } catch (...) {
            preset.estimatedRuntime = 0.0;
        }
        
        // Extract settings
        size_t settingsPos = content.find('"settings": {');
        if (settingsPos != std::string::npos) {
            size_t settingsEnd = content.find('}', settingsPos);
            if (settingsEnd != std::string::npos) {
                std::string settingsStr = content.substr(settingsPos + 13, settingsEnd - settingsPos - 13);
                SimpleJson::parse(settingsStr, preset.settings);
            }
        }
        
        presets_.push_back(preset);
        presetName = preset.name;
        return true;
    }
    
    // Get configuration recommendations based on use case
    std::vector<Preset> getRecommendedPresets(const std::string& useCase) {
        std::vector<Preset> result;
        for (const auto& preset : presets_) {
            if (preset.useCase == useCase) {
                result.push_back(preset);
            }
        }
        return result;
    }
    
    // Export all presets to JSON format
    bool exportAllPresets(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        // Write as simple array
        file << "[\n";
        
        for (size_t i = 0; i < presets_.size(); ++i) {
            const auto& preset = presets_[i];
            
            file << "{\n";
            file << "  \"name\": \"" << preset.name << "\",\n";
            file << "  \"description\": \"" << preset.description << "\",\n";
            file << "  \"category\": \"" << preset.category << "\",\n";
            file << "  \"useCase\": \"" << preset.useCase << "\",\n";
            file << "  \"estimatedRuntime\": " << preset.estimatedRuntime << ',\n';
            file << "  \"settings\": {\n";
            
            bool firstSetting = true;
            for (const auto& setting : preset.settings) {
                if (!firstSetting) file << ",\n";
                file << "    \"" << setting.first << "\": \"" << setting.second << "\"";
                firstSetting = false;
            }
            
            file << "\n  }\n";
            
            if (i < presets_.size() - 1) {
                file << "}\n,\n";
            } else {
                file << "}\n";
            }
        }
        
        file << "]\n";
        
        return true;
    }
    
    // Import presets from JSON format
    bool importPresets(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        // Simple parsing
        std::vector<Preset> importedPresets;
        size_t arrayStart = content.find('[');
        size_t arrayEnd = content.rfind(']');
        
        if (arrayStart == std::string::npos || arrayEnd == std::string::npos) {
            return false;
        }
        
        std::string arrayContent = content.substr(arrayStart + 1, arrayEnd - arrayStart - 1);
        
        // Simple split by "}{
        size_t pos = 0;
        while (pos < arrayContent.size()) {
            size_t braceStart = arrayContent.find('{', pos);
            size_t braceEnd = arrayContent.find('}', braceStart);
            
            if (braceStart == std::string::npos || braceEnd == std::string::npos) break;
            
            std::string presetStr = arrayContent.substr(braceStart, braceEnd - braceStart + 1);
            std::map<std::string, std::string> settings;
            
            if (SimpleJson::parse(presetStr, settings)) {
                Preset preset;
                preset.name = getValueFromJson(presetStr, "name");
                preset.description = getValueFromJson(presetStr, "description");
                preset.category = getValueFromJson(presetStr, "category");
                preset.useCase = getValueFromJson(presetStr, "useCase");
                
                try {
                    preset.estimatedRuntime = std::stod(getValueFromJson(presetStr, "estimatedRuntime"));
                } catch (...) {
                    preset.estimatedRuntime = 0.0;
                }
                
                // Extract settings
                size_t settingsPos = presetStr.find('"settings": {');
                if (settingsPos != std::string::npos) {
                    size_t settingsEnd = presetStr.find('}', settingsPos);
                    if (settingsEnd != std::string::npos) {
                        std::string settingsStr = presetStr.substr(settingsPos + 13, settingsEnd - settingsPos - 13);
                        SimpleJson::parse(settingsStr, preset.settings);
                    }
                }
                
                importedPresets.push_back(preset);
            }
            
            pos = braceEnd + 1;
            // Skip comma
            if (pos < arrayContent.size() && arrayContent[pos] == ',') {
                pos++;
            }
        }
        
        // Restore built-in presets by clearing and re-adding them
        std::vector<Preset> builtinPresets = presets_;
        presets_.clear();
        
        // Add imported presets
        presets_.insert(presets_.end(), importedPresets.begin(), importedPresets.end());
        
        // Add built-in presets
        presets_.insert(presets_.end(), builtinPresets.begin(), builtinPresets.end());
        
        return true;
    }
    
private:
    std::string getValueFromJson(const std::string& jsonStr, const std::string& key) {
        std::string search = "\"" + key + "\": \"";
        size_t pos = jsonStr.find(search);
        if (pos == std::string::npos) return "";
        
        size_t valueStart = pos + search.length();
        size_t valueEnd = jsonStr.find('"', valueStart);
        if (valueEnd == std::string::npos) return "";
        
        return jsonStr.substr(valueStart, valueEnd - valueStart);
    }
    
    void initializePresets() {
        // Beginner-friendly presets
        {
            Preset preset;
            preset.name = "neural-beginner";
            preset.description = "Simple neural network for learning and experimentation";
            preset.category = "beginner";
            preset.useCase = "learning";
            preset.estimatedRuntime = 10.0;
            preset.settings = {
                {"neuron_count", "100"},
                {"region_count", "1"},
                {"connection_probability", "0.05"},
                {"random_seed", "42"},
                {"simulation_timestep", "0.001"},
                {"enable_structural_plasticity", "false"},
                {"enable_development", "false"},
                {"enable_curiosity", "false"}
            };
            presets_.push_back(preset);
        }
        
        {
            Preset preset;
            preset.name = "neural-expert";
            preset.description = "Advanced neural network with full capabilities";
            preset.category = "expert";
            preset.useCase = "research";
            preset.estimatedRuntime = 60.0;
            preset.settings = {
                {"neuron_count", "2000"},
                {"region_count", "4"},
                {"connection_probability", "0.15"},
                {"random_seed", "42"},
                {"simulation_timestep", "0.001"},
                {"enable_structural_plasticity", "true"},
                {"enable_development", "true"},
                {"enable_curiosity", "true"},
                {"enable_reward_modulation", "true"},
                {"enable_novelty_detection", "true"},
                {"enable_prediction", "true"},
                {"enable_planning", "true"},
                {"enable_social_learning", "false"}
            };
            presets_.push_back(preset);
        }
        
        // Performance presets
        {
            Preset preset;
            preset.name = "neural-fast";
            preset.description = "Fast neural network for quick demos and testing";
            preset.category = "performance";
            preset.useCase = "demo";
            preset.estimatedRuntime = 5.0;
            preset.settings = {
                {"neuron_count", "200"},
                {"region_count", "2"},
                {"connection_probability", "0.2"},
                {"random_seed", "42"},
                {"simulation_timestep", "0.005"},
                {"enable_structural_plasticity", "false"},
                {"enable_development", "false"},
                {"enable_curiosity", "false"}
            };
            presets_.push_back(preset);
        }
        
        {
            Preset preset;
            preset.name = "neural-large";
            preset.description = "Large neural network for comprehensive research";
            preset.category = "performance";
            preset.useCase = "research";
            preset.estimatedRuntime = 120.0;
            preset.settings = {
                {"neuron_count", "5000"},
                {"region_count", "8"},
                {"connection_probability", "0.1"},
                {"random_seed", "42"},
                {"simulation_timestep", "0.001"},
                {"enable_structural_plasticity", "true"},
                {"enable_development", "true"},
                {"enable_curiosity", "true"},
                {"enable_reward_modulation", "true"},
                {"enable_novelty_detection", "true"},
                {"enable_prediction", "true"},
                {"enable_planning", "true"},
                {"enable_social_learning", "true"},
                {"enable_episodic_memory", "true"},
                {"enable_working_memory", "true"}
            };
            presets_.push_back(preset);
        }
        
        // Scientific experiment presets
        {
            Preset preset;
            preset.name = "neural-phase3";
            preset.description = "Phase 3 experimental setup: World interaction and learning";
            preset.category = "experiment";
            preset.useCase = "experiment";
            preset.estimatedRuntime = 30.0;
            preset.settings = {
                {"neuron_count", "500"},
                {"region_count", "2"},
                {"connection_probability", "0.1"},
                {"random_seed", "42"},
                {"simulation_timestep", "0.001"},
                {"enable_structural_plasticity", "true"},
                {"enable_development", "true"},
                {"enable_curiosity", "true"},
                {"enable_reward_modulation", "true"},
                {"enable_novelty_detection", "true"},
                {"world_width", "20"},
                {"world_height", "20"},
                {"vision_width", "16"},
                {"vision_height", "16"},
                {"agent_start_x", "10"},
                {"agent_start_y", "10"}
            };
            presets_.push_back(preset);
        }
        
        {
            Preset preset;
            preset.name = "neural-phase4";
            preset.description = "Phase 4 experimental setup: Cognitive mechanisms and memory";
            preset.category = "experiment";
            preset.useCase = "experiment";
            preset.estimatedRuntime = 90.0;
            preset.settings = {
                {"neuron_count", "1000"},
                {"region_count", "3"},
                {"connection_probability", "0.15"},
                {"random_seed", "42"},
                {"simulation_timestep", "0.001"},
                {"enable_structural_plasticity", "true"},
                {"enable_development", "true"},
                {"enable_curiosity", "true"},
                {"enable_reward_modulation", "true"},
                {"enable_novelty_detection", "true"},
                {"enable_prediction", "true"},
                {"enable_planning", "true"},
                {"enable_social_learning", "true"},
                {"enable_episodic_memory", "true"},
                {"enable_working_memory", "true"},
                {"working_memory_capacity", "50"},
                {"max_episodes", "100"},
                {"steps_per_episode", "200"}
            };
            presets_.push_back(preset);
        }
        
        {
            Preset preset;
            preset.name = "neural-phase6";
            preset.description = "Phase 6 experimental setup: Complete system integration";
            preset.category = "experiment";
            preset.useCase = "experiment";
            preset.estimatedRuntime = 180.0;
            preset.settings = {
                {"neuron_count", "1500"},
                {"region_count", "4"},
                {"connection_probability", "0.12"},
                {"random_seed", "42"},
                {"simulation_timestep", "0.001"},
                {"enable_structural_plasticity", "true"},
                {"enable_development", "true"},
                {"enable_curiosity", "true"},
                {"enable_reward_modulation", "true"},
                {"enable_novelty_detection", "true"},
                {"enable_prediction", "true"},
                {"enable_planning", "true"},
                {"enable_social_learning", "true"},
                {"enable_episodic_memory", "true"},
                {"enable_working_memory", "true"},
                {"enable_associative_memory", "true"},
                {"enable_checkpointing", "true"},
                {"enable_replay", "true"},
                {"max_episodes", "200"},
                {"steps_per_episode", "300"},
                {"world_width", "30"},
                {"world_height", "30"},
                {"vision_width", "24"},
                {"vision_height", "24"}
            };
            presets_.push_back(preset);
        }
        
        // Quick demo presets
        {
            Preset preset;
            preset.name = "neural-demo-quick";
            preset.description = "Quick demonstration of NLM capabilities";
            preset.category = "demo";
            preset.useCase = "demo";
            preset.estimatedRuntime = 3.0;
            preset.settings = {
                {"neuron_count", "50"},
                {"region_count", "1"},
                {"connection_probability", "0.1"},
                {"random_seed", "42"},
                {"simulation_timestep", "0.001"},
                {"enable_structural_plasticity", "false"},
                {"enable_development", "false"},
                {"enable_curiosity", "false"},
                {"enable_reward_modulation", "false"}
            };
            presets_.push_back(preset);
        }
        
        {
            Preset preset;
            preset.name = "neural-demo-visualization";
            preset.description = "Preset optimized for visualization and debugging";
            preset.category = "demo";
            preset.useCase = "demo";
            preset.estimatedRuntime = 8.0;
            preset.settings = {
                {"neuron_count", "200"},
                {"region_count", "2"},
                {"connection_probability", "0.08"},
                {"random_seed", "42"},
                {"simulation_timestep", "0.001"},
                {"enable_structural_plasticity", "true"},
                {"enable_development", "true"},
                {"enable_curiosity", "true"},
                {"enable_reward_modulation", "true"},
                {"enable_novelty_detection", "true"},
                {"enable_prediction", "true"},
                {"enable_planning", "false"},
                {"enable_social_learning", "false"},
                {"log_level", "debug"}
            };
            presets_.push_back(preset);
        }
    }
    
    void applySetting(std::shared_ptr<Config> config, const std::string& key, const std::string& value) {
        // Convert string value to appropriate type based on key
        if (key == "neuron_count" || key == "region_count" || key == "world_width" || key == "world_height" || key == "vision_width" || key == "vision_height" || key == "working_memory_capacity" || key == "max_episodes" || key == "steps_per_episode") {
            try {
                int64_t intValue = std::stoll(value);
                config->set(key, intValue);
            } catch (...) {
                config->set(key, 0);
            }
        } else if (key == "connection_probability" || key == "enable_structural_plasticity" || key == "enable_development" || key == "enable_curiosity" || key == "enable_reward_modulation" || key == "enable_novelty_detection" || key == "enable_prediction" || key == "enable_planning" || key == "enable_social_learning" || key == "enable_episodic_memory" || key == "enable_working_memory" || key == "enable_associative_memory" || key == "enable_checkpointing" || key == "enable_replay") {
            bool boolValue = (value == "true" || value == "1" || value == "yes" || value == "on");
            config->set(key, boolValue);
        } else if (key == "random_seed" || key == "agent_start_x" || key == "agent_start_y") {
            try {
                uint64_t uintValue = std::stoull(value);
                config->set(key, uintValue);
            } catch (...) {
                config->set(key, static_cast<uint64_t>(42));
            }
        } else if (key == "simulation_timestep" || key == "estimatedRuntime") {
            try {
                double doubleValue = std::stod(value);
                config->set(key, doubleValue);
            } catch (...) {
                config->set(key, 0.001);
            }
        } else if (key == "log_level") {
            config->set(key, value);
        } else {
            // Generic string setting
            config->set(key, value);
        }
    }
    
    std::vector<Preset> presets_;
};

// Configuration manager with preset support
class ConfigManager {
public:
    ConfigManager() : currentPreset_("") {}
    
    // Get configuration presets
    ConfigPresets getPresets() const {
        return presets_;
    }
    
    // Get preset by name
    ConfigPresets::Preset getPreset(const std::string& presetName) const {
        return presets_.getPreset(presetName);
    }
    
    // Apply preset to config
    void applyPreset(std::shared_ptr<Config> config, const std::string& presetName) {
        presets_.applyPreset(config, presetName);
        currentPreset_ = presetName;
    }
    
    // Get current preset
    std::string getCurrentPreset() const {
        return currentPreset_;
    }
    
    // Get recommended configurations for a use case
    std::vector<std::string> getRecommendedPresets(const std::string& useCase) {
        return presets_.getRecommendedPresets(useCase);
    }
    
    // Save preset to file
    bool savePreset(const std::string& filename, const std::string& presetName) {
        return presets_.savePreset(filename, presetName);
    }
    
    // Load preset from file
    bool loadPreset(std::string& presetName, const std::string& filename) {
        return presets_.loadPreset(presetName, filename);
    }
    
    // Export all presets
    bool exportAllPresets(const std::string& filename) {
        return presets_.exportAllPresets(filename);
    }
    
    // Import presets
    bool importPresets(const std::string& filename) {
        return presets_.importPresets(filename);
    }
    
private:
    ConfigPresets presets_;
    std::string currentPreset_;
};

// Convenience functions
inline std::shared_ptr<Config> createConfigFromPreset(const std::string& presetName) {
    auto config = std::make_shared<Config>();
    ConfigManager configManager;
    configManager.applyPreset(config, presetName);
    return config;
}

inline std::shared_ptr<Config> createDefaultConfig() {
    return std::make_shared<Config>();
}

inline std::string getPresetDescription(const std::string& presetName) {
    try {
        ConfigManager configManager;
        ConfigPresets::Preset preset = configManager.getPreset(presetName);
        return preset.description;
    } catch (const std::runtime_error& e) {
        return "Preset not found: " + presetName;
    }
}

inline std::vector<std::string> getAllPresetNames() {
    ConfigManager configManager;
    std::vector<ConfigPresets::Preset> allPresets = configManager.getPresets().getAllPresets();
    std::vector<std::string> names;
    
    for (const auto& preset : allPresets) {
        names.push_back(preset.name);
    }
    
    return names;
}

}} // namespace nlm