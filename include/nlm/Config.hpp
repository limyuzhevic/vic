#pragma once

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <cmath>
#include <random>

#include "nlm/Constants.hpp"

namespace nlm {

/**
 * Configuration system for NLM
 * Provides type-safe configuration access with validation
 */
class Config {
public:
    // Configuration value
    class Value {
    public:
        enum class Type {
            Integer,
            Float,
            String,
            Bool,
            Double,
            Unknown
        };

        Value() = default;
        Value(int64_t val) : type(Type::Integer), intValue(val) {}
        Value(double val) : type(Type::Double), doubleValue(val) {}
        Value(float val) : type(Type::Float), floatValue(val) {}
        Value(const std::string& val) : type(Type::String), strValue(val) {}
        Value(bool val) : type(Type::Bool), boolValue(val) {}

        Type getType() const { return type; }
        
        bool isInteger() const { return type == Type::Integer; }
        bool isDouble() const { return type == Type::Double; }
        bool isFloat() const { return type == Type::Float; }
        bool isString() const { return type == Type::String; }
        bool isBool() const { return type == Type::Bool; }
        
        int64_t asInt() const { return intValue; }
        double asDouble() const { return doubleValue; }
        float asFloat() const { return floatValue; }
        const std::string& asString() const { return strValue; }
        bool asBool() const { return boolValue; }
        
        void setInt(int64_t val) { type = Type::Integer; intValue = val; }
        void setDouble(double val) { type = Type::Double; doubleValue = val; }
        void setFloat(float val) { type = Type::Float; floatValue = val; }
        void setString(const std::string& val) { type = Type::String; strValue = val; }
        void setBool(bool val) { type = Type::Bool; boolValue = val; }
        
        bool isEmpty() const {
            return type == Type::Unknown || (type == Type::String && strValue.empty());
        }
        
        std::string toString() const;
        
        static Value parseFromString(const std::string& str);
        static bool compare(const Value& a, const Value& b);

    private:
        Type type = Type::Unknown;
        int64_t intValue = 0;
        double doubleValue = 0.0;
        float floatValue = 0.0f;
        std::string strValue;
        bool boolValue = false;
    };

    // Configuration source
    enum class ConfigSource {
        Default,
        ConfigFile,
        CommandLine,
        Runtime,
        Environment
    };

    // Configuration parameter definition
    struct ParameterDef {
        std::string name;
        std::string type;
        std::string description;
        Value defaultValue;
        Value minValue;
        Value maxValue;
        bool required = false;
        std::vector<std::string> allowedValues;
        std::string validationRegex;
        
        ParameterDef(const std::string& name, const std::string& type,
                    const std::string& description, const Value& defaultValue = Value())
            : name(name), type(type), description(description), defaultValue(defaultValue),
              required(false) {}
    };

    Config();
    ~Config();

    // Load configuration from file
    bool loadFromFile(const std::string& path);
    
    // Load configuration from arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Apply default values
    void applyDefaults();
    
    // Set configuration value
    void set(const std::string& key, const Value& value, ConfigSource source = ConfigSource::Runtime);
    
    // Get configuration value
    Value get(const std::string& key) const;
    
    // Get configuration value with type conversion
    template<typename T>
    T getAs(const std::string& key) const;
    
    // Get configuration value with default
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Check if key exists
    bool has(const std::string& key) const;
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get configuration source
    ConfigSource getSource(const std::string& key) const;
    
    // Validate configuration
    bool validate() const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
    // Get configuration summary
    std::string getSummary() const;
    
    // Reset configuration to defaults
    void reset();

protected:
    std::unordered_map<std::string, Value> values;
    std::unordered_map<std::string, ConfigSource> sources;
    std::vector<ParameterDef> parameterDefinitions;
    mutable std::vector<std::string> validationErrors;
    
    // Initialize parameter definitions
    void initializeParameterDefinitions();
    
    // Parse configuration line
    bool parseConfigLine(const std::string& line, std::string& key, std::string& value);
    
    // Parse command line arguments
    bool parseArgs(int argc, char** argv);
    
    // Parse --key=value or --key value format
    void parseArgument(const std::string& arg);
    
    // Validate single parameter
    bool validateParameter(const std::string& key, const Value& value) const;
    
    // Convert string to appropriate type
    static Value stringToValue(const std::string& str, const std::string& type);
    
    // Trim whitespace
    static std::string trim(const std::string& str);
    
    // Check if string is numeric
    static bool isNumeric(const std::string& str);
    
    // Check if string is boolean
    static bool isBoolean(const std::string& str);
    
    // Get parameter definition
    const ParameterDef* getParameterDefinition(const std::string& key) const;
    
    // Create default configuration
    void createDefaultConfiguration();
};

// Template implementations
namespace config {
    // Helper functions for type conversion
    int64_t stringToInt(const std::string& str);
    double stringToDouble(const std::string& str);
    float stringToFloat(const std::string& str);
    bool stringToBool(const std::string& str);
    std::string intToString(int64_t val);
    std::string doubleToString(double val);
    std::string floatToString(float val);
    std::string boolToString(bool val);
}

// Convenience functions
Value makeInt(int64_t val);
Value makeDouble(double val);
Value makeFloat(float val);
Value makeString(const std::string& val);
Value makeBool(bool val);

} // namespace nlm
