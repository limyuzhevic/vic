#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, size_t> keyToIndex;
    
    // Configuration schema for validation
    struct ConfigSchema {
        std::string key;
        std::string type;
        bool required = false;
        std::string defaultValue;
        std::string description;
        
        // Validation rules
        std::function<bool(const ConfigValue&)> validator;
        
        ConfigSchema(const std::string& k, const std::string& t, bool req = false, 
                    const std::string& def = "", const std::string& desc = "")
            : key(k), type(t), required(req), defaultValue(def), description(desc) {}
    };
    
    static std::vector<ConfigSchema> createDefaultSchema() {
        return {
            // Core simulation parameters
            {"neuron_count", "int64_t", true, "1000", "Number of neurons in the brain", 
             [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; }},
            {"region_count", "int64_t", true, "1", "Number of neural regions", 
             [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; }},
            {"simulation_timestep", "double", true, "0.001", "Simulation timestep in seconds", 
             [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) > 0.0 && std::get<double>(v) <= 0.1; }},
            
            // Connectivity parameters
            {"connection_probability", "float", true, "0.1", "Probability of synaptic connections", 
             [](const ConfigValue& v) { return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f; }},
            {"mean_synaptic_weight", "float", false, "0.2", "Mean synaptic weight magnitude", 
             [](const ConfigValue& v) { return std::holds_alternative<float>(v); }},
            {"weight_variance", "float", false, "0.1", "Standard deviation of synaptic weights", 
             [](const ConfigValue& v) { return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f; }},
            
            // STDP parameters
            {"stdp_ltp_weight", "float", false, "0.01", "STDP LTP learning rate", 
             [](const ConfigValue& v) { return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f; }},
            {"stdp_ltd_weight", "float", false, "0.012", "STDP LTD learning rate", 
             [](const ConfigValue& v) { return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f; }},
            {"stdp_tau", "float", false, "20.0", "STDP time constant (ms)", 
             [](const ConfigValue& v) { return std::holds_alternative<float>(v) && std::get<float>(v) > 0.0f; }},
            
            // Structural plasticity
            {"synaptogenesis_rate", "float", false, "0.0001", "Rate of new synapse formation", 
             [](const ConfigValue& v) { return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f; }},
            {"pruning_rate", "float", false, "0.00001", "Rate of synapse pruning", 
             [](const ConfigValue& v) { return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f; }},
            
            // Neuromodulation
            {"dopamine_level", "float", false, "0.5", "Baseline dopamine level", 
             [](const ConfigValue& v) { return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f; }},
            {"curiosity_level", "float", false, "0.5", "Baseline curiosity level", 
             [](const ConfigValue& v) { return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f; }},
            
            // Memory systems
            {"working_memory_capacity", "int64_t", false, "100", "Working memory capacity (number of traces)", 
             [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; }},
            {"episodic_memory_max_episodes", "int64_t", false, "1000", "Maximum episodic memory episodes", 
             [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; }},
            {"replay_interval", "size_t", false, "100", "Steps between memory replay events", 
             [](const ConfigValue& v) { return std::holds_alternative<size_t>(v) && std::get<size_t>(v) > 0; }},
            {"consolidation_interval", "size_t", false, "1000", "Steps between consolidation events", 
             [](const ConfigValue& v) { return std::holds_alternative<size_t>(v) && std::get<size_t>(v) > 0; }},
            
            // Development parameters
            {"development_age_factor", "float", false, "1.0", "Factor affecting development rate", 
             [](const ConfigValue& v) { return std::holds_alternative<float>(v) && std::get<float>(v) > 0.0f; }},
            
            // Performance and debugging
            {"checkpoint_dir", "string", false, "checkpoints", "Directory for checkpoint files", 
             [](const ConfigValue& v) { return std::holds_alternative<std::string>(v); }},
            {"checkpoint_interval", "size_t", false, "1000", "Steps between checkpoint saves", 
             [](const ConfigValue& v) { return std::holds_alternative<size_t>(v) && std::get<size_t>(v) > 0; }},
            {"max_checkpoints", "size_t", false, "10", "Maximum number of checkpoints to keep", 
             [](const ConfigValue& v) { return std::holds_alternative<size_t>(v) && std::get<size_t>(v) > 0; }},
            
            // Logging
            {"log_level", "string", false, "info", "Logging level (debug, info, warning, error, critical)", 
             [](const ConfigValue& v) { 
                 if (!std::holds_alternative<std::string>(v)) return false;
                 std::string level = std::get<std::string>(v);
                 std::transform(level.begin(), level.end(), level.begin(), ::tolower);
                 return level == "debug" || level == "info" || level == "warning" || level == "error" || level == "critical";
             }},
            
            // Advanced features
            {"enable_replay", "bool", false, "true", "Enable episodic memory replay", 
             [](const ConfigValue& v) { return std::holds_alternative<bool>(v); }},
            {"enable_structural_plasticity", "bool", false, "true", "Enable structural plasticity", 
             [](const ConfigValue& v) { return std::holds_alternative<bool>(v); }},
            {"enable_neuromodulation", "bool", false, "true", "Enable neuromodulation systems", 
             [](const ConfigValue& v) { return std::holds_alternative<bool>(v); }},
            
            // Experiment-specific
            {"experiment_mode", "string", false, "", "Experiment mode (blank for standard)", 
             [](const ConfigValue& v) { return std::holds_alternative<std::string>(v); }},
        };
    };
    
    static std::vector<ConfigSchema> schema;
    
    Impl() {
        initializeDefaultConfig();
    }
    
    void initializeDefaultConfig() {
        // Add all default schema entries
        schema = createDefaultSchema();
        
        // Set default values from schema
        for (const auto& schemaEntry : schema) {
            setDefaultFromSchema(schemaEntry);
        }
    }
    
    void setDefaultFromSchema(const ConfigSchema& schemaEntry) {
        ConfigValue defaultValue;
        
        if (schemaEntry.type == "int") {
            try {
                defaultValue = std::stoi(schemaEntry.defaultValue);
            } catch (...) {
                defaultValue = 0;
            }
        } else if (schemaEntry.type == "int64_t") {
            try {
                defaultValue = static_cast<int64_t>(std::stoll(schemaEntry.defaultValue));
            } catch (...) {
                defaultValue = 0;
            }
        } else if (schemaEntry.type == "double") {
            try {
                defaultValue = std::stod(schemaEntry.defaultValue);
            } catch (...) {
                defaultValue = 0.0;
            }
        } else if (schemaEntry.type == "float") {
            try {
                defaultValue = static_cast<float>(std::stod(schemaEntry.defaultValue));
            } catch (...) {
                defaultValue = 0.0f;
            }
        } else if (schemaEntry.type == "bool") {
            std::string lower = schemaEntry.defaultValue;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            defaultValue = (lower == "true" || lower == "1" || lower == "yes");
        } else if (schemaEntry.type == "string") {
            defaultValue = schemaEntry.defaultValue;
        } else if (schemaEntry.type == "size_t") {
            try {
                defaultValue = static_cast<size_t>(std::stoull(schemaEntry.defaultValue));
            } catch (...) {
                defaultValue = 0;
            }
        }
        
        // Add default entry
        ConfigEntry entry(schemaEntry.key, defaultValue, ConfigSource::Default, schemaEntry.description);
        entries.push_back(entry);
        keyToIndex[entry.key] = entries.size() - 1;
    }
    
    static void initializeSchema() {
        if (schema.empty()) {
            schema = createDefaultSchema();
        }
    }
    
    bool validateConfig(const ConfigEntry& entry) const {
        initializeSchema();
        
        auto it = keyToIndex.find(entry.key);
        if (it == keyToIndex.end()) {
            return false; // Unknown key
        }
        
        const ConfigSchema& schemaEntry = schema[it->second];
        
        // Type validation
        if (!validateType(entry.value, schemaEntry.type)) {
            return false;
        }
        
        // Schema validation (custom validator)
        if (schemaEntry.validator && !schemaEntry.validator(entry.value)) {
            return false;
        }
        
        return true;
    }
    
    bool validateType(const ConfigValue& value, const std::string& type) const {
        if (type == "int" && std::holds_alternative<int>(value)) return true;
        if (type == "int64_t" && std::holds_alternative<int64_t>(value)) return true;
        if (type == "double" && std::holds_alternative<double>(value)) return true;
        if (type == "float" && std::holds_alternative<float>(value)) return true;
        if (type == "bool" && std::holds_alternative<bool>(value)) return true;
        if (type == "string" && std::holds_alternative<std::string>(value)) return true;
        if (type == "size_t" && std::holds_alternative<size_t>(value)) return true;
        
        // Vector types (simplified validation)
        if (type.find("vector<") == 0) {
            if (std::holds_alternative<std::vector<int>>(value) ||
                std::holds_alternative<std::vector<double>>(value) ||
                std::holds_alternative<std::vector<std::string>>(value)) {
                return true;
            }
        }
        
        return false;
    }
    
    std::string configValueToString(const ConfigValue& value) const {
        std::ostringstream oss;
        
        if (std::holds_alternative<int>(value)) {
            oss << std::get<int>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            oss << std::get<int64_t>(value);
        } else if (std::holds_alternative<double>(value)) {
            oss << std::get<double>(value);
        } else if (std::holds_alternative<float>(value)) {
            oss << std::get<float>(value);
        } else if (std::holds_alternative<bool>(value)) {
            oss << (std::get<bool>(value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(value)) {
            oss << std::get<std::string>(value);
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            oss << "[";
            const auto& vec = std::get<std::vector<int>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                oss << vec[i];
                if (i < vec.size() - 1) oss << ", ";
            }
            oss << "]";
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            oss << "[";
            const auto& vec = std::get<std::vector<double>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                oss << vec[i];
                if (i < vec.size() - 1) oss << ", ";
            }
            oss << "]";
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            oss << "[";
            const auto& vec = std::get<std::vector<std::string>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                oss << "\"" << vec[i] << "\"";
                if (i < vec.size() - 1) oss << ", ";
            }
            oss << "]";
        } else if (std::holds_alternative<size_t>(value)) {
            oss << std::get<size_t>(value);
        }
        
        return oss.str();
    }
    
    std::string logLevelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::Debug: return "debug";
            case LogLevel::Info: return "info";
            case LogLevel::Warning: return "warning";
            case LogLevel::Error: return "error";
            case LogLevel::Critical: return "critical";
            default: return "info";
        }
    }
    
    LogLevel stringToLogLevel(const std::string& level) const {
        std::string lower = level;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "debug") return LogLevel::Debug;
        if (lower == "info") return LogLevel::Info;
        if (lower == "warning") return LogLevel::Warning;
        if (lower == "error") return LogLevel::Error;
        if (lower == "critical") return LogLevel::Critical;
        
        return LogLevel::Info; // Default
    }
};

std::vector<Config::Impl::ConfigSchema> Config::Impl::schema = {};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Check if file exists
        if (!std::filesystem::exists(filepath)) {
            Logger::getGlobal().warning("Config file not found: " + filepath);
            return false;
        }
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            Logger::getGlobal().error("Failed to open config file: " + filepath);
            return false;
        }
        
        // Clear existing entries
        pImpl->entries.clear();
        pImpl->keyToIndex.clear();
        
        std::string line;
        while (std::getline(file, line)) {
            // Skip empty lines and comments
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Parse key = value
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                Logger::getGlobal().warning("Invalid config line (no '='): " + line);
                continue;
            }
            
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            // Determine value type and set
            setFromString(key, value, ConfigSource::File);
        }
        
        Logger::getGlobal().info("Loaded configuration from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Exception loading config file " + filepath + ": " + e.what());
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        bool loaded = false;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Check for --key=value format
            size_t pos = arg.find('=');
            if (pos != std::string::npos && arg.substr(0, 2) == "--") {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" -"));
                key.erase(key.find_last_not_of(" -") + 1);
                
                setFromString(key, value, ConfigSource::CommandLine);
                loaded = true;
            }
            // Check for --key value format (when value is next argument)
            else if (arg.substr(0, 2) == "--" && i + 1 < argc && argv[i + 1][0] != '-') {
                std::string key = arg.substr(2);
                std::string value = argv[++i];
                
                key.erase(0, key.find_first_not_of(" -"));
                key.erase(key.find_last_not_of(" -") + 1);
                
                setFromString(key, value, ConfigSource::CommandLine);
                loaded = true;
            }
        }
        
        if (loaded) {
            Logger::getGlobal().info("Loaded configuration from command line arguments");
        }
        
        return loaded;
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Exception loading config from args: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            Logger::getGlobal().error("Failed to open config file for writing: " + filepath);
            return false;
        }
        
        // Write configuration summary
        file << "# NLM Configuration File" << std::endl;
        file << "# Generated by NLM Configuration System" << std::endl;
        file << "# " << std::endl;
        
        for (const auto& entry : pImpl->entries) {
            file << entry.key << " = " << pImpl->configValueToString(entry.value) << " # " << entry.description << std::endl;
        }
        
        file.flush();
        
        if (file.good()) {
            Logger::getGlobal().info("Saved configuration to: " + filepath);
            return true;
        } else {
            Logger::getGlobal().error("Failed to write config file: " + filepath);
            return false;
        }
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Exception saving config file " + filepath + ": " + e.what());
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = pImpl->keyToIndex.find(key);
    if (it == pImpl->keyToIndex.end()) {
        return std::nullopt;
    }
    
    const ConfigEntry& entry = pImpl->entries[it->second];
    
    if (!pImpl->validateType(entry.value, getTypeName<T>())) {
        Logger::getGlobal().warning("Type mismatch for config key " + key + ": expected " + getTypeName<T>());
        return std::nullopt;
    }
    
    try {
        return convertToType<T>(entry.value);
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Exception converting config value for key " + key + ": " + e.what());
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    if (value) {
        return *value;
    }
    
    Logger::getGlobal().info("Using default value for config key " + key + ": " + std::to_string(defaultValue));
    return defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (!pImpl->validateConfig(ConfigEntry(key, value, source))) {
        Logger::getGlobal().warning("Invalid configuration key or value: " + key);
        return;
    }
    
    auto it = pImpl->keyToIndex.find(key);
    if (it != pImpl->keyToIndex.end()) {
        pImpl->entries[it->second].value = value;
        pImpl->entries[it->second].source = source;
    } else {
        pImpl->entries.push_back(ConfigEntry(key, value, source));
        pImpl->keyToIndex[key] = pImpl->entries.size() - 1;
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
    return pImpl->keyToIndex.find(key) != pImpl->keyToIndex.end();
}

void Config::remove(const std::string& key) {
    auto it = pImpl->keyToIndex.find(key);
    if (it != pImpl->keyToIndex.end()) {
        size_t index = it->second;
        pImpl->entries.erase(pImpl->entries.begin() + index);
        
        // Update indices
        pImpl->keyToIndex.clear();
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            pImpl->keyToIndex[pImpl->entries[i].key] = i;
        }
    }
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
    pImpl->keyToIndex.clear();
    pImpl->initializeDefaultConfig();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "=== NLM Configuration ===" << std::endl;
    oss << "Loaded " << pImpl->entries.size() << " configuration entries:" << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = " << pImpl->configValueToString(entry.value) << " (" << configSourceToString(entry.source) << ")" << std::endl;
    }
    
    return oss.str();
}

std::string Config::configSourceToString(ConfigSource source) const {
    switch (source) {
        case ConfigSource::Default: return "Default";
        case ConfigSource::File: return "File";
        case ConfigSource::CommandLine: return "CommandLine";
        case ConfigSource::Runtime: return "Runtime";
        default: return "Unknown";
    }
}

std::string Config::getTypeName(const std::type_info& type) const {
    std::string name = type.name();
    
    // Simplify type names
    if (name.find("int") != std::string::npos) return "int64_t";
    if (name.find("double") != std::string::npos) return "double";
    if (name.find("float") != std::string::npos) return "float";
    if (name.find("bool") != std::string::npos) return "bool";
    if (name.find("string") != std::string::npos) return "string";
    if (name.find("vector") != std::string::npos) {
        if (name.find("int") != std::string::npos) return "vector<int>";
        if (name.find("double") != std::string::npos) return "vector<double>";
        if (name.find("string") != std::string::npos) return "vector<string>";
    }
    
    return name;
}

void Config::setFromString(const std::string& key, const std::string& valueStr, ConfigSource source) {
    // Find schema entry to determine expected type
    pImpl->initializeSchema();
    
    auto it = pImpl->keyToIndex.find(key);
    ConfigSchema schemaEntry("unknown", "string");
    if (it != pImpl->keyToIndex.end()) {
        size_t schemaIndex = it->second;
        if (schemaIndex < pImpl->schema.size()) {
            schemaEntry = pImpl->schema[schemaIndex];
        }
    }
    
    ConfigValue configValue;
    
    try {
        if (schemaEntry.type == "int" || schemaEntry.type == "int64_t") {
            long long val = std::stoll(valueStr);
            configValue = static_cast<int64_t>(val);
        } else if (schemaEntry.type == "double") {
            configValue = std::stod(valueStr);
        } else if (schemaEntry.type == "float") {
            configValue = static_cast<float>(std::stod(valueStr));
        } else if (schemaEntry.type == "bool") {
            std::string lower = valueStr;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            configValue = (lower == "true" || lower == "1" || lower == "yes");
        } else if (schemaEntry.type == "string") {
            configValue = valueStr;
        } else if (schemaEntry.type == "size_t") {
            unsigned long long val = std::stoull(valueStr);
            configValue = static_cast<size_t>(val);
        } else {
            // Try to infer type
            configValue = inferType(valueStr);
        }
        
        set(key, configValue, source);
        
    } catch (const std::exception& e) {
        Logger::getGlobal().warning("Failed to parse config value for key " + key + ": " + valueStr + " (" + e.what() + ")");
        
        // Set as string if parsing fails
        set(key, valueStr, source);
    }
}

ConfigValue Config::inferType(const std::string& valueStr) const {
    // Try different types in order of likelihood
    
    // Check for int
    try {
        size_t pos;
        int64_t val = std::stoll(valueStr, &pos);
        if (pos == valueStr.size()) {
            return val;
        }
    } catch (...) {}
    
    // Check for unsigned int
    try {
        size_t pos;
        uint64_t val = std::stoull(valueStr, &pos);
        if (pos == valueStr.size()) {
            return static_cast<size_t>(val);
        }
    } catch (...) {}
    
    // Check for double
    try {
        size_t pos;
        double val = std::stod(valueStr, &pos);
        if (pos == valueStr.size()) {
            return val;
        }
    } catch (...) {}
    
    // Check for bool
    std::string lower = valueStr;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower == "true" || lower == "false" || lower == "1" || lower == "0") {
        return lower == "true" || lower == "1";
    }
    
    // Default to string
    return valueStr;
}

} // namespace nlm
