#pragma once

/**
 * Benchmark Configuration System for NLM
 * 
 * Comprehensive configuration system for benchmark scenarios, monitoring,
 * and analysis. Supports file-based configuration, validation, and help system.
 */

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

/**
 * Configuration error types
 */
enum class ConfigErrorType {
    NONE,
    FILE_NOT_FOUND,
    INVALID_SYNTAX,
    MISSING_VALUE,
    INVALID_VALUE,
    VALIDATION_FAILED,
    DUPLICATE_KEY
};

/**
 * Configuration error
 */
struct ConfigError {
    ConfigErrorType type;
    std::string message;
    std::string key;
    std::string file;
    int line;
    
    ConfigError(ConfigErrorType t, const std::string& msg, const std::string& key = "",
                const std::string& file = "", int line = 0)
        : type(t), message(msg), key(key), file(file), line(line) {}
};

/**
 * Configuration source (where a value came from)
 */
enum class ConfigSource {
    DEFAULT,    // Default value
    CONFIG_FILE, // Configuration file
    COMMAND_LINE, // Command line argument
    RUNTIME      // Set at runtime
};

/**
 * Configuration value wrapper (supports multiple types)
 */
template<typename T>
struct ConfigValue {
    T value;
    ConfigSource source;
    std::chrono::high_resolution_clock::time_point timestamp;
    
    ConfigValue() : value(T()), source(ConfigSource::DEFAULT) {}
    ConfigValue(const T& val, ConfigSource src) 
        : value(val), source(src), timestamp(std::chrono::high_resolution_clock::now()) {}
};

/**
 * Schema validation for configuration values
 */
struct ConfigSchema {
    std::string key;
    std::string description;
    std::string type;          // "int", "float", "string", "bool"
    std::string defaultValue;
    std::vector<std::string> validValues; // For enum-like values
    bool required;            // Whether this key must be present
    std::string validationRegex; // For string validation
    std::string minValue;     // For numeric types
    std::string maxValue;     // For numeric types
    
    ConfigSchema() : required(false) {}
    
    ConfigSchema(const std::string& k, const std::string& desc, const std::string& t,
                 const std::string& def = "", bool req = false)
        : key(k), description(desc), type(t), defaultValue(def), required(req) {}
};

/**
 * Configuration entry (key-value pair)
 */
class ConfigEntry {
public:
    std::string key;
    std::string stringValue;
    double numericValue;
    bool boolValue;
    ConfigSource source;
    std::chrono::high_resolution_clock::time_point timestamp;
    int lineNumber;
    
    ConfigEntry(const std::string& k, const std::string& val, ConfigSource src, int line)
        : key(k), stringValue(val), source(src), lineNumber(line) {
        timestamp = std::chrono::high_resolution_clock::now();
        parseValue();
    }
    
    template<typename T>
    T getValue() const {
        throw std::runtime_error("Generic getValue not supported for this type");
    }
    
    std::string getValueAsString() const { return stringValue; }
    double getValueAsNumber() const { return numericValue; }
    bool getValueAsBool() const { return boolValue; }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "  " << key << " = " << stringValue << " [source: ";
        switch (source) {
            case ConfigSource::DEFAULT: oss << "DEFAULT"; break;
            case ConfigSource::CONFIG_FILE: oss << "CONFIG_FILE"; break;
            case ConfigSource::COMMAND_LINE: oss << "COMMAND_LINE"; break;
            case ConfigSource::RUNTIME: oss << "RUNTIME"; break;
        }
        oss << "]";
        return oss.str();
    }
    
private:
    void parseValue() {
        // Try bool first
        if (stringValue == "true" || stringValue == "false") {
            boolValue = (stringValue == "true");
            numericValue = boolValue ? 1.0 : 0.0;
        }
        // Try int
        else {
            try {
                size_t pos;
                numericValue = std::stod(stringValue, &pos);
                if (pos == stringValue.length()) {
                    // Pure number
                    boolValue = (numericValue != 0.0);
                } else {
                    // Contains non-numeric characters
                    boolValue = false;
                }
            } catch (...) {
                numericValue = 0.0;
                boolValue = false;
            }
        }
    }
};

/**
 * Main configuration class
 */
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    // Value setting
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::RUNTIME);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::RUNTIME);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::RUNTIME);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::RUNTIME);
    
    // Value getting
    bool has(const std::string& key) const;
    std::string getAsString(const std::string& key) const;
    int64_t getAsInt(const std::string& key) const;
    double getAsDouble(const std::string& key) const;
    bool getAsBool(const std::string& key) const;
    
    // Type-specific getters
    template<typename T>
    T get(const std::string& key) const {
        throw std::runtime_error("Generic get not supported for this type");
    }
    
    std::string getOr(const std::string& key, const std::string& defaultValue) const;
    int64_t getOr(const std::string& key, int64_t defaultValue) const;
    double getOr(const std::string& key, double defaultValue) const;
    bool getOr(const std::string& key, bool defaultValue) const;
    
    // Key management
    std::vector<std::string> getKeys() const;
    std::string getAllKeysAsString() const;
    
    // Configuration loading
    bool loadFromFile(const std::string& filename);
    bool loadFromArgs(int argc, char** argv);
    
    // Configuration export
    std::string toString() const;
    bool saveToFile(const std::string& filename) const;
    
    // Schema validation
    bool validateSchema(const std::vector<ConfigSchema>& schema) const;
    std::vector<ConfigError> getValidationErrors(const std::vector<ConfigSchema>& schema) const;
    
    // Validation
    bool validateAll(std::vector<std::string>& errors) const;
    
    // Help and documentation
    static void printHelp(const std::vector<ConfigSchema>& schema);
    static void printExamples();
    
    // Factory methods
    static std::shared_ptr<Config> createDefaultConfig();
    static std::shared_ptr<Config> createMinimalConfig();
    static std::shared_ptr<Config> createExpertConfig();
    
    // Benchmark-specific configuration
    static std::shared_ptr<Config> createBenchmarkConfig(size_t neuronCount, size_t steps);
    
private:
    std::unordered_map<std::string, ConfigEntry> entries_;
    std::vector<ConfigError> errors_;
    
    // Internal parsing
    bool parseLine(const std::string& line, int lineNumber);
    bool parseKeyValue(const std::string& line, std::string& key, std::string& value, int lineNumber);
    std::string trimWhitespace(const std::string& str) const;
    
    // Error handling
    void addError(const ConfigError& error);
    bool hasErrors() const { return !errors_.empty(); }
    
    // Command line argument parsing
    bool parseCommandLineArg(const std::string& arg);
}; // class Config

} // namespace nlm