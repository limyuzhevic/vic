#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <set>
#include <stdexcept>
#include <vector>
#include <memory>
#include <typeinfo>
#include <functional>

namespace nlm {

class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message, 
                         const std::string& key = "", 
                         const std::string& expectedType = "")
        : std::runtime_error(message), key_(key), expectedType_(expectedType) {}
    
    const std::string& getKey() const { return key_; }
    const std::string& getExpectedType() const { return expectedType_; }
    
private:
    std::string key_;
    std::string expectedType_;
};

class ConfigSchema {
public:
    enum class Type { INT, INT64, DOUBLE, BOOL, STRING, STRING_LIST, INT_LIST, DOUBLE_LIST };
    struct SchemaEntry {
        std::string key;
        Type type;
        bool required;
        std::string description;
        std::any defaultValue;
        
        // Validation ranges
        std::optional<std::pair<double, double>> range;
        std::optional<std::set<std::string>> allowedValues;
        
        SchemaEntry(const std::string& k, Type t, bool req, const std::string& desc = "")
            : key(k), type(t), required(req), description(desc) {}
    };
    
    void addEntry(const std::string& key, Type type, bool required, 
                  const std::string& description = "",
                  const std::any& defaultValue = std::any(),
                  const std::optional<std::pair<double, double>>& range = std::nullopt,
                  const std::optional<std::set<std::string>>& allowedValues = std::nullopt) {
        SchemaEntry entry(key, type, required, description, defaultValue, range, allowedValues);
        entries_.push_back(entry);
    }
    
    const std::vector<SchemaEntry>& getEntries() const { return entries_; }
    
    bool validate(const Config& config) const {
        for (const auto& entry : entries_) {
            if (entry.required && !config.has(entry.key)) {
                throw ConfigValidationError("Required configuration key is missing", entry.key);
            }
            
            if (config.has(entry.key)) {
                validateEntry(config, entry);
            }
        }
        return true;
    }
    
private:
    void validateEntry(const Config& config, const SchemaEntry& entry) const {
        try {
            switch (entry.type) {
                case Type::INT: validateInt(config, entry); break;
                case Type::INT64: validateInt64(config, entry); break;
                case Type::DOUBLE: validateDouble(config, entry); break;
                case Type::BOOL: validateBool(config, entry); break;
                case Type::STRING: validateString(config, entry); break;
                case Type::STRING_LIST: validateStringList(config, entry); break;
                case Type::INT_LIST: validateIntList(config, entry); break;
                case Type::DOUBLE_LIST: validateDoubleList(config, entry); break;
            }
        } catch (const ConfigValidationError& e) {
            throw ConfigValidationError(
                "Validation failed for key '" + entry.key + "': " + e.what(),
                entry.key, getTypeName(entry.type)
            );
        }
    }
    
    void validateInt(const Config& config, const SchemaEntry& entry) const {
        auto value = config.get<int>(entry.key);
        if (!value) throw ConfigValidationError("Configuration value has wrong type", entry.key, "int");
        
        if (entry.range) {
            if (*value < entry.range->first || *value > entry.range->second) {
                throw ConfigValidationError(
                    "Value " + std::to_string(*value) + " is outside valid range [" +
                    std::to_string(entry.range->first) + ", " + 
                    std::to_string(entry.range->second) + "]",
                    entry.key, "int"
                );
            }
        }
    }
    
    void validateInt64(const Config& config, const SchemaEntry& entry) const {
        auto value = config.get<int64_t>(entry.key);
        if (!value) throw ConfigValidationError("Configuration value has wrong type", entry.key, "int64_t");
        
        if (entry.range) {
            if (*value < static_cast<int64_t>(entry.range->first) || 
                *value > static_cast<int64_t>(entry.range->second)) {
                throw ConfigValidationError(
                    "Value " + std::to_string(*value) + " is outside valid range [" +
                    std::to_string(entry.range->first) + ", " + 
                    std::to_string(entry.range->second) + "]",
                    entry.key, "int64_t"
                );
            }
        }
    }
    
    void validateDouble(const Config& config, const SchemaEntry& entry) const {
        auto value = config.get<double>(entry.key);
        if (!value) throw ConfigValidationError("Configuration value has wrong type", entry.key, "double");
        
        if (entry.range) {
            if (*value < entry.range->first || *value > entry.range->second) {
                throw ConfigValidationError(
                    "Value " + std::to_string(*value) + " is outside valid range [" +
                    std::to_string(entry.range->first) + ", " + 
                    std::to_string(entry.range->second) + "]",
                    entry.key, "double"
                );
            }
        }
    }
    
    void validateBool(const Config& config, const SchemaEntry& entry) const {
        auto value = config.get<bool>(entry.key);
        if (!value) throw ConfigValidationError("Configuration value has wrong type", entry.key, "bool");
    }
    
    void validateString(const Config& config, const SchemaEntry& entry) const {
        auto value = config.get<std::string>(entry.key);
        if (!value) throw ConfigValidationError("Configuration value has wrong type", entry.key, "string");
        
        if (entry.allowedValues && entry.allowedValues->find(*value) == entry.allowedValues->end()) {
            throw ConfigValidationError(
                "Value '" + *value + "' is not in allowed set: " +
                formatSet(*entry.allowedValues),
                entry.key, "string"
            );
        }
    }
    
    void validateStringList(const Config& config, const SchemaEntry& entry) const {
        auto value = config.get<std::vector<std::string>>(entry.key);
        if (!value) throw ConfigValidationError("Configuration value has wrong type", entry.key, "string list");
    }
    
    void validateIntList(const Config& config, const SchemaEntry& entry) const {
        auto value = config.get<std::vector<int>>(entry.key);
        if (!value) throw ConfigValidationError("Configuration value has wrong type", entry.key, "int list");
    }
    
    void validateDoubleList(const Config& config, const SchemaEntry& entry) const {
        auto value = config.get<std::vector<double>>(entry.key);
        if (!value) throw ConfigValidationError("Configuration value has wrong type", entry.key, "double list");
    }
    
    std::string getTypeName(Type type) const {
        switch (type) {
            case Type::INT: return "int";
            case Type::INT64: return "int64_t";
            case Type::DOUBLE: return "double";
            case Type::BOOL: return "bool";
            case Type::STRING: return "string";
            case Type::STRING_LIST: return "string list";
            case Type::INT_LIST: return "int list";
            case Type::DOUBLE_LIST: return "double list";
            default: return "unknown";
        }
    }
    
    std::string formatSet(const std::set<std::string>& values) const {
        std::string result;
        bool first = true;
        for (const auto& val : values) {
            if (!first) result += ", ";
            result += "'" + val + "'";
            first = false;
        }
        return result;
    }
    
    std::vector<SchemaEntry> entries_;
};

class ConfigurationManager {
public:
    static ConfigurationManager& getInstance() {
        static ConfigurationManager instance;
        return instance;
    }
    
    void registerSchema(const std::string& name, const ConfigSchema& schema) {
        schemas_[name] = schema;
    }
    
    const ConfigSchema& getSchema(const std::string& name) const {
        auto it = schemas_.find(name);
        if (it == schemas_.end()) {
            throw std::runtime_error("Schema not found: " + name);
        }
        return it->second;
    }
    
    bool validate(const Config& config, const std::string& schemaName) {
        const auto& schema = getSchema(schemaName);
        return schema.validate(config);
    }
    
    std::vector<std::string> getSchemaNames() const {
        std::vector<std::string> names;
        names.reserve(schemas_.size());
        for (const auto& pair : schemas_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Create presets
    static void createDefaultPreset() {
        ConfigurationManager& manager = getInstance();
        
        ConfigSchema defaultSchema;
        defaultSchema.addEntry("random_seed", ConfigSchema::Type::INT64, false, 
                             "Random seed for reproducible experiments", 42LL,
                             std::pair<double, double>(0, std::numeric_limits<int64_t>::max()));
        defaultSchema.addEntry("simulation_timestep", ConfigSchema::Type::DOUBLE, false,
                             "Simulation timestep in seconds", 0.001,
                             std::pair<double, double>(0.0001, 1.0));
        defaultSchema.addEntry("neuron_count", ConfigSchema::Type::INT64, false,
                             "Number of neurons in the brain", 1000LL,
                             std::pair<double, double>(10, 100000));
        defaultSchema.addEntry("region_count", ConfigSchema::Type::INT64, false,
                             "Number of brain regions", 1LL,
                             std::pair<double, double>(1, 100));
        defaultSchema.addEntry("connection_probability", ConfigSchema::Type::DOUBLE, false,
                             "Connection probability for random connectivity", 0.1,
                             std::pair<double, double>(0.0, 1.0));
        
        manager.registerSchema("default", defaultSchema);
    }
    
    static void createQuickTestPreset() {
        ConfigurationManager& manager = getInstance();
        
        ConfigSchema quickTestSchema;
        quickTestSchema.addEntry("random_seed", ConfigSchema::Type::INT64, false,
                               "Random seed for reproducible tests", 123456789LL);
        quickTestSchema.addEntry("neuron_count", ConfigSchema::Type::INT64, false,
                               "Quick test neuron count", 100LL);
        quickTestSchema.addEntry("max_simulation_steps", ConfigSchema::Type::INT64, false,
                               "Maximum simulation steps for quick test", 1000LL);
        
        manager.registerSchema("quick_test", quickTestSchema);
    }
    
    static void createDevelopmentPreset() {
        ConfigurationManager& manager = getInstance();
        
        ConfigSchema devSchema;
        devSchema.addEntry("random_seed", ConfigSchema::Type::INT64, false,
                         "Random seed for development", std::any());
        devSchema.addEntry("simulation_timestep", ConfigSchema::Type::DOUBLE, false,
                         "High precision timestep", 0.0001,
                         std::pair<double, double>(0.00001, 0.1));
        devSchema.addEntry("neuron_count", ConfigSchema::Type::INT64, false,
                         "Development brain size", 1000LL);
        devSchema.addEntry("log_level", ConfigSchema::Type::STRING, false,
                         "Log level", std::string("DEBUG"),
                         std::nullopt,
                         std::set<std::string>{"DEBUG", "INFO", "WARN", "ERROR"});
        devSchema.addEntry("log_to_file", ConfigSchema::Type::BOOL, false,
                         "Log to file for development", true);
        
        manager.registerSchema("development", devSchema);
    }
    
private:
    ConfigurationManager() {
        createDefaultPreset();
        createQuickTestPreset();
        createDevelopmentPreset();
    }
    
    std::map<std::string, ConfigSchema> schemas_;
};

// Enhanced Config class with validation and schema support
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (supports JSON, YAML, and key=value formats)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonStr);
    
    // Load from YAML string
    bool loadFromYAML(const std::string& yamlStr);
    
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
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration validation
    bool validate(const std::string& schemaName = "default") const;
    std::vector<std::string> getAvailableSchemas() const;
    
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
    
    // Preset configurations
    static void createPreset(const std::string& name, const ConfigSchema& schema);
    std::string applyPreset(const std::string& name);
    
    // Error handling
    class ConfigError : public std::runtime_error {
    public:
        ConfigError(const std::string& message, ErrorCode code = ErrorCode::UNKNOWN)
            : std::runtime_error(message), code_(code) {}
        
        ErrorCode getCode() const { return code_; }
        
    private:
        ErrorCode code_;
    };
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isEmpty(const std::string& str);
    
    // JSON/YAML parsing helpers
    static nlohmann::json convertConfigValueToJSON(const ConfigValue& value);
    static ConfigValue convertJSONToConfigValue(const nlohmann::json& jsonValue);
    static YAML::Node convertConfigValueToYAML(const ConfigValue& value);
    static ConfigValue convertYAMLToConfigValue(const YAML::Node& yamlValue);
    
    // File type detection
    static std::string detectFileType(const std::string& filepath);
    
    // Error handling helpers
    static std::string errorCodeToString(ErrorCode code);
};

// Implementation details
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::string> fileHeaders;  // Comments and metadata
};

// Error codes for configuration
enum class ErrorCode {
    UNKNOWN = 0,
    FILE_NOT_FOUND = 1,
    PARSE_ERROR = 2,
    VALIDATION_ERROR = 3,
    INVALID_TYPE = 4,
    MISSING_KEY = 5,
    INVALID_VALUE = 6
};

// Helper function for getting value from variant
template<typename T>
T variantGetAs(const ConfigValue& value) {
    return std::get<T>(value);
}

} // namespace nlm