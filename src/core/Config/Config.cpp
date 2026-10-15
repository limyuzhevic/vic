#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <regex>
#include <unordered_map>
#include <chrono>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Schema for validation and documentation
    std::unordered_map<std::string, ConfigSchema> schema;
};

std::string Config::getDescription(const std::string& key) const {
    auto it = pImpl->schema.find(key);
    if (it != pImpl->schema.end()) {
        return it->second.description;
    }
    return "No description available";
}

ConfigSource Config::getDefaultSource(const std::string& key) const {
    auto it = pImpl->schema.find(key);
    if (it != pImpl->schema.end()) {
        return it->second.defaultSource;
    }
    return ConfigSource::Default;
}

ValidationResult Config::validate(const std::string& key, const ConfigValue& value) const {
    auto it = pImpl->schema.find(key);
    if (it != pImpl->schema.end()) {
        return validateValue(it->second, value);
    }
    // If key is not in schema, still accept it but warn
    return ValidationResult(true, "Warning: Key '" + key + "' is not in the schema");
}

bool Config::hasDescription(const std::string& key) const {
    return pImpl->schema.find(key) != pImpl->schema.end();
}

std::string Config::getType(const std::string& key) const {
    auto it = pImpl->schema.find(key);
    if (it != pImpl->schema.end()) {
        return it->second.type;
    }
    return "unknown";
}

std::string Config::getUnits(const std::string& key) const {
    auto it = pImpl->schema.find(key);
    if (it != pImpl->schema.end()) {
        return it->second.units;
    }
    return "";
}

void Config::addSchema(const ConfigSchema& schema) {
    pImpl->schema[schema.name] = schema;
}

std::vector<std::string> Config::getAllSchemaKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->schema.size());
    for (const auto& pair : pImpl->schema) {
        keys.push_back(pair.first);
    }
    return keys;
}

// String to value conversions
ConfigValue stringToValue(const std::string& valueStr, const std::string& type) {
    if (type == "int") {
        return std::stoi(valueStr);
    } else if (type == "int64") {
        return static_cast<int64_t>(std::stoll(valueStr));
    } else if (type == "double") {
        return std::stod(valueStr);
    } else if (type == "bool") {
        std::string lower = toLower(valueStr);
        if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
            return true;
        } else if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
            return false;
        } else {
            throw std::invalid_argument("Invalid boolean value: " + valueStr);
        }
    } else if (type == "string") {
        return valueStr;
    } else if (type == "vector<int>") {
        // Parse comma-separated list
        std::vector<int> result;
        std::stringstream ss(valueStr);
        std::string token;
        while (std::getline(ss, token, ',')) {
            result.push_back(std::stoi(trim(token)));
        }
        return result;
    } else if (type == "vector<double>") {
        // Parse comma-separated list
        std::vector<double> result;
        std::stringstream ss(valueStr);
        std::string token;
        while (std::getline(ss, token, ',')) {
            result.push_back(std::stod(trim(token)));
        }
        return result;
    }
    return valueStr; // Default to string
}

ValidationResult validateValue(const ConfigSchema& schema, const ConfigValue& value) {
    // Check type consistency
    std::string type = schema.type;
    bool typeMatches = false;
    
    if (type == "int") typeMatches = std::holds_alternative<int>(value);
    else if (type == "int64") typeMatches = std::holds_alternative<int64_t>(value);
    else if (type == "double") typeMatches = std::holds_alternative<double>(value);
    else if (type == "bool") typeMatches = std::holds_alternative<bool>(value);
    else if (type == "string") typeMatches = std::holds_alternative<std::string>(value);
    else if (type == "vector<int>") typeMatches = std::holds_alternative<std::vector<int>>(value);
    else if (type == "vector<double>") typeMatches = std::holds_alternative<std::vector<double>>(value);
    
    if (!typeMatches) {
        return ValidationResult(false, "Expected type " + schema.type + " but got different type");
    }
    
    // Validate numeric ranges if applicable
    if (schema.minValue > -1e99 || schema.maxValue < 1e99) {
        try {
            double numericValue;
            if (type == "int") numericValue = static_cast<double>(std::get<int>(value));
            else if (type == "int64") numericValue = static_cast<double>(std::get<int64_t>(value));
            else if (type == "double") numericValue = std::get<double>(value);
            else if (type == "bool") numericValue = std::get<bool>(value) ? 1.0 : 0.0;
            else continue; // String or vector doesn't have numeric range check
            
            if (numericValue < schema.minValue) {
                return ValidationResult(false, "Value " + std::to_string(numericValue) + 
                    " is below minimum " + std::to_string(schema.minValue) + " for " + schema.name);
            }
            if (numericValue > schema.maxValue) {
                return ValidationResult(false, "Value " + std::to_string(numericValue) + 
                    " exceeds maximum " + std::to_string(schema.maxValue) + " for " + schema.name);
            }
        } catch (...) {
            return ValidationResult(false, "Failed to convert value to numeric for range checking");
        }
    }
    
    // Apply custom validator if provided
    if (schema.validator) {
        ValidationResult customResult = schema.validator(value);
        if (!customResult.valid) {
            return customResult;
        }
    }
    
    return ValidationResult(true);
}

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize default schema definitions
    initializeDefaultSchema();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaultSchema() {
    // Define all configurable parameters with their properties
    pImpl->schema["random_seed"] = ConfigSchema(
        "random_seed", "Random seed for reproducible simulations", ConfigSource::Default,
        "42", "int", "simulation", -2147483648, 2147483647
    );
    
    pImpl->schema["simulation_timestep"] = ConfigSchema(
        "simulation_timestep", "Simulation timestep in seconds", ConfigSource::Default,
        "0.001", "double", "seconds", 0.0, 10.0
    );
    
    pImpl->schema["neuron_count"] = ConfigSchema(
        "neuron_count", "Number of neurons in the network", ConfigSource::Default,
        "500", "int64", "neurons", 1, 1000000
    );
    
    pImpl->schema["region_count"] = ConfigSchema(
        "region_count", "Number of neural regions", ConfigSource::Default,
        "1", "int64", "regions", 1, 100
    );
    
    pImpl->schema["connection_probability"] = ConfigSchema(
        "connection_probability", "Probability of connection between neurons", ConfigSource::Default,
        "0.15", "double", "probability", 0.0, 1.0,
        [](const ConfigValue& v) {
            try {
                double val = std::get<double>(v);
                return val >= 0.0 && val <= 1.0;
            } catch (...) { return false; }
        }
    );
    
    pImpl->schema["stdp_ltp_weight"] = ConfigSchema(
        "stdp_ltp_weight", "STDP LTP weight (A+)", ConfigSource::Default,
        "0.02", "double", "weight", 0.0, 10.0
    );
    
    pImpl->schema["stdp_ltd_weight"] = ConfigSchema(
        "stdp_ltd_weight", "STDP LTD weight (A-)", ConfigSource::Default,
        "0.015", "double", "weight", 0.0, 10.0
    );
    
    pImpl->schema["stdp_tau"] = ConfigSchema(
        "stdp_tau", "STDP time constant (tau)", ConfigSource::Default,
        "20.0", "double", "milliseconds", 1.0, 100.0
    );
    
    pImpl->schema["synaptogenesis_rate"] = ConfigSchema(
        "synaptogenesis_rate", "Rate of new synapse formation", ConfigSource::Default,
        "0.0001", "double", "per_step", 0.0, 1.0
    );
    
    pImpl->schema["pruning_rate"] = ConfigSchema(
        "pruning_rate", "Rate of synapse elimination", ConfigSource::Default,
        "0.00001", "double", "per_step", 0.0, 1.0
    );
    
    pImpl->schema["enable_checkpointing"] = ConfigSchema(
        "enable_checkpointing", "Enable checkpoint/save functionality", ConfigSource::Default,
        "1", "bool", "enabled", 0.0, 1.0
    );
    
    pImpl->schema["enable_replay"] = ConfigSchema(
        "enable_replay", "Enable memory replay functionality", ConfigSource::Default,
        "1", "bool", "enabled", 0.0, 1.0
    );
}

std::string Config::getDescription(const std::string& key) const {
    auto it = pImpl->schema.find(key);
    if (it != pImpl->schema.end()) {
        return it->second.description;
    }
    return "No description available";
}

ConfigSource Config::getDefaultSource(const std::string& key) const {
    auto it = pImpl->schema.find(key);
    if (it != pImpl->schema.end()) {
        return it->second.defaultSource;
    }
    return ConfigSource::Default;
}

ValidationResult Config::validate(const std::string& key, const ConfigValue& value) const {
    auto it = pImpl->schema.find(key);
    if (it != pImpl->schema.end()) {
        return validateValue(it->second, value);
    }
    // If key is not in schema, still accept it but warn
    return ValidationResult(true, "Warning: Key '" + key + "' is not in the schema");
}

bool Config::hasDescription(const std::string& key) const {
    return pImpl->schema.find(key) != pImpl->schema.end();
}

std::string Config::getType(const std::string& key) const {
    auto it = pImpl->schema.find(key);
    if (it != pImpl->schema.end()) {
        return it->second.type;
    }
    return "unknown";
}

std::string Config::getUnits(const std::string& key) const {
    auto it = pImpl->schema.find(key);
    if (it != pImpl->schema.end()) {
        return it->second.units;
    }
    return "";
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        line = trim(line);
        
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Check schema for type information
            auto schemaIt = pImpl->schema.find(key);
            std::string expectedType = "string"; // Default type
            if (schemaIt != pImpl->schema.end()) {
                expectedType = schemaIt->second.type;
            }
            
            try {
                ConfigValue configValue = stringToValue(value, expectedType);
                ValidationResult validation = validate(key, configValue);
                if (!validation.valid) {
                    NLM_LOG_WARNING("Invalid value for key '" + key + "' in " + filepath + ": " + validation.errorMessage);
                    continue;
                }
                set(key, configValue, ConfigSource::File);
                NLM_LOG_INFO("Loaded " + key + " = " + value + " from " + filepath);
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Error parsing line " + std::to_string(lineNum) + " in " + filepath + ": " + e.what());
                continue;
            }
        } else {
            NLM_LOG_WARNING("Invalid line format at line " + std::to_string(lineNum) + " in " + filepath);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Check schema for type information
                auto schemaIt = pImpl->schema.find(key);
                std::string expectedType = "string";
                if (schemaIt != pImpl->schema.end()) {
                    expectedType = schemaIt->second.type;
                }
                
                try {
                    ConfigValue configValue = stringToValue(value, expectedType);
                    ValidationResult validation = validate(key, configValue);
                    if (!validation.valid) {
                        NLM_LOG_WARNING("Invalid command line argument for key '" + key + "': " + validation.errorMessage);
                        continue;
                    }
                    set(key, configValue, ConfigSource::CommandLine);
                    NLM_LOG_INFO("Set " + key + " = " + value + " from command line");
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING("Error parsing command line argument '--" + key + "=' " + e.what());
                    continue;
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Check schema for type information
            auto schemaIt = pImpl->schema.find(key);
            std::string expectedType = "string";
            if (schemaIt != pImpl->schema.end()) {
                expectedType = schemaIt->second.type;
            }
            
            try {
                ConfigValue configValue = stringToValue(value, expectedType);
                ValidationResult validation = validate(key, configValue);
                if (!validation.valid) {
                    NLM_LOG_WARNING("Invalid command line argument for key '" + key + "': " + validation.errorMessage);
                    continue;
                }
                set(key, configValue, ConfigSource::CommandLine);
                NLM_LOG_INFO("Set " + key + " = " + value + " from command line");
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Error parsing command line argument '-" + key + "' " + e.what());
                continue;
            }
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File" << std::endl;
    file << "# Generated by NLM on " << std::chrono::system_clock::now() << std::endl;
    file << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << getDescription(entry.key) << std::endl;
        file << "# Type: " << getType(entry.key) << std::endl;
        file << "# Source: " << static_cast<int>(entry.source) << std::endl;
        if (!getUnits(entry.key).empty()) {
            file << "# Units: " << getUnits(entry.key) << std::endl;
        }
        file << entry.key << " = ";
        
        // Format based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ",";
                    file << arg[i];
                }
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ",";
                    file << arg[i];
                }
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

namespace nlm {
// Template instantiations for get
    template std::optional<int> Config::get<int>(const std::string&) const;
    template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
    template std::optional<double> Config::get<double>(const std::string&) const;
    template std::optional<bool> Config::get<bool>(const std::string&) const;
    template std::optional<std::string> Config::get<std::string>(const std::string&) const;
    
// Template instantiations for getOr
    template int Config::getOr<int>(const std::string&, const int&) const;
    template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
    template double Config::getOr<double>(const std::string&, const double&) const;
    template bool Config::getOr<bool>(const std::string&, const bool&) const;
    template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
}