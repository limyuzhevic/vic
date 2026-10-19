#include "core/Config/Config.hpp"
#include "core/Config/ConfigValidator.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <vector>
#include <string>
#include <cmath>
#include <iostream>
#include "core/Logger/Logger.hpp"

namespace nlm {

class Config::Impl {
public:
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::vector<std::string>> parameterDependencies;
    std::unordered_map<std::string, std::pair<double, double>> parameterRanges;
    std::unordered_map<std::string, std::string> parameterTypes;
    
    Impl() {
        initializeSchema();
    }
    
    void initializeSchema() {
        // Neuron count validation
        parameterRanges["neuron_count"] = {1, 1000000};
        parameterTypes["neuron_count"] = "size_t";
        
        // Region count validation
        parameterRanges["region_count"] = {1, 100};
        parameterTypes["region_count"] = "size_t";
        
        // Connection probability validation
        parameterRanges["connection_probability"] = {0.0, 1.0};
        parameterTypes["connection_probability"] = "float";
        
        // STDP parameters validation
        parameterRanges["stdp_ltp_weight"] = {0.0, 1.0};
        parameterTypes["stdp_ltp_weight"] = "float";
        
        parameterRanges["stdp_ltd_weight"] = {0.0, 1.0};
        parameterTypes["stdp_ltd_weight"] = "float";
        
        parameterRanges["stdp_tau"] = {1.0, 1000.0};
        parameterTypes["stdp_tau"] = "float";
        
        // Structural plasticity parameters validation
        parameterRanges["synaptogenesis_rate"] = {0.0, 0.1};
        parameterTypes["synaptogenesis_rate"] = "float";
        
        parameterRanges["pruning_rate"] = {0.0, 0.1};
        parameterTypes["pruning_rate"] = "float";
        
        // Time parameters validation
        parameterRanges["simulation_timestep"] = {0.0001, 0.1};
        parameterTypes["simulation_timestep"] = "double";
        
        parameterRanges["replay_interval"] = {1, 100000};
        parameterTypes["replay_interval"] = "size_t";
        
        parameterRanges["consolidation_interval"] = {1, 100000};
        parameterTypes["consolidation_interval"] = "size_t";
        
        // Parameter dependencies
        parameterDependencies["region_count"] = {"neuron_count"};
        parameterDependencies["connection_probability"] = {};
        parameterDependencies["synaptogenesis_rate"] = {};
        parameterDependencies["pruning_rate"] = {};
    }
    
    bool validateAndAddEntry(const std::string& key, const ConfigValue& value, ConfigSource source) {
        std::vector<ConfigEntry> tempEntries = entries;
        tempEntries.emplace_back(key, value, source);
        
        auto errors = ConfigValidator::validate(tempEntries);
        if (!errors.empty()) {
            std::string errorMsg = "Configuration validation failed for parameter \"" + key + "\"\n";
            for (const auto& error : errors) {
                errorMsg += "  " + errorCodeToString(error.code) + ": " + error.message;
                if (!error.suggestion.empty()) {
                    errorMsg += " (Suggested: " + error.suggestion + ")";
                }
                errorMsg += "\n";
            }
            NLM_LOG_ERROR(errorMsg);
            return false;
        }
        
        entries = tempEntries;
        return true;
    }
    
    bool validateAllEntries() const {
        auto errors = ConfigValidator::validate(entries);
        return errors.empty();
    }
    
    std::string getValidationErrors() const {
        auto errors = ConfigValidator::validate(entries);
        if (errors.empty()) {
            return "";
        }
        
        std::string result = "Configuration validation errors:\n";
        for (const auto& error : errors) {
            result += "  " + errorCodeToString(error.code) + ": " + error.parameter;
            if (!error.value.empty()) {
                result += " = \"" + error.value + "\"";
            }
            result += " - " + error.message + "\n";
            if (!error.suggestion.empty()) {
                result += "    Suggestion: " + error.suggestion + "\n";
            }
        }
        return result;
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Configuration file not found: " + filepath);
        return false;
    }
    
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    
    // Validate file format
    auto fileErrors = ConfigValidator::validateFile(filepath);
    if (!fileErrors.empty()) {
        std::string errorMsg = "Configuration file validation failed for " + filepath + ":\n";
        for (const auto& error : fileErrors) {
            errorMsg += "  " + ConfigValidator::errorCodeToString(error.code) + ": " + error.message;
            if (!error.suggestion.empty()) {
                errorMsg += " (Suggested: " + error.suggestion + ")";
            }
            errorMsg += "\n";
        }
        NLM_LOG_ERROR(errorMsg);
        return false;
    }
    
    // Clear existing entries and load new ones
    pImpl->clear();
    
    std::string currentDescription;
    for (size_t i = 0; i < lines.size(); ++i) {
        std::string line = lines[i];
        line = pImpl->trim(line);
        
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            if (line.find("Configuration:") != std::string::npos || line.find("neuron_count") != std::string::npos) {
                currentDescription = line;
            }
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = pImpl->trim(line.substr(0, pos));
            std::string value = pImpl->trim(line.substr(pos + 1));
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            if (!pImpl->validateAndAddEntry(key, ConfigValue(value), ConfigSource::File)) {
                NLM_LOG_ERROR("Failed to parse configuration parameter: " + key + " = " + value);
            }
        }
    }
    
    NLM_LOG_INFO("Configuration loaded successfully from " + filepath);
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                ConfigValue configValue;
                if (!setStringToConfigValue(value, configValue)) {
                    NLM_LOG_ERROR("Invalid value format for argument --" + key + ": " + value);
                    return false;
                }
                
                if (!pImpl->validateAndAddEntry(key, configValue, ConfigSource::CommandLine)) {
                    NLM_LOG_ERROR("Invalid value for argument --" + key + ": " + value);
                    return false;
                }
            }
        }
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            ConfigValue configValue;
            if (!setStringToConfigValue(value, configValue)) {
                NLM_LOG_ERROR("Invalid value format for argument -" + key + ": " + value);
                return false;
            }
            
            if (!pImpl->validateAndAddEntry(key, configValue, ConfigSource::CommandLine)) {
                NLM_LOG_ERROR("Invalid value for argument -" + key + ": " + value);
                return false;
            }
        }
    }
    
    NLM_LOG_INFO("Configuration loaded successfully from command line arguments");
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (!pImpl->validateAndAddEntry(key, value, source)) {
        throw std::runtime_error("Configuration validation failed for parameter: " + key);
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

std::vector<ConfigValidationError> ConfigValidator::validate(const std::vector<ConfigEntry>& entries) {
    std::vector<ConfigValidationError> errors;
    
    // Validate each entry individually
    for (const auto& entry : entries) {
        auto paramErrors = validateParameter(entry.key, entry.value);
        errors.insert(errors.end(), paramErrors.begin(), paramErrors.end());
    }
    
    // Validate parameter dependencies and consistency
    auto depErrors = validateParameterDependencies(entries);
    errors.insert(errors.end(), depErrors.begin(), depErrors.end());
    
    return errors;
}

std::vector<ConfigValidationError> ConfigValidator::validateParameter(const std::string& key, const ConfigValue& value) {
    std::vector<ConfigValidationError> errors;
    
    // Find the range for this parameter
    auto rangeIt = Config::Impl().parameterRanges.find(key);
    if (rangeIt == Config::Impl().parameterRanges.end()) {
        return errors;
    }
    
    auto [min, max] = rangeIt->second;
    
    try {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) {
                double val = static_cast<double>(arg);
                if (val < min || val > max) {
                    ConfigValidationError error;
                    error.code = ConfigValidationError::ErrorCode::InvalidRange;
                    error.parameter = key;
                    error.value = std::to_string(val);
                    error.message = "Value is outside valid range [" + std::to_string(min) + ", " + std::to_string(max) + "]";
                    error.suggestion = "Adjust to a value within the specified range";
                    errors.push_back(error);
                }
            }
            else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                double val = static_cast<double>(arg);
                if (val < min || val > max) {
                    ConfigValidationError error;
                    error.code = ConfigValidationError::ErrorCode::InvalidRange;
                    error.parameter = key;
                    error.value = std::to_string(val);
                    error.message = "Value is outside valid range [" + std::to_string(min) + ", " + std::to_string(max) + "]";
                    error.suggestion = "Adjust to a value within the specified range";
                    errors.push_back(error);
                }
            }
        }, value);
    }
    catch (const std::bad_variant_access&) {
        ConfigValidationError error;
        error.code = ConfigValidationError::ErrorCode::InvalidType;
        error.parameter = key;
        error.value = "";
        error.message = "Invalid type for configuration parameter";
        error.suggestion = "Ensure the value matches the expected type";
        errors.push_back(error);
    }
    
    return errors;
}

std::vector<ConfigValidationError> ConfigValidator::validateParameterDependencies(const std::vector<ConfigEntry>& entries) {
    std::vector<ConfigValidationError> errors;
    
    Config::Impl tempImpl;
    
    for (const auto& entry : entries) {
        auto depIt = tempImpl.parameterDependencies.find(entry.key);
        if (depIt != tempImpl.parameterDependencies.end()) {
            for (const auto& depKey : depIt->second) {
                bool depFound = false;
                ConfigValue depValue;
                
                for (const auto& depEntry : entries) {
                    if (depEntry.key == depKey) {
                        depFound = true;
                        depValue = depEntry.value;
                        break;
                    }
                }
                
                if (!depFound) {
                    ConfigValidationError error;
                    error.code = ConfigValidationError::ErrorCode::MissingParameter;
                    error.parameter = entry.key;
                    error.value = "";
                    error.message = "Required parameter \"" + depKey + "\" is missing";
                    error.suggestion = "Add the required parameter: --" + depKey + "=<value>";
                    errors.push_back(error);
                }
            }
        }
    }
    
    // Validate region count doesn't exceed neuron count
    size_t neuronCount = 0;
    size_t regionCount = 0;
    
    for (const auto& entry : entries) {
        if (entry.key == "neuron_count") {
            try {
                neuronCount = std::get<size_t>(entry.value);
            } catch (...) {
                continue;
            }
        }
        else if (entry.key == "region_count") {
            try {
                regionCount = std::get<size_t>(entry.value);
            } catch (...) {
                continue;
            }
        }
    }
    
    if (neuronCount > 0 && regionCount > 0 && regionCount > neuronCount) {
        ConfigValidationError error;
        error.code = ConfigValidationError::ErrorCode::InvalidRange;
        error.parameter = "region_count";
        error.value = std::to_string(regionCount);
        error.message = "Region count cannot exceed neuron count";
        error.suggestion = "Reduce region count or increase neuron count";
        errors.push_back(error);
    }
    
    return errors;
}

std::vector<ConfigValidationError> ConfigValidator::validateFile(const std::string& filepath) {
    std::vector<ConfigValidationError> errors;
    
    if (!std::filesystem::exists(filepath)) {
        ConfigValidationError error;
        error.code = ConfigValidationError::ErrorCode::FileNotFound;
        error.parameter = filepath;
        error.value = "";
        error.message = "Configuration file not found";
        error.suggestion = "Ensure the file path is correct and the file exists";
        errors.push_back(error);
        return errors;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        ConfigValidationError error;
        error.code = ConfigValidationError::ErrorCode::InvalidFileFormat;
        error.parameter = filepath;
        error.value = "";
        error.message = "Cannot open configuration file";
        error.suggestion = "Check file permissions and ensure the file is readable";
        errors.push_back(error);
        return errors;
    }
    
    std::string line;
    size_t lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        line = Config::Impl().trim(line);
        
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            ConfigValidationError error;
            error.code = ConfigValidationError::ErrorCode::InvalidFileFormat;
            error.parameter = filepath;
            error.value = line;
            error.message = "Invalid configuration line format (expected key=value)";
            error.suggestion = "Fix line " + std::to_string(lineNum) + ": 'key = value' format";
            errors.push_back(error);
        }
    }
    
    return errors;
}

std::string ConfigValidator::errorCodeToString(ConfigValidationError::ErrorCode code) {
    switch (code) {
        case ConfigValidationError::ErrorCode::InvalidRange: return "InvalidRange";
        case ConfigValidationError::ErrorCode::InvalidType: return "InvalidType";
        case ConfigValidationError::ErrorCode::MissingParameter: return "MissingParameter";
        case ConfigValidationError::ErrorCode::InvalidFileFormat: return "InvalidFileFormat";
        case ConfigValidationError::ErrorCode::FileNotFound: return "FileNotFound";
        case ConfigValidationError::ErrorCode::InvalidValue: return "InvalidValue";
        case ConfigValidationError::ErrorCode::ParameterDependency: return "ParameterDependency";
        default: return "Unknown";
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot save configuration to file: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated on " << std::to_string(std::time(nullptr)) << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n\n";
    }
    
    NLM_LOG_INFO("Configuration saved successfully to " + filepath);
    return true;
}

bool Config::has(const std::string& key) const {
    return pImpl->has(key);
}

void Config::remove(const std::string& key) {
    pImpl->remove(key);
}

std::vector<std::string> Config::getKeys() const {
    return pImpl->getKeys();
}

void Config::clear() {
    pImpl->clear();
}

std::string Config::summary() const {
    std::string validationErrors = pImpl->getValidationErrors();
    if (!validationErrors.empty()) {
        return validationErrors;
    }
    
    return pImpl->summary();
}

bool Config::validate() const {
    return pImpl->validateAllEntries();
}

bool Config::loadFromFile(const std::string& filepath, bool validateOnly) {
    if (validateOnly) {
        auto errors = ConfigValidator::validateFile(filepath);
        if (!errors.empty()) {
            std::string errorMsg = "Configuration file validation failed for " + filepath + ":\n";
            for (const auto& error : errors) {
                errorMsg += "  " + ConfigValidator::errorCodeToString(error.code) + ": " + error.message;
                if (!error.suggestion.empty()) {
                    errorMsg += " (Suggested: " + error.suggestion + ")";
                }
                errorMsg += "\n";
            }
            NLM_LOG_ERROR(errorMsg);
            return false;
        }
        return true;
    }
    
    return loadFromFile(filepath);
}

bool Config::loadFromArgs(int argc, char** argv, bool validateOnly) {
    if (validateOnly) {
        std::vector<ConfigEntry> tempEntries;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    
                    ConfigValue configValue;
                    if (!setStringToConfigValue(value, configValue)) {
                        NLM_LOG_ERROR("Invalid value format for argument --" + key + ": " + value);
                        return false;
                    }
                    
                    tempEntries.emplace_back(key, configValue, ConfigSource::CommandLine);
                }
            }
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                
                ConfigValue configValue;
                if (!setStringToConfigValue(value, configValue)) {
                    NLM_LOG_ERROR("Invalid value format for argument -" + key + ": " + value);
                    return false;
                }
                
                tempEntries.emplace_back(key, configValue, ConfigSource::CommandLine);
            }
        }
        
        auto errors = ConfigValidator::validate(tempEntries);
        return errors.empty();
    }
    
    return loadFromArgs(argc, argv);
}

bool Config::setStringToConfigValue(const std::string& str, ConfigValue& value) {
    if (str.empty()) {
        return false;
    }
    
    // Try to parse as boolean
    if (str == "true" || str == "false") {
        value = (str == "true");
        return true;
    }
    
    // Try to parse as integer
    try {
        size_t pos;
        int64_t intVal = std::stoll(str, &pos);
        if (pos == str.size()) {
            value = intVal;
            return true;
        }
    } catch (...) {}
    
    // Try to parse as floating point
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            value = doubleVal;
            return true;
        }
    } catch (...) {}
    
    // Treat as string
    value = str;
    return true;
}

} // namespace nlm