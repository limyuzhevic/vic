#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <map>
#include <set>
#include <numeric>

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
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : isValid(true) {}
    
    void addError(const std::string& error) {
        isValid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
};

// Configuration schema entry
struct ConfigSchemaEntry {
    std::string key;
    std::string type; // "int", "double", "bool", "string", "int[]", "double[]", "string[]"
    std::string description;
    ConfigValue defaultValue;
    bool required;
    std::function<ValidationResult(const ConfigValue&)> validator;
    
    // Value range constraints
    std::optional<std::pair<double, double>> range; // For numeric types
    std::set<std::string> allowedValues; // For string types
    size_t minSize; // For array types
    size_t maxSize; // For array types
    
    ConfigSchemaEntry() 
        : key(), type(), description(), defaultValue(), required(false)
        , validator(nullptr), minSize(0), maxSize(std::numeric_limits<size_t>::max()) {}
    
    ConfigSchemaEntry(const std::string& k, const std::string& t, const std::string& desc = "",
                     const ConfigValue& defaultVal = ConfigValue(), bool req = false)
        : key(k), type(t), description(desc), defaultValue(defaultVal), required(req)
        , validator(nullptr), minSize(0), maxSize(std::numeric_limits<size_t>::max()) {}
};

// Configuration schema
class ConfigSchema {
public:
    ConfigSchema() = default;
    
    // Add a schema entry
    void addEntry(const ConfigSchemaEntry& entry) {
        entries_[entry.key] = entry;
    }
    
    // Add a schema entry with convenience methods
    ConfigSchemaEntry& beginEntry(const std::string& key, const std::string& type,
                                  const std::string& description = "",
                                  const ConfigValue& defaultValue = ConfigValue(),
                                  bool required = false) {
        currentEntry_ = ConfigSchemaEntry(key, type, description, defaultValue, required);
        return currentEntry_;
    }
    
    // Convenience methods
    ConfigSchema& addInt(const std::string& key, const std::string& description = "",
                        int defaultValue = 0, bool required = false) {
        currentEntry_ = ConfigSchemaEntry(key, "int", description, defaultValue, required);
        entries_[key] = currentEntry_;
        return *this;
    }
    
    ConfigSchema& addDouble(const std::string& key, const std::string& description = "",
                           double defaultValue = 0.0, bool required = false) {
        currentEntry_ = ConfigSchemaEntry(key, "double", description, defaultValue, required);
        entries_[key] = currentEntry_;
        return *this;
    }
    
    ConfigSchema& addBool(const std::string& key, const std::string& description = "",
                         bool defaultValue = false, bool required = false) {
        currentEntry_ = ConfigSchemaEntry(key, "bool", description, defaultValue, required);
        entries_[key] = currentEntry_;
        return *this;
    }
    
    ConfigSchema& addString(const std::string& key, const std::string& description = "",
                           const std::string& defaultValue = "", bool required = false) {
        currentEntry_ = ConfigSchemaEntry(key, "string", description, defaultValue, required);
        entries_[key] = currentEntry_;
        return *this;
    }
    
    // Add constraints
    ConfigSchema& withRange(double min, double max) {
        currentEntry_.range = std::make_pair(min, max);
        return *this;
    }
    
    ConfigSchema& withAllowedValues(const std::set<std::string>& values) {
        currentEntry_.allowedValues = values;
        return *this;
    }
    
    ConfigSchema& withArraySize(size_t min, size_t max = std::numeric_limits<size_t>::max()) {
        currentEntry_.minSize = min;
        currentEntry_.maxSize = max;
        return *this;
    }
    
    ConfigSchema& withValidator(std::function<ValidationResult(const ConfigValue&)> validator) {
        currentEntry_.validator = validator;
        return *this;
    }
    
    // Finish current entry and return reference to schema
    ConfigSchema& endEntry() {
        entries_[currentEntry_.key] = currentEntry_;
        currentEntry_ = ConfigSchemaEntry();
        return *this;
    }
    
    // Get schema entry by key
    const ConfigSchemaEntry* getEntry(const std::string& key) const {
        auto it = entries_.find(key);
        return it != entries_.end() ? &it->second : nullptr;
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        keys.reserve(entries_.size());
        for (const auto& pair : entries_) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    // Validate configuration against schema
    ValidationResult validate(const Config& config) const {
        ValidationResult result;
        
        for (const auto& pair : entries_) {
            const auto& entry = pair.second;
            const std::string& key = entry.key;
            
            if (entry.required && !config.has(key)) {
                result.addError("Required configuration key missing: " + key);
                continue;
            }
            
            if (!config.has(key)) {
                // Use default value if not present
                continue;
            }
            
            // Get current value from config
            std::optional<ConfigValue> currentValue;
            if (entry.type == "int") {
                currentValue = config.get<int>(key);
            } else if (entry.type == "double") {
                currentValue = config.get<double>(key);
            } else if (entry.type == "bool") {
                currentValue = config.get<bool>(key);
            } else if (entry.type == "string") {
                currentValue = config.get<std::string>(key);
            } else if (entry.type == "int[]") {
                currentValue = config.get<std::vector<int>>(key);
            } else if (entry.type == "double[]") {
                currentValue = config.get<std::vector<double>>(key);
            } else if (entry.type == "string[]") {
                currentValue = config.get<std::vector<std::string>>(key);
            }
            
            if (!currentValue.has_value()) {
                result.addError("Cannot get configuration value for key: " + key);
                continue;
            }
            
            // Apply type-specific validation
            ValidationResult typeValidation = validateType(entry, currentValue.value());
            if (!typeValidation.isValid) {
                result.errors.insert(result.errors.end(), 
                                   typeValidation.errors.begin(), 
                                   typeValidation.errors.end());
            }
            
            // Apply custom validator
            if (entry.validator) {
                ValidationResult customValidation = entry.validator(currentValue.value());
                if (!customValidation.isValid) {
                    result.errors.insert(result.errors.end(), 
                                       customValidation.errors.begin(), 
                                       customValidation.errors.end());
                }
                result.warnings.insert(result.warnings.end(),
                                      customValidation.warnings.begin(),
                                      customValidation.warnings.end());
            }
        }
        
        return result;
    }
    
    // Get default value for key
    std::optional<ConfigValue> getDefault(const std::string& key) const {
        auto it = entries_.find(key);
        if (it != entries_.end()) {
            return it->second.defaultValue;
        }
        return std::nullopt;
    }
    
    // Get schema summary
    std::string summary() const {
        std::ostringstream oss;
        oss << "Configuration Schema (" << entries_.size() << " entries):\n";
        for (const auto& pair : entries_) {
            const auto& entry = pair.second;
            oss << "  " << entry.key << " [" << entry.type << "]";
            if (entry.required) oss << " (required)";
            if (!entry.description.empty()) {
                oss << " - " << entry.description;
            }
            oss << "\n";
        }
        return oss.str();
    }
    
private:
    ValidationResult validateType(const ConfigSchemaEntry& entry, const ConfigValue& value) const {
        ValidationResult result;
        
        // Check type matching
        bool typeMatch = false;
        if (entry.type == "int" && std::holds_alternative<int>(value)) typeMatch = true;
        else if (entry.type == "int64_t" && std::holds_alternative<int64_t>(value)) typeMatch = true;
        else if (entry.type == "double" && std::holds_alternative<double>(value)) typeMatch = true;
        else if (entry.type == "bool" && std::holds_alternative<bool>(value)) typeMatch = true;
        else if (entry.type == "string" && std::holds_alternative<std::string>(value)) typeMatch = true;
        else if (entry.type == "int[]" && std::holds_alternative<std::vector<int>>(value)) typeMatch = true;
        else if (entry.type == "double[]" && std::holds_alternative<std::vector<double>>(value)) typeMatch = true;
        else if (entry.type == "string[]" && std::holds_alternative<std::vector<std::string>>(value)) typeMatch = true;
        
        if (!typeMatch) {
            result.addError("Type mismatch for key '" + entry.key + "': expected " + 
                          entry.type + ", got " + getTypeName(value));
            return result;
        }
        
        // Range validation for numeric types
        if (entry.range.has_value()) {
            double min = entry.range->first;
            double max = entry.range->second;
            
            if (std::holds_alternative<int>(value)) {
                double val = static_cast<double>(std::get<int>(value));
                if (val < min || val > max) {
                    result.addError("Value for key '" + entry.key + "' out of range: " +
                                  std::to_string(val) + " (expected " + 
                                  std::to_string(min) + " to " + std::to_string(max) + ")");
                }
            } else if (std::holds_alternative<int64_t>(value)) {
                double val = static_cast<double>(std::get<int64_t>(value));
                if (val < min || val > max) {
                    result.addError("Value for key '" + entry.key + "' out of range: " +
                                  std::to_string(val) + " (expected " + 
                                  std::to_string(min) + " to " + std::to_string(max) + ")");
                }
            } else if (std::holds_alternative<double>(value)) {
                double val = std::get<double>(value);
                if (val < min || val > max) {
                    result.addError("Value for key '" + entry.key + "' out of range: " +
                                  std::to_string(val) + " (expected " + 
                                  std::to_string(min) + " to " + std::to_string(max) + ")");
                }
            }
        }
        
        // Allowed values validation for string types
        if (!entry.allowedValues.empty() && std::holds_alternative<std::string>(value)) {
            const std::string& val = std::get<std::string>(value);
            if (entry.allowedValues.find(val) == entry.allowedValues.end()) {
                result.addError("Invalid value for key '" + entry.key + "': '" + val + 
                              "' is not in allowed set");
            }
        }
        
        // Array size validation
        if ((entry.type == "int[]" || entry.type == "double[]" || entry.type == "string[]") &&
            std::holds_alternative<std::vector<int>>(value) || 
            std::holds_alternative<std::vector<double>>(value) ||
            std::holds_alternative<std::vector<std::string>>(value)) {
            
            auto getArraySize = [&value]() {
                if (std::holds_alternative<std::vector<int>>(value)) {
                    return std::get<std::vector<int>>(value).size();
                } else if (std::holds_alternative<std::vector<double>>(value)) {
                    return std::get<std::vector<double>>(value).size();
                } else {
                    return std::get<std::vector<std::string>>(value).size();
                }
            };
            
            size_t size = getArraySize();
            if (size < entry.minSize) {
                result.addError("Array for key '" + entry.key + "' too small: " +
                              std::to_string(size) + " (expected at least " +
                              std::to_string(entry.minSize) + ")");
            }
            if (size > entry.maxSize) {
                result.addError("Array for key '" + entry.key + "' too large: " +
                              std::to_string(size) + " (expected at most " +
                              std::to_string(entry.maxSize) + ")");
            }
        }
        
        return result;
    }
    
    std::string getTypeName(const ConfigValue& value) const {
        if (std::holds_alternative<int>(value)) return "int";
        if (std::holds_alternative<int64_t>(value)) return "int64_t";
        if (std::holds_alternative<double>(value)) return "double";
        if (std::holds_alternative<bool>(value)) return "bool";
        if (std::holds_alternative<std::string>(value)) return "string";
        if (std::holds_alternative<std::vector<int>>(value)) return "int[]";
        if (std::holds_alternative<std::vector<double>>(value)) return "double[]";
        if (std::holds_alternative<std::vector<std::string>>(value)) return "string[]";
        return "unknown";
    }
    
private:
    std::map<std::string, ConfigSchemaEntry> entries_;
    ConfigSchemaEntry currentEntry_;
};

// Enhanced configuration class with schema support
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
    
    // Schema management
    void setSchema(const ConfigSchema& schema) { schema_ = schema; }
    ValidationResult validateSchema() const {
        return schema_.validate(*this);
    }
    
    // Configuration validation with detailed error reporting
    ValidationResult validate() const {
        ValidationResult result;
        
        // Basic validation (existing checks)
        for (const auto& entry : pImpl->entries) {
            // Type consistency validation
            if (entry.value.index() >= std::variant_size_v<ConfigValue>) {
                result.addError("Invalid configuration value type for key: " + entry.key);
            }
            
            // Check for obviously invalid values
            if (entry.key == "neuron_count" && std::holds_alternative<int64_t>(entry.value)) {
                int64_t count = std::get<int64_t>(entry.value);
                if (count <= 0 || count > 1000000) {
                    result.addError("Invalid neuron_count: " + std::to_string(count) +
                                  " (must be between 1 and 1,000,000)");
                }
            }
            
            if (entry.key == "simulation_timestep" && std::holds_alternative<double>(entry.value)) {
                double timestep = std::get<double>(entry.value);
                if (timestep <= 0.0 || timestep > 1.0) {
                    result.addError("Invalid simulation_timestep: " + std::to_string(timestep) +
                                  " (must be between 0.0 and 1.0)");
                }
            }
            
            if (entry.key == "connection_probability" && std::holds_alternative<double>(entry.value)) {
                double prob = std::get<double>(entry.value);
                if (prob < 0.0 || prob > 1.0) {
                    result.addError("Invalid connection_probability: " + std::to_string(prob) +
                                  " (must be between 0.0 and 1.0)");
                }
            }
        }
        
        // Schema validation if present
        if (schema_.getKeys().size() > 0) {
            ValidationResult schemaValidation = validateSchema();
            if (!schemaValidation.isValid) {
                result.errors.insert(result.errors.end(),
                                   schemaValidation.errors.begin(),
                                   schemaValidation.errors.end());
            }
            result.warnings.insert(result.warnings.end(),
                                  schemaValidation.warnings.begin(),
                                  schemaValidation.warnings.end());
        }
        
        return result;
    }
    
    // Apply schema defaults for missing values
    void applySchemaDefaults() {
        if (!schema_.getKeys().empty()) {
            for (const auto& key : schema_.getKeys()) {
                if (!has(key)) {
                    auto defaultValue = schema_.getDefault(key);
                    if (defaultValue.has_value()) {
                        set(key, defaultValue.value(), ConfigSource::Default);
                    }
                }
            }
        }
    }
    
    // Configuration health check
    std::string healthCheck() const {
        std::ostringstream oss;
        oss << "Configuration Health Check:\n";
        
        ValidationResult validation = validate();
        
        if (validation.isValid) {
            oss << "✓ Configuration is valid\n";
        } else {
            oss << "✗ Configuration validation errors:\n";
            for (const auto& error : validation.errors) {
                oss << "  - " << error << "\n";
            }
        }
        
        if (!validation.warnings.empty()) {
            oss << "Warnings:\n";
            for (const auto& warning : validation.warnings) {
                oss << "  - " << warning << "\n";
            }
        }
        
        oss << "Total entries: " << pImpl->entries.size() << "\n";
        return oss.str();
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    ConfigSchema schema_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

#endif // CONFIG_HPP
