#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <iostream>
#include <unordered_map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Configuration validation rules
    struct ValidationRule {
        std::string keyPattern;
        std::string valueType;
        bool required;
        std::string description;
        std::string errorMessage;
    };
    
    std::vector<ValidationRule> validationRules;
    
    // Validation errors
    std::vector<std::string> validationErrors;
    
    // Index for O(1) key lookup (key -> index in entries vector)
    std::unordered_map<std::string, size_t> keyIndex;
};

Config::Config() : pImpl(new Impl) {}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

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
        
        size_t pos = line.find('=');
        if (pos != std::string::npos && pos > 0) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Parse value based on type (simplified)
            ConfigValue configValue;
            if (value == "true" || value == "false") {
                configValue = (value == "true");
            } else if (isNumber(value)) {
                if (value.find('.') != std::string::npos) {
                    configValue = std::stod(value);
                } else {
                    configValue = std::stoll(value);
                }
            } else {
                configValue = value;
            }
            
            // Use the new set method with index
            set(key, configValue, ConfigSource::File);
        }
    }
    
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Check if key already exists in index
    auto it = pImpl->keyIndex.find(key);
    
    if (it != pImpl->keyIndex.end()) {
        // Update existing entry
        size_t index = it->second;
        if (index < pImpl->entries.size()) {
            pImpl->entries[index].value = value;
            pImpl->entries[index].source = source;
        }
    } else {
        // Add new entry
        pImpl->entries.emplace_back(key, value, source);
        pImpl->keyIndex[key] = pImpl->entries.size() - 1;
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
    // Use index for O(1) lookup
    return pImpl->keyIndex.find(key) != pImpl->keyIndex.end();
}

void Config::remove(const std::string& key) {
    auto it = pImpl->keyIndex.find(key);
    if (it != pImpl->keyIndex.end()) {
        size_t index = it->second;
        if (index < pImpl->entries.size()) {
            // Remove from entries vector
            pImpl->entries.erase(pImpl->entries.begin() + index);
            
            // Update index for all entries after the removed one
            for (auto& pair : pImpl->keyIndex) {
                if (pair.second > index) {
                    pair.second--;
                }
            }
            
            // Remove from index
            pImpl->keyIndex.erase(it);
        }
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = pImpl->keyIndex.find(key);
    if (it == pImpl->keyIndex.end()) {
        return std::nullopt;
    }
    
    size_t index = it->second;
    if (index >= pImpl->entries.size()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(pImpl->entries[index].value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Validate configuration against rules
bool Config::validate() const {
    std::vector<std::string> errors;
    
    // Check required parameters
    for (const auto& rule : pImpl->validationRules) {
        if (rule.required) {
            bool found = false;
            for (const auto& entry : pImpl->entries) {
                if (matchesPattern(entry.key, rule.keyPattern)) {
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                errors.push_back("Missing required configuration parameter matching pattern: " + rule.keyPattern);
                if (!rule.errorMessage.empty()) {
                    errors.push_back("  " + rule.errorMessage);
                }
            }
        }
    }
    
    // Check value types
    for (const auto& entry : pImpl->entries) {
        bool matched = false;
        for (const auto& rule : pImpl->validationRules) {
            if (matchesPattern(entry.key, rule.keyPattern)) {
                matched = true;
                if (!validateValueType(entry.value, rule.valueType)) {
                    errors.push_back("Invalid value type for configuration parameter: " + entry.key);
                    errors.push_back("  Expected type: " + rule.valueType);
                    errors.push_back("  Actual value: " + getValueString(entry.value));
                }
                break;
            }
        }
        
        if (!matched) {
            errors.push_back("Unexpected configuration parameter: " + entry.key);
        }
    }
    
    // Store validation errors
    if (!errors.empty()) {
        pImpl->validationErrors = errors;
        return false;
    }
    
    pImpl->validationErrors.clear();
    return true;
}

std::vector<std::string> Config::getValidationErrors() const {
    return pImpl->validationErrors;
}

// Add validation rule
void Config::addValidationRule(const std::string& keyPattern, const std::string& valueType, 
                               bool required, const std::string& description, 
                               const std::string& errorMessage) {
    ValidationRule rule;
    rule.keyPattern = keyPattern;
    rule.valueType = valueType;
    rule.required = required;
    rule.description = description;
    rule.errorMessage = errorMessage;
    
    pImpl->validationRules.push_back(rule);
}

// Clear all validation rules
void Config::clearValidationRules() {
    pImpl->validationRules.clear();
    pImpl->validationErrors.clear();
}

// Helper function for trimming strings
static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) return str;
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

// Helper function for checking if a string is a number
static bool isNumber(const std::string& str) {
    return !str.empty() && std::find_if(str.begin(), str.end(), [](char c) { return !std::isdigit(c) && c != '.'; }) == str.end();
}

// Helper function to match key patterns (simple wildcard matching)
static bool matchesPattern(const std::string& key, const std::string& pattern) {
    // Simple wildcard matching: * matches any sequence, ? matches single character
    size_t keyPos = 0;
    size_t patternPos = 0;
    
    while (keyPos < key.size() && patternPos < pattern.size()) {
        if (pattern[patternPos] == '*') {
            // Skip consecutive asterisks
            while (patternPos + 1 < pattern.size() && pattern[patternPos + 1] == '*') {
                patternPos++;
            }
            
            if (patternPos + 1 >= pattern.size()) {
                // Trailing asterisk matches rest of key
                return true;
            }
            
            // Find next pattern character in key
            char nextChar = pattern[patternPos + 1];
            size_t nextKeyPos = key.find(nextChar, keyPos);
            if (nextKeyPos == std::string::npos) {
                return false;
            }
            
            keyPos = nextKeyPos;
            patternPos++;
        } else if (pattern[patternPos] == '?') {
            keyPos++;
            patternPos++;
        } else {
            if (key[keyPos] != pattern[patternPos]) {
                return false;
            }
            keyPos++;
            patternPos++;
        }
    }
    
    // Skip trailing asterisks in pattern
    while (patternPos < pattern.size() && pattern[patternPos] == '*') {
        patternPos++;
    }
    
    return keyPos == key.size() && patternPos == pattern.size();
}

// Helper function to validate value type
static bool validateValueType(const ConfigValue& value, const std::string& type) {
    if (type == "int" || type == "integer") {
        return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
    } else if (type == "float" || type == "double") {
        return std::holds_alternative<double>(value) || std::holds_alternative<float>(value);
    } else if (type == "string") {
        return std::holds_alternative<std::string>(value);
    } else if (type == "bool" || type == "boolean") {
        return std::holds_alternative<bool>(value);
    } else if (type == "list") {
        return std::holds_alternative<std::vector<int>>(value) ||
               std::holds_alternative<std::vector<double>>(value) ||
               std::holds_alternative<std::vector<std::string>>(value);
    }
    
    return false; // Unknown type
}

// Helper function to get string representation of a ConfigValue
static std::string getValueString(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        return "[int list]";
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        return "[float list]";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        return "[string list]";
    }
    
    return "unknown";
}

} // namespace nlm