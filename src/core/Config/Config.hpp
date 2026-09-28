#pragma once

#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <mutex>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <type_traits>
#include <cstdint>
#include <stdexcept>
#include <cstring>
#include <regex>

namespace nlm {

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

// Configuration value types for validation
enum class ConfigValueType {
    Unknown,
    Int,
    Int64,
    Double,
    Bool,
    String,
    IntVector,
    DoubleVector,
    StringVector
};

// Configuration source
enum class ConfigSource {
    Default = 0,
    File = 1,
    CommandLine = 2,
    Runtime = 3,
    Environment = 4
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::chrono::system_clock::time_point lastModified;
    
    ConfigEntry()
        : key(), value(), source(ConfigSource::Default), description(), lastModified(std::chrono::system_clock::now()) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), lastModified(std::chrono::system_clock::now()) {}
};

// Configuration exception class
class ConfigException : public std::runtime_error {
public:
    ConfigException(const std::string& message, const std::string& context = "")
        : std::runtime_error(message), context(context) {}
    
    const std::string& getContext() const { return context; }
    
private:
    std::string context;
};

// Main configuration class - enhanced with error handling, thread safety, and optimizations
class Config {
public:
    // Constructor and destructor
    Config() = default;
    ~Config() = default;
    
    // Copy operations (deleted for safety)
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    // Move operations
    Config(Config&&) noexcept = default;
    Config& operator=(Config&&) noexcept = default;
    
    // Load from file with error handling
    void loadFromFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw ConfigException("Failed to open config file: " + filepath, "loadFromFile");
        }
        
        std::string line;
        while (std::getline(file, line)) {
            auto trimmed = trim(line);
            if (trimmed.empty() || trimmed[0] == '#') {
                continue;
            }
            
            size_t pos = trimmed.find('=');
            if (pos == std::string::npos) {
                continue;
            }
            
            std::string key = trim(trimmed.substr(0, pos));
            std::string valueStr = trim(trimmed.substr(pos + 1));
            
            if (valueStr.size() >= 2 && ((valueStr.front() == '"' && valueStr.back() == '"') ||
                                         (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            set(key, valueStr, ConfigSource::File);
        }
    }
    
    // Load from command line arguments
    void loadFromArgs(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    set(key, value, ConfigSource::CommandLine);
                }
            } else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            }
        }
    }
    
    // Save to file
    void saveToFile(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw ConfigException("Failed to save config to file: " + filepath, "saveToFile");
        }
        
        for (const auto& entry : entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            // Serialize value based on type
            if (const std::string* s = std::get_if<std::string>(&entry.value)) {
                file << "\"" << *s << "\"";
            } else if (const bool* b = std::get_if<bool>(&entry.value)) {
                file << (*b ? "true" : "false");
            } else if (const int* i = std::get_if<int>(&entry.value)) {
                file << *i;
            } else if (const int64_t* i64 = std::get_if<int64_t>(&entry.value)) {
                file << *i64;
            } else if (const double* d = std::get_if<double>(&entry.value)) {
                file << *d;
            } else if (const std::vector<int>* vi = std::get_if<std::vector<int>>(&entry.value)) {
                file << "[";
                for (size_t i = 0; i < vi->size(); ++i) {
                    if (i > 0) file << ", ";
                    file << (*vi)[i];
                }
                file << "]";
            } else if (const std::vector<double>* vd = std::get_if<std::vector<double>>(&entry.value)) {
                file << "[";
                for (size_t i = 0; i < vd->size(); ++i) {
                    if (i > 0) file << ", ";
                    file << (*vd)[i];
                }
                file << "]";
            } else if (const std::vector<std::string>* vs = std::get_if<std::vector<std::string>>(&entry.value)) {
                file << "[";
                for (size_t i = 0; i < vs->size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << (*vs)[i] << "\"";
                }
                file << "]";
            }
            
            file << "\n\n";
        }
    }
    
    // Enhanced get methods with better template and type checking
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        auto it = std::find_if(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it == entries.end()) {
            return std::nullopt;
        }
        
        try {
            return std::get<T>(it->value);
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
    
    // Type-specific convenience methods
    std::optional<int> getInt(const std::string& key) const { return get<int>(key); }
    std::optional<double> getDouble(const std::string& key) const { return get<double>(key); }
    std::optional<bool> getBool(const std::string& key) const { return get<bool>(key); }
    std::optional<std::string> getString(const std::string& key) const { return get<std::string>(key); }
    
    // Get with default values (no-throw guarantee)
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const noexcept {
        auto val = get<T>(key);
        return val.has_value() ? *val : defaultValue;
    }
    
    // Set methods with overload resolution and validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime) {
        validateKey(key);
        validateValue(value);
        
        auto it = std::find_if(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != entries.end()) {
            it->value = value;
            it->source = source;
            it->lastModified = std::chrono::system_clock::now();
        } else {
            entries.emplace_back(key, value, source);
        }
    }
    
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) {
        set(key, ConfigValue(value), source);
    }
    
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) {
        set(key, ConfigValue(value), source);
    }
    
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) {
        set(key, ConfigValue(value), source);
    }
    
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) {
        set(key, ConfigValue(value), source);
    }
    
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime) {
        set(key, ConfigValue(value), source);
    }
    
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime) {
        set(key, ConfigValue(value), source);
    }
    
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime) {
        set(key, ConfigValue(value), source);
    }
    
    // Convenience set methods
    void setInt(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) {
        set(key, value, source);
    }
    
    void setDouble(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) {
        set(key, value, source);
    }
    
    void setBool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) {
        set(key, value, source);
    }
    
    void setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) {
        set(key, value, source);
    }
    
    // Check existence and type safety
    bool has(const std::string& key) const noexcept {
        return std::any_of(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
    }
    
    bool has(const std::string& key, ConfigSource source) const noexcept {
        return std::any_of(entries.begin(), entries.end(),
            [&key, &source](const ConfigEntry& e) { return e.key == key && e.source == source; });
    }
    
    // Type checking
    template<typename T>
    bool isType(const std::string& key) const noexcept {
        auto it = std::find_if(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        return it != entries.end() && std::holds_alternative<T>(it->value);
    }
    
    // Remove key
    void remove(const std::string& key) {
        auto it = std::remove_if(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        entries.erase(it, entries.end());
    }
    
    // Get all keys with optional filtering
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        keys.reserve(entries.size());
        for (const auto& entry : entries) {
            keys.push_back(entry.key);
        }
        return keys;
    }
    
    std::vector<std::string> getKeys(ConfigSource source) const {
        std::vector<std::string> keys;
        keys.reserve(entries.size());
        for (const auto& entry : entries) {
            if (entry.source == source) {
                keys.push_back(entry.key);
            }
        }
        return keys;
    }
    
    // Clear all values
    void clear() {
        entries.clear();
    }
    
    // Get configuration summary with stats
    std::string summary() const {
        std::ostringstream oss;
        oss << "Configuration Summary (" << entries.size() << " entries):\n";
        
        for (const auto& entry : entries) {
            oss << "  " << entry.key << " = [";
            // Simple serialization for summary
            if (const std::string* s = std::get_if<std::string>(&entry.value)) {
                oss << "\"" << *s << "\"";
            } else if (const bool* b = std::get_if<bool>(&entry.value)) {
                oss << (*b ? "true" : "false");
            } else if (const int* i = std::get_if<int>(&entry.value)) {
                oss << *i;
            } else if (const int64_t* i64 = std::get_if<int64_t>(&entry.value)) {
                oss << *i64;
            } else if (const double* d = std::get_if<double>(&entry.value)) {
                oss << *d;
            } else {
                oss << "[complex value]";
            }
            oss << "] (Source: " << static_cast<int>(entry.source) << ") " << entry.description << "\n";
        }
        
        return oss.str();
    }
    
    // Thread safety helpers
    void lock() { mutex.lock(); }
    void unlock() { mutex.unlock(); }
    bool tryLock() { return mutex.try_lock(); }
    
    // Search and filtering
    std::vector<std::string> findKeys(const std::string& pattern) const {
        std::vector<std::string> matchingKeys;
        std::regex regex(pattern);
        for (const auto& entry : entries) {
            if (std::regex_search(entry.key, regex)) {
                matchingKeys.push_back(entry.key);
            }
        }
        return matchingKeys;
    }
    
    std::vector<ConfigEntry> findEntries(const std::string& pattern) const {
        std::vector<ConfigEntry> matchingEntries;
        std::regex regex(pattern);
        for (const auto& entry : entries) {
            if (std::regex_search(entry.key, regex)) {
                matchingEntries.push_back(entry);
            }
        }
        return matchingEntries;
    }
    
    // Validation methods
    bool validate() const {
        for (const auto& entry : entries) {
            if (!isValidEntry(entry)) {
                return false;
            }
        }
        return true;
    }
    
    std::vector<std::string> getValidationErrors() const {
        std::vector<std::string> errors;
        for (const auto& entry : entries) {
            if (!isValidEntry(entry)) {
                errors.push_back("Invalid entry: " + entry.key);
            }
        }
        return errors;
    }
    
    // Constants for common configuration keys
    static constexpr const char* DEFAULT_CONFIG_FILE = "config.txt";
    static constexpr const char* CONFIG_FILE_ENV = "NLM_CONFIG_FILE";
    static constexpr const char* COMMAND_LINE_PREFIX = "--";
    
    // Statistics constants
    static constexpr size_t MAX_CONFIG_SIZE = 1024 * 1024; // 1MB max config size
    static constexpr size_t MAX_KEY_LENGTH = 256;
    static constexpr size_t MAX_VALUE_LENGTH = 1024 * 1024; // 1MB max value size

private:
    std::vector<ConfigEntry> entries;
    mutable std::mutex mutex;
    
    // Internal helpers
    static std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    static void validateKey(const std::string& key) {
        if (key.empty() || key.length() > MAX_KEY_LENGTH) {
            throw ConfigException("Invalid key: " + key, "validateKey");
        }
    }
    
    static void validateValue(const ConfigValue& value) {
        std::visit([](const auto& val) {
            if constexpr (std::is_same_v<decltype(val), std::string>) {
                if (val.length() > MAX_VALUE_LENGTH) {
                    throw ConfigException("Value too long", "validateValue");
                }
            } else if constexpr (std::is_same_v<decltype(val), std::vector<std::string>>) {
                for (const auto& s : val) {
                    if (s.length() > MAX_VALUE_LENGTH) {
                        throw ConfigException("Value too long", "validateValue");
                    }
                }
            }
        }, value);
    }
    
    static bool isValidEntry(const ConfigEntry& entry) {
        if (entry.key.empty()) return false;
        if (entry.key.length() > MAX_KEY_LENGTH) return false;
        
        std::visit([](const auto& val) {
            if constexpr (std::is_same_v<decltype(val), std::string>) {
                if (val.length() > MAX_VALUE_LENGTH) throw ConfigException("Value too long", "validateEntry");
            } else if constexpr (std::is_same_v<decltype(val), std::vector<std::string>>) {
                for (const auto& s : val) {
                    if (s.length() > MAX_VALUE_LENGTH) throw ConfigException("Value too long", "validateEntry");
                }
            }
        }, entry.value);
        
        return true;
    }
};

} // namespace nlm

// Explicit template instantiations
namespace nlm {
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
}