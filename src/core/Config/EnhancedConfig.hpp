#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstring>
#include <cctype>
#include <map>
#include <set>
#include <optional>
#include <variant>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <regex>
#include <chrono>

namespace nlm {

// Forward declarations
class Config;

// Enhanced configuration value types
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

// Enhanced configuration source enum
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Environment,
    Runtime,
    Preset
};

// Configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string typeHint;
    bool required;
    std::string validator;
    std::map<std::string, std::string> metadata;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                   typeHint(), required(false), validator() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "",
                const std::string& type = "", bool req = false, const std::string& valid = "")
        : key(k), value(v), source(s), description(desc), typeHint(type), required(req), validator(valid) {}
};

// JSON utilities
namespace json_utils {
    // Simple JSON parser for basic types
    class JSONParser {
    public:
        static bool parseJSON(const std::string& jsonStr, std::vector<ConfigEntry>& entries);
        static std::string toJSON(const std::vector<ConfigEntry>& entries);
        static bool parseYAML(const std::string& yamlStr, std::vector<ConfigEntry>& entries);
        static std::string toYAML(const std::vector<ConfigEntry>& entries);
    
    private:
        static std::string trim(const std::string& str);
        static bool isWhitespace(char c);
        static std::string extractString(const std::string& str, size_t& pos);
        static std::string unescapeString(const std::string& str);
    };

// Type conversion utilities
namespace type_utils {
    static std::string valueToString(const ConfigValue& value);
    static std::optional<ConfigValue> stringToValue(const std::string& str, const std::string& typeHint = "");
    static bool validateValue(const ConfigValue& value, const std::string& validator);
    static std::string typeToString(const ConfigValue& value);
}

// Environment variable utilities
namespace env_utils {
    static std::string prefixEnvVars(const std::vector<ConfigEntry>& entries);
    static std::string envVarToKey(const std::string& envVar);
}

// Command-line utilities
namespace cli_utils {
    static struct ParsedArgs {
        std::string configFile;
        std::map<std::string, std::string> overrides;
        bool verbose;
        bool debug;
        bool help;
        bool version;
        std::string experiment;
        std::string logLevel;
        bool dryRun;
        bool validate;
    };
    
    static ParsedArgs parseCommandLine(int argc, char** argv);
    static void printHelp(const std::string& programName);
}

// Enhanced configuration class with full JSON/YAML support
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load configuration from multiple sources with priority order
    bool loadFromFile(const std::string& filepath, ConfigSource source = ConfigSource::File);
    bool loadFromJSON(const std::string& jsonStr, ConfigSource source = ConfigSource::File);
    bool loadFromYAML(const std::string& yamlStr, ConfigSource source = ConfigSource::File);
    bool loadFromEnvironment(ConfigSource source = ConfigSource::Environment);
    bool loadFromArgs(int argc, char** argv, ConfigSource source = ConfigSource::CommandLine);
    
    // Save configuration
    bool saveToFile(const std::string& filepath) const;
    bool saveToJSON(std::ostream& output) const;
    bool saveToYAML(std::ostream& output) const;
    
    // Enhanced get methods with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    template<typename T>
    T getRequired(const std::string& key) const; // Throws if not found
    
    // Enhanced set methods with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& typeHint = "",
             bool required = false, const std::string& validator = "");
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& typeHint = "",
             bool required = false, const std::string& validator = "");
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& typeHint = "",
             bool required = false, const std::string& validator = "");
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& typeHint = "",
             bool required = false, const std::string& validator = "");
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& typeHint = "",
             bool required = false, const std::string& validator = "");
    
    // Configuration management
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    void clear();
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    
    // Enhanced information methods
    ConfigSource getSource(const std::string& key) const;
    std::string getTypeHint(const std::string& key) const;
    std::string getDescription(const std::string& key) const;
    std::string getValidator(const std::string& key) const;
    
    // Validation and statistics
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    std::string summary() const;
    std::string getSourceSummary() const;
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    
    // Preset management
    bool loadPreset(const std::string& presetName, ConfigSource source = ConfigSource::Preset);
    static std::vector<std::string> getAvailablePresets();
    
    // Configuration history (for versioning)
    void enableHistory(bool enable = true);
    bool hasHistory() const;
    std::vector<std::chrono::system_clock::time_point> getHistoryTimestamps() const;
    bool restoreFromHistory(size_t index);
    
    // Configuration comparison
    bool equals(const Config& other) const;
    double similarity(const Config& other) const;
    
    // Batch operations
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    void applyOverrides(const std::map<std::string, std::string>& overrides);
    
    // Configuration discovery
    std::vector<std::string> suggestKeys(const std::string& partial) const;
    std::vector<std::string> getKeysByType(const std::string& typeHint) const;
    std::vector<std::string> getRequiredKeys() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    void initializeDefaults();
    bool validateValue(const ConfigEntry& entry) const;
    std::string getValueAsString(const ConfigEntry& entry) const;
    ConfigValue parseValueFromString(const std::string& str, const std::string& typeHint = "") const;
    static std::string trim(const std::string& str);
    static bool isWhitespace(char c);
    static std::string toLower(const std::string& str);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
};

// Convenience functions for enhanced CLI integration
namespace config_helpers {
    static Config createDefaultConfig();
    static Config createPhase2Config();
    static Config createPhase3Config();
    static Config createPhase4Config();
    static Config createMinimalConfig();
    static Config createResearchConfig();
}

// Configuration validation rules
namespace validation_rules {
    static bool isPositive(const ConfigValue& value);
    static bool isNonNegative(const ConfigValue& value);
    static bool isInRange(const ConfigValue& value, double min, double max);
    static bool isStringLength(const ConfigValue& value, size_t minLen, size_t maxLen);
    static bool matchesPattern(const ConfigValue& value, const std::string& pattern);
    static bool isValidEnum(const ConfigValue& value, const std::vector<std::string>& validValues);
}

// Configuration templates
namespace config_templates {
    static const std::string RESEARCH_FAST = "research_fast";
    static const std::string RESEARCH_DETAILED = "research_detailed";
    static const std::string PRODUCTION = "production";
    static const std::string DEVELOPMENT = "development";
    static const std::string MINIMAL = "minimal";
    static const std::string HIGH_PRECISION = "high_precision";
}

} // namespace nlm
