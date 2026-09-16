// Enhanced Configuration Management
#pragma once

#include "core/ErrorHandling/Error.hpp"
#include "core/Logger/Logger.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <chrono>
#include <filesystem>

namespace nlm {

// Configuration validation rules
struct ValidationRule {
    std::string key;
    std::string type; // "int", "double", "string", "bool", "range"
    std::string errorMessage;
    std::string condition; // For range validation: "min=X,max=Y"
    bool required = false;
    std::string defaultValue;
};

// Enhanced configuration class with validation
class EnhancedConfig {
public:
    EnhancedConfig() = default;
    ~EnhancedConfig() = default;
    
    // Load configuration from file with error handling
    bool loadFromFile(const std::string& filepath) {
        auto startTime = std::chrono::steady_clock::now();
        
        try {
            CHECK_CONDITION(!filepath.empty(), ErrorCode::CONFIG_VALUE_INVALID, 
                          "Configuration file path cannot be empty");
            
            std::filesystem::path path(filepath);
            CHECK_CONDITION(std::filesystem::exists(path), ErrorCode::CONFIG_FILE_NOT_FOUND,
                          "Configuration file not found: " + filepath);
            CHECK_CONDITION(std::filesystem::is_regular_file(path), 
                          ErrorCode::CONFIG_FILE_INVALID_FORMAT,
                          "Path is not a regular file: " + filepath);
            
            if (!parseFile(filepath)) {
                throwError(ErrorCode::CONFIG_FILE_INVALID_FORMAT,
                          "Failed to parse configuration file: " + filepath);
            }
            
            if (!validate()) {
                throwError(ErrorCode::CONFIG_VALIDATION_FAILED,
                          "Configuration validation failed");
            }
            
            applyDefaults();
            
            auto endTime = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            if (duration.count() > 100) { // Log if loading takes more than 100ms
                Logger::getGlobal().warning("Configuration loading took " + std::to_string(duration.count()) + "ms");
            }
            
            return true;
            
        } catch (const NLMError& e) {
            Logger::getGlobal().error("Failed to load configuration from " + filepath + ": " + e.what());
            return false;
        } catch (const std::exception& e) {
            Logger::getGlobal().error("Unexpected error loading configuration: " + std::string(e.what()));
            return false;
        }
    }
    
    // Load with fallback mechanism
    bool loadWithFallback(const std::vector<std::string>& filepaths) {
        std::string lastError;
        
        for (const auto& filepath : filepaths) {
            if (loadFromFile(filepath)) {
                Logger::getGlobal().info("Successfully loaded configuration from: " + filepath);
                return true;
            }
            lastError = "Failed to load " + filepath;
        }
        
        Logger::getGlobal().error("All fallback configurations failed. Last error: " + lastError);
        return false;
    }
    
    // Configuration presets
    class Presets {
    public:
        static EnhancedConfig createMinimalConfig() {
            EnhancedConfig config;
            config.set("simulation_timestep", 0.001);
            config.set("neuron_count", 100);
            config.set("region_count", 1);
            config.set("connection_probability", 0.1);
            config.set("random_seed", 123);
            config.set("stdp_ltp_weight", 0.01);
            config.set("stdp_ltd_weight", 0.01);
            config.set("synaptogenesis_rate", 0.00001);
            config.set("pruning_rate", 0.000001);
            return config;
        }
        
        static EnhancedConfig createLearningConfig() {
            EnhancedConfig config;
            config.set("simulation_timestep", 0.001);
            config.set("neuron_count", 500);
            config.set("region_count", 1);
            config.set("connection_probability", 0.15);
            config.set("random_seed", 42);
            config.set("stdp_ltp_weight", 0.03);
            config.set("stdp_ltd_weight", 0.02);
            config.set("synaptogenesis_rate", 0.0002);
            config.set("pruning_rate", 0.00002);
            return config;
        }
        
        static EnhancedConfig createStabilityConfig() {
            EnhancedConfig config;
            config.set("simulation_timestep", 0.001);
            config.set("neuron_count", 200);
            config.set("region_count", 1);
            config.set("connection_probability", 0.05);
            config.set("random_seed", 999);
            config.set("stdp_ltp_weight", 0.01);
            config.set("stdp_ltd_weight", 0.01);
            config.set("synaptogenesis_rate", 0.000005);
            config.set("pruning_rate", 0.0000005);
            return config;
        }
        
        static EnhancedConfig createProductionConfig() {
            EnhancedConfig config;
            config.set("simulation_timestep", 0.0005);
            config.set("neuron_count", 1000);
            config.set("region_count", 2);
            config.set("connection_probability", 0.2);
            config.set("random_seed", 0); // Use time-based seed in production
            config.set("stdp_ltp_weight", 0.02);
            config.set("stdp_ltd_weight", 0.015);
            config.set("synaptogenesis_rate", 0.0001);
            config.set("pruning_rate", 0.00001);
            return config;
        }
    };
    
    // Get configuration value with type checking
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        auto value = get(key);
        if (value.has_value()) {
            try {
                return std::get<T>(value.value());
            } catch (const std::bad_variant_access&) {
                throwError(ErrorCode::CONFIG_VALUE_INVALID,
                          "Type mismatch for key '" + key + "'", __FILE__, __LINE__, __func__);
            }
        }
        return defaultValue;
    }
    
    // Set configuration value with validation
    template<typename T>
    void set(const std::string& key, const T& value) {
        CHECK_CONDITION(!key.empty(), ErrorCode::CONFIG_VALUE_INVALID,
                      "Configuration key cannot be empty");
        
        // Validate value based on key
        if (!validateValue(key, value)) {
            throwConfigError(ErrorCode::CONFIG_VALIDATION_FAILED,
                           "Invalid value for configuration key: " + key,
                           key, __FILE__, __LINE__, __func__);
        }
        
        std::visit([this, &key](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int>) {
                setInt(key, val);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                setInt64(key, val);
            } else if constexpr (std::is_same_v<T, double>) {
                setDouble(key, val);
            } else if constexpr (std::is_same_v<T, bool>) {
                setBool(key, val);
            } else if constexpr (std::is_same_v<T, std::string>) {
                setString(key, val);
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                setIntVector(key, val);
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                setDoubleVector(key, val);
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                setStringVector(key, val);
            }
        }, value);
    }
    
    // Validate all configuration rules
    bool validate() const {
        bool allValid = true;
        
        for (const auto& rule : validationRules_) {
            if (!validateRule(rule)) {
                Logger::getGlobal().error("Configuration validation failed for key: " + rule.key + 
                                       " - " + rule.errorMessage);
                allValid = false;
            }
        }
        
        return allValid;
    }
    
    // Add validation rule
    void addValidationRule(const ValidationRule& rule) {
        validationRules_.push_back(rule);
    }
    
    // Get configuration summary
    std::string getSummary() const {
        std::ostringstream oss;
        oss << "=== Configuration Summary ===\n";
        oss << "Total entries: " << getKeys().size() << "\n";
        oss << "\nValues:\n";
        
        for (const auto& key : getKeys()) {
            auto value = get(key);
            oss << "  " << key << " = ";
            std::visit([&oss](auto&& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << val << "\"";
                } else {
                    oss << val;
                }
            }, value.value());
            oss << "\n";
        }
        
        oss << "\nValidation Rules: " << validationRules_.size() << " rules\n";
        return oss.str();
    }
    
    // Check if configuration is valid
    bool isValid() const {
        return isValid_;
    }
    
    // Get last error message
    const std::string& getLastError() const {
        return lastError_;
    }
    
private:
    bool parseFile(const std::string& filepath) {
        // Implementation would parse the file and populate values
        // For now, return true to allow testing
        return true;
    }
    
    void applyDefaults() {
        // Apply default values for missing keys
        if (!has("simulation_timestep")) {
            set("simulation_timestep", 0.001);
        }
        if (!has("neuron_count")) {
            set("neuron_count", 500);
        }
        // Add more default values as needed
    }
    
    bool validateRule(const ValidationRule& rule) const {
        if (rule.required && !has(rule.key)) {
            return false;
        }
        
        if (has(rule.key)) {
            auto value = get(rule.key);
            if (!value.has_value()) {
                return false;
            }
            
            // Type validation
            if (rule.type == "int") {
                if (!std::holds_alternative<int>(value.value())) {
                    return false;
                }
                // Range validation
                if (!rule.condition.empty()) {
                    // Parse condition like "min=X,max=Y"
                    size_t minPos = rule.condition.find("min=");
                    size_t maxPos = rule.condition.find(",max=");
                    if (minPos != std::string::npos && maxPos != std::string::npos) {
                        int minVal = std::stoi(rule.condition.substr(minPos + 4, maxPos - minPos - 5));
                        int maxVal = std::stoi(rule.condition.substr(maxPos + 5));
                        int actualVal = std::get<int>(value.value());
                        if (actualVal < minVal || actualVal > maxVal) {
                            return false;
                        }
                    }
                }
            } else if (rule.type == "double") {
                if (!std::holds_alternative<double>(value.value())) {
                    return false;
                }
            } else if (rule.type == "string") {
                if (!std::holds_alternative<std::string>(value.value())) {
                    return false;
                }
            } else if (rule.type == "bool") {
                if (!std::holds_alternative<bool>(value.value())) {
                    return false;
                }
            }
        }
        
        return true;
    }
    
    bool validateValue(const std::string& key, const std::string& value) const {
        // Basic validation for string values
        if (value.empty() && key != "description") {
            return false;
        }
        return true;
    }
    
    template<typename T>
    bool validateValue(const std::string& key, const T& value) const {
        return true; // Basic validation
    }
    
    // Internal storage methods
    void setInt(const std::string& key, int value) {
        entries_[key] = ConfigValue(value);
    }
    
    void setInt64(const std::string& key, int64_t value) {
        entries_[key] = ConfigValue(value);
    }
    
    void setDouble(const std::string& key, double value) {
        entries_[key] = ConfigValue(value);
    }
    
    void setBool(const std::string& key, bool value) {
        entries_[key] = ConfigValue(value);
    }
    
    void setString(const std::string& key, const std::string& value) {
        entries_[key] = ConfigValue(value);
    }
    
    void setIntVector(const std::string& key, const std::vector<int>& value) {
        entries_[key] = ConfigValue(value);
    }
    
    void setDoubleVector(const std::string& key, const std::vector<double>& value) {
        entries_[key] = ConfigValue(value);
    }
    
    void setStringVector(const std::string& key, const std::vector<std::string>& value) {
        entries_[key] = ConfigValue(value);
    }
    
    std::optional<ConfigValue> get(const std::string& key) const {
        auto it = entries_.find(key);
        if (it != entries_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    bool has(const std::string& key) const {
        return entries_.find(key) != entries_.end();
    }
    
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        keys.reserve(entries_.size());
        for (const auto& pair : entries_) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    // Member variables
    std::unordered_map<std::string, ConfigValue> entries_;
    std::vector<ValidationRule> validationRules_;
    bool isValid_ = false;
    std::string lastError_;
};

// Macro for improved error handling
#define CHECK_AND_THROW(condition, code, message) \
    do { \
        if (!(condition)) { \
            throwError(code, message, __FILE__, __LINE__, __func__); \
        } \
    } while(0)

#define VALIDATE_AND_LOG(key, value, condition, message) \
    do { \
        if (!(condition)) { \
            Logger::getGlobal().error("Configuration validation failed for " + key + ": " + message); \
            return false; \
        } \
    } while(0)

} // namespace nlm
