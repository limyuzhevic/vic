#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <chrono>
#include <regex>

namespace nlm {

// Forward declarations
class Config;

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

// Configuration source with timestamp
struct ConfigSource {
    enum class Type {
        Default,
        File,
        CommandLine,
        Runtime
    };
    
    Type type;
    std::string filename;
    std::chrono::system_clock::time_point timestamp;
    int lineNumber;
    
    ConfigSource(Type t = Type::Default, const std::string& filename = "", int line = -1)
        : type(t), filename(filename), timestamp(std::chrono::system_clock::now()), lineNumber(line) {}
    
    bool isUserDefined() const {
        return type == Type::CommandLine || type == Type::Runtime;
    }
    
    std::string getSourceName() const {
        switch (type) {
            case Type::Default: return "default";
            case Type::File: return "file:" + filename;
            case Type::CommandLine: return "command line";
            case Type::Runtime: return "runtime";
            default: return "unknown";
        }
    }
};

// Configuration entry with validation support
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string validationRegex;
    bool required;
    
    ConfigEntry() 
        : key(), value(), source(ConfigSource()), description(), validationRegex(), required(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), validationRegex(), required(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc, 
                const std::string& regex, bool req = false)
        : key(k), value(v), source(s), description(desc), validationRegex(regex), required(req) {}
    
    bool isValid() const {
        if (required && (value.index() == 0 || value.index() == 1)) {
            return false;  // Empty int or int64_t might indicate uninitialized
        }
        
        if (!validationRegex.empty()) {
            if (std::holds_alternative<std::string>(value)) {
                const std::string& strVal = std::get<std::string>(value);
                std::regex regex(validationRegex);
                return std::regex_match(strVal, regex);
            } else if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value) ||
                       std::holds_alternative<double>(value)) {
                // Numeric values - always valid for now
                return true;
            }
        }
        return true;
    }
    
    std::string getTypeName() const {
        if (std::holds_alternative<int>(value)) return "int";
        if (std::holds_alternative<int64_t>(value)) return "int64_t";
        if (std::holds_alternative<double>(value)) return "double";
        if (std::holds_alternative<bool>(value)) return "bool";
        if (std::holds_alternative<std::string>(value)) return "string";
        if (std::holds_alternative<std::vector<int>>(value)) return "vector<int>";
        if (std::holds_alternative<std::vector<double>>(value)) return "vector<double>";
        if (std::holds_alternative<std::vector<std::string>>(value)) return "vector<string>";
        return "unknown";
    }
};

// Configuration validator callback
using ConfigValidator = std::function<bool(const ConfigEntry&)>;

// Main configuration class with enhanced features
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from different formats
    bool loadFromFile(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    bool loadFromJSON(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    
    // Load from strings
    bool loadFromYAMLString(const std::string& content);
    bool loadFromJSONString(const std::string& content);
    
    // Save to different formats
    bool saveToFile(const std::string& filepath, bool pretty = true) const;
    bool saveToYAML(const std::string& filepath, bool pretty = true) const;
    bool saveToJSON(const std::string& filepath, bool pretty = true) const;
    
    std::string saveToYAMLString(bool pretty = true) const;
    std::string saveToJSONString(bool pretty = true) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Advanced configuration
    void setValidated(const std::string& key, const ConfigValue& value, 
                     const std::string& regex = "", bool required = false,
                     ConfigSource source = ConfigSource::Runtime);
    void addValidator(const std::string& key, ConfigValidator validator);
    bool validateAll() const;
    
    // Check existence and type
    bool has(const std::string& key) const;
    std::string getType(const std::string& key) const;
    bool isValid(const std::string& key) const;
    
    // Remove and management
    void remove(const std::string& key);
    void remove(const std::vector<std::string>& keys);
    void clear();
    void merge(const Config& other);
    
    // Get all keys and entries
    std::vector<std::string> getKeys() const;
    std::vector<ConfigEntry> getEntries() const;
    std::vector<ConfigEntry> getUserDefinedEntries() const;
    std::vector<ConfigEntry> getFileEntries() const;
    
    // Get configuration summary with validation info
    std::string summary() const;
    std::string summary(bool includeUserOnly) const;
    
    // Configuration statistics
    size_t getEntryCount() const;
    size_t getUserDefinedCount() const;
    size_t getFileCount() const;
    std::vector<std::string> getSources() const;
    
    // Import/export
    bool importFromJSON(const std::string& jsonContent, ConfigSource source = ConfigSource::File);
    bool importFromYAML(const std::string& yamlContent, ConfigSource source = ConfigSource::File);
    
    // Profile and statistics
    struct Statistics {
        size_t totalEntries = 0;
        size_t userEntries = 0;
        size_t fileEntries = 0;
        size_t runtimeEntries = 0;
        std::map<std::string, size_t> typeCounts;
        double validationScore = 1.0;
    };
    
    Statistics getStatistics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool validateString(const std::string& value, const std::string& regex);
    static bool parseLine(const std::string& line, std::string& key, std::string& value);
    
    // YAML/JSON helpers
    static std::string escapeYAMLString(const std::string& str);
    static std::string escapeJSONString(const std::string& str);
    
    // Legacy support
    void loadFromLegacyFile(const std::string& filepath);
    bool loadFromLegacyArgs(int argc, char** argv);
};

} // namespace nlm
