#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <unordered_set>

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

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    
    ValidationResult(bool v = true, const std::string& msg = "") 
        : valid(v), errorMessage(msg) {}
    
    static ValidationResult Success() { return ValidationResult(true); }
    static ValidationResult Error(const std::string& msg) { return ValidationResult(false, msg); }
};

// Configuration template definition
struct ConfigTemplate {
    std::string name;
    std::map<std::string, ConfigValue> defaultValues;
    std::map<std::string, std::function<ValidationResult(const ConfigValue&)>> validators;
    std::unordered_set<std::string> requiredKeys;
    
    ConfigTemplate() = default;
    
    ConfigTemplate(const std::string& n) : name(n) {}
    
    void addValidation(const std::string& key, 
                      const std::function<ValidationResult(const ConfigValue&)>& validator) {
        validators[key] = validator;
    }
    
    void addRequired(const std::string& key) {
        requiredKeys.insert(key);
    }
    
    ValidationResult validate(const std::map<std::string, ConfigValue>& values) const;
};

// Advanced configuration manager
class AdvancedConfig {
public:
    AdvancedConfig();
    ~AdvancedConfig();
    
    // Template management
    void addTemplate(const ConfigTemplate& template);
    void removeTemplate(const std::string& name);
    std::vector<std::string> getTemplateNames() const;
    ConfigTemplate* getTemplate(const std::string& name);
    
    // Template application
    bool applyTemplate(const std::string& templateName, 
                      std::map<std::string, ConfigValue>& configData,
                      ConfigSource source = ConfigSource::Default) const;
    
    // Validation
    ValidationResult validateConfig(const std::map<std::string, ConfigValue>& configData,
                                   const std::string& templateName = "") const;
    
    // Configuration merging
    std::map<std::string, ConfigValue> mergeConfigs(
        const std::map<std::string, ConfigValue>& base,
        const std::map<std::string, ConfigValue>& override) const;
    
    // Configuration comparison
    bool configsEqual(const std::map<std::string, ConfigValue>& config1,
                    const std::map<std::string, ConfigValue>& config2) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
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
    
    // Get values
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
    void setAdvancedConfig(const AdvancedConfig& advancedConfig);
    std::vector<std::string> getAvailableTemplates() const;
    bool applyTemplate(const std::string& templateName, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration validation
    bool validateConfig(const std::string& templateName = "") const;
    std::string getValidationErrors() const;
    
    // Configuration comparison
    bool configsEqual(const Config& other) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::shared_ptr<AdvancedConfig> advancedConfig;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
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
