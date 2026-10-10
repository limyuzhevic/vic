#pragma once

#include <string>
#include <map>
#include <vector>
#include <variant>

// Forward declaration to match Config.hpp
namespace nlm {
class Config;
}

// Configuration value types (must match Config.hpp)
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

// Configuration schema types
enum class ConfigType {
    Integer,
    Integer64,
    Float,
    Boolean,
    String,
    IntVector,
    FloatVector,
    StringVector,
    Unknown
};

// Configuration schema entry
struct ConfigSchemaEntry {
    std::string name;
    ConfigType type;
    std::string description;
    ConfigValue defaultValue;
    std::vector<ConfigValue> validValues;
    std::pair<double, double> range;  // For numeric types
    bool required;
    bool experimental;
    
    ConfigSchemaEntry() : type(ConfigType::Unknown), required(false), experimental(false) {}
    
    ConfigSchemaEntry(const std::string& name, ConfigType type, const std::string& description,
                     const ConfigValue& defaultVal,
                     bool required = false, bool experimental = false)
        : name(name), type(type), description(description), defaultValue(defaultVal),
          required(required), experimental(experimental) {}
    
    // Set valid values
    void setValidValues(const std::vector<int>& values) {
        validValues.clear();
        for (int val : values) {
            validValues.push_back(val);
        }
    }
    
    void setValidValues(const std::vector<int64_t>& values) {
        validValues.clear();
        for (int64_t val : values) {
            validValues.push_back(val);
        }
    }
    
    void setValidValues(const std::vector<double>& values) {
        validValues.clear();
        for (double val : values) {
            validValues.push_back(val);
        }
    }
    
    void setValidValues(const std::vector<bool>& values) {
        validValues.clear();
        for (bool val : values) {
            validValues.push_back(val);
        }
    }
    
    void setValidValues(const std::vector<std::string>& values) {
        validValues.clear();
        for (const std::string& val : values) {
            validValues.push_back(val);
        }
    }
    
    // Set range
    void setRange(double min, double max) {
        range.first = min;
        range.second = max;
    }
    
    // Get type name as string
    std::string getTypeName() const {
        switch (type) {
            case ConfigType::Integer: return "int";
            case ConfigType::Integer64: return "int64_t";
            case ConfigType::Float: return "float/double";
            case ConfigType::Boolean: return "bool";
            case ConfigType::String: return "string";
            case ConfigType::IntVector: return "vector<int>";
            case ConfigType::FloatVector: return "vector<double>";
            case ConfigType::StringVector: return "vector<string>";
            default: return "unknown";
        }
    }
    
    // Check if value is valid
    bool isValid(const ConfigValue& value) const {
        // Type check
        bool typeMatches = false;
        switch (type) {
            case ConfigType::Integer:
                typeMatches = std::holds_alternative<int>(value);
                break;
            case ConfigType::Integer64:
                typeMatches = std::holds_alternative<int64_t>(value);
                break;
            case ConfigType::Float:
                typeMatches = std::holds_alternative<double>(value);
                break;
            case ConfigType::Boolean:
                typeMatches = std::holds_alternative<bool>(value);
                break;
            case ConfigType::String:
                typeMatches = std::holds_alternative<std::string>(value);
                break;
            case ConfigType::IntVector:
                typeMatches = std::holds_alternative<std::vector<int>>(value);
                break;
            case ConfigType::FloatVector:
                typeMatches = std::holds_alternative<std::vector<double>>(value);
                break;
            case ConfigType::StringVector:
                typeMatches = std::holds_alternative<std::vector<std::string>>(value);
                break;
            default:
                return false;
        }
        
        if (!typeMatches) return false;
        
        // Range/valid values check
        if (!validValues.empty()) {
            // Check if value is in validValues
            for (const auto& valid : validValues) {
                if (value == valid) return true;
            }
            return false;
        } else if (range.first != 0.0 || range.second != 0.0) {
            // Check if value is in range (for numeric types only)
            if (std::holds_alternative<double>(value)) {
                double val = std::get<double>(value);
                return val >= range.first && val <= range.second;
            }
        }
        
        return true;
    }
};

// Configuration schema
class ConfigSchema {
public:
    ConfigSchema();
    ~ConfigSchema() = default;
    
    // Add a configuration entry
    void addEntry(const ConfigSchemaEntry& entry);
    
    // Get a configuration entry
    ConfigSchemaEntry getEntry(const std::string& name) const;
    
    // Check if entry exists
    bool hasEntry(const std::string& name) const;
    
    // Get all entry names
    std::vector<std::string> getEntryNames() const;
    
    // Get all entries
    std::vector<ConfigSchemaEntry> getAllEntries() const;
    
    // Get entries by type
    std::vector<ConfigSchemaEntry> getEntriesByType(ConfigType type) const;
    
    // Get entries by category (based on naming convention)
    std::vector<ConfigSchemaEntry> getEntriesByCategory(const std::string& category) const;
    
    // Validate configuration value against schema
    bool validate(const std::string& key, const ConfigValue& value) const;
    
    // Validate configuration against schema
    bool validate(const std::map<std::string, ConfigValue>& config) const;
    
    // Get configuration suggestions
    std::vector<std::string> getSuggestions(const std::string& partialKey) const;
    
    // Get schema summary
    std::string getSummary() const;
    
    // Get schema description
    std::string getDescription(const std::string& key) const;
    
    // Get default value
    ConfigValue getDefaultValue(const std::string& key) const;
    
    // Check if entry is required
    bool isRequired(const std::string& key) const;
    
    // Check if entry is experimental
    bool isExperimental(const std::string& key) const;
    
    // Initialize default schema
    static ConfigSchema createDefaultSchema();
    
    // Create configuration from schema defaults
    static std::map<std::string, ConfigValue> createConfigFromSchema(const ConfigSchema& schema);
    
private:
    std::map<std::string, ConfigSchemaEntry> entries;
    
    // Initialize default schema
    void initializeSchema();
};

} // namespace nlm