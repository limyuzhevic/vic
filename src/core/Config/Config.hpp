// Config.hpp
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <optional>
#include <variant>
#include <chrono>

namespace nlm {

enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Programmatic
};

enum class ValidationResult {
    Success,
    InvalidType,
    InvalidValue,
    OutOfRange,
    MissingRequired
};

struct ValidationError {
    std::string key;
    std::string message;
    ValidationResult result;
};

using ConfigValue = std::variant<int64_t, double, bool, std::string>;

struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    // Validation metadata
    std::optional<ConfigValue> minValue;
    std::optional<ConfigValue> maxValue;
    bool required;
    
    // Type information
    std::string typeName;
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
    
    // Configuration loading
    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool loadFromJson(const std::string& jsonContent);
    bool loadFromYaml(const std::string& yamlContent);
    
    // Configuration saving
    bool saveToFile(const std::string& filepath) const;
    std::string exportToJson() const;
    std::string exportToYaml() const;
    std::string summary() const;
    
    // Get configuration values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set configuration values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Programmatic);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Programmatic);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Programmatic);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Programmatic);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Programmatic);
    
    // Configuration management
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    void clear();
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    ConfigSource getSource(const std::string& key) const;
    
    // Validation and error handling
    std::vector<ValidationError> validate() const;
    bool validateKey(const std::string& key, std::vector<ValidationError>& errors) const;
    bool isValid() const;
    
    // Configuration metadata
    void setDescription(const std::string& key, const std::string& description);
    std::string getDescription(const std::string& key) const;
    
    void setTypeValidation(const std::string& key, const std::string& typeName);
    std::string getTypeName(const std::string& key) const;
    
    void setRangeValidation(const std::string& key, const ConfigValue& minValue, const ConfigValue& maxValue);
    bool isWithinRange(const std::string& key, const ConfigValue& value) const;
    
    void markRequired(const std::string& key, bool required = true);
    bool isRequired(const std::string& key) const;
    
    // Configuration merging and presets
    void merge(const Config& other, ConfigSource source = ConfigSource::File);
    void applyPreset(const std::string& presetName);
    bool hasPreset(const std::string& presetName) const;
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
    // Configuration serialization
    std::string toString(const std::string& key) const;
    static std::string configValueToString(const ConfigValue& value);
    static std::optional<ConfigValue> stringToConfigValue(const std::string& str, const std::string& typeName);
    
    // Performance optimization
    size_t getEntryCount() const;
    void reserve(size_t capacity);
    
    // Statistics
    size_t getLoadCount() const;
    double getAverageLoadTime() const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Helper functions
inline bool operator==(const ConfigValue& lhs, const ConfigValue& rhs) {
    return std::visit([&](const auto& lhsVal) -> bool {
        return std::visit([&](const auto& rhsVal) -> bool {
            return lhsVal == rhsVal;
        }, rhs);
    }, lhs);
}

inline bool operator!=(const ConfigValue& lhs, const ConfigValue& rhs) {
    return !(lhs == rhs);
}

inline std::ostream& operator<<(std::ostream& os, const ConfigValue& value) {
    return os << Config::configValueToString(value);
}

inline std::istream& operator>>(std::istream& is, ConfigValue& value) {
    std::string str;
    is >> str;
    value = ConfigValue{}; // This will be set by stringToConfigValue
    return is;
}

// Explicit template instantiations
extern template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
extern template std::optional<double> Config::get<double>(const std::string&) const;
extern template std::optional<bool> Config::get<bool>(const std::string&) const;
extern template std::optional<std::string> Config::get<std::string>(const std::string&) const;

extern template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
extern template double Config::getOr<double>(const std::string&, const double&) const;
extern template bool Config::getOr<bool>(const std::string&, const bool&) const;
extern template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
