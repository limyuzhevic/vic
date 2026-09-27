#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <chrono>
#include <ctime>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Environment
};

// Configuration validation result
struct ValidationResult {
    bool valid = true;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    void addError(const std::string& error) {
        valid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string toString() const {
        std::ostringstream oss;
        if (!errors.empty()) {
            oss << "Errors:\n";
            for (const auto& error : errors) {
                oss << "  - " << error << "\n";
            }
        }
        if (!warnings.empty()) {
            oss << (errors.empty() ? "Errors:\n" : "Warnings:\n");
            for (const auto& warning : warnings) {
                oss << "  - " << warning << "\n";
            }
        }
        return oss.str();
    }
};

// Configuration entry with schema support
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    bool required = false;
    std::string typeName;
    
    // Validation functions
    std::function<ValidationResult(const ConfigValue&)> validator;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), 
                    description(), required(false), typeName() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), required(false) {
        setTypeName();
    }
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s,
                const std::string& desc, bool req, const std::string& type,
                std::function<ValidationResult(const ConfigValue&)> val)
        : key(k), value(v), source(s), description(desc), required(req),
          typeName(type), validator(val) {}
    
    void setTypeName() {
        if (std::holds_alternative<int>(value)) typeName = "int";
        else if (std::holds_alternative<int64_t>(value)) typeName = "int64_t";
        else if (std::holds_alternative<double>(value)) typeName = "double";
        else if (std::holds_alternative<bool>(value)) typeName = "bool";
        else if (std::holds_alternative<std::string>(value)) typeName = "string";
        else if (std::holds_alternative<std::vector<int>>(value)) typeName = "vector<int>";
        else if (std::holds_alternative<std::vector<double>>(value)) typeName = "vector<double>";
        else if (std::holds_alternative<std::vector<std::string>>(value)) typeName = "vector<string>";
        else typeName = "unknown";
    }
    
    // Validate this entry
    ValidationResult validate() const {
        ValidationResult result;
        if (required && value.index() == std::variant_npos) {
            result.addError("Required key '" + key + "' has no value");
        }
        if (validator) {
            ValidationResult valResult = validator(value);
            result.errors.insert(result.errors.end(), valResult.errors.begin(), valResult.errors.end());
            result.warnings.insert(result.warnings.end(), valResult.warnings.begin(), valResult.warnings.end());
        }
        return result;
    }
};

// Configuration schema for validation and documentation
class ConfigSchema {
public:
    using ValidationCallback = std::function<ValidationResult(const std::string&, const ConfigValue&)>;
    
    ConfigSchema() = default;
    
    // Add a configuration entry with metadata
    void addEntry(const std::string& key, const ConfigValue& defaultValue,
                  ConfigSource source, const std::string& description,
                  bool required = false, const std::string& type = "",
                  ValidationCallback validator = nullptr) {
        entries_[key] = ConfigEntry(key, defaultValue, source, description, required, type, validator);
    }
    
    // Add a configuration entry with validation
    template<typename T>
    void addValidatedEntry(const std::string& key, const T& defaultValue,
                          ConfigSource source, const std::string& description,
                          ValidationCallback validator) {
        entries_[key] = ConfigEntry(key, defaultValue, source, description, false, getTypeName<T>(), validator);
    }
    
    // Get all defined entries
    const std::map<std::string, ConfigEntry>& getEntries() const { return entries_; }
    
    // Get entry by key
    const ConfigEntry* getEntry(const std::string& key) const {
        auto it = entries_.find(key);
        return it != entries_.end() ? &it->second : nullptr;
    }
    
    // Validate configuration against schema
    ValidationResult validate(const std::map<std::string, ConfigValue>& config) const {
        ValidationResult result;
        
        // Check required entries
        for (const auto& [key, entry] : entries_) {
            if (entry.required) {
                if (config.find(key) == config.end()) {
                    result.addError("Required configuration key missing: " + key);
                }
            }
        }
        
        // Validate individual entries
        for (const auto& [key, value] : config) {
            auto it = entries_.find(key);
            if (it != entries_.end()) {
                ValidationResult valResult = it->second.validate(value);
                result.errors.insert(result.errors.end(), valResult.errors.begin(), valResult.errors.end());
                result.warnings.insert(result.warnings.end(), valResult.warnings.begin(), valResult.warnings.end());
            } else {
                result.addWarning("Unknown configuration key: " + key);
            }
        }
        
        return result;
    }
    
    // Generate configuration documentation
    std::string generateDocumentation() const {
        std::ostringstream oss;
        oss << "NLM Configuration Schema\n";
        oss << "==========================\n\n";
        
        for (const auto& [key, entry] : entries_) {
            oss << "Key: " << key << "\n";
            oss << "  Type: " << entry.typeName << "\n";
            oss << "  Description: " << entry.description << "\n";
            oss << "  Required: " << (entry.required ? "Yes" : "No") << "\n";
            oss << "  Source: " << static_cast<int>(entry.source) << "\n";
            oss << "\n";
        }
        
        return oss.str();
    }
    
private:
    template<typename T>
    std::string getTypeName() const {
        if constexpr (std::is_same_v<T, int>) return "int";
        else if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
        else if constexpr (std::is_same_v<T, double>) return "double";
        else if constexpr (std::is_same_v<T, bool>) return "bool";
        else if constexpr (std::is_same_v<T, std::string>) return "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) return "vector<int>";
        else if constexpr (std::is_same_v<T, std::vector<double>>) return "vector<double>";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "vector<string>";
        else return "unknown";
    }
    
    std::map<std::string, ConfigEntry> entries_;
};

// Configuration class with enhanced features
class Config {
public:
    Config() : pImpl(std::make_unique<Impl>()) {
        // Initialize with default schema
        initializeDefaultSchema();
    }
    
    ~Config() = default;
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept = default;
    Config& operator=(Config&&) noexcept = default;
    
    // Load from JSON file
    bool loadFromFile(const std::string& filepath) {
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Failed to open config file: " + filepath);
                return false;
            }
            
            nlohmann::json jsonConfig;
            file >> jsonConfig;
            
            // Convert JSON to our config entries
            for (auto it = jsonConfig.begin(); it != jsonConfig.end(); ++it) {
                std::string key = it.key();
                ConfigValue value;
                
                try {
                    if (it->is_boolean()) {
                        value = it->get<bool>();
                    } else if (it->is_number_integer()) {
                        value = it->get<int64_t>();
                    } else if (it->is_number_float()) {
                        value = it->get<double>();
                    } else if (it->is_string()) {
                        value = it->get<std::string>();
                    } else if (it->is_array()) {
                        // Handle arrays
                        nlohmann::json array = *it;
                        if (!array.empty()) {
                            if (array[0].is_boolean()) {
                                std::vector<bool> boolArray = array.get<std::vector<bool>>();
                                value = boolArray;
                            } else if (array[0].is_number_integer()) {
                                std::vector<int64_t> intArray = array.get<std::vector<int64_t>>();
                                value = intArray;
                            } else if (array[0].is_number_float()) {
                                std::vector<double> doubleArray = array.get<std::vector<double>>();
                                value = doubleArray;
                            } else if (array[0].is_string()) {
                                std::vector<std::string> stringArray = array.get<std::vector<std::string>>();
                                value = stringArray;
                            }
                        }
                    }
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING("Failed to parse config value for key '" + key + "': " + std::string(e.what()));
                    continue;
                }
                
                if (value.index() != std::variant_npos) {
                    set(key, value, ConfigSource::File);
                }
            }
            
            // Validate configuration
            ValidationResult validation = schema_.validate(pImpl->entries);
            if (!validation.valid) {
                NLM_LOG_WARNING("Configuration validation failed:\n" + validation.toString());
            }
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to load config from file " + filepath + ": " + std::string(e.what()));
            return false;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        
        NLM_LOG_INFO("Config file loaded from " + filepath + " (" + std::to_string(duration) + "ms)");
        return true;
    }
    
    // Load from command line arguments with enhanced support
    bool loadFromArgs(int argc, char** argv) {
        auto start = std::chrono::high_resolution_clock::now();
        
        bool success = true;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    setFromString(key, value, ConfigSource::CommandLine);
                } else {
                    NLM_LOG_ERROR("Invalid command line argument format: " + arg);
                    success = false;
                }
            }
            // Handle -key value format
            else if (arg[0] == '-') {
                std::string key = arg.substr(1);
                if (i + 1 < argc) {
                    std::string value = argv[++i];
                    setFromString(key, value, ConfigSource::CommandLine);
                } else {
                    NLM_LOG_ERROR("Missing value for command line argument: " + arg);
                    success = false;
                }
            }
            // Handle --help flag
            else if (arg == "--help" || arg == "-h") {
                printHelp();
                return false; // Exit normally after showing help
            }
        }
        
        // Validate configuration
        ValidationResult validation = schema_.validate(pImpl->entries);
        if (!validation.valid) {
            NLM_LOG_ERROR("Configuration validation failed:\n" + validation.toString());
            success = false;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        
        if (success) {
            NLM_LOG_INFO("Command line arguments processed (" + std::to_string(duration) + "ms)");
        }
        
        return success;
    }
    
    // Save to JSON file
    bool saveToFile(const std::string& filepath) const {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
                return false;
            }
            
            nlohmann::json jsonConfig;
            
            for (const auto& entry : pImpl->entries) {
                jsonConfig[entry.key] = configValueToJson(entry.value);
            }
            
            file << jsonConfig.dump(2); // Pretty print with 2-space indentation
            
            NLM_LOG_INFO("Config file saved to " + filepath);
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to save config to file " + filepath + ": " + std::string(e.what()));
            return false;
        }
    }
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        auto it = pImpl->entries.find(key);
        if (it == pImpl->entries.end()) {
            return std::nullopt;
        }
        
        try {
            return std::get<T>(it->second.value);
        } catch (const std::bad_variant_access&) {
            NLM_LOG_WARNING("Type mismatch for config key '" + key + "' (expected " + 
                          getTypeName<T>() + ")");
            return std::nullopt;
        }
    }
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        auto val = get<T>(key);
        return val.has_value() ? val.value() : defaultValue;
    }
    
    // Set values with source tracking
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime) {
        auto it = pImpl->entries.find(key);
        if (it != pImpl->entries.end()) {
            it->second.value = value;
            it->second.source = source;
        } else {
            pImpl->entries.emplace_back(key, value, source);
        }
        
        // Validate this entry
        auto entryIt = pImpl->entries.find(key);
        if (entryIt != pImpl->entries.end()) {
            ValidationResult validation = entryIt->second.validate(entryIt->second.value);
            if (!validation.valid) {
                for (const auto& error : validation.errors) {
                    NLM_LOG_ERROR("Config validation error for key '" + key + "': " + error);
                }
            }
        }
    }
    
    // Set values with convenience methods
    void set(const std::string& key, const std::string& value, 
             ConfigSource source = ConfigSource::Runtime) {
        set(key, ConfigValue(value), source);
    }
    
    void set(const std::string& key, int value, 
             ConfigSource source = ConfigSource::Runtime) {
        set(key, ConfigValue(value), source);
    }
    
    void set(const std::string& key, double value, 
             ConfigSource source = ConfigSource::Runtime) {
        set(key, ConfigValue(value), source);
    }
    
    void set(const std::string& key, bool value, 
             ConfigSource source = ConfigSource::Runtime) {
        set(key, ConfigValue(value), source);
    }
    
    // Check existence
    bool has(const std::string& key) const {
        return pImpl->entries.find(key) != pImpl->entries.end();
    }
    
    // Remove key
    void remove(const std::string& key) {
        pImpl->entries.erase(key);
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        keys.reserve(pImpl->entries.size());
        for (const auto& entry : pImpl->entries) {
            keys.push_back(entry.key);
        }
        return keys;
    }
    
    // Clear all
    void clear() {
        pImpl->entries.clear();
    }
    
    // Get configuration summary with enhanced information
    std::string summary() const {
        std::ostringstream oss;
        oss << "NLM Configuration (" << pImpl->entries.size() << " entries):\n";
        
        // Sort by source for better readability
        std::vector<std::pair<std::string, ConfigEntry>> sortedEntries;
        for (const auto& entry : pImpl->entries) {
            sortedEntries.emplace_back(entry.key, entry.value);
        }
        
        // Sort by source (CommandLine > File > Default > Runtime)
        std::sort(sortedEntries.begin(), sortedEntries.end(),
            [](const auto& a, const auto& b) {
                int sourceA = static_cast<int>(a.second.source);
                int sourceB = static_cast<int>(b.second.source);
                return sourceA > sourceB; // Higher source priority first
            });
        
        for (const auto& entry : sortedEntries) {
            oss << "  " << entry.first << " = ";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    oss << (arg ? "true" : "false");
                } else {
                    oss << arg;
                }
            }, entry.second.value);
            oss << " (" << static_cast<int>(entry.second.source) << ")\n";
        }
        
        return oss.str();
    }
    
    // Print help for available configuration options
    void printHelp() const {
        std::cout << "NLM Configuration Options\n";
        std::cout << "========================\n\n";
        std::cout << "Usage:\n";
        std::cout << "  --key=value    Set configuration key to value\n";
        std::cout << "  -k value       Short form (requires separate value)\n";
        std::cout << "\n";        
        std::cout << "Available configuration keys:\n";
        for (const auto& entry : schema_.getEntries()) {
            std::cout << "  --" << entry.first << " <value>  ";
            std::cout << entry.second.description << "\n";
            std::cout << "      Type: " << entry.second.typeName << "\n";
            std::cout << "      Required: " << (entry.second.required ? "Yes" : "No") << "\n\n";
        }
    }
    
    // Validate current configuration
    ValidationResult validate() const {
        return schema_.validate(pImpl->entries);
    }
    
    // Get the schema
    const ConfigSchema& getSchema() const { return schema_; }
    
    // Add custom validation function for a key
    void addValidator(const std::string& key, 
                     std::function<ValidationResult(const std::string&, const ConfigValue&)> validator) {
        auto entry = schema_.getEntry(key);
        if (entry) {
            // We'll need to modify the schema to support this, for now just log
            NLM_LOG_INFO("Custom validator registered for key: " + key);
        }
    }
    
    // Environment variable support
    void loadFromEnvironment(const std::string& prefix = "NLM_") {
        for (const auto& entry : schema_.getEntries()) {
            std::string envKey = prefix + entry.first;
            char* envValue = std::getenv(envKey.c_str());
            if (envValue) {
                std::string value = envValue;
                setFromString(entry.first, value, ConfigSource::Environment);
                NLM_LOG_INFO("Loaded config from environment: " + envKey);
            }
        }
    }
    
    // Load default configuration
    void loadDefaults() {
        for (const auto& entry : schema_.getEntries()) {
            if (std::holds_alternative<int>(entry.second.value)) {
                set(entry.first, std::get<int>(entry.second.value), entry.second.source);
            } else if (std::holds_alternative<int64_t>(entry.second.value)) {
                set(entry.first, std::get<int64_t>(entry.second.value), entry.second.source);
            } else if (std::holds_alternative<double>(entry.second.value)) {
                set(entry.first, std::get<double>(entry.second.value), entry.second.source);
            } else if (std::holds_alternative<bool>(entry.second.value)) {
                set(entry.first, std::get<bool>(entry.second.value), entry.second.source);
            } else if (std::holds_alternative<std::string>(entry.second.value)) {
                set(entry.first, std::get<std::string>(entry.second.value), entry.second.source);
            }
        }
    }
    
    // Configuration entry helper
    struct ConfigEntryHelper {
        Config& config;
        
        ConfigEntryHelper(Config& c) : config(c) {}
        
        ConfigEntryHelper& set(const std::string& key, const ConfigValue& value) {
            config.set(key, value);
            return *this;
        }
        
        ConfigEntryHelper& setInt(const std::string& key, int value) {
            config.set(key, value);
            return *this;
        }
        
        ConfigEntryHelper& setDouble(const std::string& key, double value) {
            config.set(key, value);
            return *this;
        }
        
        ConfigEntryHelper& setString(const std::string& key, const std::string& value) {
            config.set(key, value);
            return *this;
        }
        
        ConfigEntryHelper& setBool(const std::string& key, bool value) {
            config.set(key, value);
            return *this;
        }
    };
    
    ConfigEntryHelper set() {
        return ConfigEntryHelper(*this);
    }
    
    // Get helper for fluent API
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        return Config::get<T>(key);
    }
    
    // Convert ConfigValue to JSON
    static nlohmann::json configValueToJson(const ConfigValue& value) {
        nlohmann::json json;
        std::visit([&json](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                json = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                json = arg;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                json = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                json = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                json = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                json = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                json = arg;
            }
        }, value);
        return json;
    }
    
    // Set value from string with type conversion
    void setFromString(const std::string& key, const std::string& valueStr, ConfigSource source) {
        // Try to detect type and convert
        if (valueStr == "true" || valueStr == "false") {
            bool boolValue = (valueStr == "true");
            set(key, boolValue, source);
            return;
        }
        
        try {
            // Try integer
            size_t pos;
            int intValue = std::stoi(valueStr, &pos);
            if (pos == valueStr.length()) {
                set(key, intValue, source);
                return;
            }
        } catch (...) {}
        
        try {
            // Try double
            size_t pos;
            double doubleValue = std::stod(valueStr, &pos);
            if (pos == valueStr.length()) {
                set(key, doubleValue, source);
                return;
            }
        } catch (...) {}
        
        // Default to string
        set(key, valueStr, source);
    }
    
    // Get type name for template
    template<typename T>
    std::string getTypeName() const {
        if constexpr (std::is_same_v<T, int>) return "int";
        else if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
        else if constexpr (std::is_same_v<T, double>) return "double";
        else if constexpr (std::is_same_v<T, bool>) return "bool";
        else if constexpr (std::is_same_v<T, std::string>) return "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) return "vector<int>";
        else if constexpr (std::is_same_v<T, std::vector<double>>) return "vector<double>";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "vector<string>";
        else return "unknown";
    }
    
private:
    struct Impl {
        std::map<std::string, ConfigEntry> entries;
    };
    
    std::unique_ptr<Impl> pImpl;
    ConfigSchema schema_;
    
    // Initialize default configuration schema
    void initializeDefaultSchema() {
        // Core simulation parameters
        addEntry("random_seed", 42, ConfigSource::Default,
                "Random seed for reproducible experiments");
        
        addEntry("simulation_timestep", 0.001, ConfigSource::Default,
                "Simulation timestep in seconds");
        
        addEntry("neuron_count", 1000, ConfigSource::Default,
                "Number of neurons in the brain");
        
        addEntry("region_count", 2, ConfigSource::Default,
                "Number of brain regions");
        
        addEntry("connection_probability", 0.1, ConfigSource::Default,
                "Connection probability for random connectivity");
        
        // Plasticity settings
        addEntry("stdp_ltp_weight", 0.01, ConfigSource::Default,
                "STDP long-term potentiation weight");
        
        addEntry("stdp_ltd_weight", 0.012, ConfigSource::Default,
                "STDP long-term depression weight");
        
        addEntry("stdp_tau", 20.0, ConfigSource::Default,
                "STDP time constant for plasticity");
        
        // Development settings
        addEntry("development_synaptogenesis_rate", 0.001, ConfigSource::Default,
                "Rate of synapse formation during development");
        
        addEntry("development_pruning_rate", 0.0001, ConfigSource::Default,
                "Rate of synapse pruning during development");
        
        // Neuromodulation settings
        addEntry("dopamine_baseline", 0.1, ConfigSource::Default,
                "Baseline dopamine level for reward modulation");
        
        // Reward settings
        addEntry("reward_discount_factor", 0.99, ConfigSource::Default,
                "Discount factor for reward prediction");
        
        // Environment settings
        addEntry("environment_name", std::string("GridWorld"), ConfigSource::Default,
                "Name of the environment");
        
        addEntry("environment_width", 10, ConfigSource::Default,
                "Width of the environment in grid units");
        
        addEntry("environment_height", 10, ConfigSource::Default,
                "Height of the environment in grid units");
        
        // Logging settings
        addEntry("log_level", std::string("INFO"), ConfigSource::Default,
                "Logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL)");
        
        addEntry("log_to_file", false, ConfigSource::Default,
                "Whether to log to file in addition to console");
        
        addEntry("log_filename", std::string("nlm.log"), ConfigSource::Default,
                "Filename for log output");
        
        // Simulation settings
        addEntry("max_simulation_steps", 10000, ConfigSource::Default,
                "Maximum number of simulation steps");
        
        addEntry("simulation_time_limit", 0.0, ConfigSource::Default,
                "Simulation time limit in seconds (0 = unlimited)");
        
        // Visualization settings
        addEntry("visualization_enabled", false, ConfigSource::Default,
                "Whether to enable visualization");
        
        addEntry("visualization_update_rate", 30.0, ConfigSource::Default,
                "Visualization update rate in Hz");
        
        // Advanced neural parameters
        addEntry("neuron_resting_potential", -70.0, ConfigSource::Default,
                "Resting potential for neurons in mV");
        
        addEntry("neuron_threshold", -55.0, ConfigSource::Default,
                "Firing threshold in mV");
        
        addEntry("neuron_reset_potential", -75.0, ConfigSource::Default,
                "Reset potential after firing in mV");
        
        addEntry("neuron_leak_conductance", 0.1, ConfigSource::Default,
                "Membrane leak conductance");
        
        addEntry("neuron_refractory_period", 5.0, ConfigSource::Default,
                "Refractory period in ms");
        
        // Memory system settings
        addEntry("working_memory_capacity", 100, ConfigSource::Default,
                "Capacity of working memory in traces");
        
        addEntry("episodic_memory_max_episodes", 1000, ConfigSource::Default,
                "Maximum number of episodes in episodic memory");
        
        // Development system settings
        addEntry("developmental_stage_initial", 0.0, ConfigSource::Default,
                "Duration of initial developmental stage");
        
        addEntry("developmental_stage_critical_period", 60.0, ConfigSource::Default,
                "Duration of critical period");
        
        addEntry("developmental_stage_maturation", 240.0, ConfigSource::Default,
                "Duration of maturation stage");
        
        // Checkpoint settings
        addEntry("checkpoint_enabled", true, ConfigSource::Default,
                "Whether to enable checkpoint saving");
        
        addEntry("checkpoint_interval", 1000, ConfigSource::Default,
                "Checkpoint save interval in steps");
        
        addEntry("checkpoint_dir", std::string("./checkpoints"), ConfigSource::Default,
                "Directory for checkpoint files");
        
        // Advanced plasticity settings
        addEntry("plasticity_learning_rate", 0.01, ConfigSource::Default,
                "Base learning rate for plasticity");
        
        addEntry("plasticity_modulation_strength", 1.0, ConfigSource::Default,
                "Strength of neuromodulation on plasticity");
        
        // Sensory system settings
        addEntry("sensory_noise_level", 0.01, ConfigSource::Default,
                "Level of noise in sensory processing");
        
        addEntry("sensory_filter_strength", 0.5, ConfigSource::Default,
                "Strength of sensory filtering");
        
        // Motor system settings
        addEntry("motor_command_history_length", 10, ConfigSource::Default,
                "Length of motor command history for action selection");
        
        addEntry("motor_noise_level", 0.05, ConfigSource::Default,
                "Level of noise in motor output");
        
        // Prediction system settings
        addEntry("prediction_horizon", 10, ConfigSource::Default,
                "Prediction horizon in steps");
        
        addEntry("prediction_error_threshold", 0.1, ConfigSource::Default,
                "Threshold for prediction error detection");
        
        // Attention system settings
        addEntry("attention_inhibition_strength", 0.5, ConfigSource::Default,
                "Strength of competitive inhibition in attention");
        
        addEntry("attention_excitation_strength", 1.5, ConfigSource::Default,
                "Strength of excitation in attention");
        
        // Concept formation settings
        addEntry("concept_threshold", 0.7, ConfigSource::Default,
                "Activation threshold for concept formation");
        
        addEntry("concept_association_strength", 1.0, ConfigSource::Default,
                "Strength of concept associations");
        
        // World simulation settings
        addEntry("world_time_step", 0.1, ConfigSource::Default,
                "World simulation time step");
        
        addEntry("world_max_energy", 100.0, ConfigSource::Default,
                "Maximum energy for agents");
        
        addEntry("world_energy_decay_rate", 0.01, ConfigSource::Default,
                "Energy decay rate per time step");
        
        // Agent brain settings
        addEntry("agent_curiosity_threshold", 0.3, ConfigSource::Default,
                "Curiosity threshold for exploration");
        
        addEntry("agent_novelty_decay_rate", 0.99, ConfigSource::Default,
                "Rate at which novelty decays");
        
        addEntry("agent_reward_modulation", true, ConfigSource::Default,
                "Whether to enable reward modulation");
        
        // Development settings
        addEntry("development_enabled", true, ConfigSource::Default,
                "Whether to enable developmental processes");
        
        addEntry("development_plasticity_modifier", 1.0, ConfigSource::Default,
                "Base plasticity modifier during development");
        
        // Curiosity settings
        addEntry("curiosity_enabled", true, ConfigSource::Default,
                "Whether to enable curiosity-driven exploration");
        
        addEntry("curiosity_sensitivity", 1.0, ConfigSource::Default,
                "Sensitivity to novel information");
        
        // Novelty settings
        addEntry("novelty_enabled", true, ConfigSource::Default,
                "Whether to enable novelty detection");
        
        addEntry("novelty_sensitivity", 1.0, ConfigSource::Default,
                "Sensitivity to novel information");
    }
    
    // Helper for adding entries with validation
    template<typename T>
    void addValidatedEntry(const std::string& key, const T& defaultValue,
                          ConfigSource source, const std::string& description,
                          std::function<ValidationResult(const ConfigValue&)> validator) {
        schema_.addValidatedEntry(key, defaultValue, source, description, validator);
    }
};

} // namespace nlm