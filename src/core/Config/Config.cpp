#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <string>
#include <variant>
#include <memory>
#include <stdexcept>

// Include nlohmann/json for JSON parsing
#include <nlohmann/json.hpp>

// Forward declare YAML parser if available
#ifdef HAVE_YAML_CPP
#include <yaml-cpp/yaml.h>
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to parse as JSON first
    std::ifstream jsonFile(filepath);
    if (jsonFile.good()) {
        try {
            nlohmann::json jsonData;
            jsonFile >> jsonData;
            clear();
            parseJson(jsonData, ConfigSource::File);
            return true;
        } catch (const nlohmann::json::exception& e) {
            // Log JSON parsing error but continue with other formats
            // NLM_LOG_ERROR("JSON parsing failed for " + filepath + ": " + std::string(e.what()));
        } catch (const std::exception& e) {
            // Log generic error but continue with other formats
            // NLM_LOG_ERROR("JSON parsing failed for " + filepath + ": " + std::string(e.what()));
        }
    }
    
    // Try to parse as YAML (if available)
#ifdef HAVE_YAML_CPP
    try {
        YAML::Node yamlData = YAML::LoadFile(filepath);
        clear();
        parseYaml(yamlData, ConfigSource::File);
        return true;
    } catch (const YAML::Exception& e) {
        // Log YAML parsing error but continue with other formats
        // NLM_LOG_ERROR("YAML parsing failed for " + filepath + ": " + e.what());
    } catch (const std::exception& e) {
        // Log generic error but continue with other formats
        // NLM_LOG_ERROR("YAML parsing failed for " + filepath + ": " + std::string(e.what()));
    }
#endif
    
    // Fallback to simple key=value format for backward compatibility
    std::ifstream file(filepath);
    if (!file.is_open()) {
        // NLM_LOG_ERROR("Could not open file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            // NLM_LOG_WARNING("Invalid line in config file " + filepath + ": line " + 
            //                std::to_string(lineNumber) + ": missing '='")
            continue; // Skip malformed lines
        }
        
        try {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
            
        } catch (const std::exception& e) {
            // NLM_LOG_ERROR("Error parsing line " + std::to_string(lineNumber) + 
            //               " in config file " + filepath + ": " + std::string(e.what()));
            continue; // Continue with other lines
        #endif

// Configuration preset implementations
namespace nlm {

std::shared_ptr<Config> Config::createPresetConfig(const std::string& presetName) {
    auto config = std::make_shared<Config>();
    
    if (presetName == "fast") {
        // Fast, small brain for real-time applications
        config->set("neuron_count", 100, ConfigSource::Default);
        config->set("region_count", 1, ConfigSource::Default);
        config->set("connection_probability", 0.05f, ConfigSource::Default);
        config->set("simulation_timestep", 0.0001, ConfigSource::Default);
        config->set("random_seed", 42u, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
    } else if (presetName == "accurate") {
        // Large, accurate brain for research
        config->set("neuron_count", 5000, ConfigSource::Default);
        config->set("region_count", 4, ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("random_seed", 123u, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.structural.enable", true, ConfigSource::Default);
    } else if (presetName == "balanced") {
        // Balanced configuration for general use
        config->set("neuron_count", 1000, ConfigSource::Default);
        config->set("region_count", 2, ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("random_seed", 42u, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
    } else if (presetName == "exploratory") {
        // High exploration, curiosity-driven learning
        config->set("neuron_count", 2000, ConfigSource::Default);
        config->set("region_count", 3, ConfigSource::Default);
        config->set("connection_probability", 0.12f, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("random_seed", 999u, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        config->set("neuromod.curiosity.strength", 2.0f, ConfigSource::Default);
    } else if (presetName == "conservative") {
        // Conservative, stable learning with low plasticity
        config->set("neuron_count", 500, ConfigSource::Default);
        config->set("region_count", 1, ConfigSource::Default);
        config->set("connection_probability", 0.08f, ConfigSource::Default);
        config->set("simulation_timestep", 0.0005, ConfigSource::Default);
        config->set("random_seed", 777u, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", false, ConfigSource::Default);
        config->set("plasticity.structural.enable", false, ConfigSource::Default);
        config->set("development.enable", false, ConfigSource::Default);
    } else {
        throw std::invalid_argument("Unknown preset name: " + presetName);
    }
    
    return config;
}

// Static method to list available preset names
std::vector<std::string> Config::getAvailablePresets() {
    return {"fast", "accurate", "balanced", "exploratory", "conservative"};
}

// Static method to create config with default values
std::shared_ptr<Config> Config::createDefaultConfig() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000, ConfigSource::Default);
    config->set("region_count", 2, ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("random_seed", 42u, ConfigSource::Default);
    return config;
}

// Static method to create simple config for beginners
std::shared_ptr<Config> Config::createSimpleConfig() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100, ConfigSource::Default);
    config->set("region_count", 1, ConfigSource::Default);
    config->set("connection_probability", 0.05f, ConfigSource::Default);
    return config;
}

} // namespace nlm

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Create JSON object with all configuration entries
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        try {
            // Convert the value based on its type
            std::string valueStr;
            if (std::holds_alternative<std::string>(entry.value)) {
                valueStr = std::get<std::string>(entry.value);
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                valueStr = std::to_string(std::get<int64_t>(entry.value));
            } else if (std::holds_alternative<double>(entry.value)) {
                valueStr = std::to_string(std::get<double>(entry.value));
            } else if (std::holds_alternative<bool>(entry.value)) {
                valueStr = std::get<bool>(entry.value) ? "true" : "false";
            }
            
            jsonData[entry.key] = valueStr;
            
        } catch (const std::exception& e) {
            // Log error but continue with other entries
            continue;
        }
    }
    
    // Write JSON with pretty formatting
    file << jsonData.dump(2);
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    if (key.empty()) {
        // NLM_LOG_ERROR("Cannot get configuration with empty key");
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        // NLM_LOG_WARNING("Configuration key not found: " + key);
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        // NLM_LOG_ERROR("Type mismatch for configuration key: " + key);
        return std::nullopt;
    }
}

void Config::validate() const {
    // Perform basic validation of configuration
    for (const auto& entry : pImpl->entries) {
        try {
            // Try to access the value to validate it's properly stored
            std::visit([](auto&&) {}, entry.value);
        } catch (const std::bad_variant_access&) {
            // NLM_LOG_ERROR("Invalid configuration entry for key: " + entry.key);
        }
    }
    
    // Log configuration summary for debugging
    // NLM_LOG_INFO(summary());
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (key.empty()) {
        // NLM_LOG_ERROR("Cannot set empty key in configuration");
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        // NLM_LOG_DEBUG("Updated configuration key: " + key);
    } else {
        pImpl->entries.emplace_back(key, value, source);
        // NLM_LOG_DEBUG("Added new configuration key: " + key);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}
// Private helper to parse JSON data into config entries
void Config::parseJson(const nlohmann::json& jsonData, ConfigSource source) {
    if (!jsonData.is_object()) {
        return; // Not an object, nothing to parse
    }
    
    for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
        std::string key = it.key();
        
        // Recursively parse nested JSON objects
        if (it.value().is_object()) {
            // Create a nested key representation
            std::string nestedKey = key;
            std::string nestedValue = serializeJsonValue(it.value());
            set(nestedKey, nestedValue, source);
        } else {
            // Direct key-value pair
            std::string value = serializeJsonValue(it.value());
            set(key, value, source);
        }
    }
}

// Helper to serialize JSON values to strings
std::string Config::serializeJsonValue(const nlohmann::json& value) {
    if (value.is_string()) {
        return value.get<std::string>();
    } else if (value.is_number_integer()) {
        return std::to_string(value.get<int64_t>());
    } else if (value.is_number_float()) {
        // Check if it's an integer value stored as float
        double floatVal = value.get<double>();
        if (floatVal == static_cast<int64_t>(floatVal)) {
            return std::to_string(static_cast<int64_t>(floatVal));
        }
        return std::to_string(floatVal);
    } else if (value.is_boolean()) {
        return value.get<bool>() ? "true" : "false";
    } else if (value.is_array()) {
        // For arrays, create a comma-separated string
        std::string result = "[";
        for (size_t i = 0; i < value.size(); ++i) {
            if (i > 0) result += ",";
            result += serializeJsonValue(value[i]);
        }
        result += "]";
        return result;
    } else if (value.is_null()) {
        return "null";
    } else {
        // Fallback to string representation
        return value.dump();
    }
}

// Private helper to parse YAML data into config entries
#ifdef HAVE_YAML_CPP
void Config::parseYaml(const YAML::Node& yamlData, ConfigSource source) {
    if (!yamlData.IsMap()) {
        return; // Not a map, nothing to parse
    }
    
    for (const auto& pair : yamlData) {
        std::string key = pair.first.as<std::string>();
        
        // Recursively parse nested YAML nodes
        if (pair.second.IsMap()) {
            // Create a nested key representation
            std::string nestedKey = key;
            std::string nestedValue = serializeYamlValue(pair.second);
            set(nestedKey, nestedValue, source);
        } else {
            // Direct key-value pair
            std::string value = serializeYamlValue(pair.second);
            set(key, value, source);
        }
    }
}

// Helper to serialize YAML values to strings
std::string Config::serializeYamlValue(const YAML::Node& value) {
    if (value.IsScalar()) {
        std::string str = value.as<std::string>();
        
        // Determine if it's a boolean or number
        try {
            if (str == "true" || str == "false") {
                return str;
            }
            size_t pos;
            try {
                int64_t intVal = std::stoll(str);
                return std::to_string(intVal);
            } catch (...) {
                try {
                    double floatVal = std::stod(str);
                    // Check if it's an integer value stored as float
                    if (floatVal == static_cast<int64_t>(floatVal)) {
                        return std::to_string(static_cast<int64_t>(floatVal));
                    }
                    return std::to_string(floatVal);
                } catch (...) {
                    // Keep as string
                    return str;
                }
            }
        } catch (...) {
            // Return as-is
            return str;
        }
    } else if (value.IsSequence()) {
        // For sequences, create a comma-separated string
        std::string result = "[";
        for (size_t i = 0; i < value.size(); ++i) {
            if (i > 0) result += ",";
            result += serializeYamlValue(value[i]);
        }
        result += "]";
        return result;
    } else if (value.IsNull()) {
        return "null";
    } else {
        // Fallback to string representation
        return value.as<std::string>();
    }
}
#endif
