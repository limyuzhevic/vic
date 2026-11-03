#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <functional>
#include <limits>

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

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration range information
struct ConfigRange {
    double min = std::numeric_limits<double>::lowest();
    double max = std::numeric_limits<double>::max();
    double step = 0.0;  // For numeric values
    bool inclusive = true;
    
    ConfigRange() = default;
    ConfigRange(double min, double max) : min(min), max(max) {}
    ConfigRange(double min, double max, double step) : min(min), max(max), step(step) {}
    
    bool isValid(double value) const {
        return (inclusive ? value >= min && value <= max : value > min && value < max);
    }
};

// Configuration schema definition
struct ConfigSchema {
    std::string key;
    std::string type;
    std::string description;
    std::string defaultValue;
    ConfigRange range;
    std::function<bool(const ConfigValue& value)> validator;
    bool required;
    
    ConfigSchema() : required(false) {}
    ConfigSchema(const std::string& k, const std::string& t, const std::string& desc, 
                const std::string& def = "", bool req = false)
        : key(k), type(t), description(desc), defaultValue(def), required(req) {}
};

// Main configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values (defined here so templates instantiate in every TU)
template<typename T>
    std::optional<T> get(const std::string& key) const;

    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    // Validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime, const std::string& description = "");
    bool validate(const std::string& key) const;
    
    // Schema management
    void addSchema(const ConfigSchema& schema);
    void removeSchema(const std::string& key);
    std::vector<ConfigSchema> getSchemas() const;
    
    // Value validation helpers
    static bool validateValue(const ConfigValue& value, const ConfigRange& range);
    static bool validateValue(const ConfigValue& value, const std::string& type);
    static ConfigValue convertToType(const ConfigValue& value, const std::string& targetType);
    
    // Configuration summary with validation status
    std::string summary(bool includeValidation = true) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
