// #include "Config.hpp"
// #include <fstream>
// #include <sstream>
// #include <algorithm>
// #include <filesystem>
// #include <nlohmann/json.hpp>
// #include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string schema;  // JSON schema for validation
    bool useExternalParsers;  // Flag to enable external JSON/YAML parsers
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Set default schema for configuration validation
    pImpl->schema = R"({
        "type": "object",
        "properties": {
            "random_seed": {"type": "integer"},
            "simulation_timestep": {"type": "number"},
            "neuron_count": {"type": "integer"},
            "region_count": {"type": "integer"},
            "connection_probability": {"type": "number"},
            "plasticity_learning_rate": {"type": "number"},
            "stdp_ltp_weight": {"type": "number"},
            "stdp_ltd_weight": {"type": "number"},
            "stdp_time_constant": {"type": "number"},
            "development_synaptogenesis_rate": {"type": "number"},
            "development_pruning_rate": {"type": "number"},
            "dopamine_baseline": {"type": "number"},
            "reward_discount_factor": {"type": "number"},
            "environment_name": {"type": "string"},
            "environment_width": {"type": "integer"},
            "environment_height": {"type": "integer"},
            "log_level": {"type": "string", "enum": ["DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"]},
            "log_to_file": {"type": "boolean"},
            "log_filename": {"type": "string"},
            "max_simulation_steps": {"type": "integer"},
            "simulation_time_limit": {"type": "number"},
            "visualization_enabled": {"type": "boolean"},
            "visualization_update_rate": {"type": "number"}
        },
        "required": ["random_seed", "simulation_timestep", "neuron_count", "region_count"]
    })";
    
    // Try to detect if external JSON/YAML parsers are available
    pImpl->useExternalParsers = checkExternalParserAvailability();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file format based on extension
    std::string ext = filepath.substr(filepath.find_last_of(".") + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        return loadFromJSON(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return loadFromYAML(filepath);
    } else {
        // Default to simple key=value format for backward compatibility
        return loadFromSimpleFile(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open JSON configuration file: " + filepath);
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Validate against schema
        if (!validateJSONSchema(jsonData)) {
            NLM_LOG_ERROR("JSON configuration validation failed: " + filepath);
            return false;
        }
        
        // Convert JSON to ConfigEntry vector
        convertJSONToConfig(jsonData);
        
        NLM_LOG_INFO("Successfully loaded JSON configuration from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading JSON configuration from " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        YAML::Node yamlData = YAML::LoadFile(filepath);
        
        // Convert YAML to JSON for consistent validation
        nlohmann::json jsonData = nlohmann::json::parse(yamlData.as<std::string>());
        
        // Validate against schema
        if (!validateJSONSchema(jsonData)) {
            NLM_LOG_ERROR("YAML configuration validation failed: " + filepath);
            return false;
        }
        
        // Convert JSON to ConfigEntry vector
        convertJSONToConfig(jsonData);
        
        NLM_LOG_INFO("Successfully loaded YAML configuration from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading YAML configuration from " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    NLM_LOG_INFO("Successfully loaded simple configuration from: " + filepath);
    return true;
}

bool Config::validateJSONSchema(const nlohmann::json& data) const {
    try {
        // Use JSON schema validation (simplified implementation)
        // In production, you'd use a full JSON schema validator
        
        // Check required fields
        if (!data.contains("random_seed") || !data.contains("simulation_timestep") || 
            !data.contains("neuron_count") || !data.contains("region_count")) {
            return false;
        }
        
        // Validate types
        if (!data["random_seed"].is_number_integer()) return false;
        if (!data["simulation_timestep"].is_number()) return false;
        if (!data["neuron_count"].is_number_integer()) return false;
        if (!data["region_count"].is_number_integer()) return false;
        
        // Check that connection_probability is between 0 and 1 if present
        if (data.contains("connection_probability") && 
            (data["connection_probability"].is_number() && 
             (data["connection_probability"].get<double>() < 0.0 || 
              data["connection_probability"].get<double>() > 1.0))) {
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON schema validation error: " + std::string(e.what()));
        return false;
    }
}

void Config::convertJSONToConfig(const nlohmann::json& data) {
    clear();
    
    // Helper function to convert JSON value to ConfigValue
    auto convertValue = [](const nlohmann::json& value) -> ConfigValue {
        if (value.is_string()) {
            return value.get<std::string>();
        } else if (value.is_number_integer()) {
            return value.get<int64_t>();
        } else if (value.is_number_float()) {
            return value.get<double>();
        } else if (value.is_boolean()) {
            return value.get<bool>();
        } else {
            // Default to string for unknown types
            return value.get<std::string>();
        }
    };
    
    // Convert all JSON fields to ConfigEntry
    for (auto it = data.begin(); it != data.end(); ++it) {
        std::string key = it.key();
        ConfigValue configValue = convertValue(it.value());
        set(key, configValue, ConfigSource::File);
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::string ext = filepath.substr(filepath.find_last_of(".") + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        return saveToJSON(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return saveToYAML(filepath);
    } else {
        return saveToSimpleFile(filepath);
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
    try {
        nlohmann::json jsonData;
        
        // Convert ConfigEntry vector to JSON
        for (const auto& entry : pImpl->entries) {
            std::visit([&jsonData, &entry](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    jsonData[entry.key] = arg;
                }
            }, entry.value);
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to save JSON configuration to: " + filepath);
            return false;
        }
        
        file << std::setw(4) << jsonData;
        
        NLM_LOG_INFO("Successfully saved JSON configuration to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving JSON configuration to " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToYAML(const std::string& filepath) const {
    try {
        // Convert ConfigEntry vector to YAML
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            emitter << YAML::Key << entry.key;
            
            std::visit([&emitter](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    emitter << YAML::Value << arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    emitter << YAML::Value << static_cast<int>(arg);
                } else if constexpr (std::is_same_v<T, double>) {
                    emitter << YAML::Value << arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    emitter << YAML::Value << arg;
                }
            }, entry.value);
        }
        
        emitter << YAML::EndMap;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to save YAML configuration to: " + filepath);
            return false;
        }
        
        file << emitter.c_str();
        
        NLM_LOG_INFO("Successfully saved YAML configuration to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving YAML configuration to " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToSimpleFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to save simple configuration to: " + filepath);
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.key << "\n";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, double>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            }
        }, entry.value);
    }
    
    NLM_LOG_INFO("Successfully saved simple configuration to: " + filepath);
    return true;
}

bool Config::checkExternalParserAvailability() const {
    // Check if external JSON/YAML parsers are available
    // This is a simple check - in production, you'd use more sophisticated detection
    
    // For now, we assume they're available for the demo
    // In a real implementation, you'd check:
    // 1. nlohmann/json.hpp availability
    // 2. yaml-cpp availability
    // 3. Proper compiler support
    
    return true;  // Enable external parsers for demo purposes
}

bool Config::loadFromYAML(const std::string& filepath) {
    return loadFromSimpleFile(filepath);  // TODO: Implement YAML parsing
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    NLM_LOG_INFO("Successfully loaded configuration from: " + filepath);
    return true;
}

bool Config::validateJSONSchema(const nlohmann::json& data) const {
    // TODO: Implement full JSON schema validation
    // For now, basic validation
    if (!data.contains("random_seed") || !data.contains("simulation_timestep") || 
        !data.contains("neuron_count") || !data.contains("region_count")) {
        return false;
    }
    
    if (!data["random_seed"].is_number_integer()) return false;
    if (!data["simulation_timestep"].is_number()) return false;
    if (!data["neuron_count"].is_number_integer()) return false;
    if (!data["region_count"].is_number_integer()) return false;
    
    return true;
}

void Config::convertJSONToConfig(const nlohmann::json& data) {
    clear();
    
    auto convertValue = [](const nlohmann::json& value) -> ConfigValue {
        if (value.is_string()) {
            return value.get<std::string>();
        } else if (value.is_number_integer()) {
            return value.get<int64_t>();
        } else if (value.is_number_float()) {
            return value.get<double>();
        } else if (value.is_boolean()) {
            return value.get<bool>();
        } else {
            return value.get<std::string>();
        }
    };
    
    for (auto it = data.begin(); it != data.end(); ++it) {
        std::string key = it.key();
        ConfigValue configValue = convertValue(it.value());
        set(key, configValue, ConfigSource::File);
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
    // TODO: Implement JSON saving
    return saveToSimpleFile(filepath);
}

bool Config::saveToYAML(const std::string& filepath) const {
    // TODO: Implement YAML saving
    return saveToSimpleFile(filepath);
}

bool Config::saveToSimpleFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to save configuration to: " + filepath);
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.key << "\n";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, double>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            }
        }, entry.value);
    }
    
    NLM_LOG_INFO("Successfully saved configuration to: " + filepath);
    return true;
}

} // namespace nlm
