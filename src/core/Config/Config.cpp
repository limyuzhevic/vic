#include "core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <numeric>
#include <cctype>

namespace nlm {

// Forward declarations
namespace {
    std::string trim(const std::string& str);
    std::string toLower(const std::string& str);
    std::vector<std::string> splitKey(const std::string& key);
    std::string joinPath(const std::vector<std::string>& path);
    bool parseValue(const std::string& valueStr, ConfigValue& outValue);
    std::string sourceToString(ConfigSource source);
    std::string sourceToLabel(const std::string& filepath, ConfigSource source, const std::string& arg);
    bool wildcardMatch(const std::string& pattern, const std::string& text);
    bool isDigit(char c);
    bool isWhitespace(char c);
}

// JSON/YAML parsing helpers
namespace {
    bool loadFromModernFormat(const std::string& filepath);
    bool parseJSON(const std::string& content, const std::string& filepath);
    bool parseYAML(const std::string& content, const std::string& filepath);
}

// Validation functions
namespace {
    ValidationResult validateNeuronCount(const ConfigValue& value, const std::string& key);
    ValidationResult validateSynapseDensity(const ConfigValue& value, const std::string& key);
    ValidationResult validateConnectionProbability(const ConfigValue& value, const std::string& key);
    ValidationResult validateTemperature(const ConfigValue& value, const std::string& key);
    ValidationResult validateLogLevel(const ConfigValue& value, const std::string& key);
    ValidationResult validateLearningRate(const ConfigValue& value, const std::string& key);
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ValidationFunc> validators;
    std::map<std::string, bool> lockedKeys;
    std::map<std::string, std::string> descriptions;
};

// Helper function implementations
namespace {
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    std::vector<std::string> splitKey(const std::string& key) {
        std::vector<std::string> parts;
        size_t start = 0;
        size_t end;
        while ((end = key.find('.', start)) != std::string::npos) {
            if (end > start) {
                parts.push_back(key.substr(start, end - start));
            }
            start = end + 1;
        }
        if (start < key.size()) {
            parts.push_back(key.substr(start));
        }
        return parts;
    }
    
    std::string joinPath(const std::vector<std::string>& path) {
        if (path.empty()) return "";
        std::string result = path[0];
        for (size_t i = 1; i < path.size(); ++i) {
            result += "." + path[i];
        }
        return result;
    }
    
    bool parseValue(const std::string& valueStr, ConfigValue& outValue) {
        if (valueStr.empty()) {
            outValue = "";
            return true;
        }
        
        // Try bool
        std::string lower = toLower(valueStr);
        if (lower == "true" || lower == "false") {
            outValue = (lower == "true");
            return true;
        }
        
        // Try int
        try {
            size_t pos;
            int intVal = std::stoi(valueStr, &pos);
            if (pos == valueStr.size()) {
                outValue = intVal;
                return true;
            }
        } catch (...) {}
        
        // Try int64_t (for long integers)
        try {
            size_t pos;
            int64_t int64Val = std::stoll(valueStr, &pos);
            if (pos == valueStr.size()) {
                outValue = int64Val;
                return true;
            }
        } catch (...) {}
        
        // Try double
        try {
            size_t pos;
            double doubleVal = std::stod(valueStr, &pos);
            if (pos == valueStr.size()) {
                outValue = doubleVal;
                return true;
            }
        } catch (...) {}
        
        // Try string
        outValue = valueStr;
        return true;
    }
    
    std::string sourceToString(ConfigSource source) {
        switch (source) {
            case ConfigSource::Default: return "Default";
            case ConfigSource::File: return "File";
            case ConfigSource::CommandLine: return "CommandLine";
            case ConfigSource::Runtime: return "Runtime";
            default: return "Unknown";
        }
    }
    
    std::string sourceToLabel(const std::string& filepath, ConfigSource source, const std::string& arg) {
        switch (source) {
            case ConfigSource::Default: return "default";
            case ConfigSource::File: return filepath;
            case ConfigSource::CommandLine: return arg;
            case ConfigSource::Runtime: return "runtime";
            default: return "unknown";
        }
    }
    
    bool isDigit(char c) { return std::isdigit(static_cast<unsigned char>(c)); }
    
    bool isWhitespace(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }
    
    bool wildcardMatch(const std::string& pattern, const std::string& text) {
        if (pattern == text) return true;
        
        size_t starPos = pattern.find('*');
        if (starPos == std::string::npos) {
            return false;
        }
        
        std::string prefix = pattern.substr(0, starPos);
        std::string suffix = pattern.substr(starPos + 1);
        
        return text.substr(0, prefix.size()) == prefix &&
               text.substr(text.size() - suffix.size()) == suffix;
    }
}}

// Validation function implementations
namespace {
    ValidationResult validateNeuronCount(const ConfigValue& value, const std::string& key) {
        ValidationResult result;
        try {
            auto val = std::get<int64_t>(value);
            if (val <= 0) {
                result.valid = false;
                result.errorMessage = "Neuron count must be positive"; 
                result.source = "validation";
            }
        } catch (...) {
            result.valid = false;
            result.errorMessage = "Neuron count must be an integer";
            result.source = "validation";
        }
        return result;
    }
    
    ValidationResult validateSynapseDensity(const ConfigValue& value, const std::string& key) {
        ValidationResult result;
        try {
            double val = std::get<double>(value);
            if (val < 0.0 || val > 1.0) {
                result.valid = false;
                result.errorMessage = "Synapse density must be between 0.0 and 1.0";
                result.source = "validation";
            }
        } catch (...) {
            result.valid = false;
            result.errorMessage = "Synapse density must be a number";
            result.source = "validation";
        }
        return result;
    }
    
    ValidationResult validateConnectionProbability(const ConfigValue& value, const std::string& key) {
        ValidationResult result;
        try {
            double val = std::get<double>(value);
            if (val < 0.0 || val > 1.0) {
                result.valid = false;
                result.errorMessage = "Connection probability must be between 0.0 and 1.0";
                result.source = "validation";
            }
        } catch (...) {
            result.valid = false;
            result.errorMessage = "Connection probability must be a number";
            result.source = "validation";
        }
        return result;
    }
    
    ValidationResult validateTemperature(const ConfigValue& value, const std::string& key) {
        ValidationResult result;
        try {
            double val = std::get<double>(value);
            if (val < -10.0 || val > 50.0) {
                result.valid = false;
                result.errorMessage = "Temperature must be between -10.0 and 50.0 degrees Celsius";
                result.source = "validation";
            }
        } catch (...) {
            result.valid = false;
            result.errorMessage = "Temperature must be a number";
            result.source = "validation";
        }
        return result;
    }
    
    ValidationResult validateLogLevel(const ConfigValue& value, const std::string& key) {
        ValidationResult result;
        try {
            std::string val = std::get<std::string>(value);
            std::string lower = toLower(val);
            if (lower != "debug" && lower != "info" && lower != "warn" && lower != "error" && lower != "fatal") {
                result.valid = false;
                result.errorMessage = "Log level must be one of: DEBUG, INFO, WARN, ERROR, FATAL";
                result.source = "validation";
            }
        } catch (...) {
            result.valid = false;
            result.errorMessage = "Log level must be a string";
            result.source = "validation";
        }
        return result;
    }
    
    ValidationResult validateLearningRate(const ConfigValue& value, const std::string& key) {
        ValidationResult result;
        try {
            double val = std::get<double>(value);
            if (val < 0.0 || val > 1.0) {
                result.valid = false;
                result.errorMessage = "Learning rate must be between 0.0 and 1.0";
                result.source = "validation";
            }
        } catch (...) {
            result.valid = false;
            result.errorMessage = "Learning rate must be a number";
            result.source = "validation";
        }
        return result;
    }
}}

void Config::addBuiltInValidators() {
    // Register built-in validation rules
    addValidation("brain.neuron_count", validateNeuronCount);
    addValidation("brain.synapse_density", validateSynapseDensity);
    addValidation("brain.connection_probability", validateConnectionProbability);
    addValidation("environment.temperature", validateTemperature);
    addValidation("log.level", validateLogLevel);
    addValidation("plasticity.learning_rate", validateLearningRate);
}

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Register built-in validation rules
    addBuiltInValidators();
}

Config::~Config() = default;

// Set values with dot notation
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& sourceLabel) {
    std::string actualKey = key;
    
    // Check if key is locked
    if (pImpl->lockedKeys.find(actualKey) != pImpl->lockedKeys.end() && pImpl->lockedKeys[actualKey]) {
        return; // Cannot modify locked key
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&actualKey](const ConfigEntry& e) { return e.key == actualKey; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->sourceLabel = sourceLabel;
    } else {
        pImpl->entries.emplace_back(actualKey, value, source, "", sourceLabel);
    }
}

void Config::set(const std::string& key, const std::string& valueStr, ConfigSource source, const std::string& sourceLabel) {
    ConfigValue value;
    if (parseValue(valueStr, value)) {
        set(key, value, source, sourceLabel);
    }
}

void Config::set(const std::string& key, int value, ConfigSource source, const std::string& sourceLabel) {
    set(key, ConfigValue(value), source, sourceLabel);
}

void Config::set(const std::string& key, double value, ConfigSource source, const std::string& sourceLabel) {
    set(key, ConfigValue(value), source, sourceLabel);
}

void Config::set(const std::string& key, bool value, ConfigSource source, const std::string& sourceLabel) {
    set(key, ConfigValue(value), source, sourceLabel);
}

// Set nested values using dot notation with validation
bool Config::setValidated(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& sourceLabel) {
    // Check if key is locked
    if (pImpl->lockedKeys.find(key) != pImpl->lockedKeys.end() && pImpl->lockedKeys[key]) {
        return false; // Cannot modify locked key
    }
    
    // Validate if there are validators for this key or its parents
    bool validated = true;
    ValidationResult validationResult;
    
    // Check exact match validator
    auto it = pImpl->validators.find(key);
    if (it != pImpl->validators.end()) {
        validationResult = it->second(value, key);
        if (!validationResult.valid) {
            validated = false;
        }
    }
    
    // Check parent validators (e.g., for brain.neuron_count, check brain.*)
    std::vector<std::string> path = splitKey(key);
    for (size_t i = 1; i < path.size(); ++i) {
        std::string parentPrefix = joinPath(std::vector<std::string>(path.begin(), path.begin() + i));
        parentPrefix += ".*";
        
        for (const auto& validator : pImpl->validators) {
            if (wildcardMatch(validator.first, parentPrefix)) {
                ValidationResult parentResult = validator.second(value, key);
                if (!parentResult.valid) {
                    validationResult = parentResult;
                    validated = false;
                }
            }
        }
    }
    
    if (!validated && !validationResult.errorMessage.empty()) {
        std::cerr << "Validation error for key \"" << key << "\": " << validationResult.errorMessage << std::endl;
        return false;
    }
    
    set(key, value, source, sourceLabel);
    return true;
}

// Get values using dot notation
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

// Helper get methods
std::optional<int64_t> Config::getInt(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<int64_t>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

std::optional<double> Config::getDouble(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<double>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

std::optional<bool> Config::getBool(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<bool>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

std::optional<std::string> Config::getString(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<std::string>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

// Get nested values using dot notation with dot separated path
template<typename T>
std::optional<T> Config::getNested(const std::vector<std::string>& path) const {
    if (path.empty()) {
        return std::nullopt;
    }
    
    // Check if the key exists in entries
    std::string fullKey = joinPath(path);
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&fullKey](const ConfigEntry& e) { return e.key == fullKey; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        if constexpr (std::is_same_v<T, int64_t>) {
            return std::get<int64_t>(it->value);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::get<double>(it->value);
        } else if constexpr (std::is_same_v<T, bool>) {
            return std::get<bool>(it->value);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return std::get<std::string>(it->value);
        }
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
    
    return std::nullopt;
}

// Get parameter source information
ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return ConfigSource::Default;
    }
    
    return it->source;
}

std::string Config::getSourceLabel(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "default";
    }
    
    return it->sourceLabel.empty() ? sourceToString(it->source) : it->sourceLabel;
}

// Validate configuration parameter
ValidationResult Config::validate(const std::string& key) const {
    ValidationResult result;
    result.valid = true;
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return result; // Key doesn't exist, nothing to validate
    }
    
    // Check exact match validator
    auto validatorIt = pImpl->validators.find(key);
    if (validatorIt != pImpl->validators.end()) {
        result = validatorIt->second(it->value, key);
        return result;
    }
    
    // Check parent validators
    std::vector<std::string> path = splitKey(key);
    for (size_t i = 1; i < path.size(); ++i) {
        std::string parentPrefix = joinPath(std::vector<std::string>(path.begin(), path.begin() + i));
        parentPrefix += ".*";
        
        for (const auto& validator : pImpl->validators) {
            if (wildcardMatch(validator.first, parentPrefix)) {
                ValidationResult parentResult = validator.second(it->value, key);
                if (!parentResult.valid) {
                    return parentResult;
                }
            }
        }
    }
    
    return result;
}

// Add validation rule for a parameter
void Config::addValidation(const std::string& key, ValidationFunc validator) {
    pImpl->validators[key] = validator;
}

// Get all keys
std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

// Get flattened keys with dot notation
std::vector<std::string> Config::getFlatKeys() const {
    return getKeys();
}

// Get keys with specific prefix
std::vector<std::string> Config::getKeysWithPrefix(const std::string& prefix) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find(prefix) == 0) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

// Get configuration as nested structure (JSON/YAML format)
nlohmann::json Config::getAsNested() const {
    nlohmann::json j;
    
    // Convert entries to nested JSON structure
    for (const auto& entry : pImpl->entries) {
        std::vector<std::string> path = splitKey(entry.key);
        nlohmann::json* current = &j;
        
        // Navigate to the parent object
        for (size_t i = 0; i < path.size() - 1; ++i) {
            if (!current->contains(path[i])) {
                current->emplace(path[i], nlohmann::json::object());
            }
            current = &((*current)[path[i]]);
        }
        
        // Set the value
        setNestedValue(*current, path.back(), entry.value);
    }
    
    return j;
}

void Config::setNestedValue(nlohmann::json& j, const std::string& key, const ConfigValue& value) {
    std::visit([&j, &key](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            j[key] = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            j[key] = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            j[key] = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            j[key] = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>> || std::is_same_v<T, std::vector<int>>) {
            j[key] = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            j[key] = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            j[key] = arg;
        }
    }, value);
}

std::any Config::getAsAny() const {
    // Convert to JSON string representation
    nlohmann::json j = getAsNested();
    return std::make_any<std::string>(j.dump());
}

std::any Config::getAsNested() const {
    // Wrapper for getAsAny
    return getAsAny();
}

// Get keys by source
std::vector<std::string> Config::getKeysBySource(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

// Merge another configuration
void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        if (!isLocked(entry.key)) {
            set(entry.key, entry.value, entry.source, entry.sourceLabel);
        }
    }
}

// Get parameter description
std::string Config::getDescription(const std::string& key) const {
    auto it = pImpl->descriptions.find(key);
    if (it != pImpl->descriptions.end()) {
        return it->second;
    }
    return "";
}

// Set parameter description
void Config::setDescription(const std::string& key, const std::string& description) {
    pImpl->descriptions[key] = description;
}

// Check if parameter is locked (cannot be changed after loading)
void Config::setLocked(const std::string& key, bool locked) {
    pImpl->lockedKeys[key] = locked;
}

bool Config::isLocked(const std::string& key) const {
    auto it = pImpl->lockedKeys.find(key);
    return it != pImpl->lockedKeys.end() && it->second;
}

// Check if key exists
bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

// Remove key
void Config::remove(const std::string& key) {
    if (isLocked(key)) {
        return; // Cannot remove locked key
    }
    
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

// Get configuration summary
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
        oss << "] (" << sourceToString(entry.source) << ") [" << entry.sourceLabel << "]\n";
    }
    return oss.str();
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->validators.clear();
    pImpl->lockedKeys.clear();
    pImpl->descriptions.clear();
}

// File I/O methods
bool Config::loadFromFile(const std::string& filepath) {
    // Try modern format (JSON/YAML) first
    if (loadFromModernFormat(filepath)) {
        return true;
    }
    
    // Fallback to simple key=value format for backward compatibility
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
            
            set(key, value, ConfigSource::File, filepath);
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
                set(key, value, ConfigSource::CommandLine, arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine, arg + " " + value);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Convert to JSON format for saving
    nlohmann::json j = getAsNested();
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << std::setw(2) << j;
    
    return true;
}

// JSON/YAML parsing implementations
namespace {
    bool loadFromModernFormat(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // Try JSON first
        try {
            auto j = nlohmann::json::parse(content);
            return parseJSON(content, filepath);
        } catch (...) {}
        
        // Try YAML
        try {
            auto y = YAML::Load(content);
            return parseYAML(content, filepath);
        } catch (...) {}
        
        return false;
    }
    
    bool parseJSON(const std::string& content, const std::string& filepath) {
        auto j = nlohmann::json::parse(content);
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            ConfigValue value;
            
            // Try to parse JSON value to ConfigValue
            if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_number_integer()) {
                value = it->get<int64_t>();
            } else if (it->is_number_float()) {
                value = it->get<double>();
            } else if (it->is_boolean()) {
                value = it->get<bool>();
            } else if (it->is_array()) {
                // Handle arrays
                if (!it->empty() && it->front().is_number_integer()) {
                    std::vector<int64_t> arr;
                    for (const auto& item : *it) {
                        arr.push_back(item.get<int64_t>());
                    }
                    value = arr;
                } else if (!it->empty() && it->front().is_number_float()) {
                    std::vector<double> arr;
                    for (const auto& item : *it) {
                        arr.push_back(item.get<double>());
                    }
                    value = arr;
                } else if (!it->empty() && it->front().is_string()) {
                    std::vector<std::string> arr;
                    for (const auto& item : *it) {
                        arr.push_back(item.get<std::string>());
                    }
                    value = arr;
                }
            }
            
            if (!value.index()) {
                value = "";
            }
            
            set(key, value, ConfigSource::File, filepath);
        }
        
        return true;
    }
    
    bool parseYAML(const std::string& content, const std::string& filepath) {
        auto y = YAML::Load(content);
        
        for (const auto& pair : y) {
            std::string key = pair.first.as<std::string>();
            ConfigValue value;
            
            // Try to parse YAML value to ConfigValue
            if (pair.second.IsScalar()) {
                if (pair.second.IsInteger()) {
                    value = pair.second.as<int64_t>();
                } else if (pair.second.IsFloat()) {
                    value = pair.second.as<double>();
                } else if (pair.second.IsBoolean()) {
                    value = pair.second.as<bool>();
                } else {
                    value = pair.second.as<std::string>();
                }
            } else if (pair.second.IsSequence()) {
                // Handle YAML sequences (arrays)
                if (!pair.second.empty() && pair.second[0].IsInteger()) {
                    std::vector<int64_t> arr;
                    for (const auto& item : pair.second) {
                        arr.push_back(item.as<int64_t>());
                    }
                    value = arr;
                } else if (!pair.second.empty() && pair.second[0].IsFloat()) {
                    std::vector<double> arr;
                    for (const auto& item : pair.second) {
                        arr.push_back(item.as<double>());
                    }
                    value = arr;
                } else if (!pair.second.empty() && pair.second[0].IsScalar()) {
                    std::vector<std::string> arr;
                    for (const auto& item : pair.second) {
                        arr.push_back(item.as<std::string>());
                    }
                    value = arr;
                }
            }
            
            if (!value.index()) {
                value = "";
            }
            
            set(key, value, ConfigSource::File, filepath);
        }
        
        return true;
    }
}}