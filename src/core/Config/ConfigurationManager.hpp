#pragma once
#include "../Exceptions/NLMErrors.hpp"

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <mutex>
#include <filesystem>
#include <functional>
#include <chrono>
#include <any>
#include <stdexcept>
#include <regex>
#include <type_traits>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <thread>
#include <atomic>

namespace nlm {

// Forward declarations
class ConfigurationManager;
class ConfigurationSection;

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

// Configuration source tracking
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Environment,
    Runtime,
    Template
};

// Configuration validation result
struct ValidationResult {
    bool isValid;
    std::string errorMessage;
    std::string fieldName;
    
    ValidationResult() : isValid(true) {}
    ValidationResult(bool valid, const std::string& error, const std::string& field)
        : isValid(valid), errorMessage(error), fieldName(field) {}
};

// Configuration schema definition
struct ConfigSchema {
    std::string type; // "int", "double", "string", "bool", "array"
    std::string defaultValue;
    bool required = false;
    std::string validationPattern;
    std::string description;
    std::vector<std::string> allowedValues;
    double minValue = std::numeric_limits<double>::lowest();
    double maxValue = std::numeric_limits<double>::max();
    int minSize = 0;
    int maxSize = std::numeric_limits<int>::max();
    std::string section;
};

// Configuration entry with inheritance and source tracking
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string section;
    bool isLocked = false;
    std::string templateName;
    
    ConfigEntry() : source(ConfigSource::Default) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Configuration template
class ConfigurationTemplate {
public:
    std::string name;
    std::map<std::string, ConfigSchema> schema;
    bool inheritsFromDefault = false;
    
    ConfigurationTemplate() = default;
    explicit ConfigurationTemplate(const std::string& name) : name(name) {}
    
    void addField(const std::string& key, const ConfigSchema& field) {
        schema[key] = field;
    }
    
    bool validate(const std::map<std::string, ConfigValue>& values) const {
        for (const auto& [key, schema] : schema) {
            auto it = values.find(key);
            if (schema.required && it == values.end()) {
                return false;
            }
            if (it != values.end()) {
                const auto& value = it->second;
                if (!validateField(value, schema)) {
                    return false;
                }
            }
        }
        return true;
    }
    
private:
    bool validateField(const ConfigValue& value, const ConfigSchema& schema) const {
        // Type-specific validation
        if (schema.type == "int") {
            if (!std::holds_alternative<int>(value)) return false;
            if (std::holds_alternative<int>(value)) {
                double val = std::get<int>(value);
                return val >= schema.minValue && val <= schema.maxValue;
            }
        } else if (schema.type == "double") {
            if (!std::holds_alternative<double>(value)) return false;
            if (std::holds_alternative<double>(value)) {
                double val = std::get<double>(value);
                return val >= schema.minValue && val <= schema.maxValue;
            }
        } else if (schema.type == "string") {
            if (!std::holds_alternative<std::string>(value)) return false;
            if (std::holds_alternative<std::string>(value)) {
                const std::string& val = std::get<std::string>(value);
                if (!schema.validationPattern.empty()) {
                    std::regex pattern(schema.validationPattern);
                    return std::regex_match(val, pattern);
                }
                if (!schema.allowedValues.empty()) {
                    return std::find(schema.allowedValues.begin(), schema.allowedValues.end(), val) != schema.allowedValues.end();
                }
            }
        } else if (schema.type == "bool") {
            return std::holds_alternative<bool>(value);
        } else if (schema.type == "array") {
            return std::holds_alternative<std::vector<int>>(value) ||
                   std::holds_alternative<std::vector<double>>(value) ||
                   std::holds_alternative<std::vector<std::string>>(value);
        }
        return true;
    }
};

// Nested configuration section
class ConfigurationSection {
public:
    std::string name;
    std::map<std::string, ConfigEntry> entries;
    std::map<std::string, ConfigurationSection> subsections;
    bool isLocked = false;
    
    ConfigurationSection() = default;
    explicit ConfigurationSection(const std::string& sectionName) : name(sectionName) {}
    
    void addEntry(const std::string& key, const ConfigEntry& entry) {
        entries[key] = entry;
    }
    
    bool hasEntry(const std::string& key) const {
        return entries.find(key) != entries.end();
    }
    
    const ConfigEntry* getEntry(const std::string& key) const {
        auto it = entries.find(key);
        return it != entries.end() ? &it->second : nullptr;
    }
    
    ConfigurationSection* getSubsection(const std::string& name) {
        auto it = subsections.find(name);
        return it != subsections.end() ? &it->second : nullptr;
    }
    
    const ConfigurationSection* getSubsection(const std::string& name) const {
        auto it = subsections.find(name);
        return it != subsections.end() ? &it->second : nullptr;
    }
    
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        for (const auto& pair : entries) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    void mergeFrom(const ConfigurationSection& other, bool override = true) {
        for (const auto& pair : other.entries) {
            if (override || !hasEntry(pair.first)) {
                entries[pair.first] = pair.second;
            }
        }
        for (const auto& pair : other.subsections) {
            auto* subsection = getSubsection(pair.first);
            if (subsection) {
                subsection->mergeFrom(pair.second, override);
            } else {
                subsections[pair.first] = pair.second;
            }
        }
    }
    
    std::string summary(int indent = 0) const {
        std::string result;
        std::string indentStr(indent, ' ');
        
        for (const auto& [key, entry] : entries) {
            result += indentStr + "  " + key + " = ";
            std::visit([&result](const auto& val) {
                if constexpr (std::is_same_v<decltype(val), std::string>) {
                    result += "\"" + val + "\"";
                } else {
                    result += std::to_string(val);
                }
            }, entry.value);
            result += " (" + std::to_string(static_cast<int>(entry.source)) + ")\n";
        }
        
        for (const auto& [name, subsection] : subsections) {
            result += indentStr + name + ":\n";
            result += subsection.summary(indent + 2);
        }
        
        return result;
    }
};

// Main ConfigurationManager class
class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager();
    
    // Disable copying, enable moving
    ConfigurationManager(const ConfigurationManager&) = delete;
    ConfigurationManager& operator=(const ConfigurationManager&) = delete;
    ConfigurationManager(ConfigurationManager&&) noexcept = default;
    ConfigurationManager& operator=(ConfigurationManager&&) noexcept = default;
    
    // Global instance management
    static ConfigurationManager& getGlobal();
    static void setGlobal(std::shared_ptr<ConfigurationManager> manager);
    
    // Configuration templates
    void addTemplate(const std::string& name, const ConfigurationTemplate& template_);
    void removeTemplate(const std::string& name);
    bool hasTemplate(const std::string& name) const;
    const ConfigurationTemplate* getTemplate(const std::string& name) const;
    
    // Section management
    ConfigurationSection& getSection(const std::string& name);
    const ConfigurationSection& getSection(const std::string& name) const;
    bool hasSection(const std::string& name) const;
    void removeSection(const std::string& name);
    std::vector<std::string> getSectionNames() const;
    
    // Configuration value operations (type-safe)
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    std::optional<T> get(const std::string& section, const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    template<typename T>
    T getOr(const std::string& section, const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& section, const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Convenience setters for primitive types
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    void set(const std::string& section, const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& section, const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& section, const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& section, const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    // Environment variable support
    void loadFromEnvironment(const std::string& prefix = "NLM_");
    void addEnvironmentMapping(const std::string& key, const std::string& envVar);
    
    // Command line support
    bool loadFromArgs(int argc, char** argv);
    
    // File I/O
    bool loadFromFile(const std::string& filepath);
    bool loadFromFile(const std::filesystem::path& filepath);
    bool saveToFile(const std::string& filepath) const;
    bool saveToFile(const std::filesystem::path& filepath) const;
    
    // File format support
    bool loadFromJSON(const std::string& content);
    bool loadFromYAML(const std::string& content);
    bool loadFromINI(const std::string& content);
    
    // Validation
    std::vector<ValidationResult> validate() const;
    bool isValid() const;
    std::string getValidationError() const;
    
    // Templates and schemas
    void applyTemplate(const std::string& templateName, const std::map<std::string, ConfigValue>& values);
    bool validateAgainstTemplate(const std::string& templateName) const;
    
    // Source tracking
    ConfigSource getSource(const std::string& key) const;
    ConfigSource getSource(const std::string& section, const std::string& key) const;
    
    // Configuration control
    void lock(const std::string& key);
    void lock(const std::string& section, const std::string& key);
    void unlock(const std::string& key);
    void unlock(const std::string& section, const std::string& key);
    bool isLocked(const std::string& key) const;
    bool isLocked(const std::string& section, const std::string& key) const;
    
    // Configuration management
    void remove(const std::string& key);
    void remove(const std::string& section, const std::string& key);
    bool has(const std::string& key) const;
    bool has(const std::string& section, const std::string& key) const;
    
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeys(const std::string& section) const;
    
    void clear();
    void clear(const std::string& section);
    
    // Reloading and persistence
    void setAutoReload(bool enable, std::chrono::milliseconds interval = std::chrono::milliseconds(5000));
    void stopAutoReload();
    std::chrono::milliseconds getReloadInterval() const;
    
    // Backward compatibility
    template<typename T>
    std::optional<T> legacyGet(const std::string& key) const {
        return get<T>(key);
    }
    
    template<typename T>
    T legacyGetOr(const std::string& key, const T& defaultValue) const {
        return getOr<T>(key, defaultValue);
    }
    
    // Configuration summary
    std::string summary() const;
    std::string summary(const std::string& section) const;
    
    // Thread safety
    void setThreadSafe(bool enable);
    bool isThreadSafe() const;
    
    // Configuration inheritance
    void setInheritance(bool enable);
    bool hasInheritance() const;
    void setDefaultFile(const std::string& filepath);
    bool loadDefaultConfiguration();
    
    // Debug and utilities
    size_t getEntryCount() const;
    size_t getSectionCount() const;
    std::string getVersion() const { return "1.0.0"; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    ConfigurationSection* getSectionPtr(const std::string& name);
    const ConfigurationSection* getSectionPtr(const std::string& name) const;
    ConfigEntry* getEntryPtr(const std::string& section, const std::string& key);
    const ConfigEntry* getEntryPtr(const std::string& section, const std::string& key) const;
    
    std::string normalizeKey(const std::string& key) const;
    std::string normalizeSection(const std::string& section) const;
    
    bool loadFromEnvironmentPrefix(const std::string& prefix, ConfigSource source);
    bool parseCommandLineArg(const std::string& arg, ConfigSource source);
    
    void mergeConfiguration(const ConfigurationManager& other, bool override = true);
    void applyEnvironmentVariables();
    
    std::string jsonToString(const ConfigValue& value) const;
    ConfigValue stringToJSON(const std::string& str) const;
    
    std::chrono::system_clock::time_point lastReloadTime;
    bool autoReloadEnabled;
    std::chrono::milliseconds reloadInterval;
    std::thread reloadThread;
    std::atomic<bool> reloadStopFlag;
    mutable std::mutex mutex;
    bool threadSafe;
    bool inheritanceEnabled;
    std::string defaultConfigFile;
    
    static std::shared_ptr<ConfigurationManager> globalManager;
    
    // JSON/YAML parsing helpers (simplified)
    bool parseJSONValue(const std::string& str, ConfigValue& outValue) const;
    bool parseYAMLValue(const std::string& str, ConfigValue& outValue) const;
    bool parseINIValue(const std::string& str, ConfigValue& outValue) const;
};