#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <iostream>
#include <iomanip>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ValidationFunction> validators;
    std::map<std::string, std::string> errorMessages;
    std::map<std::string, ConfigValue> defaultValues;
    
    // Statistics
    size_t loadCount;
    size_t validationCount;
    
    Impl() : loadCount(0), validationCount(0) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    return loadFromFileEnhanced(filepath, true);
}

bool Config::loadFromFileEnhanced(const std::string& filepath, bool validateOnLoad) {
    pImpl->loadCount++;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Cannot open config file: " + filepath);
        return false;
    }
    
    // Try JSON format first
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        bool success = importFromJSON(content);
        if (success && validateOnLoad) {
            std::vector<ValidationError> errors = getValidationErrors();
            if (!errors.empty()) {
                NLM_LOG_WARNING("Configuration validation failed after JSON import:");
                for (const auto& error : errors) {
                    NLM_LOG_WARNING("  " + error.key + ": " + error.message);
                }
                return false;
            }
        }
        return success;
    }
    
    // Fall back to simple key=value format
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
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
    
    if (validateOnLoad) {
        std::vector<ValidationError> errors = getValidationErrors();
        if (!errors.empty()) {
            NLM_LOG_WARNING("Configuration validation failed after file import:");
            for (const auto& error : errors) {
                NLM_LOG_WARNING("  " + error.key + ": " + error.message);
            }
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    return parseCommandLineArguments(argc, argv);
}

bool Config::parseCommandLineArguments(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Try to parse value as different types
                bool parsed = false;
                
                // Try int
                try {
                    size_t posEnd;
                    int intVal = std::stoi(value, &posEnd);
                    if (posEnd == value.size()) {
                        set<int>(key, intVal, ConfigSource::CommandLine);
                        parsed = true;
                    }
                } catch (...) {}
                
                // Try double
                if (!parsed) {
                    try {
                        size_t posEnd;
                        double doubleVal = std::stod(value, &posEnd);
                        if (posEnd == value.size()) {
                            set<double>(key, doubleVal, ConfigSource::CommandLine);
                            parsed = true;
                        }
                    } catch (...) {}
                }
                
                // Try bool
                if (!parsed) {
                    std::string lowerValue = toLower(value);
                    if (lowerValue == "true" || lowerValue == "1" || lowerValue == "yes" || lowerValue == "on") {
                        set<bool>(key, true, ConfigSource::CommandLine);
                        parsed = true;
                    } else if (lowerValue == "false" || lowerValue == "0" || lowerValue == "no" || lowerValue == "off") {
                        set<bool>(key, false, ConfigSource::CommandLine);
                        parsed = true;
                    }
                }
                
                // If still not parsed, treat as string
                if (!parsed) {
                    set(key, value, ConfigSource::CommandLine);
                }
                
                // Validate if validator exists
                auto validatorIt = pImpl->validators.find(key);
                if (validatorIt != pImpl->validators.end()) {
                    ConfigValue configValue;
                    if (auto valOpt = get<std::string>(key)) {
                        configValue = *valOpt;
                    }
                    
                    if (!validatorIt->second(configValue)) {
                        std::string errorMsg = "Validation failed for key '" + key + "'";
                        auto errorIt = pImpl->errorMessages.find(key);
                        if (errorIt != pImpl->errorMessages.end()) {
                            errorMsg = errorIt->second;
                        }
                        NLM_LOG_WARNING(errorMsg);
                        success = false;
                    }
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Use JSON format for better readability
    file << exportToJSON();
    
    return true;
}

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

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template<typename T>
void Config::set(const std::string& key, const T& value, ConfigSource source) {
    ConfigValue configValue;
    
    if constexpr (std::is_same_v<T, int>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, int64_t>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, double>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, bool>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, std::string>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
        configValue = value;
    } else {
        static_assert(!std::is_same_v<T, T>, "Unsupported type for Config::set");
    }
    
    set(key, configValue, source);
}

template<typename T>
bool Config::getValidated(const std::string& key, T& out) const {
    auto valOpt = get<T>(key);
    if (!valOpt) {
        return false;
    }
    
    out = *valOpt;
    
    // Run validators
    auto validatorIt = pImpl->validators.find(key);
    if (validatorIt != pImpl->validators.end()) {
        if (!validatorIt->second(*valOpt)) {
            return false;
        }
    }
    
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source, "", getValueTypeIndex(value));
    }
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    // Also remove validators
    pImpl->validators.erase(key);
    pImpl->errorMessages.erase(key);
    pImpl->defaultValues.erase(key);
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
    pImpl->validators.clear();
    pImpl->errorMessages.clear();
    pImpl->defaultValues.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, " 
        << pImpl->validators.size() << " validators):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [" << convertToString(entry.value) << "] (" 
            << static_cast<int>(entry.source) << ")";
        
        if (pImpl->validators.find(entry.key) != pImpl->validators.end()) {
            oss << " [VALIDATED]";
        }
        
        oss << "\n";
    }
    
    if (!pImpl->validators.empty()) {
        oss << "\nValidators:";
        for (const auto& validator : pImpl->validators) {
            oss << "\n  " << validator.first;
        }
    }
    
    return oss.str();
}

void Config::addValidator(const std::string& key, ValidationFunction validator) {
    pImpl->validators[key] = validator;
}

void Config::addValidator(const std::string& key, ValidationFunction validator, const std::string& errorMessage) {
    pImpl->validators[key] = validator;
    pImpl->errorMessages[key] = errorMessage;
}

bool Config::validate() const {
    pImpl->validationCount++;
    bool allValid = true;
    
    for (const auto& entry : pImpl->entries) {
        auto validatorIt = pImpl->validators.find(entry.key);
        if (validatorIt != pImpl->validators.end()) {
            if (!validatorIt->second(entry.value)) {
                allValid = false;
            }
        }
    }
    
    return allValid;
}

std::vector<ValidationError> Config::getValidationErrors() const {
    std::vector<ValidationError> errors;
    
    for (const auto& entry : pImpl->entries) {
        auto validatorIt = pImpl->validators.find(entry.key);
        if (validatorIt != pImpl->validators.end()) {
            if (!validatorIt->second(entry.value)) {
                std::string errorMsg = "Validation failed for key '" + entry.key + "'";
                auto errorIt = pImpl->errorMessages.find(entry.key);
                if (errorIt != pImpl->errorMessages.end()) {
                    errorMsg = errorIt->second;
                }
                
                errors.emplace_back(entry.key, ValidationErrorType::InvalidType, errorMsg);
            }
        }
    }
    
    return errors;
}

void Config::resetToDefaults() {
    // Reset all entries to Default source
    for (auto& entry : pImpl->entries) {
        entry.source = ConfigSource::Default;
    }
}

void Config::resetToSource(ConfigSource source) {
    // Reset entries to specified source
    for (auto& entry : pImpl->entries) {
        if (entry.source == source) {
            entry.source = ConfigSource::Runtime;
        }
    }
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

std::vector<ConfigEntry> Config::search(const std::string& pattern) const {
    std::vector<ConfigEntry> results;
    std::regex regex(pattern, std::regex::icase);
    
    for (const auto& entry : pImpl->entries) {
        if (std::regex_search(entry.key, regex)) {
            results.push_back(entry);
        }
    }
    
    return results;
}

std::string Config::exportToJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "  \"" << entry.key << "\": " << convertToJSON(entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    
    oss << "}\n";
    return oss.str();
}

bool Config::importFromJSON(const std::string& jsonString) {
    try {
        // Simple JSON parsing - for now just handle basic key-value pairs
        // This can be enhanced with proper JSON library
        clear();
        
        size_t pos = 0;
        while (pos < jsonString.size()) {
            // Find next "key": "value"
            size_t keyStart = jsonString.find('"', pos);
            if (keyStart == std::string::npos) break;
            
            size_t keyEnd = jsonString.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = jsonString.substr(keyStart + 1, keyEnd - keyStart - 1);
            
            size_t valueStart = jsonString.find('"', keyEnd + 2);
            if (valueStart == std::string::npos) break;
            
            size_t valueEnd = jsonString.find('"', valueStart + 1);
            if (valueEnd == std::npos) break;
            
            std::string value = jsonString.substr(valueStart + 1, valueEnd - valueStart - 1);
            
            set(key, value, ConfigSource::File);
            
            pos = valueEnd + 2;
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

bool Config::isEmpty() const {
    return pImpl->entries.empty();
}

bool Config::validateInt(const ConfigValue& value, int min, int max) {
    try {
        int val = std::get<int>(value);
        return val >= min && val <= max;
    } catch (...) {
        return false;
    }
}

bool Config::validateDouble(const ConfigValue& value, double min, double max) {
    try {
        double val = std::get<double>(value);
        return val >= min && val <= max;
    } catch (...) {
        return false;
    }
}

bool Config::validateString(const ConfigValue& value, size_t minLength, size_t maxLength) {
    try {
        std::string val = std::get<std::string>(value);
        return val.size() >= minLength && val.size() <= maxLength;
    } catch (...) {
        return false;
    }
}

bool Config::validateBool(const ConfigValue& value) {
    try {
        bool val = std::get<bool>(value);
        return true; // Bool validation is just presence
    } catch (...) {
        return false;
    }
}

std::string Config::getDefaultErrorMessage(const std::string& key, const std::type_index& expectedType) {
    std::string typeName;
    if (expectedType == typeid(int)) typeName = "integer";
    else if (expectedType == typeid(double)) typeName = "number";
    else if (expectedType == typeid(bool)) typeName = "boolean";
    else if (expectedType == typeid(std::string)) typeName = "string";
    else if (expectedType == typeid(std::vector<int>)) typeName = "integer array";
    else if (expectedType == typeid(std::vector<double>)) typeName = "float array";
    else if (expectedType == typeid(std::vector<std::string>)) typeName = "string array";
    else typeName = "value";
    
    return "Key '" + key + "' must be a " + typeName;
}

std::string Config::convertToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        oss << arg;
    }, value);
    return oss.str();
}

std::string Config::convertToJSON(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        if constexpr (std::is_same_v<decltype(arg), std::string>) {
            oss << "\"" << arg << "\"";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

std::type_index Config::getValueTypeIndex(const ConfigValue& value) {
    return std::visit([](auto&& arg) {
        return std::type_index(typeid(decltype(arg)));
    }, value);
}

// Explicit template instantiations

template std::optional<int> Config::get<int>(const std::string&) const;

template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;

template std::optional<double> Config::get<double>(const std::string&) const;

template std::optional<bool> Config::get<bool>(const std::string&) const;

template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;

template double Config::getOr<double>(const std::string&, const double&) const;

template bool Config::getOr<bool>(const std::string&, const bool&) const;

template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
