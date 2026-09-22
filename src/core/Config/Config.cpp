// Config.cpp - Config class implementation with advanced features
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <memory>
#include <unordered_set>

// Helper function to convert string to number template
template<typename T>
T stringToNumber(const std::string& str) {
    std::istringstream iss(str);
    T result;
    if (!(iss >> result)) {
        throw std::invalid_argument("Cannot convert string to number: " + str);
    }
    return result;
}

// Template specialization for double
inline double stringToNumber<double>(const std::string& str) {
    return std::stod(str);
}

// Template specialization for float
inline float stringToNumber<float>(const std::string& str) {
    return std::stof(str);
}

// Check if a string can be parsed as boolean
inline bool stringToBool(const std::string& str) {
    std::string lower = toLower(str);
    if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
        return true;
    } else if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
        return false;
    }
    throw std::invalid_argument("Cannot convert string to boolean: " + str);
}

// Check if string represents a numeric value
inline bool isNumeric(const std::string& str) {
    if (str.empty()) return false;
    char* end;
    std::strtod(str.c_str(), &end);
    return *end == '\0';
}

// Check if string represents an integer
inline bool isInteger(const std::string& str) {
    if (str.empty()) return false;
    char* end;
    std::strtol(str.c_str(), &end, 10);
    return *end == '\0';
}

// Check if string represents a float
inline bool isFloat(const std::string& str) {
    if (str.empty()) return false;
    char* end;
    std::strtod(str.c_str(), &end);
    return *end == '\0' && (std::strchr(str.c_str(), '.') || std::strchr(str.c_str(), 'e') || std::strchr(str.c_str(), 'E'));
}

// Parse string to appropriate ConfigValue type
inline ConfigValue parseStringToConfigValue(const std::string& str) {
    if (str.empty()) {
        return std::string("");
    }
    
    // Try boolean first (common case)
    try {
        return stringToBool(str);
    } catch (...) {}
    
    // Try numeric types
    if (isNumeric(str)) {
        if (isInteger(str)) {
            try {
                return stringToNumber<int64_t>(str);
            } catch (...) {
                try {
                    return stringToNumber<int>(str);
                } catch (...) {}
            }
        }
        if (isFloat(str)) {
            try {
                return stringToNumber<double>(str);
            } catch (...) {
                try {
                    return stringToNumber<float>(str);
                } catch (...) {}
            }
        }
    }
    
    // Default to string
    return str;
}

// Enhanced file loading with better error handling and format detection
bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    bool hasErrors = false;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Try to parse as key=value pair
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
            
            try {
                ConfigValue parsedValue = parseStringToConfigValue(value);
                set(key, parsedValue, ConfigSource::File);
            } catch (const std::exception& e) {
                // Log error but continue with other entries
                hasErrors = true;
                // Could log to a debug logger here
            }
        } else {
            // Line doesn't have =, could be malformed
            hasErrors = true;
        }
    }
    
    return !hasErrors;  // Return false if there were parsing errors
}

// Enhanced command line argument parsing with flags and better error handling
bool Config::loadFromArgs(int argc, char** argv) {
    bool hasErrors = false;
    std::vector<std::string> unknownFlags;
    
    // Define known flags for validation
    std::unordered_set<std::string> knownFlags = {
        "help", "verbose", "quiet", "debug", "loglevel",
        "neuron_count", "region_count", "connection_probability",
        "random_seed", "simulation_timestep", "checkpoint_dir",
        "replay_interval", "consolidation_interval", "development_enabled"
    };
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key format
                if (key.empty() || !isalpha(key[0]) && key[0] != '_') {
                    hasErrors = true;
                    continue;
                }
                
                try {
                    ConfigValue parsedValue = parseStringToConfigValue(value);
                    set(key, parsedValue, ConfigSource::CommandLine);
                } catch (const std::exception& e) {
                    hasErrors = true;
                }
            } else {
                hasErrors = true;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            
            // Validate key format
            if (key.empty() || !isalpha(key[0]) && key[0] != '_') {
                hasErrors = true;
                continue;
            }
            
            std::string value = argv[++i];
            
            try {
                ConfigValue parsedValue = parseStringToConfigValue(value);
                set(key, parsedValue, ConfigSource::CommandLine);
            } catch (const std::exception& e) {
                hasErrors = true;
            }
        }
        // Handle boolean flags (--flag)
        else if (arg[0] == '-' && arg.size() > 1 && arg[1] != '-') {
            // Short flag like -v
            std::string key = arg.substr(1);
            
            // Check if it's a known flag or if we want to treat it as boolean
            if (knownFlags.find(key) != knownFlags.end() || 
                key == "verbose" || key == "debug" || key == "help") {
                set(key, true, ConfigSource::CommandLine);
            } else {
                hasErrors = true;
            }
        }
        // Handle unknown flags
        else if (arg.substr(0, 2) == "--") {
            std::string key = arg.substr(2);
            if (knownFlags.find(key) == knownFlags.end()) {
                unknownFlags.push_back(key);
                hasErrors = true;
            } else {
                hasErrors = true;
            }
        } else {
            hasErrors = true;
        }
    }
    
    if (!unknownFlags.empty()) {
        // Log warning about unknown flags
        for (const auto& flag : unknownFlags) {
            // Could log to a debug logger here
        }
    }
    
    return !hasErrors;
}

// Advanced file loading with format detection and error recovery
bool Config::loadFromAdvanced(const std::string& filepath) {
    // Detect file format by extension and content
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool success = false;
    
    if (ext == "json") {
        success = loadFromJson(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        success = loadFromYaml(filepath);
    } else {
        // Try simple key=value first
        success = loadFromFile(filepath);
        if (!success) {
            // If that fails, try as JSON
            success = loadFromJson(filepath);
        }
        if (!success) {
            // Try as YAML as last resort
            success = loadFromYaml(filepath);
        }
    }
    
    return success;
}

// Enhanced value setting with type validation and constraints
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check if validation rule exists for this key
    bool isValid = true;
    std::string validationError;
    
    auto ruleIt = pImpl->validationRules.find(key);
    if (ruleIt != pImpl->validationRules.end()) {
        isValid = ruleIt->second.validate(value);
        if (!isValid) {
            validationError = "Validation failed for key: " + key + ". " + ruleIt->second.errorMessage;
            // Log validation error
        }
    }
    
    // Type-specific validation
    if (isValid) {
        isValid = validateType(value);
        if (!isValid) {
            validationError = "Type validation failed for key: " + key;
            // Log validation error
        }
    }
    
    if (!isValid) {
        // Could throw exception or log error
        return;
    }
    
    // Update both data structures
    auto it = pImpl->entriesMap.find(key);
    if (it != pImpl->entriesMap.end()) {
        it->second.value = value;
        it->second.source = source;
    } else {
        pImpl->entries.push_back(ConfigEntry(key, value, source));
        pImpl->entriesMap[key] = pImpl->entries.back();
    }
}

// Type validation for specific values
bool Config::validateType(const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) {
        int val = std::get<int>(value);
        if (val < std::numeric_limits<int>::min() || val > std::numeric_limits<int>::max()) {
            return false;
        }
    } else if (std::holds_alternative<int64_t>(value)) {
        int64_t val = std::get<int64_t>(value);
        if (val < std::numeric_limits<int64_t>::min() || val > std::numeric_limits<int64_t>::max()) {
            return false;
        }
    } else if (std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        if (std::isnan(val) || std::isinf(val)) {
            return false;
        }
    } else if (std::holds_alternative<float>(value)) {
        float val = std::get<float>(value);
        if (std::isnan(val) || std::isinf(val)) {
            return false;
        }
    }
    
    return true;
}

// Helper function to recursively convert JSON to Config entries
void Config::convertJsonToConfig(const nlohmann::json& json, const std::string& prefix, const std::string& path) {
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = prefix.empty() ? it.key() : prefix + "." + it.key();
        std::string fullPath = path.empty() ? key : path + "." + key;
        
        if (it->is_object()) {
            convertJsonToConfig(*it, key, fullPath);
        } else if (it->is_array()) {
            // Convert array to vector<string> for now (can be improved)
            std::vector<std::string> vec;
            for (const auto& item : *it) {
                vec.push_back(item.dump());
            }
            // Check if we can preserve original type
            if (vec.empty()) {
                set(key, std::vector<int>(), ConfigSource::File);
            } else {
                // Try to parse as numbers
                bool allNumbers = true;
                std::vector<double> numVec;
                for (const auto& item : vec) {
                    if (isNumeric(item)) {
                        numVec.push_back(std::stod(item));
                    } else {
                        allNumbers = false;
                        break;
                    }
                }
                
                if (allNumbers && !numVec.empty()) {
                    set(key, numVec, ConfigSource::File);
                } else {
                    set(key, vec, ConfigSource::File);
                }
            }
        } else {
            // Convert JSON value to ConfigValue
            ConfigValue configValue;
            if (it->is_string()) {
                configValue = it->get<std::string>();
            } else if (it->is_number_integer()) {
                configValue = it->get<int64_t>();
            } else if (it->is_number_float()) {
                configValue = it->get<double>();
            } else if (it->is_boolean()) {
                configValue = it->get<bool>();
            } else if (it->is_null()) {
                configValue = std::string("");
            }
            
            set(key, configValue, ConfigSource::File);
        }
    }
}

// Helper function to recursively convert YAML to Config entries
void Config::convertYamlToConfig(const std::string& yamlStr, const std::string& prefix, const std::string& path) {
    // Parse YAML string (simplified - in real implementation would use yaml-cpp)
    // For now, parse basic key=value pairs from YAML-like format
    std::istringstream iss(yamlStr);
    std::string line;
    
    while (std::getline(iss, line)) {
        line = trim(line);
        
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#' || line[0] == '%') {
            continue;
        }
        
        // Try to parse as key=value or nested structure
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            
            // Remove quotes
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Parse value based on type
            ConfigValue configValue;
            if (isNumeric(value)) {
                if (isInteger(value)) {
                    try {
                        configValue = stringToNumber<int64_t>(value);
                    } catch (...) {
                        configValue = value;
                    }
                } else if (isFloat(value)) {
                    try {
                        configValue = stringToNumber<double>(value);
                    } catch (...) {
                        configValue = value;
                    }
                } else {
                    configValue = value;
                }
            } else if (value == "true" || value == "false") {
                configValue = (value == "true");
            } else {
                configValue = value;
            }
            
            std::string newKey = prefix.empty() ? key : prefix + "." + key;
            set(newKey, configValue, ConfigSource::File);
        }
    }
}

// Helper function to save Config to JSON format
bool Config::saveToJson(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        // Convert ConfigValue to JSON
        nlohmann::json jsonValue;
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, int> || 
                                std::is_same_v<T, int64_t>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, double> || 
                                std::is_same_v<T, float>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, std::vector<float>>) {
                // Convert float vector to double vector for JSON
                std::vector<double> doubleVec;
                doubleVec.reserve(arg.size());
                for (float f : arg) {
                    doubleVec.push_back(static_cast<double>(f));
                }
                jsonValue = doubleVec;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                jsonValue = arg;
            }
        }, entry.value);
        
        // Add to JSON object
        jsonData[entry.key] = jsonValue;
    }
    
    // Write JSON with pretty formatting
    file << jsonData.dump(2);
    
    return true;
}

// Helper function to save Config to YAML format
bool Config::saveToYaml(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write YAML header
    file << "# NLM Configuration\n";
    file << "# Generated automatically by NLM Config system\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << ": ";
        
        // Format value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, int> || 
                                std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double> || 
                                std::is_same_v<T, float>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<float>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

// Enhanced JSON loading implementation
bool Config::loadFromJson(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        pImpl->entries.clear();
        pImpl->entriesMap.clear();
        
        // Convert JSON to config entries
        convertJsonToConfig(jsonData, "", "");
        
        return true;
    } catch (const std::exception& e) {
        // Log error
        return false;
    }
}

// Enhanced YAML loading implementation
bool Config::loadFromYaml(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        std::string yamlStr((std::istreambuf_iterator<char>(file)), 
                           std::istreambuf_iterator<char>());
        
        // Clear existing entries
        pImpl->entries.clear();
        pImpl->entriesMap.clear();
        
        // Convert YAML to config entries
        convertYamlToConfig(yamlStr, "", "");
        
        return true;
    } catch (const std::exception& e) {
        // Log error
        return false;
    }
}

// Set convenience methods
void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, float value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

// Save convenience methods
bool Config::saveToFile(const std::string& filepath) const {
    // Detect format by extension
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        return saveToJson(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return saveToYaml(filepath);
    } else {
        // Default to simple key=value format
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, int> || 
                                    std::is_same_v<T, int64_t>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, double> || 
                                    std::is_same_v<T, float>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                }
            }, entry.value);
            
            file << "\n\n";
        }
        
        return true;
    }
}

// Get convenience methods with validation
std::optional<int> Config::get<int>(const std::string& key) const {
    return get<int>(key);
}

std::optional<int64_t> Config::get<int64_t>(const std::string& key) const {
    return get<int64_t>(key);
}

std::optional<double> Config::get<double>(const std::string& key) const {
    return get<double>(key);
}

std::optional<float> Config::get<float>(const std::string& key) const {
    return get<float>(key);
}

std::optional<bool> Config::get<bool>(const std::string& key) const {
    return get<bool>(key);
}

std::optional<std::string> Config::get<std::string>(const std::string& key) const {
    return get<std::string>(key);
}

// GetOr convenience methods
int Config::getOr<int>(const std::string& key, const int& defaultValue) const {
    return getOr<int>(key, defaultValue);
}

int64_t Config::getOr<int64_t>(const std::string& key, const int64_t& defaultValue) const {
    return getOr<int64_t>(key, defaultValue);
}

double Config::getOr<double>(const std::string& key, const double& defaultValue) const {
    return getOr<double>(key, defaultValue);
}

float Config::getOr<float>(const std::string& key, const float& defaultValue) const {
    return getOr<float>(key, defaultValue);
}

bool Config::getOr<bool>(const std::string& key, const bool& defaultValue) const {
    return getOr<bool>(key, defaultValue);
}

std::string Config::getOr<std::string>(const std::string& key, const std::string& defaultValue) const {
    return getOr<std::string>(key, defaultValue);
}

// Additional helper methods
void Config::mergeFrom(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        auto it = pImpl->entriesMap.find(entry.key);
        if (it != pImpl->entriesMap.end()) {
            it->second.value = entry.value;
            it->second.source = source;
        } else {
            pImpl->entries.push_back(entry);
            pImpl->entriesMap[entry.key] = pImpl->entries.back();
        }
    }
}

void Config::inheritFrom(const Config& parent, const std::string& prefix) {
    std::string newPrefix = prefix.empty() ? "" : prefix + ".";
    
    for (const auto& entry : parent.pImpl->entries) {
        std::string newKey = newPrefix + entry.key;
        
        if (!has(newKey)) {
            set(newKey, entry.value, ConfigSource::Runtime);
        }
    }
}

size_t Config::getEntryCount() const {
    return pImpl->entriesMap.size();
}

std::vector<ConfigEntry> Config::getEntriesBySource(ConfigSource source) const {
    std::vector<ConfigEntry> entries;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            entries.push_back(entry);
        }
    }
    return entries;
}