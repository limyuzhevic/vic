#pragma once

#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <algorithm>
#include <iomanip>
#include <regex>

#include "nlm/CLI.hpp"
#include "nlm/Exception.hpp"
#include "nlm/Constants.hpp"

namespace nlm {

/**
 * Configuration management system for NLM
 * Provides schema validation, file loading, and parameter management
 */
class ConfigManager {
public:
    // Configuration parameter definition
    struct ConfigParam {
        std::string name;
        std::string type;
        std::string description;
        std::string defaultValue;
        std::string minValue;
        std::string maxValue;
        bool required;
        std::vector<std::string> allowedValues;
        
        ConfigParam(const std::string& name, const std::string& type,
                   const std::string& description, const std::string& defaultValue = "",
                   const std::string& minValue = "", const std::string& maxValue = "",
                   bool required = false)
            : name(name), type(type), description(description), defaultValue(defaultValue),
              minValue(minValue), maxValue(maxValue), required(required) {}
    };

    // Configuration value with type safety
    class ConfigValue {
    public:
        enum class Type { String, Int, Float, Bool, Double };
        
        ConfigValue() = default;
        ConfigValue(const std::string& str);
        ConfigValue(int64_t i);
        ConfigValue(double d);
        ConfigValue(float f);
        ConfigValue(bool b);
        
        Type getType() const { return type; }
        const std::string& asString() const;
        int64_t asInt() const;
        double asDouble() const;
        float asFloat() const;
        bool asBool() const;
        
        bool isValid() const { return valid; }
        void invalidate() { valid = false; }
        
    private:
        Type type;
        bool valid = true;
        std::string strValue;
        int64_t intValue;
        double doubleValue;
        bool boolValue;
    };

    ConfigManager(const std::string& programName = "nlm");
    ~ConfigManager();

    // Load configuration from file
    bool loadFromFile(const std::string& path);
    
    // Load configuration from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Apply default values
    void applyDefaults();
    
    // Validate configuration
    bool validate() const;
    
    // Get configuration value as string
    std::string getString(const std::string& key) const;
    
    // Get configuration value with type conversion and validation
    int64_t getInt(const std::string& key) const;
    double getDouble(const std::string& key) const;
    float getFloat(const std::string& key) const;
    bool getBool(const std::string& key) const;
    
    // Get configuration value with default fallback
    int64_t getOrInt(const std::string& key, int64_t defaultValue) const;
    double getOrDouble(const std::string& key, double defaultValue) const;
    float getOrFloat(const std::string& key, float defaultValue) const;
    bool getOrBool(const std::string& key, bool defaultValue) const;
    
    // Set configuration value
    void set(const std::string& key, const std::string& value);
    void setInt(const std::string& key, int64_t value);
    void setDouble(const std::string& key, double value);
    void setFloat(const std::string& key, float value);
    void setBool(const std::string& key, bool value);
    
    // Check if configuration key exists
    bool hasKey(const std::string& key) const;
    
    // Get all configuration keys
    std::vector<std::string> getKeys() const;
    
    // Get configuration parameter definition
    const ConfigParam* getParameterInfo(const std::string& key) const;
    
    // Get schema version
    static const std::map<std::string, ConfigParam>& getSchema();
    
    // Get program name
    const std::string& getProgramName() const { return programName; }

private:
    std::string programName;
    std::map<std::string, ConfigValue> values;
    std::map<std::string, ConfigParam> parameterInfo;
    bool configLoaded;
    bool defaultsApplied;
    
    // Initialize parameter schema
    void initializeParameterSchema();
    
    // Parse configuration file
    bool parseConfigFile(const std::string& path);
    
    // Parse key-value line
    bool parseConfigLine(const std::string& line, std::string& key, std::string& value);
    
    // Validate individual configuration value
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    
    // Get value type from string
    static ConfigValue::Type getTypeFromString(const std::string& typeStr);
    
    // Convert string to appropriate type
    static ConfigValue stringToValue(const std::string& str, const std::string& type);
    
    // Trim whitespace from string
    static std::string trim(const std::string& str);
    
    // Check if string contains only digits (for integer validation)
    static bool isInteger(const std::string& str);
    
    // Check if string contains a float (for float/double validation)
    static bool isFloat(const std::string& str);
    
    // Check if string is boolean value
    static bool isBoolean(const std::string& str);
};

} // namespace nlm
