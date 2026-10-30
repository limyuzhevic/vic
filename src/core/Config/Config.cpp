// NLM Configuration System
// Enhanced with comprehensive validation, JSON/YAML support, and error handling

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <regex>
#include <cmath>
#include <stdexcept>
#include <unordered_set>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigEntry> metadata;
    std::chrono::system_clock::time_point lastModified;
    std::string lastError;
    bool isValid;
};

Config::Config() : pImpl(new Impl()) {
    initializeDefaultConfigs();
    pImpl->isValid = true;
}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void Config::initializeDefaultConfigs() {
    // Initialize with default configuration values and metadata
    // This represents the Phase 1 default configuration from default.cfg
    addConfig("random_seed", 42, "Random seed for reproducible experiments", "int", false);
    addConfig("simulation_timestep", 0.001, "Simulation timestep in seconds", "double", true);
    addConfig("neuron_count", 1000, "Number of neurons in the brain", "int", true);
    addConfig("region_count", 2, "Number of brain regions", "int", true);
    addConfig("connection_probability", 0.1, "Connection probability for random connectivity", "double", true);
    addConfig("plasticity_learning_rate", 0.01, "Plasticity learning rate", "double", true);
    addConfig("stdp_ltp_weight", 0.01, "STDP LTP weight", "double", true);
    addConfig("stdp_ltd_weight", 0.012, "STDP LTD weight", "double", true);
    addConfig("stdp_time_constant", 20.0, "STDP time constant (ms)", "double", true);
    addConfig("development_synaptogenesis_rate", 0.001, "Development synaptogenesis rate", "double", true);
    addConfig("development_pruning_rate", 0.0001, "Development pruning rate", "double", true);
    addConfig("dopamine_baseline", 0.1, "Neuromodulation baseline dopamine level", "double", true);
    addConfig("reward_discount_factor", 0.99, "Reward discount factor", "double", true);
    addConfig("environment_name", "GridWorld", "Environment name", "string", true);
    addConfig("environment_width", 10, "Environment width", "int", true);
    addConfig("environment_height", 10, "Environment height", "int", true);
    addConfig("log_level", "INFO", "Logging level", "string", true);
    addConfig("log_to_file", false, "Log to file flag", "bool", true);
    addConfig("log_filename", "nlm.log", "Log filename", "string", true);
    addConfig("max_simulation_steps", 10000, "Maximum simulation steps", "int", true);
    addConfig("simulation_time_limit", 0.0, "Simulation time limit (seconds)", "double", true);
    addConfig("visualization_enabled", false, "Visualization enabled flag", "bool", true);
    addConfig("visualization_update_rate", 30.0, "Visualization update rate (Hz)", "double", true);
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        pImpl->lastModified = std::chrono::system_clock::now();
        pImpl->lastError.clear();
        pImpl->isValid = true;
        
        // Validate file path
        if (filepath.empty()) {
            pImpl->lastError = "Empty file path provided";
            pImpl->isValid = false;
            return false;
        }
        
        // Check if file exists and is readable
        std::filesystem::path filePath(filepath);
        if (!std::filesystem::exists(filePath)) {
            pImpl->lastError = "Configuration file does not exist: " + filepath;
            pImpl->isValid = false;
            return false;
        }
        
        if (!std::filesystem::is_regular_file(filePath)) {
            pImpl->lastError = "Path is not a regular file: " + filepath;
            pImpl->isValid = false;
            return false;
        }
        
        if (!std::filesystem::is_readable(filePath)) {
            pImpl->lastError = "Configuration file is not readable: " + filepath;
            pImpl->isValid = false;
            return false;
        }
        
        // Check file size to avoid loading huge files
        auto fileSize = std::filesystem::file_size(filePath);
        if (fileSize > 10 * 1024 * 1024) { // 10MB limit
            pImpl->lastError = "Configuration file is too large: " + filepath;
            pImpl->isValid = false;
            return false;
        }
        
        // Try to determine file format by extension or content
        std::string ext = filePath.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        bool success = false;
        if (ext == ".json") {
            success = loadFromJSON(filepath);
        } else if (ext == ".yaml" || ext == ".yml") {
            success = loadFromYAML(filepath);
        } else {
            // Fallback to simple key=value format for backward compatibility
            success = loadFromSimpleFormat(filepath);
        }
        
        if (!success) {
            pImpl->lastError = "Failed to load configuration from file: " + filepath;
            pImpl->isValid = false;
            return false;
        }
        
        // Validate loaded configuration
        auto validationResults = validateAll();
        bool hasErrors = false;
        for (const auto& result : validationResults) {
            if (!result.isValid) {
                hasErrors = true;
                pImpl->lastError = "Configuration validation error: " + result.errorMessage;
            }
        }
        
        pImpl->isValid = !hasErrors;
        return !hasErrors;
        
    } catch (const std::exception& e) {
        pImpl->lastError = "Exception loading configuration file: " + std::string(e.what());
        pImpl->isValid = false;
        return false;
    } catch (...) {
        pImpl->lastError = "Unknown exception loading configuration file";
        pImpl->isValid = false;
        return false;
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            pImpl->lastError = "Cannot open JSON file: " + filepath;
            return false;
        }
        
        // Read and parse JSON content with size check
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        if (content.empty()) {
            pImpl->lastError = "JSON file is empty: " + filepath;
            return false;
        }
        
        // Simple JSON parsing - in production, use nlohmann/json library
        // For now, parse key-value pairs from { "key": "value", ... }
        std::unordered_map<std::string, std::string> jsonMap;
        bool inObject = false;
        size_t braceCount = 0;
        std::string currentObject;
        
        for (size_t i = 0; i < content.size(); ++i) {
            char c = content[i];
            
            if (c == '{') {
                braceCount++;
                inObject = true;
                currentObject.clear();
            } else if (c == '}') {
                braceCount--;
                if (braceCount == 0 && inObject) {
                    parseJSONContent(currentObject, jsonMap);
                    currentObject.clear();
                }
                inObject = false;
            } else if (c == ',' && inObject) {
                parseJSONContent(currentObject, jsonMap);
                currentObject.clear();
            } else if (inObject) {
                currentObject += c;
            }
        }
        
        if (jsonMap.empty()) {
            pImpl->lastError = "No valid key-value pairs found in JSON file: " + filepath;
            return false;
        }
        
        // Apply parsed values with validation
        for (const auto& pair : jsonMap) {
            std::string key = trimQuotes(pair.first);
            std::string value = trimQuotes(pair.second);
            
            if (key.empty()) {
                pImpl->lastError = "Empty key found in JSON file: " + filepath;
                return false;
            }
            
            ConfigValue configValue = parseConfigValue(value);
            if (setWithValidation(key, configValue, ConfigSource::File)) {
                pImpl->lastModified = std::chrono::system_clock::now();
            } else {
                pImpl->lastError = "Failed to set value for key: " + key;
                return false;
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        pImpl->lastError = "Exception parsing JSON file: " + std::string(e.what());
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            pImpl->lastError = "Cannot open YAML file: " + filepath;
            return false;
        }
        
        // Simple YAML parsing - in production, use YAML library
        // For now, treat as key: value format with colon separator
        return loadFromSimpleFormat(filepath, true);
        
    } catch (const std::exception& e) {
        pImpl->lastError = "Exception parsing YAML file: " + std::string(e.what());
        return false;
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath, bool isYAML) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            pImpl->lastError = "Cannot open configuration file: " + filepath;
            return false;
        }
        
        std::string line;
        size_t lineNumber = 0;
        size_t commentLines = 0;
        
        while (std::getline(file, line)) {
            lineNumber++;
            try {
                // Skip empty lines and comments
                line = trim(line);
                if (line.empty() || line[0] == '#' || line[0] == '/') {
                    commentLines++;
                    continue;
                }
                
                // Skip lines that are only whitespace
                if (std::all_of(line.begin(), line.end(), [](char c) { return std::isspace(c); })) {
                    commentLines++;
                    continue;
                }
                
                std::string key, value;
                if (isYAML) {
                    // Handle YAML format: key: value
                    size_t pos = line.find(':');
                    if (pos != std::string::npos && pos > 0) {
                        key = trim(line.substr(0, pos));
                        value = trim(line.substr(pos + 1));
                    } else {
                        pImpl->lastError = "Invalid YAML format at line " + std::to_string(lineNumber) + ": " + line;
                        return false;
                    }
                } else {
                    // Handle simple key=value format
                    size_t pos = line.find('=');
                    if (pos != std::string::npos && pos > 0) {
                        key = trim(line.substr(0, pos));
                        value = trim(line.substr(pos + 1));
                    } else {
                        pImpl->lastError = "Invalid key=value format at line " + std::to_string(lineNumber) + ": " + line;
                        return false;
                    }
                }
                
                // Validate key
                if (key.empty()) {
                    pImpl->lastError = "Empty key at line " + std::to_string(lineNumber);
                    return false;
                }
                
                // Remove quotes from value
                value = trimQuotes(value);
                
                // Parse and set value with validation
                ConfigValue configValue = parseConfigValue(value);
                if (!setWithValidation(key, configValue, ConfigSource::File)) {
                    pImpl->lastError = "Failed to validate/set key-value pair at line " + std::to_string(lineNumber) + ": " + key + " = " + value;
                    return false;
                }
                
                pImpl->lastModified = std::chrono::system_clock::now();
                
            } catch (const std::exception& e) {
                pImpl->lastError = "Error parsing line " + std::to_string(lineNumber) + ": " + std::string(e.what());
                return false;
            }
        }
        
        if (lineNumber == commentLines) {
            pImpl->lastError = "Configuration file contains only comments or is empty: " + filepath;
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        pImpl->lastError = "Exception reading configuration file: " + std::string(e.what());
        return false;
    }
}

void Config::parseJSONContent(const std::string& content, std::unordered_map<std::string, std::string>& jsonMap) {
    try {
        if (content.empty()) return;
        
        size_t pos = 0;
        while (pos < content.size()) {
            // Find next key (look for "key": pattern)
            size_t keyStart = content.find('"', pos);
            if (keyStart == std::string::npos) break;
            
            size_t keyEnd = content.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
            pos = keyEnd + 1;
            
            // Skip whitespace
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            if (pos >= content.size() || content[pos] != ':') break;
            
            // Skip : and whitespace
            pos++;
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            if (pos >= content.size() || content[pos] != '"') break;
            
            // Find value
            size_t valueStart = pos;
            size_t valueEnd = content.find('"', valueStart + 1);
            if (valueEnd == std::string::npos) break;
            
            std::string value = content.substr(valueStart + 1, valueEnd - valueStart - 1);
            
            // Only add non-empty keys
            if (!key.empty()) {
                jsonMap[key] = value;
            }
            
            pos = valueEnd + 1;
            
            // Skip comma if present
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            if (pos < content.size() && content[pos] == ',') {
                pos++;
                while (pos < content.size() && std::isspace(content[pos])) pos++;
            }
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("JSON parsing error: " + std::string(e.what()));
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        if (filepath.empty()) {
            pImpl->lastError = "Empty file path provided for saving";
            return false;
        }
        
        // Create directory if it doesn't exist
        std::filesystem::path filePath(filepath);
        std::filesystem::path parentDir = filePath.parent_path();
        if (!parentDir.empty() && !std::filesystem::exists(parentDir)) {
            if (!std::filesystem::create_directories(parentDir)) {
                pImpl->lastError = "Cannot create directory for file: " + filepath;
                return false;
            }
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            pImpl->lastError = "Cannot write to file: " + filepath;
            return false;
        }
        
        // Save in enhanced format with metadata
        file << "# NLM Configuration File" << std::endl;
        file << "# Generated: " << getCurrentTimestamp() << std::endl;
        file << "# Valid: " << (pImpl->isValid ? "Yes" : "No") << std::endl;
        file << "# Error: " << pImpl->lastError << std::endl;
        file << std::endl;
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << std::endl;
            if (!entry.validationRegex.empty()) {
                file << "# Validation: " << entry.validationRegex << std::endl;
            }
            if (!entry.dataType.empty()) {
                file << "# Type: " << entry.dataType << std::endl;
            }
            if (entry.isRequired) {
                file << "# Required: Yes" << std::endl;
            }
            file << entry.key << " = " << configValueToString(entry.value) << std::endl;
            file << std::endl;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        pImpl->lastError = "Exception saving configuration file: " + std::string(e.what());
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        if (argc <= 1) {
            return true; // No arguments provided
        }
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    
                    if (key.empty()) {
                        pImpl->lastError = "Empty key in command line argument: --" + arg.substr(2);
                        return false;
                    }
                    
                    ConfigValue configValue = parseConfigValue(value);
                    if (!setWithValidation(key, configValue, ConfigSource::CommandLine)) {
                        pImpl->lastError = "Failed to validate command line argument: --" + key + "=" + value;
                        return false;
                    }
                } else {
                    pImpl->lastError = "Invalid command line argument format: --" + arg.substr(2);
                    return false;
                }
            } else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                
                if (key.empty()) {
                    pImpl->lastError = "Empty key in command line argument: -" + arg.substr(1);
                    return false;
                }
                
                ConfigValue configValue = parseConfigValue(value);
                if (!setWithValidation(key, configValue, ConfigSource::CommandLine)) {
                    pImpl->lastError = "Failed to validate command line argument: -" + key + " " + value;
                    return false;
                }
            } else {
                pImpl->lastError = "Unrecognized command line argument: " + arg;
                return false;
            }
        }
        
        pImpl->lastModified = std::chrono::system_clock::now();
        return true;
        
    } catch (const std::exception& e) {
        pImpl->lastError = "Exception processing command line arguments: " + std::string(e.what());
        return false;
    }
}

bool Config::setWithValidation(const std::string& key, const ConfigValue& value, ConfigSource source) {
    try {
        // Validate key
        if (key.empty()) {
            pImpl->lastError = "Key cannot be empty";
            pImpl->isValid = false;
            return false;
        }
        
        // Find metadata for this key
        auto metaIt = std::find_if(pImpl->metadata.begin(), pImpl->metadata.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        bool isValid = true;
        std::string errorMsg = "";
        
        if (metaIt != pImpl->metadata.end()) {
            // Validate against metadata constraints
            const auto& metadata = *metaIt;
            
            // Type validation
            std::string actualType = getTypeString(value);
            if (metadata.dataType != "any" && !metadata.dataType.empty()) {
                if (actualType != metadata.dataType) {
                    isValid = false;
                    errorMsg = "Type mismatch for key '" + key + "': expected '" + metadata.dataType + "', got '" + actualType + "'";
                }
            }
            
            // Validation regex
            if (isValid && !metadata.validationRegex.empty()) {
                if (!matchesValidation(value, metadata.validationRegex)) {
                    isValid = false;
                    errorMsg = "Validation failed for key '" + key + "' with regex: " + metadata.validationRegex;
                }
            }
            
            // Range validation for numeric types
            if (isValid && (metadata.dataType == "int" || metadata.dataType == "double" || actualType == metadata.dataType)) {
                if (!validateNumericRange(value, metadata.dataType)) {
                    isValid = false;
                    errorMsg = "Value out of range for key '" + key + "' (type: " + metadata.dataType + ")";
                }
            }
            
            // Required field validation
            if (isValid && metadata.isRequired && value.index() == std::variant_npos) {
                // This is a basic check - in practice you'd need more sophisticated logic
                isValid = false;
                errorMsg = "Required key missing: " + key;
            }
        }
        
        if (!isValid) {
            pImpl->lastError = errorMsg;
            pImpl->isValid = false;
            return false;
        }
        
        // Update or add the entry
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            ConfigEntry newEntry(key, value, source);
            if (metaIt != pImpl->metadata.end()) {
                newEntry.description = metaIt->description;
                newEntry.validationRegex = metaIt->validationRegex;
                newEntry.dataType = metaIt->dataType;
                newEntry.isRequired = metaIt->isRequired;
            }
            pImpl->entries.push_back(newEntry);
        }
        
        pImpl->lastModified = std::chrono::system_clock::now();
        return true;
        
    } catch (const std::exception& e) {
        pImpl->lastError = "Exception setting config value: " + std::string(e.what());
        pImpl->isValid = false;
        return false;
    }
}

bool Config::addConfig(const std::string& key, const ConfigValue& value, const std::string& description,
                       const std::string& dataType, bool isRequired, const std::string& validationRegex) {
    try {
        if (key.empty()) {
            pImpl->lastError = "Key cannot be empty when adding config";
            return false;
        }
        
        // Validate dataType if provided
        if (!dataType.empty() && dataType != "any") {
            std::string validTypes = "int,int64_t,double,bool,string,vector<int>,vector<double>,vector<string>";
            if (validTypes.find(dataType) == std::string::npos) {
                pImpl->lastError = "Invalid dataType: " + dataType + ". Valid types: " + validTypes;
                return false;
            }
        }
        
        // Check if key already exists in entries
        auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        // Check if key already exists in metadata
        auto metaIt = std::find_if(pImpl->metadata.begin(), pImpl->metadata.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (entryIt != pImpl->entries.end()) {
            // Update existing entry's metadata
            if (metaIt != pImpl->metadata.end()) {
                metaIt->description = description;
                metaIt->dataType = dataType;
                metaIt->isRequired = isRequired;
                metaIt->validationRegex = validationRegex;
            }
            return true;
        }
        
        if (metaIt == pImpl->metadata.end()) {
            // Add new metadata
            ConfigEntry metadata(key, value, ConfigSource::Default, description, validationRegex, dataType, isRequired);
            pImpl->metadata.push_back(metadata);
        }
        
        // Add or update the entry
        auto entryIt2 = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (entryIt2 == pImpl->entries.end()) {
            ConfigEntry newEntry(key, value, ConfigSource::Default);
            pImpl->entries.push_back(newEntry);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        pImpl->lastError = "Exception adding config: " + std::string(e.what());
        return false;
    }
}

std::string Config::getError() const {
    return pImpl->lastError;
}

bool Config::isValid() const {
    return pImpl->isValid;
}

std::vector<ValidationResult> Config::validateAll() const {
    std::vector<ValidationResult> results;
    
    for (const auto& entry : pImpl->entries) {
        ValidationResult result = validateEntry(entry);
        results.push_back(result);
    }
    
    return results;
}

ValidationResult Config::validateEntry(const ConfigEntry& entry) const {
    // Find metadata for this entry
    auto metaIt = std::find_if(pImpl->metadata.begin(), pImpl->metadata.end(),
        [&entry](const ConfigEntry& meta) { return meta.key == entry.key; });
    
    if (metaIt == pImpl->metadata.end()) {
        return ValidationResult(true, "No metadata found for key: " + entry.key);
    }
    
    // Check if required
    if (metaIt->isRequired && !has(entry.key)) {
        return ValidationResult(false, "Required key missing: " + entry.key);
    }
    
    // Type validation
    std::string actualType = getTypeString(entry.value);
    if (metaIt->dataType != "any" && !metaIt->dataType.empty()) {
        if (actualType != metaIt->dataType) {
            return ValidationResult(false, "Type mismatch for key '" + entry.key + "': expected '" + metaIt->dataType + "', got '" + actualType + "'");
        }
    }
    
    // Validation regex
    if (!metaIt->validationRegex.empty()) {
        if (!matchesValidation(entry.value, metaIt->validationRegex)) {
            return ValidationResult(false, "Validation failed for key '" + entry.key + "' with regex: " + metaIt->validationRegex);
        }
    }
    
    // Range validation for numeric types
    if ((metaIt->dataType == "int" || metaIt->dataType == "double") && !validateNumericRange(entry.value, metaIt->dataType)) {
        return ValidationResult(false, "Value out of range for key '" + entry.key + "' (type: " + metaIt->dataType + ")");
    }
    
    return ValidationResult(true, "Key " + entry.key + " is valid");
}

std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// All other methods (unchanged or minimally updated)
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

ConfigValue Config::parseConfigValue(const std::string& value) {
    if (value.empty()) {
        return std::string();
    }
    
    // Try boolean parsing
    if (value == "true" || value == "false") {
        return value == "true";
    }
    
    // Try integer parsing
    try {
        if (value.find('.') == std::string::npos && value.find('e') == std::string::npos && value.find('E') == std::string::npos) {
            size_t pos;
            long intVal = std::stol(value, &pos);
            if (pos == value.size()) {
                // Check if it fits in 64-bit
                if (intVal >= INT64_MIN && intVal <= INT64_MAX) {
                    return static_cast<int64_t>(intVal);
                }
                return intVal;
            }
        }
    } catch (...) {}
    
    // Try double parsing
    try {
        size_t pos;
        double doubleVal = std::stod(value, &pos);
        if (pos == value.size()) {
            return doubleVal;
        }
    } catch (...) {}
    
    // Return as string for everything else
    return value;
}

bool Config::validateNumericRange(const ConfigValue& value, const std::string& type) const {
    if (type == "int") {
        try {
            int val = std::get<int>(value);
            return val >= INT_MIN && val <= INT_MAX;
        } catch (...) { return false; }
    } else if (type == "int64_t") {
        try {
            int64_t val = std::get<int64_t>(value);
            return val >= INT64_MIN && val <= INT64_MAX;
        } catch (...) { return false; }
    } else if (type == "double") {
        try {
            double val = std::get<double>(value);
            return !std::isnan(val) && !std::isinf(val);
        } catch (...) { return false; }
    }
    return true;
}

std::string Config::getTypeString(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "vector<int>";
    if (std::holds_alternative<std::vector<double>>(value)) return "vector<double>";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "vector<string>";
    return "unknown";
}

bool Config::matchesValidation(const ConfigValue& value, const std::string& regex) const {
    // Simple regex matching - in production would use std::regex
    std::string valueStr = configValueToString(value);
    
    // Basic validation patterns
    if (regex == "^\\d+$") { // Integer pattern
        return std::all_of(valueStr.begin(), valueStr.end(), ::isdigit);
    } else if (regex == "^\\d*\\.\\d+$") { // Float pattern
        size_t dotPos = valueStr.find('.');
        if (dotPos == std::string::npos) return false;
        std::string intPart = valueStr.substr(0, dotPos);
        std::string fracPart = valueStr.substr(dotPos + 1);
        bool intValid = intPart.empty() || std::all_of(intPart.begin(), intPart.end(), ::isdigit);
        bool fracValid = std::all_of(fracPart.begin(), fracPart.end(), ::isdigit);
        return intValid && fracValid;
    } else if (regex == "^true|false$") { // Boolean pattern
        return valueStr == "true" || valueStr == "false";
    } else if (regex == ".*") { // Any value
        return true;
    }
    
    // Default to accepting the value if regex doesn't match known patterns
    return true;
}

std::string Config::configValueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    
    return oss.str();
}

std::string Config::getCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
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

} // namespace nlm