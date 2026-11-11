// Enhanced JSON/YAML Configuration File Support
// This file provides JSON and YAML configuration file parsing capabilities
// for the NLM configuration system, building upon the existing simple key=value format

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <filesystem>

namespace nlm {

// Forward declarations for parsers
class JsonConfigParser;
class YamlConfigParser;

// Enhanced configuration file format support
enum class ConfigFileFormat {
    Unknown,
    SimpleKeyValue,  // key=value format (legacy)
    JSON,            // JSON format
    YAML,            // YAML format
    TOML             // TOML format (future)
};

class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file with automatic format detection
    bool loadFromFile(const std::string& filepath);
    
    // Load from file with specified format
    bool loadFromFile(const std::string& filepath, ConfigFileFormat format);
    
    // Load from JSON string
    bool loadFromJson(const std::string& jsonString);
    
    // Load from YAML string
    bool loadFromYaml(const std::string& yamlString);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values (defined here so templates instantiate in every TU)
template<typename T>
    std::optional<T> get(const std::string& key) const;
    
template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Advanced configuration features
    
    // Validate configuration against schema
    bool validate() const;
    
    // Merge configuration with another
    void merge(const Config& other);
    
    // Get configuration statistics
    struct ConfigStats {
        size_t totalEntries;
        size_t entriesBySource[4]; // Default, File, CommandLine, Runtime
        std::vector<std::string> missingRequiredKeys;
        bool isValid;
        std::string validationError;
    };
    
    ConfigStats getStats() const;
    
    // Configuration validation
    bool validateRequiredKeys(const std::vector<std::string>& requiredKeys) const;
    
    // Get configuration file format detection results
    static ConfigFileFormat detectFileFormat(const std::string& filepath);
    
    // Set parser implementations (for dependency injection in tests)
    void setJsonParser(std::shared_ptr<JsonConfigParser> parser);
    void setYamlParser(std::shared_ptr<YamlConfigParser> parser);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    
    // Parser selection logic
    bool loadWithParser(const std::string& filepath, ConfigFileFormat format);
    
    // File format detection
    static bool isJsonFile(const std::string& content);
    static bool isYamlFile(const std::string& content);
    static bool isSimpleKeyValueFile(const std::string& content);
    
    // Error handling
    void setLastError(const std::string& error);
    std::string getLastError() const;
    
    std::string lastError;
};

// JSON Parser Interface
class JsonConfigParser {
public:
    virtual ~JsonConfigParser() = default;
    virtual bool parse(const std::string& jsonString, std::vector<ConfigEntry>& entries) = 0;
};

// Default JSON parser implementation (uses nlohmann::json if available)
class DefaultJsonConfigParser : public JsonConfigParser {
public:
    bool parse(const std::string& jsonString, std::vector<ConfigEntry>& entries) override;
};

// YAML Parser Interface
class YamlConfigParser {
public:
    virtual ~YamlConfigParser() = default;
    virtual bool parse(const std::string& yamlString, std::vector<ConfigEntry>& entries) = 0;
};

// Default YAML parser implementation (uses yaml-cpp if available)
class DefaultYamlConfigParser : public YamlConfigParser {
public:
    bool parse(const std::string& yamlString, std::vector<ConfigEntry>& entries) override;
};

// Parser factory for creating appropriate parsers
class ConfigParserFactory {
public:
    static std::shared_ptr<JsonConfigParser> createJsonParser();
    static std::shared_ptr<YamlConfigParser> createYamlParser();
    static ConfigFileFormat detectFormat(const std::string& filepath);
    static std::string getFormatExtension(ConfigFileFormat format);
};

} // namespace nlm
