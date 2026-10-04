#pragma once

#include <string>
#include <variant>
#include <vector>
#include <unordered_map>
#include <optional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <json/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

// Configuration source enumeration
enum class ConfigSource {
    Default = 0,
    File = 1,
    CommandLine = 2
};

// Configuration entry structure
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
};

// Forward declarations for parser classes
class JsonConfigParser;
class YamlConfigParser;

// Configuration class with comprehensive parser support
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load configuration from file (JSON/YAML/INI)
    bool loadFromFile(const std::string& filepath);
    
    // Load configuration from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save configuration to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Set configuration values
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Default);
    
    // Specialized setters for common types
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Default);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Default);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Default);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Default);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Default);
    
    // Get configuration values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Get configuration values with defaults
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Check if configuration key exists
    bool has(const std::string& key) const;
    
    // Remove configuration key
    void remove(const std::string& key);
    
    // Get all configuration keys
    std::vector<std::string> getKeys() const;
    
    // Clear all configuration
    void clear();
    
    // Generate configuration summary
    std::string summary() const;
    
    // Configuration validation
    bool validate() const;
    
    // Set validation callback
    using ValidationCallback = std::function<bool(const ConfigEntry&)>;
    void setValidationCallback(const ValidationCallback& callback);
    
    // Merge configurations
    void merge(const Config& other, ConfigSource source = ConfigSource::File);
    
    // Get configuration entry count
    size_t getEntryCount() const;
    
    // Get configuration by index (for iteration)
    const ConfigEntry& getEntry(size_t index) const;
    
    // Configuration parser interface
    using ParserPtr = std::unique_ptr<class ConfigParserBase>;
    void registerParser(const std::string& extension, ParserPtr parser);
    
    // Detect file type and parse accordingly
    bool loadFromFileAuto(const std::string& filepath);
    
    // Configuration schema validation
    class Schema {
    public:
        using ValidationResult = std::vector<std::string>;
        
        virtual ~Schema() = default;
        virtual ValidationResult validate(const Config& config) const = 0;
        virtual std::string getName() const = 0;
    };
    
    // Register schema
    void registerSchema(std::unique_ptr<Schema> schema);
    bool validateSchema() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    std::string trim(const std::string& str) const;
    std::string toLower(const std::string& str) const;
    std::string getFileExtension(const std::string& filepath) const;
    
    // Parser base class
    class ConfigParserBase {
    public:
        virtual ~ConfigParserBase() = default;
        virtual bool parse(const std::string& content, Config& config) = 0;
        virtual bool serialize(const Config& config, std::string& output) const = 0;
    };
    
    // File parsers
    class JsonParser : public ConfigParserBase {
    public:
        bool parse(const std::string& content, Config& config) override;
        bool serialize(const Config& config, std::string& output) const override;
    };
    
    class YamlParser : public ConfigParserBase {
    public:
        bool parse(const std::string& content, Config& config) override;
        bool serialize(const Config& config, std::string& output) const override;
    };
    
    class IniParser : public ConfigParserBase {
    public:
        bool parse(const std::string& content, Config& config) override;
        bool serialize(const Config& config, std::string& output) const override;
    };
};

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
