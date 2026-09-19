#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <variant>
#include <optional>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "NLMException.hpp"
#include "ConfigEntry.h"
#include "ConfigSource.h"

namespace nlm {

// JSON/YAML parsing forward declarations
namespace Json {
    class Parser;
}

namespace Yaml {
    class Parser;
}

// Enhanced configuration system with JSON/YAML support and validation
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (supports JSON and YAML formats)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (supports JSON and YAML formats)
    bool saveToFile(const std::string& filepath, bool prettyPrint = false) const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Enhanced type conversion with validation
    template<typename T>
    bool setTyped(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Configuration validation
    bool validate() const;
    std::string validationErrors() const;
    
    // Configuration merge operations
    void merge(const Config& other, bool overwriteExisting = true);
    void mergeFromFile(const std::string& filepath);
    
    // Schema validation support
    void setSchema(const std::string& schemaJson);
    bool validateWithSchema() const;
    
    // Configuration aliases and shortcuts
    void setAlias(const std::string& alias, const std::string& realKey);
    std::optional<ConfigValue> getAlias(const std::string& alias) const;
    
    // Configuration groups for organization
    void setGroup(const std::string& key, const std::string& group);
    std::vector<std::string> getKeysInGroup(const std::string& group) const;
    void removeGroup(const std::string& group);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // JSON/YAML parsing helpers
    bool parseJson(const std::string& content);
    bool parseYaml(const std::string& content);
    std::string toJsonString(bool prettyPrint = false) const;
    std::string toYamlString() const;
    
    // Validation helpers
    void validateTypes() const;
    void checkRequiredFields() const;
    
    // Error handling
    void reportError(const std::string& error);
};

} // namespace nlm
