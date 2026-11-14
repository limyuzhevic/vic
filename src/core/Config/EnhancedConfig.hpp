// Enhanced Configuration Management for NLM
// Provides advanced configuration loading, validation, and management
// Supports environment variables, parameter overrides, and validation rules

#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <regex>

using namespace std;

// Configuration value types
enum class ConfigValueType {
    INTEGER,
    DOUBLE,
    STRING,
    BOOLEAN,
    VECTOR_FLOAT,
    UNKNOWN
};

// Configuration validation error
struct ConfigError {
    string field;
    string message;
    bool isCritical;
};

// Configuration validation rules
struct ValidationRule {
    string field;
    function<bool(const string&)> validator;
    string errorMessage;
    bool isRequired = false;
};

// Configuration value wrapper
class ConfigValue {
public:
    ConfigValue() : type_(ConfigValueType::UNKNOWN) {}
    
    template<typename T>
    void set(const T& value) {
        if constexpr (is_same_v<T, int64_t> || is_same_v<T, int>) {
            intValue_ = static_cast<int64_t>(value);
            type_ = ConfigValueType::INTEGER;
        } else if constexpr (is_same_v<T, double> || is_same_v<T, float>) {
            doubleValue_ = static_cast<double>(value);
            type_ = ConfigValueType::DOUBLE;
        } else if constexpr (is_same_v<T, string>) {
            stringValue_ = value;
            type_ = ConfigValueType::STRING;
        } else if constexpr (is_same_v<T, bool>) {
            boolValue_ = value;
            type_ = ConfigValueType::BOOLEAN;
        } else if constexpr (is_same_v<T, vector<float>>) {
            vectorFloatValue_ = value;
            type_ = ConfigValueType::VECTOR_FLOAT;
        }
    }
    
    // Getters with type safety
    int64_t getInt() const { return intValue_; }
    double getDouble() const { return doubleValue_; }
    const string& getString() const { return stringValue_; }
    bool getBool() const { return boolValue_; }
    const vector<float>& getVectorFloat() const { return vectorFloatValue_; }
    
    // Type detection
    ConfigValueType getType() const { return type_; }
    bool isEmpty() const {
        switch (type_) {
            case ConfigValueType::INTEGER: return false; // 0 is valid
            case ConfigValueType::DOUBLE: return false; // 0.0 is valid
            case ConfigValueType::STRING: return stringValue_.empty();
            case ConfigValueType::BOOLEAN: return false; // false is valid
            case ConfigValueType::VECTOR_FLOAT: return vectorFloatValue_.empty();
            default: return true;
        }
    }
    
private:
    ConfigValueType type_;
    int64_t intValue_;
    double doubleValue_;
    string stringValue_;
    bool boolValue_;
    vector<float> vectorFloatValue_;
};

// Enhanced configuration class with validation and environment support
class EnhancedConfig {
public:
    EnhancedConfig() : source_(ConfigSource::Default) {}
    virtual ~EnhancedConfig() = default;
    
    // Set configuration value with source tracking
    void set(const string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Default);
    
    // Template convenience methods
    template<typename T>
    void set(const string& key, const T& value, ConfigSource source = ConfigSource::Default) {
        ConfigValue configValue;
        configValue.set(value);
        set(key, configValue, source);
    }
    
    // Get configuration value with type safety
    const ConfigValue* get(const string& key) const;
    
    // Template convenience methods
    template<typename T>
    T get(const string& key, const T& defaultValue) const {
        const ConfigValue* value = get(key);
        if (!value) return defaultValue;
        
        // Type-specific getters
        if constexpr (is_same_v<T, int64_t> || is_same_v<T, int>) {
            return static_cast<T>(value->getInt());
        } else if constexpr (is_same_v<T, double> || is_same_v<T, float>) {
            return static_cast<T>(value->getDouble());
        } else if constexpr (is_same_v<T, string>) {
            return value->getString();
        } else if constexpr (is_same_v<T, bool>) {
            return value->getBool();
        } else if constexpr (is_same_v<T, vector<float>>) {
            return value->getVectorFloat();
        }
        return defaultValue;
    }
    
    // Configuration loading
    bool loadFromFile(const string& filename);
    bool loadFromEnv(const string& prefix = "NLM_");
    bool loadFromArgs(int argc, char* argv[]);
    
    // Configuration validation
    vector<ConfigError> validate(const vector<ValidationRule>& rules) const;
    bool isValid(const vector<ValidationRule>& rules) const;
    
    // Configuration management
    bool merge(const EnhancedConfig& other, ConfigSource source = ConfigSource::Override);
    size_t size() const { return configMap_.size(); }
    vector<string> getKeys() const;
    map<string, string> getAllAsString() const;
    
    // Environment variable helpers
    static string toEnvKey(const string& configKey);
    static string fromEnvKey(const string& envKey);
    
    // Configuration sources
    ConfigSource getSource(const string& key) const;
    
    // Utility functions
    static ConfigValueType detectType(const string& str);
    static bool parseBool(const string& str);
    static double parseDouble(const string& str);
    static int64_t parseInt(const string& str);
    static vector<float> parseFloatVector(const string& str);
    
    // Conversion helpers
    static string toString(const ConfigValue& value);
    static bool stringToValue(const string& str, ConfigValue& value, ConfigValueType targetType);
    
protected:
    map<string, pair<ConfigValue, ConfigSource>> configMap_;
    ConfigSource source_;
    
    // Validation helpers
    bool validateAgainstRules(const ValidationRule& rule, const ConfigValue* value) const;
    static bool matchesPattern(const string& str, const string& pattern);
};

// Enhanced logger with configuration support
class EnhancedLogger {
public:
    EnhancedLogger();
    virtual ~EnhancedLogger() = default;
    
    // Log levels
    enum class Level {
        OFF = 0,
        FATAL = 1,
        ERROR = 2,
        WARN = 3,
        INFO = 4,
        DEBUG = 5,
        TRACE = 6
    };
    
    // Logging interface
    void log(Level level, const string& message);
    void log(const string& level, const string& message);
    
    // Convenience methods
    void fatal(const string& message);
    void error(const string& message);
    void warn(const string& message);
    void info(const string& message);
    void debug(const string& message);
    void trace(const string& message);
    
    // Configuration methods
    void setLevel(Level level);
    void setLevelFromString(const string& levelStr);
    Level getLevel() const { return currentLevel_; }
    
    // Output configuration
    void setOutput(const string& filename);
    void setOutputToConsole();
    void setOutputToBoth(const string& filename);
    
    // Formatting and filtering
    void setFormat(const string& format);
    void addPatternFilter(const string& pattern, bool include = true);
    void setMaxLogSize(size_t maxSize);
    void setMaxLogFiles(size_t maxFiles);
    
    // File management
    void rotateLogFile();
    bool isLogFileFull() const;
    
    // Timestamp and formatting helpers
    static string getTimestamp(Level level = Level::INFO);
    static string formatMessage(Level level, const string& message);
    
protected:
    Level currentLevel_;
    string outputFile_;
    bool outputToConsole_;
    string format_;
    vector<pair<string, bool>> patternFilters_;
    size_t maxLogSize_;
    size_t maxLogFiles_;
    size_t currentLogSize_;
    string currentLogFile_;
    
    // File management
    void writeToFile(const string& message);
    void writeToConsole(const string& message);
    bool shouldLog(Level level, const string& message) const;
    static bool matchesPattern(const string& message, const string& pattern);
};

// Configuration validation helpers
namespace ConfigValidation {
    // Common validation rules
    vector<ValidationRule> getDefaultValidationRules();
    
    // Type-specific validators
    bool validateInt(const string& str);
    bool validateDouble(const string& str);
    bool validateString(const string& str);
    bool validateBool(const string& str);
    bool validateFloatVector(const string& str);
    
    // Range validators
    bool validateRange(const string& str, double min, double max);
    bool validateMin(const string& str, double min);
    bool validateMax(const string& str, double max);
    
    // File validators
    bool validateFileExists(const string& str);
    bool validateDirectoryExists(const string& str);
    
    // Format validators
    bool validateEnvVarName(const string& str);
    bool validateParamName(const string& str);
    
    // Custom validators with parameters
    bool createRangeValidator(double min, double max);
    bool createMinValidator(double min);
    bool createMaxValidator(double max);
    bool createFileExistsValidator();
    bool createDirectoryExistsValidator();
    
    // Validation error types
    enum class ValidationErrorType {
        VALUE_ERROR,
        MISSING_REQUIRED,
        TYPE_ERROR,
        RANGE_ERROR,
        FILE_ERROR,
        FORMAT_ERROR
    };
}

// Configuration batch manager
class ConfigBatchManager {
public:
    ConfigBatchManager();
    virtual ~ConfigBatchManager() = default;
    
    // Batch operations
    bool addConfiguration(const EnhancedConfig& config);
    bool addConfiguration(const string& filename);
    bool mergeConfigurations(const vector<string>& filenames);
    
    // Configuration validation
    vector<ConfigError> validateAll(const vector<ValidationRule>& rules) const;
    bool isValidAll(const vector<ValidationRule>& rules) const;
    
    // Configuration export
    EnhancedConfig exportAsSingleConfig() const;
    map<string, EnhancedConfig> exportAsSeparateConfigs() const;
    
    // Utility operations
    size_t getConfigCount() const { return configs_.size(); }
    vector<string> getConfigNames() const;
    
protected:
    vector<EnhancedConfig> configs_;
    map<string, EnhancedConfig> namedConfigs_;
};

// Configuration presets and profiles
namespace ConfigProfiles {
    // Common configuration profiles
    EnhancedConfig getDevelopmentProfile();
    EnhancedConfig getProductionProfile();
    EnhancedConfig getBenchmarkProfile();
    EnhancedConfig getDebugProfile();
    
    // Phase-specific profiles
    EnhancedConfig getPhase2Profile();
    EnhancedConfig getPhase3Profile();
    EnhancedConfig getPhase4Profile();
    EnhancedConfig getPhase5Profile();
    EnhancedConfig getPhase6Profile();
    
    // Performance profiles
    EnhancedConfig getHighPerformanceProfile();
    EnhancedConfig getBalancedProfile();
    EnhancedConfig getLowMemoryProfile();
    
    // Environment-specific profiles
    EnhancedConfig getCloudProfile();
    EnhancedConfig getLocalProfile();
    EnhancedConfig getContainerProfile();
    
    // Custom profiles
    EnhancedConfig createCustomProfile(const string& name, const EnhancedConfig& base);
    
    // Profile merging
    EnhancedConfig mergeProfiles(const vector<string>& profileNames);
    bool saveProfile(const EnhancedConfig& config, const string& filename);
    EnhancedConfig loadProfile(const string& filename);
}
