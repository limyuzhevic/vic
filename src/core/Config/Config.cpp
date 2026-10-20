#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <unordered_map>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
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
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Implementation of getValidated with type-safe validation
template<typename T>
T Config::getValidated(const std::string& key, const T& defaultValue) const {
    std::string value;
    bool hasValue = false;
    
    // Get the raw string value from schema
    const auto& schema = ConfigSchema::getSchema();
    auto fieldIt = std::find_if(schema.begin(), schema.end(), 
                                [&key](const ConfigSchema::Field& f) { return f.name == key; });
    
    if (fieldIt != schema.end()) {
        if (auto strOpt = get<std::string>(key)) {
            value = *strOpt;
            hasValue = true;
        }
    } else {
        // If key not in schema, try direct lookup
        if (auto strOpt = get<std::string>(key)) {
            value = *strOpt;
            hasValue = true;
        }
    }
    
    // If we have a value, validate and convert
    if (hasValue) {
        if (!validateValue(key, value)) {
            throw ConfigurationError(ErrorCode::INVALID_ARGUMENT, key, typeid(T).name(), value, "Type validation failed");
        }
        
        // Convert based on type
        try {
            if constexpr (std::is_same_v<T, int64_t>) {
                return std::stoll(value);
            } else if constexpr (std::is_same_v<T, double>) {
                return std::stod(value);
            } else if constexpr (std::is_same_v<T, float>) {
                return std::stof(value);
            } else if constexpr (std::is_same_v<T, std::string>) {
                return value;
            } else if constexpr (std::is_same_v<T, bool>) {
                std::string lowerValue = toLower(value);
                if (lowerValue == "true" || lowerValue == "1" || lowerValue == "yes") return true;
                if (lowerValue == "false" || lowerValue == "0" || lowerValue == "no") return false;
                throw ConfigurationError(ErrorCode::INVALID_ARGUMENT, key, "bool", value, "Invalid boolean value");
            } else {
                throw ConfigurationError(ErrorCode::INVALID_ARGUMENT, key, typeid(T).name(), "", "Unsupported type");
            }
        } catch (const std::exception&) {
            throw ConfigurationError(ErrorCode::INVALID_ARGUMENT, key, typeid(T).name(), value, "Conversion failed");
        }
    }
    
    // Return default if validation fails or value not found
    return defaultValue;
}

bool Config::validate() const {
    bool isValid = true;
    
    // Check all schema fields
    const auto& schema = ConfigSchema::getSchema();
    
    for (const auto& field : schema) {
        // Check if field exists
        if (!has(field.name)) {
            if (field.required) {
                NLM_LOG_ERROR("Missing required configuration: " + field.name);
                isValid = false;
            }
            continue;
        }
        
        // Validate the value
        if (auto strOpt = get<std::string>(field.name)) {
            if (!validateValue(field.name, *strOpt)) {
                std::string errorMsg = "Invalid configuration value for \"" + field.name + "\": ";
                errorMsg += getValidationError(field.name, *strOpt);
                NLM_LOG_ERROR(errorMsg);
                isValid = false;
            }
        }
    }
    
    return isValid;
}

bool Config::validateValue(const std::string& key, const std::string& value) const {
    // Check schema first
    const auto& schema = ConfigSchema::getSchema();
    auto fieldIt = std::find_if(schema.begin(), schema.end(), 
                                [&key](const ConfigSchema::Field& f) { return f.name == key; });
    
    if (fieldIt != schema.end()) {
        // Use schema validator if available
        if (fieldIt->validator) {
            return fieldIt->validator(value);
        } else {
            // Basic type validation based on field type
            if (fieldIt->type == "int64_t") {
                return validateInt64(value);
            } else if (fieldIt->type == "double") {
                return validateDouble(value);
            } else if (fieldIt->type == "float") {
                return validateFloat(value);
            } else if (fieldIt->type == "string") {
                return validateString(value);
            } else if (fieldIt->type == "bool") {
                return validateBool(value);
            }
        }
    }
    
    // Special validation based on key name (for backward compatibility)
    if (key == "neuron_count") {
        return validateNeuronCount(value);
    } else if (key == "region_count") {
        return validateRegionCount(value);
    } else if (key == "connection_probability") {
        return validateConnectionProbability(value);
    } else if (key.find("stdp_") != std::string::npos) {
        return validateSTDPParameters(value, key);
    } else if (key.find("_rate") != std::string::npos) {
        return validatePlasticityRate(value, key);
    }
    
    // Generic validation based on type if we can infer it
    try {
        size_t pos;
        if (key.find("weight") != std::string::npos || key.find("coefficient") != std::string::npos) {
            std::stof(value);
            return true;
        } else if (key.find("tau") != std::string::npos) {
            std::stof(value);
            return true;
        } else if (key.find("count") != std::string::npos || key.find("size") != std::string::npos) {
            std::stoll(value);
            return true;
        } else if (key.find("probability") != std::string::npos) {
            std::stof(value);
            return true;
        } else if (key.find("rate") != std::string::npos) {
            std::stof(value);
            return true;
        } else if (key.find("seed") != std::string::npos) {
            std::stoull(value);
            return true;
        } else if (key.find("interval") != std::string::npos) {
            std::stoll(value);
            return true;
        } else if (key.find("enabled") != std::string::npos || key.find("modulation") != std::string::npos) {
            std::string lowerValue = toLower(value);
            return lowerValue == "true" || lowerValue == "1" || lowerValue == "yes" ||
                   lowerValue == "false" || lowerValue == "0" || lowerValue == "no";
        } else if (key.find("dir") != std::string::npos || key.find("folder") != std::string::npos) {
            return validateString(value);
        } else if (key.find("learning") != std::string::npos || key.find("rate") != std::string::npos) {
            std::stof(value);
            return true;
        }
    } catch (...) {
        return false;
    }
    
    // If no specific validation found, try basic validation
    try {
        // Try to convert based on common patterns
        if (key.find("int") != std::string::npos || key.find("count") != std::string::npos || 
            key.find("size") != std::string::npos || key.find("interval") != std::string::npos ||
            key.find("seed") != std::string::npos) {
            std::stoll(value);
            return true;
        } else if (key.find("float") != std::string::npos || key.find("weight") != std::string::npos || 
                   key.find("probability") != std::string::npos || key.find("rate") != std::string::npos ||
                   key.find("strength") != std::string::npos || key.find("modulation") != std::string::npos ||
                   key.find("coefficient") != std::string::npos) {
            std::stof(value);
            return true;
        } else if (key.find("double") != std::string::npos) {
            std::stod(value);
            return true;
        } else if (key.find("enabled") != std::string::npos || key.find("modulation") != std::string::npos) {
            std::string lowerValue = toLower(value);
            return lowerValue == "true" || lowerValue == "1" || lowerValue == "yes" ||
                   lowerValue == "false" || lowerValue == "0" || lowerValue == "no";
        } else {
            // For unknown types, just check if it's not empty and has reasonable length
            return validateString(value, 0, 1000);
        }
    } catch (...) {
        return false;
    }
}

std::string Config::getValidationError(const std::string& key, const std::string& value) const {
    // Check schema first
    const auto& schema = ConfigSchema::getSchema();
    auto fieldIt = std::find_if(schema.begin(), schema.end(), 
                                [&key](const ConfigSchema::Field& f) { return f.name == key; });
    
    if (fieldIt != schema.end()) {
        // Use schema validator if available
        if (fieldIt->validator && !fieldIt->validator(value)) {
            return "Invalid value for \"" + key + "\" (type: " + fieldIt->type + ")";
        }
        
        // Additional type-specific validation
        if (fieldIt->type == "int64_t" && !validateInt64(value)) {
            return "Invalid int64_t value for \"" + key + "\": must be integer";
        } else if (fieldIt->type == "double" && !validateDouble(value)) {
            return "Invalid double value for \"" + key + "\": must be floating point number";
        } else if (fieldIt->type == "float" && !validateFloat(value)) {
            return "Invalid float value for \"" + key + "\": must be floating point number";
        } else if (fieldIt->type == "string" && !validateString(value)) {
            return "Invalid string value for \"" + key + "\": must be non-empty string";
        } else if (fieldIt->type == "bool" && !validateBool(value)) {
            return "Invalid bool value for \"" + key + "\": must be true/false, yes/no, 1/0";
        }
    }
    
    // Special validation based on key name
    if (key == "neuron_count" && !validateNeuronCount(value)) {
        return "Invalid neuron_count: must be positive integer less than 100000";
    } else if (key == "region_count" && !validateRegionCount(value)) {
        return "Invalid region_count: must be positive integer less than 1000";
    } else if (key == "connection_probability" && !validateConnectionProbability(value)) {
        return "Invalid connection_probability: must be between 0.0 and 1.0";
    } else if (key.find("stdp_") != std::string::npos && !validateSTDPParameters(value, key)) {
        return "Invalid STDP parameter \"" + key + "\": must be within valid range";
    } else if (key.find("_rate") != std::string::npos && !validatePlasticityRate(value, key)) {
        return "Invalid plasticity rate \"" + key + "\": must be within valid range";
    }
    
    return "Invalid value format for \"" + key + "\"";
}

// Implementation of static validation methods
bool Config::validateInt64(const std::string& value, int64_t min, int64_t max) {
    try {
        size_t pos;
        int64_t num = std::stoll(value, &pos);
        if (pos != value.size()) return false; // Not fully consumed
        return num >= min && num <= max;
    } catch (...) {
        return false;
    }
}

bool Config::validateDouble(const std::string& value, double min, double max) {
    try {
        size_t pos;
        double num = std::stod(value, &pos);
        if (pos != value.size()) return false; // Not fully consumed
        return num >= min && num <= max;
    } catch (...) {
        return false;
    }
}

bool Config::validateFloat(const std::string& value, float min, float max) {
    try {
        size_t pos;
        float num = std::stof(value, &pos);
        if (pos != value.size()) return false; // Not fully consumed
        return num >= min && num <= max;
    } catch (...) {
        return false;
    }
}

bool Config::validateString(const std::string& value, size_t minLength, size_t maxLength) {
    if (minLength > value.size()) return false;
    if (maxLength != std::string::npos && maxLength < value.size()) return false;
    return true;
}

bool Config::validateBool(const std::string& value) {
    std::string lowerValue = toLower(value);
    return lowerValue == "true" || lowerValue == "1" || lowerValue == "yes" ||
           lowerValue == "false" || lowerValue == "0" || lowerValue == "no";
}

bool Config::validateNeuronCount(const std::string& value) {
    try {
        int64_t count = std::stoll(value);
        return count > 0 && count < 100000;
    } catch (...) { return false; }
}

bool Config::validateRegionCount(const std::string& value) {
    try {
        int64_t count = std::stoll(value);
        return count > 0 && count < 1000;
    } catch (...) { return false; }
}

bool Config::validateConnectionProbability(const std::string& value) {
    try {
        float p = std::stof(value);
        return p >= 0.0f && p <= 1.0f;
    } catch (...) { return false; }
}

bool Config::validateSTDPParameters(const std::string& value, const std::string& paramType) {
    try {
        float param = std::stof(value);
        if (paramType == "stdp_ltp_weight") {
            return param >= 0.0f && param <= 1.0f;
        } else if (paramType == "stdp_ltd_weight") {
            return param >= 0.0f && param <= 1.0f;
        } else if (paramType == "stdp_tau") {
            return param > 0.0f && param < 1000.0f;
        } else if (paramType == "plasticity_rate") {
            return param >= 0.0f && param <= 0.01f;
        } else if (paramType == "pruning_rate") {
            return param >= 0.0f && param <= 0.001f;
        }
        return true;
    } catch (...) { return false; }
}

bool Config::validatePlasticityRate(const std::string& value, const std::string& rateType) {
    try {
        float rate = std::stof(value);
        if (rate < 0.0f || rate > 1.0f) return false;
        
        if (rateType == "synaptogenesis_rate") {
            return rate <= 0.01f;
        } else if (rateType == "pruning_rate") {
            return rate <= 0.001f;
        } else if (rateType == "learning_rate") {
            return rate <= 1.0f;
        } else if (rateType == "innovation_rate") {
            return rate <= 1.0f;
        } else if (rateType == "exploration_rate") {
            return rate <= 1.0f;
        }
        
        return true;
    } catch (...) { return false; }
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// Template instantiations for getValidated
template int64_t Config::getValidated<int64_t>(const std::string&, const int64_t&) const;
template double Config::getValidated<double>(const std::string&, const double&) const;
template float Config::getValidated<float>(const std::string&, const float&) const;
template<std::string> Config::getValidated<std::string>(const std::string&, const std::string&) const;
template bool Config::getValidated<bool>(const std::string&, const bool&) const;

} // namespace nlm
