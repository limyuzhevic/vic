#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <nlohmann/json.hpp>

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
    std::vector<std::string>,
    std::vector<float>
>;

// Configuration validation error
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message, const std::string& key, 
                         const ConfigValue& value, const std::string& expectedType)
        : std::runtime_error(message), key_(key), valueStr_(toString(value)), 
          expectedType_(expectedType) {}

    const std::string& getKey() const { return key_; }
    const std::string& getValueStr() const { return valueStr_; }
    const std::string& getExpectedType() const { return expectedType_; }

private:
    static std::string toString(const ConfigValue& value) {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            oss << arg;
        }, value);
        return oss.str();
    }

    std::string key_;
    std::string valueStr_;
    std::string expectedType_;
};

// Configuration validation rule
class ValidationRule {
public:
    virtual ~ValidationRule() = default;
    virtual void validate(const std::string& key, const ConfigValue& value) const = 0;
    virtual std::string getErrorMessage(const std::string& key, const ConfigValue& value) const = 0;
    virtual std::string getExpectedType() const = 0;
};

// Numeric range validation
class NumericRangeRule : public ValidationRule {
public:
    NumericRangeRule(double min, double max, const std::string& description = "")
        : min_(min), max_(max), description_(description) {}

    void validate(const std::string& key, const ConfigValue& value) const override {
        if (const auto* val = std::get_if<double>(&value)) {
            if (*val < min_ || *val > max_) {
                throw ConfigValidationError(
                    "Value " + std::to_string(*val) + " for key '" + key + 
                    "' is outside valid range [" + std::to_string(min_) + ", " + 
                    std::to_string(max_) + "]",
                    key, value, "double in range [" + std::to_string(min_) + ", " + std::to_string(max_) + "]"
                );
            }
        } else if (const auto* val = std::get_if<int>(&value)) {
            double dval = static_cast<double>(*val);
            if (dval < min_ || dval > max_) {
                throw ConfigValidationError(
                    "Value " + std::to_string(*val) + " for key '" + key + 
                    "' is outside valid range [" + std::to_string(min_) + ", " + 
                    std::to_string(max_) + "]",
                    key, value, "int in range [" + std::to_string(min_) + ", " + std::to_string(max_) + "]"
                );
            }
        }
    }

    std::string getErrorMessage(const std::string& key, const ConfigValue& value) const override {
        std::string valStr;
        std::visit([&valStr](auto&& arg) { valStr = std::to_string(arg); }, value);
        return "Invalid value for key '" + key + "': " + valStr + 
               " is outside range [" + std::to_string(min_) + ", " + std::to_string(max_) + "]";
    }

    std::string getExpectedType() const override {
        return "numeric in range [" + std::to_string(min_) + ", " + std::to_string(max_) + "]";
    }

private:
    double min_;
    double max_;
    std::string description_;
};

// Choice validation
class ChoiceRule : public ValidationRule {
public:
    ChoiceRule(const std::vector<std::string>& choices, const std::string& description = "")
        : choices_(choices), description_(description) {}

    void validate(const std::string& key, const ConfigValue& value) const override {
        std::string valStr;
        if (const auto* val = std::get_if<std::string>(&value)) {
            valStr = *val;
        } else {
            throw ConfigValidationError(
                "Value for key '" + key + "' must be a string",
                key, value, "string choice from " + formatChoices()
            );
        }

        bool valid = false;
        for (const auto& choice : choices_) {
            if (valStr == choice) {
                valid = true;
                break;
            }
        }

        if (!valid) {
            throw ConfigValidationError(
                "Value '" + valStr + "' for key '" + key + 
                "' is not one of the allowed choices",
                key, value, "one of: " + formatChoices()
            );
        }
    }

    std::string getErrorMessage(const std::string& key, const ConfigValue& value) const override {
        std::string valStr;
        std::visit([&valStr](auto&& arg) { valStr = std::to_string(arg); }, value);
        return "Invalid value for key '" + key + "': '" + valStr + 
               "' is not one of the allowed choices: " + formatChoices();
    }

    std::string getExpectedType() const override {
        return "one of: " + formatChoices();
    }

private:
    std::string formatChoices() const {
        std::string result;
        for (size_t i = 0; i < choices_.size(); ++i) {
            if (i > 0) result += ", ";
            result += "'" + choices_[i] + "'";
        }
        return result;
    }

    std::vector<std::string> choices_;
    std::string description_;
};

// Configuration schema definition
struct ConfigSchema {
    std::string key;
    ConfigValue defaultValue;
    ConfigSource defaultSource;
    std::string description;
    std::vector<std::unique_ptr<ValidationRule>> validationRules;
    bool required;
    std::function<bool(const ConfigValue&)> customValidator;

    ConfigSchema(const std::string& k, const ConfigValue& defaultVal, 
                ConfigSource source = ConfigSource::Default,
                const std::string& desc = "", bool req = false)
        : key(k), defaultValue(defaultVal), defaultSource(source),
          description(desc), required(req), customValidator(nullptr) {}

    // Add validation rule
    template<typename RuleType, typename... Args>
    ConfigSchema& withValidation(Args&&... args) {
        validationRules.push_back(std::make_unique<RuleType>(std::forward<Args>(args)...));
        return *this;
    }

    // Add custom validator
    ConfigSchema& withCustomValidator(std::function<bool(const ConfigValue&)> validator) {
        customValidator = validator;
        return *this;
    }

    // Mark as required
    ConfigSchema& required(bool req = true) {
        required = req;
        return *this;
    }

    // Validate a value against all rules
    void validate(const std::string& key, const ConfigValue& value) const {
        // Apply all validation rules
        for (const auto& rule : validationRules) {
            rule->validate(key, value);
        }

        // Apply custom validator if provided
        if (customValidator && !customValidator(value)) {
            throw ConfigValidationError(
                "Custom validation failed for key '" + key + "'",
                key, value, "custom validation rule"
            );
        }
    }

    // Get human-readable expected type description
    std::string getExpectedTypeDescription() const {
        if (validationRules.empty()) {
            return "any valid type";
        }

        std::string result;
        for (size_t i = 0; i < validationRules.size(); ++i) {
            if (i > 0) result += " or ";
            result += validationRules[i]->getExpectedType();
        }
        return result;
    }
};

// Enhanced configuration class
class Config {
public:
    Config();
    ~Config();

    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;

    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);

    // Load from JSON string
    bool loadFromJSON(const std::string& jsonString);

    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);

    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;

    // Save to JSON string
    std::string saveToJSON() const;

    // Get values with validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;

    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);

    // Convenience setters with validation
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

    // Apply schema (ensure all required keys are present)
    bool applySchema(const std::vector<ConfigSchema>& schema, std::vector<std::string>& errors) const;

    // Validate all entries against schema
    bool validateAgainstSchema(const std::vector<ConfigSchema>& schema, std::vector<std::string>& errors) const;

    // Merge with another config (runtime values override)
    void merge(const Config& other);

    // Get configuration summary with validation info
    std::string summary() const;

    // Get environment variables (prefixed with NLM_)
    void loadFromEnvironment(const std::string& prefix = "NLM_");

    // Set validation enabled/disabled
    void setValidationEnabled(bool enabled) { validationEnabled_ = enabled; }
    bool isValidationEnabled() const { return validationEnabled_; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static ConfigValue jsonToConfigValue(const nlohmann::json& json);
    static nlohmann::json configValueToJSON(const ConfigValue& value);
    static std::string valueToString(const ConfigValue& value);

    // Validation helpers
    bool validateValue(const ConfigEntry& entry) const;
    void applyDefaultSchema(const std::vector<ConfigSchema>& schema);

    bool validationEnabled_;

    // Default schema for NLM
    std::vector<ConfigSchema> getDefaultSchema() const;
};

// Legacy Config class for backward compatibility - re-export
class LegacyConfig {
public:
    LegacyConfig();
    ~LegacyConfig();

    // Disable copying, enable moving
    LegacyConfig(const LegacyConfig&) = delete;
    LegacyConfig& operator=(const LegacyConfig&) = delete;
    LegacyConfig(LegacyConfig&&) noexcept;
    LegacyConfig& operator=(LegacyConfig&&) noexcept;

    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool saveToFile(const std::string& filepath) const;

    template<typename T>
    std::optional<T> get(const std::string& key) const;

    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);

    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();
    std::string summary() const;

    // Get underlying config for advanced operations
    std::shared_ptr<Config> getAdvancedConfig() const { return advancedConfig_; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::shared_ptr<Config> advancedConfig_;
};

} // namespace nlm

// Include implementation headers
#include "Config.cpp.implementation"  // Forward declaration to avoid circular includes
