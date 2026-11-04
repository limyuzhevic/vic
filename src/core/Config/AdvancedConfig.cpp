// Advanced Configuration Implementation
// Implements the AdvancedConfig class with full functionality

#include "AdvancedConfig.hpp"
#include <fstream>
#include <sstream>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <cstring>

namespace nlm {

// JSON parsing and serialization (simplified implementation)
namespace json {
    std::string toJson(const std::string& str) {
        // Escape JSON string
        std::string result;
        result += "\"";
        for (char c : str) {
            switch (c) {
                case '\"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default:
                    if (c < ' ') {
                        result += "\\u" + std::to_string(static_cast<int>(c));
                    } else {
                        result += c;
                    }
            }
        }
        result += "\"";
        return result;
    }
    
    std::string toJson(double value) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(6) << value;
        return ss.str();
    }
    
    std::string toJson(int value) {
        return std::to_string(value);
    }
    
    std::string toJson(int64_t value) {
        return std::to_string(value);
    }
    
    std::string toJson(bool value) {
        return value ? "true" : "false";
    }
}

// Initialize the AdvancedConfig implementation
AdvancedConfig::AdvancedConfig() {
    setupDefaultSchema();
}

void AdvancedConfig::setupDefaultSchema() {
    // Common NLM configuration defaults
    
    // Core simulation parameters
    addValidationRule("simulation_timestep", 
        [](const ConfigValue& v) { return std::get<double>(v) > 0.0 && std::get<double>(v) < 1.0; },
        "Timestep must be between 0 and 1 second");
    
    addValidationRule("neuron_count", 
        [](const ConfigValue& v) { return std::get<size_t>(v) > 0 && std::get<size_t>(v) <= 100000; },
        "Neuron count must be positive and <= 100000");
    
    addValidationRule("region_count", 
        [](const ConfigValue& v) { return std::get<size_t>(v) > 0 && std::get<size_t>(v) <= 1000; },
        "Region count must be positive and <= 1000");
    
    addValidationRule("connection_probability", 
        [](const ConfigValue& v) { 
            float p = std::get<float>(v); 
            return p >= 0.0f && p <= 1.0f; },
        "Connection probability must be between 0 and 1");
    
    // Random seed validation
    addValidationRule("random_seed", 
        [](const ConfigValue& v) { return std::get<int64_t>(v) >= 0; },
        "Random seed must be non-negative");
    
    // Plasticity parameters
    addValidationRule("stdp_ltp_weight", 
        [](const ConfigValue& v) { return std::get<float>(v) >= 0.0f && std::get<float>(v) <= 10.0f; },
        "STDP LTP weight must be between 0 and 10");
    
    addValidationRule("stdp_ltd_weight", 
        [](const ConfigValue& v) { return std::get<float>(v) >= 0.0f && std::get<float>(v) <= 10.0f; },
        "STDP LTD weight must be between 0 and 10");
    
    addValidationRule("stdp_tau", 
        [](const ConfigValue& v) { return std::get<float>(v) > 0.0f; },
        "STDP tau must be positive");
    
    // Structural plasticity
    addValidationRule("synaptogenesis_rate", 
        [](const ConfigValue& v) { return std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f; },
        "Synaptogenesis rate must be between 0 and 1");
    
    addValidationRule("pruning_rate", 
        [](const ConfigValue& v) { return std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f; },
        "Pruning rate must be between 0 and 1");
    
    // Development parameters
    addValidationRule("critical_period_start", 
        [](const ConfigValue& v) { return std::get<float>(v) >= 0.0f; },
        "Critical period start must be non-negative");
    
    addValidationRule("critical_period_end", 
        [](const ConfigValue& v) { return std::get<float>(v) >= 0.0f; },
        "Critical period end must be non-negative");
    
    // Environment parameters
    addValidationRule("environment_width", 
        [](const ConfigValue& v) { return std::get<int>(v) > 0; },
        "Environment width must be positive");
    
    addValidationRule("environment_height", 
        [](const ConfigValue& v) { return std::get<int>(v) > 0; },
        "Environment height must be positive");
    
    // Vision parameters
    addValidationRule("vision_width", 
        [](const ConfigValue& v) { return std::get<int>(v) > 0; },
        "Vision width must be positive");
    
    addValidationRule("vision_height", 
        [](const ConfigValue& v) { return std::get<int>(v) > 0; },
        "Vision height must be positive");
    
    // Memory parameters
    addValidationRule("working_memory_capacity", 
        [](const ConfigValue& v) { return std::get<size_t>(v) > 0; },
        "Working memory capacity must be positive");
    
    addValidationRule("episodic_memory_capacity", 
        [](const ConfigValue& v) { return std::get<size_t>(v) > 0; },
        "Episodic memory capacity must be positive");
    
    // Neuromodulation parameters
    addValidationRule("dopamine_baseline", 
        [](const ConfigValue& v) { return std::get<float>(v) >= 0.0f; },
        "Dopamine baseline must be non-negative");
    
    addValidationRule("curiosity_baseline", 
        [](const ConfigValue& v) { return std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f; },
        "Curiosity baseline must be between 0 and 1");
    
    addValidationRule("novelty_threshold", 
        [](const ConfigValue& v) { return std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f; },
        "Novelty threshold must be between 0 and 1");
    
    // Development parameters
    addValidationRule("development_rate", 
        [](const ConfigValue& v) { return std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f; },
        "Development rate must be between 0 and 1");
    
    addValidationRule("maturation_rate", 
        [](const ConfigValue& v) { return std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f; },
        "Maturation rate must be between 0 and 1");
}

bool AdvancedConfig::loadFromFile(const std::string& filepath) {
    // Simple file existence check for now
    // In a full implementation, this would parse JSON, YAML, or TOML
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Configuration file not found: " + filepath);
        return false;
    }
    
    // For now, just read the file and show content
    std::string line;
    NLM_LOG_INFO("Reading configuration from file: " + filepath);
    int lineCount = 0;
    
    while (std::getline(file, line)) {
        if (++lineCount <= 10) { // Log first 10 lines
            NLM_LOG_INFO("  " + line);
        }
    }
    
    file.close();
    return true;
}

bool AdvancedConfig::loadFromJson(const std::string& jsonStr) {
    // Simplified JSON parsing
    // In a full implementation, use a proper JSON library
    
    std::istringstream stream(jsonStr);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Trim whitespace
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line.find("#") == 0) {
            continue;
        }
        
        // Parse key-value pairs (simplified)
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = trim(line.substr(0, colonPos));
            std::string valueStr = trim(line.substr(colonPos + 1));
            
            // Try to parse value
            ConfigValue value;
            if (parseJsonValue(valueStr, value)) {
                set(key, value, ConfigSource::File);
                NLM_LOG_INFO("  Loaded config: " + key + " = " + valueStr);
            }
        }
    }
    
    return true;
}

bool AdvancedConfig::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--config" && i + 1 < argc) {
            std::string configFile = argv[++i];
            loadFromFile(configFile);
        } else if (arg == "--seed" && i + 1 < argc) {
            int seed = std::atoi(argv[++i]);
            set("random_seed", static_cast<int64_t>(seed), ConfigSource::CommandLine);
        } else if (arg == "--neuron-count" && i + 1 < argc) {
            int neuronCount = std::atoi(argv[++i]);
            set("neuron_count", static_cast<size_t>(neuronCount), ConfigSource::CommandLine);
        } else if (arg == "--region-count" && i + 1 < argc) {
            int regionCount = std::atoi(argv[++i]);
            set("region_count", static_cast<size_t>(regionCount), ConfigSource::CommandLine);
        } else if (arg == "--connection-prob" && i + 1 < argc) {
            float prob = std::atof(argv[++i]);
            set("connection_probability", prob, ConfigSource::CommandLine);
        } else if (arg == "--simulation-timestep" && i + 1 < argc) {
            double timestep = std::atof(argv[++i]);
            set("simulation_timestep", timestep, ConfigSource::CommandLine);
        }
    }
    
    return true;
}

bool AdvancedConfig::loadFromEnvironment(const std::string& prefix) {
    // Load from environment variables
    // In a full implementation, this would read actual environment variables
    
    NLM_LOG_INFO("Loading configuration from environment variables (prefix: " + prefix + ")");
    
    // Example environment variables
    const char* seedEnv = std::getenv("NLM_RANDOM_SEED");
    if (seedEnv) {
        int seed = std::atoi(seedEnv);
        set("random_seed", static_cast<int64_t>(seed), ConfigSource::Environment);
        NLM_LOG_INFO("  Loaded from NLM_RANDOM_SEED: " + std::to_string(seed));
    }
    
    const char* neuronCountEnv = std::getenv("NLM_NEURON_COUNT");
    if (neuronCountEnv) {
        int neuronCount = std::atoi(neuronCountEnv);
        set("neuron_count", static_cast<size_t>(neuronCount), ConfigSource::Environment);
        NLM_LOG_INFO("  Loaded from NLM_NEURON_COUNT: " + std::to_string(neuronCount));
    }
    
    return true;
}

bool AdvancedConfig::saveToFile(const std::string& filepath, const std::string& format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot write to file: " + filepath);
        return false;
    }
    
    if (format == "json" || format == "auto") {
        file << "{" << std::endl;
        for (const auto& pair : configMap) {
            const ConfigEntry& entry = pair.second;
            file << "  \"" << pair.first << "\": " << toJson(entry.value) << "," << std::endl;
        }
        file << "}" << std::endl;
    } else {
        // Plain text format
        for (const auto& pair : configMap) {
            const ConfigEntry& entry = pair.second;
            file << pair.first << " = ";
            
            std::visit([&](const auto& val) {
                file << val;
            }, entry.value);
            
            file << " # " << entry.description << " (" 
                 << (entry.source == ConfigSource::Default ? "default" :
                     entry.source == ConfigSource::File ? "file" :
                     entry.source == ConfigSource::CommandLine ? "command_line" :
                     entry.source == ConfigSource::Runtime ? "runtime" : "unknown")
                 << ")" << std::endl;
        }
    }
    
    file.close();
    NLM_LOG_INFO("Configuration saved to: " + filepath);
    return true;
}

template<typename T>
std::optional<T> AdvancedConfig::get(const std::string& key) const {
    auto it = configMap.find(key);
    if (it == configMap.end()) {
        return std::nullopt;
    }
    
    const ConfigEntry& entry = it->second;
    if (entry.isType<T>()) {
        return std::get<T>(entry.value);
    }
    return std::nullopt;
}

template<typename T>
T AdvancedConfig::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    if (value.has_value()) {
        return *value;
    }
    return defaultValue;
}

void AdvancedConfig::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Check if key exists in schema
    if (validationRules.find(key) != validationRules.end()) {
        // Validate against schema
        if (!validationRules[key](value)) {
            NLM_LOG_ERROR("Configuration validation failed for key '" + key + "': " + 
                         "value '" + std::to_string(value) + "' is invalid");
            return;
        }
    }
    
    // Create or update entry
    configMap[key] = ConfigEntry(key, value, source);
    
    NLM_LOG_INFO("Configuration set: " + key + " = " + std::to_string(value) + 
                " (source: " + (source == ConfigSource::Default ? "default" :
                               source == ConfigSource::File ? "file" :
                               source == ConfigSource::CommandLine ? "command_line" :
                               source == ConfigSource::Runtime ? "runtime" :
                               source == ConfigSource::Environment ? "environment" : "imported") + ")");
}

void AdvancedConfig::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, static_cast<ConfigValue>(value), source);
}

void AdvancedConfig::set(const std::string& key, int value, ConfigSource source) {
    set(key, static_cast<ConfigValue>(value), source);
}

void AdvancedConfig::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, static_cast<ConfigValue>(value), source);
}

void AdvancedConfig::set(const std::string& key, double value, ConfigSource source) {
    set(key, static_cast<ConfigValue>(value), source);
}

void AdvancedConfig::set(const std::string& key, bool value, ConfigSource source) {
    set(key, static_cast<ConfigValue>(value), source);
}

void AdvancedConfig::addValidationRule(const std::string& key,
                                      const std::function<bool(const ConfigValue&)>& validator,
                                      const std::string& errorMessage) {
    validationRules[key] = validator;
    if (!errorMessage.empty()) {
        NLM_LOG_INFO("Added validation rule for key '" + key + "': " + errorMessage);
    }
}

bool AdvancedConfig::has(const std::string& key) const {
    return configMap.find(key) != configMap.end();
}

void AdvancedConfig::remove(const std::string& key) {
    auto it = configMap.find(key);
    if (it != configMap.end()) {
        NLM_LOG_INFO("Removed configuration key: " + key);
        configMap.erase(it);
    }
}

std::vector<std::string> AdvancedConfig::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(configMap.size());
    for (const auto& pair : configMap) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::optional<ConfigEntry> AdvancedConfig::getEntry(const std::string& key) const {
    auto it = configMap.find(key);
    if (it == configMap.end()) {
        return std::nullopt;
    }
    return it->second;
}

void AdvancedConfig::clear() {
    NLM_LOG_INFO("Cleared all configuration");
    configMap.clear();
}

std::string AdvancedConfig::summary() const {
    std::stringstream ss;
    ss << "Configuration Summary:" << std::endl;
    ss << "  Total keys: " << configMap.size() << std::endl;
    
    std::map<ConfigSource, size_t> sourceCounts;
    for (const auto& pair : configMap) {
        sourceCounts[pair.second.source]++;
    }
    
    for (const auto& pair : sourceCounts) {
        ss << "  Keys from " << static_cast<int>(pair.first) << ": " << pair.second << std::endl;
    }
    
    return ss.str();
}

std::vector<std::string> AdvancedConfig::validate() const {
    std::vector<std::string> errors;
    
    for (const auto& pair : configMap) {
        const ConfigEntry& entry = pair.second;
        if (!entry.isValid()) {
            errors.push_back("Invalid configuration for key '" + pair.first + "': " + 
                           std::to_string(entry.value));
        }
    }
    
    return errors;
}

std::string AdvancedConfig::getStats() const {
    ConfigStats stats = calculateStats();
    
    std::stringstream ss;
    ss << "Configuration Statistics:" << std::endl;
    ss << "  Total keys: " << stats.totalKeys << std::endl;
    ss << "  Valid keys: " << stats.validKeys << std::endl;
    ss << "  Invalid keys: " << stats.invalidKeys << std::endl;
    ss << std::endl;
    
    ss << "  Keys by source:" << std::endl;
    for (const auto& pair : stats.keysBySource) {
        ss << "    " << static_cast<int>(pair.first) << ": " << pair.second << std::endl;
    }
    
    ss << std::endl << "  Value types by count:" << std::endl;
    for (const auto& pair : stats.typesByCount) {
        ss << "    " << pair.first << ": " << pair.second << std::endl;
    }
    
    return ss.str();
}

bool AdvancedConfig::importFrom(const AdvancedConfig& other, ConfigSource source) {
    bool changed = false;
    
    for (const auto& pair : other.configMap) {
        if (configMap.find(pair.first) == configMap.end()) {
            configMap[pair.first] = pair.second;
            configMap[pair.first].source = source;
            changed = true;
            NLM_LOG_INFO("Imported configuration key: " + pair.first);
        }
    }
    
    return changed;
}

void AdvancedConfig::mergeFrom(const AdvancedConfig& other) {
    for (const auto& pair : other.configMap) {
        configMap[pair.first] = pair.second;
        NLM_LOG_INFO("Merged configuration key: " + pair.first);
    }
}

std::vector<std::string> AdvancedConfig::getKeysBySource(ConfigSource source) const {
    std::vector<std::string> keys;
    
    for (const auto& pair : configMap) {
        if (pair.second.source == source) {
            keys.push_back(pair.first);
        }
    }
    
    return keys;
}

void AdvancedConfig::resetToDefaults() {
    NLM_LOG_INFO("Resetting configuration to defaults");
    clear();
    setupDefaultSchema();
}

bool AdvancedConfig::isValid() const {
    for (const auto& pair : configMap) {
        if (!pair.second.isValid()) {
            return false;
        }
    }
    return true;
}

std::string AdvancedConfig::getValueType(const std::string& key) const {
    auto it = configMap.find(key);
    if (it == configMap.end()) {
        return "NOT_FOUND";
    }
    
    const ConfigValue& value = it->second.value;
    
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "vector<int>";
    if (std::holds_alternative<std::vector<double>>(value)) return "vector<double>";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "vector<string>";
    
    return "UNKNOWN";
}

bool AdvancedConfig::isRequired(const std::string& key) const {
    auto descIt = descriptions.find(key);
    if (descIt == descriptions.end()) {
        return false;
    }
    
    return requiredFlags.find(key) != requiredFlags.end() && requiredFlags.at(key);
}

void AdvancedConfig::setRequired(const std::string& key, bool required) {
    requiredFlags[key] = required;
    NLM_LOG_INFO("Set key '" + key + "' as " + (required ? "required" : "optional"));
}

std::map<std::string, std::string> AdvancedConfig::getSchema() const {
    std::map<std::string, std::string> schema;
    
    for (const auto& pair : descriptions) {
        schema[pair.first] = pair.second;
    }
    
    return schema;
}

void AdvancedConfig::setDescription(const std::string& key, const std::string& description) {
    descriptions[key] = description;
}

std::string AdvancedConfig::getDescription(const std::string& key) const {
    auto it = descriptions.find(key);
    if (it == descriptions.end()) {
        return "";
    }
    return it->second;
}

std::string AdvancedConfig::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::string AdvancedConfig::toLower(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

bool AdvancedConfig::parseCommandLineArg(const std::string& arg, std::string& key, ConfigValue& value) {
    // Simple parsing for command line arguments
    // In a full implementation, this would handle more complex formats
    
    size_t equalsPos = arg.find('=');
    if (equalsPos != std::string::npos) {
        key = trim(arg.substr(0, equalsPos));
        std::string valueStr = trim(arg.substr(equalsPos + 1));
        
        // Try to parse as different types
        try {
            // Try as integer
            size_t pos;
            int intVal = std::stoi(valueStr, &pos);
            if (pos == valueStr.size()) {
                value = intVal;
                return true;
            }
        } catch (...) {}
        
        try {
            // Try as double
            size_t pos;
            double doubleVal = std::stod(valueStr, &pos);
            if (pos == valueStr.size()) {
                value = doubleVal;
                return true;
            }
        } catch (...) {}
        
        try {
            // Try as boolean
            if (valueStr == "true") {
                value = true;
                return true;
            } else if (valueStr == "false") {
                value = false;
                return true;
            }
        } catch (...) {}
        
        // Default to string
        value = valueStr;
        return true;
    }
    
    return false;
}

std::string AdvancedConfig::toJson(const ConfigEntry& entry) const {
    std::stringstream ss;
    
    std::visit([&](const auto& val) {
        if constexpr (std::is_same_v<decltype(val), int>) {
            ss << val;
        } else if constexpr (std::is_same_v<decltype(val), int64_t>) {
            ss << val;
        } else if constexpr (std::is_same_v<decltype(val), double>) {
            ss << std::fixed << std::setprecision(6) << val;
        } else if constexpr (std::is_same_v<decltype(val), bool>) {
            ss << (val ? "true" : "false");
        } else if constexpr (std::is_same_v<decltype(val), std::string>) {
            ss << "\"" << val << "\"";
        } else if constexpr (std::is_same_v<decltype(val), std::vector<int>>) {
            ss << "["; 
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << val[i];
            }
            ss << "]";
        } else if constexpr (std::is_same_v<decltype(val), std::vector<double>>) {
            ss << "["; 
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << std::fixed << std::setprecision(6) << val[i];
            }
            ss << "]";
        } else if constexpr (std::is_same_v<decltype(val), std::vector<std::string>>) {
            ss << "["; 
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << "\"" << val[i] << "\"";
            }
            ss << "]";
        }
    }, entry.value);
    
    return ss.str();
}

bool AdvancedConfig::parseJsonValue(const std::string& jsonStr, ConfigValue& value) {
    // Simple JSON value parsing
    // In a full implementation, use a proper JSON parser
    
    if (jsonStr.empty()) return false;
    
    if (jsonStr == "true") {
        value = true;
        return true;
    } else if (jsonStr == "false") {
        value = false;
        return true;
    } else if (jsonStr[0] == '\"' && jsonStr[jsonStr.size() - 1] == '\"') {
        // String
        value = jsonStr.substr(1, jsonStr.size() - 2);
        return true;
    } else if (jsonStr.find('.') != std::string::npos || jsonStr.find('e') != std::string::npos || jsonStr.find('E') != std::string::npos) {
        // Double
        try {
            double doubleVal = std::stod(jsonStr);
            value = doubleVal;
            return true;
        } catch (...) {}
    } else {
        // Integer
        try {
            int64_t intVal = std::stoll(jsonStr);
            value = intVal;
            return true;
        } catch (...) {}
    }
    
    return false;
}

bool AdvancedConfig::validateEntry(const ConfigEntry& entry) const {
    auto validatorIt = validationRules.find(entry.key);
    if (validatorIt == validationRules.end()) {
        return true; // No validation rule, assume valid
    }
    
    return validatorIt->second(entry.value);
}

AdvancedConfig::ConfigStats AdvancedConfig::calculateStats() const {
    ConfigStats stats;
    stats.totalKeys = configMap.size();
    stats.validKeys = 0;
    stats.invalidKeys = 0;
    
    for (const auto& pair : configMap) {
        if (pair.second.isValid()) {
            stats.validKeys++;
        } else {
            stats.invalidKeys++;
        }
        
        stats.keysBySource[pair.second.source]++;
        
        // Count value types
        if (std::holds_alternative<int>(pair.second.value)) {
            stats.typesByCount["int"]++;
        } else if (std::holds_alternative<int64_t>(pair.second.value)) {
            stats.typesByCount["int64_t"]++;
        } else if (std::holds_alternative<double>(pair.second.value)) {
            stats.typesByCount["double"]++;
        } else if (std::holds_alternative<bool>(pair.second.value)) {
            stats.typesByCount["bool"]++;
        } else if (std::holds_alternative<std::string>(pair.second.value)) {
            stats.typesByCount["string"]++;
        } else if (std::holds_alternative<std::vector<int>>(pair.second.value)) {
            stats.typesByCount["vector<int>"]++;
        } else if (std::holds_alternative<std::vector<double>>(pair.second.value)) {
            stats.typesByCount["vector<double>"]++;
        } else if (std::holds_alternative<std::vector<std::string>>(pair.second.value)) {
            stats.typesByCount["vector<string>"]++;
        }
    }
    
    return stats;
}

// ConfigHelpers implementation
namespace ConfigHelpers {
    double getSimulationTimestep(const AdvancedConfig& config) {
        return config.getOr<double>("simulation_timestep", 0.001);
    }
    
    size_t getNeuronCount(const AdvancedConfig& config) {
        return config.getOr<size_t>("neuron_count", 500);
    }
    
    size_t getRegionCount(const AdvancedConfig& config) {
        return config.getOr<size_t>("region_count", 2);
    }
    
    float getConnectionProbability(const AdvancedConfig& config) {
        return config.getOr<float>("connection_probability", 0.1f);
    }
    
    int64_t getRandomSeed(const AdvancedConfig& config) {
        return config.getOr<int64_t>("random_seed", 42);
    }
    
    void setSimulationTimestep(AdvancedConfig& config, double timestep) {
        config.set("simulation_timestep", timestep);
    }
    
    void setNeuronCount(AdvancedConfig& config, size_t count) {
        config.set("neuron_count", static_cast<size_t>(count));
    }
    
    void setRegionCount(AdvancedConfig& config, size_t count) {
        config.set("region_count", static_cast<size_t>(count));
    }
    
    void setConnectionProbability(AdvancedConfig& config, float probability) {
        config.set("connection_probability", probability);
    }
    
    void setRandomSeed(AdvancedConfig& config, int64_t seed) {
        config.set("random_seed", seed);
    }
    
    std::vector<std::string> validateNLMConfig(const AdvancedConfig& config) {
        std::vector<std::string> errors;
        
        auto validationErrors = config.validate();
        errors.insert(errors.end(), validationErrors.begin(), validationErrors.end());
        
        // Additional NLM-specific validation
        auto neuronCount = config.get<size_t>("neuron_count");
        if (neuronCount && *neuronCount > 100000) {
            errors.push_back("Neuron count cannot exceed 100000");
        }
        
        auto regionCount = config.get<size_t>("region_count");
        if (regionCount && *regionCount > 1000) {
            errors.push_back("Region count cannot exceed 1000");
        }
        
        auto connectionProb = config.get<float>("connection_probability");
        if (connectionProb && (*connectionProb < 0.0f || *connectionProb > 1.0f)) {
            errors.push_back("Connection probability must be between 0 and 1");
        }
        
        auto timestep = config.get<double>("simulation_timestep");
        if (timestep && (*timestep <= 0.0 || *timestep >= 1.0)) {
            errors.push_back("Simulation timestep must be between 0 and 1 second");
        }
        
        return errors;
    }
    
    AdvancedConfig createDefaultNLMConfig() {
        AdvancedConfig config;
        
        // Core simulation parameters
        config.set("simulation_timestep", 0.001);
        config.set("neuron_count", 500);
        config.set("region_count", 2);
        config.set("connection_probability", 0.1f);
        config.set("random_seed", static_cast<int64_t>(42));
        
        // Plasticity parameters
        config.set("stdp_ltp_weight", 0.01f);
        config.set("stdp_ltd_weight", 0.012f);
        config.set("stdp_tau", 20.0f);
        config.set("synaptogenesis_rate", 0.0001f);
        config.set("pruning_rate", 0.00001f);
        
        // Development parameters
        config.set("development_rate", 0.01f);
        config.set("maturation_rate", 0.001f);
        config.set("critical_period_start", 0.0f);
        config.set("critical_period_end", 100.0f);
        
        // Environment parameters
        config.set("environment_width", 20);
        config.set("environment_height", 20);
        
        // Vision parameters
        config.set("vision_width", 16);
        config.set("vision_height", 16);
        
        // Memory parameters
        config.set("working_memory_capacity", 100);
        config.set("episodic_memory_capacity", 50);
        
        // Neuromodulation parameters
        config.set("dopamine_baseline", 0.5f);
        config.set("curiosity_baseline", 0.1f);
        config.set("novelty_threshold", 0.5f);
        
        return config;
    }
}

} // namespace nlm
