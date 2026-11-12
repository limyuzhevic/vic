#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

// Enhanced configuration system with JSON/YAML support
// Replaces simple key=value parser with proper structured configuration

class Config {
public:
    // Configuration entry with type safety
    struct ConfigEntry {
        std::string key;
        std::string value;
        std::string description;
        enum class Type { STRING, INTEGER, DOUBLE, BOOLEAN };
        Type type;
        enum class Source { Default, File, CommandLine } source;
    };
    
    Config();
    ~Config();
    
    // Configuration loading from various sources
    bool loadFromFile(const std::string& filepath);  // JSON/YAML support
    bool loadFromJSON(const std::string& jsonContent);
    bool loadFromYAML(const std::string& yamlContent);
    bool loadFromArgs(int argc, char** argv);
    
    // Enhanced value setting with type conversion
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Default);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Default);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Default);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Default);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Default);
    
    // Enhanced value getting with type safety and defaults
    std::string getString(const std::string& key) const;
    int getInt(const std::string& key) const;
    int64_t getInt64(const std::string& key) const;
    double getDouble(const std::string& key) const;
    bool getBool(const std::string& key) const;
    
    // Type-safe getters with default values
    std::string getOrString(const std::string& key, const std::string& defaultValue) const;
    int getOrInt(const std::string& key, int defaultValue) const;
    int64_t getOrInt64(const std::string& key, int64_t defaultValue) const;
    double getOrDouble(const std::string& key, double defaultValue) const;
    bool getOrBool(const std::string& key, bool defaultValue) const;
    
    // Configuration management
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    void clear();
    std::vector<std::string> getKeys() const;
    std::vector<ConfigEntry> getAllEntries() const;
    
    // Enhanced save functionality
    bool saveToFile(const std::string& filepath, bool prettyPrint = true) const;
    std::string toJSON() const;
    std::string toYAML() const;
    
    // Configuration validation and processing
    bool validate() const;
    std::vector<std::string> validateAgainstSchema() const;
    
    // Configuration merging and inheritance
    void mergeFrom(const Config& other, bool override = true);
    Config getSubset(const std::vector<std::string>& keys) const;
    
    // Configuration querying and filtering
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    ConfigSource getSource(const std::string& key) const;
    bool isModified(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// JSON/YAML parsing utilities
namespace ConfigParser {
    bool parseJSON(const std::string& content, std::unordered_map<std::string, std::string>& out);
    bool parseYAML(const std::string& content, std::unordered_map<std::string, std::string>& out);
    std::string toJSON(const std::unordered_map<std::string, std::string>& config);
    std::string toYAML(const std::unordered_map<std::string, std::string>& config);
    
    // Type conversion utilities
    bool stringToBool(const std::string& str);
    int stringToInt(const std::string& str);
    double stringToDouble(const std::string& str);
}

} // namespace nlm