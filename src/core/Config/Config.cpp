// Config.cpp - Implementation of enhanced configuration system with validation and schema
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()), schema(nlm::schemas::defaultConfigSchema) {
    // Apply default values from schema
    for (const auto& entry : schema) {
        // Set default values for all schema entries
        applyDefault(entry);
    }
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
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
        
        // Parse key=value pairs with support for quotes and different value types
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string valueStr = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (valueStr.size() >= 2 && 
                ((valueStr.front() == '"' && valueStr.back() == '"') ||
                 (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            // Parse value based on schema entry
            auto schemaEntry = findSchemaEntry(key);
            if (schemaEntry) {
                // Try to convert value based on default value type
                setValueFromString(key, valueStr, schemaEntry->defaultValue, ConfigSource::File);
            } else {
                // Try common value types
                if (std::regex_match(valueStr, std::regex("true|false"))) {
                    bool boolVal = (valueStr == "true");
                    set(key, ConfigValue(boolVal), ConfigSource::File);
                } else if (std::regex_match(valueStr, std::regex("-?\d+"))) {
                    int intVal = std::stoi(valueStr);
                    set(key, ConfigValue(intVal), ConfigSource::File);
                } else if (std::regex_match(valueStr, std::regex("-?\d+\.?\d*"))) {
                    double doubleVal = std::stod(valueStr);
                    set(key, ConfigValue(doubleVal), ConfigSource::File);
                } else {
                    // Default to string
                    set(key, ConfigValue(valueStr), ConfigSource::File);
                }
            }
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
                setValueFromString(key, value, ConfigValue(""), ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            setValueFromString(key, value, ConfigValue(""), ConfigSource::CommandLine);
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
        // Only save entries that have a non-default value
        if (isModified(entry.key)) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_arithmetic_v<T>) {
                    file << arg;
                } else {
                    file << "\"" << arg << "\"";
                }
            }, entry.value);
            file << "\n\n";
        }
    }
    
    return true;
}

void Config::setValueFromString(const std::string& key, const std::string& valueStr, 
                               const ConfigValue& defaultValue, ConfigSource source) {
    // Try to match against schema entry
    auto schemaEntry = findSchemaEntry(key);
    if (schemaEntry) {
        setTypedValue(key, valueStr, schemaEntry->defaultValue, source);
    } else {
        // Try common types in order
        if (std::regex_match(valueStr, std::regex("true|false"))) {
            bool boolVal = (valueStr == "true");
            set(key, ConfigValue(boolVal), source);
        } else if (std::regex_match(valueStr, std::regex("-?\d+"))) {
            int intVal = std::stoi(valueStr);
            set(key, ConfigValue(intVal), source);
        } else if (std::regex_match(valueStr, std::regex("-?\d+\.?\d*"))) {
            double doubleVal = std::stod(valueStr);
            set(key, ConfigValue(doubleVal), source);
        } else {
            set(key, ConfigValue(valueStr), source);
        }
    }
}

void Config::setTypedValue(const std::string& key, const std::string& valueStr, 
                          const ConfigValue& defaultValue, ConfigSource source) {
    std::visit([&](auto&& defaultVal) {
        using T = std::decay_t<decltype(defaultVal)>;
        if constexpr (std::is_same_v<T, bool>) {
            bool val = (valueStr == "true");
            set(key, ConfigValue(val), source);
        } else if constexpr (std::is_same_v<T, int>) {
            int val = std::stoi(valueStr);
            set(key, ConfigValue(val), source);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            int64_t val = std::stoll(valueStr);
            set(key, ConfigValue(val), source);
        } else if constexpr (std::is_same_v<T, double>) {
            double val = std::stod(valueStr);
            set(key, ConfigValue(val), source);
        } else if constexpr (std::is_same_v<T, float>) {
            float val = std::stof(valueStr);
            set(key, ConfigValue(val), source);
        } else if constexpr (std::is_same_v<T, std::string>) {
            set(key, ConfigValue(valueStr), source);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            // Parse comma-separated list
            std::vector<int> vec;
            std::stringstream ss(valueStr);
            std::string item;
            while (std::getline(ss, item, ',')) {
                item = trim(item);
                if (!item.empty()) {
                    vec.push_back(std::stoi(item));
                }
            }
            set(key, ConfigValue(vec), source);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            // Parse comma-separated list
            std::vector<double> vec;
            std::stringstream ss(valueStr);
            std::string item;
            while (std::getline(ss, item, ',')) {
                item = trim(item);
                if (!item.empty()) {
                    vec.push_back(std::stod(item));
                }
            }
            set(key, ConfigValue(vec), source);
        }
    }, defaultValue);
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

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
    // Re-apply schema defaults after clear
    for (const auto& entry : schema) {
        applyDefault(entry);
    }
}

ValidationResult Config::validateValue(const std::string& key, const ConfigValue& value) const {
    auto schemaEntry = findSchemaEntry(key);
    if (!schemaEntry) {
        return ValidationResult(true, "No schema entry for key");
    }
    
    if (schemaEntry->validator) {
        return schemaEntry->validator(value);
    }
    
    return ValidationResult(true, "");
}

std::vector<ValidationError> Config::validate() const {
    std::vector<ValidationError> errors;
    
    for (const auto& entry : pImpl->entries) {
        ValidationResult result = validateValue(entry.key, entry.value);
        if (!result.valid) {
            errors.emplace_back(entry.key, result.message);
        }
    }
    
    return errors;
}

bool Config::isValid() const {
    return validate().empty();
}

std::vector<ValidationError> Config::getValidationErrors() const {
    return validate();
}

ConfigSummary Config::getSummary() const {
    ConfigSummary summary;
    summary.totalEntries = pImpl->entries.size();
    
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::File:
                summary.fileEntries++;
                break;
            case ConfigSource::CommandLine:
                summary.commandLineEntries++;
                break;
            case ConfigSource::Runtime:
                summary.runtimeEntries++;
                break;
            case ConfigSource::Default:
                // Default entries are not stored in entries
                break;
        }
        
        if (entry.source != ConfigSource::Default) {
            summary.overriddenKeys.push_back(entry.key);
        }
    }
    
    return summary;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    ConfigSummary summary = getSummary();
    oss << "  File: " << summary.fileEntries << " entries\n";
    oss << "  Command Line: " << summary.commandLineEntries << " entries\n";
    oss << "  Runtime: " << summary.runtimeEntries << " entries\n";
    oss << "  Overrides: " << summary.overriddenKeys.size() << " keys\n";
    
    oss << "\nEntries:\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    
    return oss.str();
}

void Config::applySchema(const std::vector<ConfigSchemaEntry>& newSchema) {
    // Store old entries for comparison
    std::vector<ConfigEntry> oldEntries;
    oldEntries.swap(pImpl->entries);
    
    // Update schema
    schema = newSchema;
    
    // Apply defaults for all schema entries
    for (const auto& entry : schema) {
        applyDefault(entry);
    }
    
    // Merge old entries, preserving non-default values
    for (const auto& oldEntry : oldEntries) {
        auto schemaEntry = findSchemaEntry(oldEntry.key);
        if (schemaEntry) {
            // Check if this is a default value (same as schema default)
            if (!isValueEqual(oldEntry.value, schemaEntry->defaultValue)) {
                // Keep the old value since it's different from default
                set(oldEntry.key, oldEntry.value, oldEntry.source);
            } else if (isModified(oldEntry.key)) {
                // Keep the old value if it was explicitly set
                set(oldEntry.key, oldEntry.value, oldEntry.source);
            }
        } else {
            // Old entry without schema, keep it
            set(oldEntry.key, oldEntry.value, oldEntry.source);
        }
    }
}

void Config::applyDefault(const ConfigSchemaEntry& entry) {
    if (!has(entry.key)) {
        set(entry.key, entry.defaultValue, ConfigSource::Default);
    }
}

bool Config::isValueEqual(const ConfigValue& a, const ConfigValue& b) const {
    return std::visit([](auto&& aVal, auto&& bVal) {
        using A = std::decay_t<decltype(aVal)>;
        using B = std::decay_t<decltype(bVal)>;
        if constexpr (std::is_same_v<A, B>) {
            return aVal == bVal;
        } else if constexpr (std::is_arithmetic_v<A> && std::is_arithmetic_v<B>) {
            return static_cast<double>(aVal) == static_cast<double>(bVal);
        } else if constexpr (std::is_same_v<A, std::string> && std::is_same_v<B, std::string>) {
            return aVal == bVal;
        } else if constexpr (std::is_same_v<A, std::vector<int>> && std::is_same_v<B, std::vector<int>>) {
            return aVal == bVal;
        } else if constexpr (std::is_same_v<A, std::vector<double>> && std::is_same_v<B, std::vector<double>>) {
            return aVal == bVal;
        } else {
            return false;
        }
    }, a, b);
}

std::optional<ConfigSchemaEntry> Config::findSchemaEntry(const std::string& key) const {
    auto it = std::find_if(schema.begin(), schema.end(),
        [&key](const ConfigSchemaEntry& e) { return e.key == key; });
    if (it != schema.end()) {
        return *it;
    }
    return std::nullopt;
}

bool Config::hasSchema(const std::string& key) const {
    return findSchemaEntry(key).has_value();
}

template<typename T>
T Config::getDefaultOr(const std::string& key, const T& defaultValue) const {
    auto entry = findSchemaEntry(key);
    if (entry) {
        // Try to convert default value to requested type
        return std::visit([&](auto&& v) -> T {
            using V = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<V, T>) {
                return v;
            } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
                return static_cast<T>(v);
            } else if constexpr (std::is_same_v<V, std::string> && std::is_same_v<T, std::string>) {
                return v;
            } else {
                return defaultValue;
            }
        }, entry->defaultValue);
    }
    return defaultValue;
}

void Config::addSchemaEntry(const ConfigSchemaEntry& entry) {
    // Check if entry already exists
    auto it = std::find_if(schema.begin(), schema.end(),
        [&entry](const ConfigSchemaEntry& e) { return e.key == entry.key; });n
    if (it != schema.end()) {
        *it = entry;  // Update existing entry
    } else {
        schema.push_back(entry);
    }
    
    // Apply default if not already set
    if (!has(entry.key)) {
        applyDefault(entry);
    }
}

bool Config::isModified(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;  // Not in entries, using default
    }
    
    // Check if value differs from default
    auto schemaEntry = findSchemaEntry(key);
    if (schemaEntry && isValueEqual(it->value, schemaEntry->defaultValue)) {
        return false;
    }
    
    return true;
}

ConfigSource Config::getSource(const std::string& key) const {
// Config.cpp - Implementation of enhanced configuration system with validation and schema
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <iostream>
#include <cmath>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()), schema(nlm::schemas::defaultConfigSchema) {
    // Apply default values from schema
    for (const auto& entry : schema) {
        // Set default values for all schema entries
        applyDefault(entry);
    }
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
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
        
        // Parse key=value pairs with support for quotes and different value types
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string valueStr = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (valueStr.size() >= 2 && 
                ((valueStr.front() == '"' && valueStr.back() == '"') ||
                 (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            // Parse value based on schema entry
            auto schemaEntry = findSchemaEntry(key);
            if (schemaEntry) {
                // Try to convert value based on default value type
                setValueFromString(key, valueStr, schemaEntry->defaultValue, ConfigSource::File);
            } else {
                // Try common value types
                if (std::regex_match(valueStr, std::regex("true|false"))) {
                    bool boolVal = (valueStr == "true");
                    set(key, ConfigValue(boolVal), ConfigSource::File);
                } else if (std::regex_match(valueStr, std::regex("-?\d+"))) {
                    int intVal = std::stoi(valueStr);
                    set(key, ConfigValue(intVal), ConfigSource::File);
                } else if (std::regex_match(valueStr, std::regex("-?\d+\.?\d*"))) {
                    double doubleVal = std::stod(valueStr);
                    set(key, ConfigValue(doubleVal), ConfigSource::File);
                } else {
                    // Default to string
                    set(key, ConfigValue(valueStr), ConfigSource::File);
                }
            }
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
                setValueFromString(key, value, ConfigValue(""), ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            setValueFromString(key, value, ConfigValue(""), ConfigSource::CommandLine);
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
        // Only save entries that have a non-default value
        if (isModified(entry.key)) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_arithmetic_v<T>) {
                    file << arg;
                } else {
                    file << "\"" << arg << "\"";
                }
            }, entry.value);
            file << "\n\n";
        }
    }
    
    return true;
}

void Config::setValueFromString(const std::string& key, const std::string& valueStr, 
                               const ConfigValue& defaultValue, ConfigSource source) {
    // Try to match against schema entry
    auto schemaEntry = findSchemaEntry(key);
    if (schemaEntry) {
        setTypedValue(key, valueStr, schemaEntry->defaultValue, source);
    } else {
        // Try common types in order
        if (std::regex_match(valueStr, std::regex("true|false"))) {
            bool boolVal = (valueStr == "true");
            set(key, ConfigValue(boolVal), source);
        } else if (std::regex_match(valueStr, std::regex("-?\d+"))) {
            int intVal = std::stoi(valueStr);
            set(key, ConfigValue(intVal), source);
        } else if (std::regex_match(valueStr, std::regex("-?\d+\.?\d*"))) {
            double doubleVal = std::stod(valueStr);
            set(key, ConfigValue(doubleVal), source);
        } else {
            set(key, ConfigValue(valueStr), source);
        }
    }
}

void Config::setTypedValue(const std::string& key, const std::string& valueStr, 
                          const ConfigValue& defaultValue, ConfigSource source) {
    std::visit([&](auto&& defaultVal) {
        using T = std::decay_t<decltype(defaultVal)>;
        if constexpr (std::is_same_v<T, bool>) {
            bool val = (valueStr == "true");
            set(key, ConfigValue(val), source);
        } else if constexpr (std::is_same_v<T, int>) {
            int val = std::stoi(valueStr);
            set(key, ConfigValue(val), source);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            int64_t val = std::stoll(valueStr);
            set(key, ConfigValue(val), source);
        } else if constexpr (std::is_same_v<T, double>) {
            double val = std::stod(valueStr);
            set(key, ConfigValue(val), source);
        } else if constexpr (std::is_same_v<T, float>) {
            float val = std::stof(valueStr);
            set(key, ConfigValue(val), source);
        } else if constexpr (std::is_same_v<T, std::string>) {
            set(key, ConfigValue(valueStr), source);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            // Parse comma-separated list
            std::vector<int> vec;
            std::stringstream ss(valueStr);
            std::string item;
            while (std::getline(ss, item, ',')) {
                item = trim(item);
                if (!item.empty()) {
                    vec.push_back(std::stoi(item));
                }
            }
            set(key, ConfigValue(vec), source);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            // Parse comma-separated list
            std::vector<double> vec;
            std::stringstream ss(valueStr);
            std::string item;
            while (std::getline(ss, item, ',')) {
                item = trim(item);
                if (!item.empty()) {
                    vec.push_back(std::stod(item));
                }
            }
            set(key, ConfigValue(vec), source);
        }
    }, defaultValue);
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

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
    // Re-apply schema defaults after clear
    for (const auto& entry : schema) {
        applyDefault(entry);
    }
}

ValidationResult Config::validateValue(const std::string& key, const ConfigValue& value) const {
    auto schemaEntry = findSchemaEntry(key);
    if (!schemaEntry) {
        return ValidationResult(true, "No schema entry for key");
    }
    
    if (schemaEntry->validator) {
        return schemaEntry->validator(value);
    }
    
    // Range validation for numeric values
    if (key == "brain.synapse_density" || key == "brain.connection_probability") {
        if (std::holds_alternative<double>(value) || std::holds_alternative<float>(value)) {
            double val = std::visit([](auto&& v) { return static_cast<double>(v); }, value);
            if (val < 0.0 || val > 1.0) {
                return ValidationResult(false, key + " must be between 0.0 and 1.0");
            }
        }
    } else if (key == "brain.neuron_count") {
        if (std::holds_alternative<int64_t>(value) || std::holds_alternative<int>(value)) {
            int64_t val = std::visit([](auto&& v) { return static_cast<int64_t>(v); }, value);
            if (val <= 0) {
                return ValidationResult(false, "brain.neuron_count must be positive");
            }
        }
    }
    
    return ValidationResult(true, "");
}

std::vector<ValidationError> Config::validate() const {
    std::vector<ValidationError> errors;
    
    for (const auto& entry : pImpl->entries) {
        ValidationResult result = validateValue(entry.key, entry.value);
        if (!result.valid) {
            errors.emplace_back(entry.key, result.message);
        }
    }
    
    return errors;
}

bool Config::isValid() const {
    return validate().empty();
}

std::vector<ValidationError> Config::getValidationErrors() const {
    return validate();
}

ConfigSummary Config::getSummary() const {
    ConfigSummary summary;
    summary.totalEntries = pImpl->entries.size();
    
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::File:
                summary.fileEntries++;
                break;
            case ConfigSource::CommandLine:
                summary.commandLineEntries++;
                break;
            case ConfigSource::Runtime:
                summary.runtimeEntries++;
                break;
            case ConfigSource::Default:
                // Default entries are not stored in entries
                break;
        }
        
        if (entry.source != ConfigSource::Default) {
            summary.overriddenKeys.push_back(entry.key);
        }
    }
    
    return summary;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    ConfigSummary summary = getSummary();
    oss << "  File: " << summary.fileEntries << " entries\n";
    oss << "  Command Line: " << summary.commandLineEntries << " entries\n";
    oss << "  Runtime: " << summary.runtimeEntries << " entries\n";
    oss << "  Overrides: " << summary.overriddenKeys.size() << " keys\n";
    
    oss << "\nEntries:\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    
    return oss.str();
}

void Config::applySchema(const std::vector<ConfigSchemaEntry>& newSchema) {
    // Store old entries for comparison
    std::vector<ConfigEntry> oldEntries;
    oldEntries.swap(pImpl->entries);
    
    // Update schema
    schema = newSchema;
    
    // Apply defaults for all schema entries
    for (const auto& entry : schema) {
        applyDefault(entry);
    }
    
    // Merge old entries, preserving non-default values
    for (const auto& oldEntry : oldEntries) {
        auto schemaEntry = findSchemaEntry(oldEntry.key);
        if (schemaEntry) {
            // Check if this is a default value (same as schema default)
            if (!isValueEqual(oldEntry.value, schemaEntry->defaultValue)) {
                // Keep the old value since it's different from default
                set(oldEntry.key, oldEntry.value, oldEntry.source);
            } else if (isModified(oldEntry.key)) {
                // Keep the old value if it was explicitly set
                set(oldEntry.key, oldEntry.value, oldEntry.source);
            }
        } else {
            // Old entry without schema, keep it
            set(oldEntry.key, oldEntry.value, oldEntry.source);
        }
    }
}

void Config::applyDefault(const ConfigSchemaEntry& entry) {
    if (!has(entry.key)) {
        set(entry.key, entry.defaultValue, ConfigSource::Default);
    }
}

bool Config::isValueEqual(const ConfigValue& a, const ConfigValue& b) const {
    return std::visit([](auto&& aVal, auto&& bVal) {
        using A = std::decay_t<decltype(aVal)>;
        using B = std::decay_t<decltype(bVal)>;
        if constexpr (std::is_same_v<A, B>) {
            return aVal == bVal;
        } else if constexpr (std::is_arithmetic_v<A> && std::is_arithmetic_v<B>) {
            return static_cast<double>(aVal) == static_cast<double>(bVal);
        } else if constexpr (std::is_same_v<A, std::string> && std::is_same_v<B, std::string>) {
            return aVal == bVal;
        } else if constexpr (std::is_same_v<A, std::vector<int>> && std::is_same_v<B, std::vector<int>>) {
            return aVal == bVal;
        } else if constexpr (std::is_same_v<A, std::vector<double>> && std::is_same_v<B, std::vector<double>>) {
            return aVal == bVal;
        } else {
            return false;
        }
    }, a, b);
}

std::optional<ConfigSchemaEntry> Config::findSchemaEntry(const std::string& key) const {
    auto it = std::find_if(schema.begin(), schema.end(),
        [&key](const ConfigSchemaEntry& e) { return e.key == key; });
    if (it != schema.end()) {
        return *it;
    }
    return std::nullopt;
}

bool Config::hasSchema(const std::string& key) const {
    return findSchemaEntry(key).has_value();
}

template<typename T>
T Config::getDefaultOr(const std::string& key, const T& defaultValue) const {
    auto entry = findSchemaEntry(key);
    if (entry) {
        // Try to convert default value to requested type
        return std::visit([&](auto&& v) -> T {
            using V = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<V, T>) {
                return v;
            } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
                return static_cast<T>(v);
            } else if constexpr (std::is_same_v<V, std::string> && std::is_same_v<T, std::string>) {
                return v;
            } else {
                return defaultValue;
            }
        }, entry->defaultValue);
    }
    return defaultValue;
}

void Config::addSchemaEntry(const ConfigSchemaEntry& entry) {
    // Check if entry already exists
    auto it = std::find_if(schema.begin(), schema.end(),
        [&entry](const ConfigSchemaEntry& e) { return e.key == entry.key; });

    if (it != schema.end()) {
        *it = entry;  // Update existing entry
    } else {
        schema.push_back(entry);
    }
    
    // Apply default if not already set
    if (!has(entry.key)) {
        applyDefault(entry);
    }
}

bool Config::isModified(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;  // Not in entries, using default
    }
    
    // Check if value differs from default
    auto schemaEntry = findSchemaEntry(key);
    if (schemaEntry && isValueEqual(it->value, schemaEntry->defaultValue)) {
        return false;
    }
    
    return true;
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->source;
    }
    
    // Default source for values not in entries
    return ConfigSource::Default;
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Config convenience function implementations
namespace nlm::config {
    std::shared_ptr<Config> createDevConfig() {
        auto config = std::make_shared<Config>();
        
        // Development settings: more neurons, easier to debug
        config->set("brain.neuron_count", static_cast<int64_t>(2000));
        config->set("brain.synapse_density", 0.2f);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation.timestep", 0.001);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        
        // Enable learning features
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.dopamine.scale", 1.0f);
        
        return config;
    }
    
    std::shared_ptr<Config> createProductionConfig() {
        auto config = std::make_shared<Config>();
        
        // Production settings: optimized for performance
        config->set("brain.neuron_count", static_cast<int64_t>(1000));
        config->set("brain.synapse_density", 0.1f);
        config->set("random_seed", static_cast<int64_t>(12345));
        config->set("simulation.timestep", 0.001);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        
        // All learning features enabled
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.dopamine.scale", 1.0f);
        
        // Set up checkpointing for long runs
        config->set("checkpoint_dir", std::string("./checkpoints"));
        config->set("replay_interval", static_cast<int64_t>(100));
        config->set("consolidation_interval", static_cast<int64_t>(1000));
        
        return config;
    }
    
    std::shared_ptr<Config> createTestConfig() {
        auto config = std::make_shared<Config>();
        
        // Testing settings: small, fast, reproducible
        config->set("brain.neuron_count", static_cast<int64_t>(100));
        config->set("brain.synapse_density", 0.05f);
        config->set("random_seed", static_cast<int64_t>(999));
        config->set("simulation.timestep", 0.001);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        
        // Disable curiosity for controlled tests
        config->set("neuromod.curiosity.enable", false);
        config->set("neuromod.novelty.enable", false);
        
        return config;
    }
    
    std::shared_ptr<Config> createMinimalConfig() {
        auto config = std::make_shared<Config>();
        
        // Minimal configuration: just enough to run
        config->set("brain.neuron_count", static_cast<int64_t>(50));
        config->set("brain.synapse_density", 0.01f);
        config->set("random_seed", static_cast<int64_t>(1));
        
        return config;
    }
    
    std::shared_ptr<Config> createDemoConfig() {
        auto config = std::make_shared<Config>();
        
        // Demo settings: good balance of activity and learning
        config->set("brain.neuron_count", static_cast<int64_t>(500));
        config->set("brain.synapse_density", 0.1f);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation.timestep", 0.001);
        
        // Enable all features for demo
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.dopamine.scale", 1.0f);
        
        return config;
    }
}

} // namespace nlm
