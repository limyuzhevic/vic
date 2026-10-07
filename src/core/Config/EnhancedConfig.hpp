#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <chrono>
#include <ctime>

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

// Configuration source with enhanced types
enum class ConfigSource {
    Default = 0,
    File = 1,
    CommandLine = 2,
    Runtime = 3,
    Environment = 4
};

// Configuration validation result
struct ValidationResult {
    bool isValid;
    std::string errorMessage;
    std::string suggestion;
    
    ValidationResult(bool valid = true, const std::string& error = "", const std::string& suggestion = "")
        : isValid(valid), errorMessage(error), suggestion(suggestion) {}
};

// Configuration constraint (validation rule)
struct ConfigConstraint {
    std::string key;
    std::function<ValidationResult(const ConfigValue&)> validator;
    std::string description;
    ConfigSource source; // Which sources this constraint applies to
    
    ConfigConstraint(const std::string& k, std::function<ValidationResult(const ConfigValue&)> v,
                    const std::string& desc = "", ConfigSource src = ConfigSource::Runtime)
        : key(k), validator(v), description(desc), source(src) {}
};

// Configuration option definition (for help/documentation)
struct ConfigOption {
    std::string key;
    ConfigValue defaultValue;
    std::string typeName;
    std::string description;
    std::string example;
    ConfigSource source;
    bool required;
    std::vector<std::string> validValues;
    double minValue;
    double maxValue;
    
    ConfigOption(const std::string& k, const ConfigValue& defaultVal, const std::string& type,
                 const std::string& desc = "", const std::string& example = "",
                 ConfigSource src = ConfigSource::Default, bool req = false)
        : key(k), defaultValue(defaultVal), typeName(type), description(desc), example(example),
          source(src), required(req), minValue(0.0), maxValue(0.0) {}
    
    ConfigOption(const std::string& k, const std::string& defaultVal, const std::string& type,
                 const std::string& desc = "", const std::string& example = "",
                 ConfigSource src = ConfigSource::Default, bool req = false)
        : key(k), defaultValue(defaultVal), typeName(type), description(desc), example(example),
          source(src), required(req), minValue(0.0), maxValue(0.0) {}
    
    ConfigOption(const std::string& k, int defaultVal, const std::string& type,
                 const std::string& desc = "", const std::string& example = "",
                 ConfigSource src = ConfigSource::Default, bool req = false, double min = 0.0, double max = 0.0)
        : key(k), defaultValue(defaultVal), typeName(type), description(desc), example(example),
          source(src), required(req), minValue(min), maxValue(max) {}
    
    ConfigOption(const std::string& k, double defaultVal, const std::string& type,
                 const std::string& desc = "", const std::string& example = "",
                 ConfigSource src = ConfigSource::Default, bool req = false, double min = 0.0, double max = 0.0)
        : key(k), defaultValue(defaultVal), typeName(type), description(desc), example(example),
          source(src), required(req), minValue(min), maxValue(max) {}
    
    ConfigOption(const std::string& k, bool defaultVal, const std::string& type,
                 const std::string& desc = "", const std::string& example = "",
                 ConfigSource src = ConfigSource::Default, bool req = false)
        : key(k), defaultValue(defaultVal), typeName(type), description(desc), example(example),
          source(src), required(req), minValue(0.0), maxValue(0.0) {}
};

// Enhanced configuration class with environment variable support, validation, and better documentation
class EnhancedConfig {
public:
    EnhancedConfig();
    ~EnhancedConfig();
    
    // Disable copying, enable moving
    EnhancedConfig(const EnhancedConfig&) = delete;
    EnhancedConfig& operator=(const EnhancedConfig&) = delete;
    EnhancedConfig(EnhancedConfig&&) noexcept;
    EnhancedConfig& operator=(EnhancedConfig&&) noexcept;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from environment variables
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with enhanced type support
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Set with validation
    template<typename T>
    bool setValidated(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get all keys for a specific source
    std::vector<std::string> getKeys(ConfigSource source) const;
    
    // Get configuration summary
    std::string summary() const;
    
    // Get help information
    std::string help() const;
    
    // Validate configuration
    bool validate() const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
    // Clear all
    void clear();
    
    // Merge with another config (runtime values override)
    void merge(const EnhancedConfig& other);
    
    // Set default values from a configuration template
    void applyDefaults(const std::vector<ConfigOption>& defaults);
    
    // Add constraint
    void addConstraint(const ConfigConstraint& constraint);
    
    // Get source for a key
    ConfigSource getSource(const std::string& key) const;
    
    // Check if key came from environment
    bool fromEnvironment(const std::string& key) const;
    
    // Get environment variable name for a key
    static std::string environmentVariableName(const std::string& key, const std::string& prefix = "NLM_");
    
    // Get configuration statistics
    struct ConfigStats {
        size_t totalEntries;
        size_t fromFile;
        size_t fromCommandLine;
        size_t fromRuntime;
        size_t fromEnvironment;
        size_t validationErrors;
        double lastModifiedTimestamp;
        
        ConfigStats()
            : totalEntries(0), fromFile(0), fromCommandLine(0), fromRuntime(0), fromEnvironment(0), validationErrors(0), lastModifiedTimestamp(0.0) {}
    };
    
    ConfigStats getStats() const;
    
    // Configuration preset management
    bool addPreset(const std::string& name, const std::vector<ConfigOption>& options);
    bool applyPreset(const std::string& name);
    std::vector<std::string> getPresets() const;
    
    // Version management
    void setVersion(const std::string& version);
    std::string getVersion() const;
    
    // Validation callback
    using ValidationCallback = std::function<bool(const std::string&, const ConfigValue&, ConfigSource)>;
    void setValidationCallback(const ValidationCallback& callback);
    
    // Change notification
    using ChangeCallback = std::function<void(const std::string&, const ConfigValue&, const ConfigValue&, ConfigSource)>;
    void setChangeCallback(const ChangeCallback& callback);
    
    // Configuration history
    struct ConfigChange {
        std::string key;
        ConfigValue oldValue;
        ConfigValue newValue;
        ConfigSource source;
        std::chrono::system_clock::time_point timestamp;
        std::string description;
        
        ConfigChange(const std::string& k, const ConfigValue& oldVal, const ConfigValue& newVal, ConfigSource src, const std::string& desc = "")
            : key(k), oldValue(oldVal), newValue(newVal), source(src), description(desc) {
            timestamp = std::chrono::system_clock::now();
        }
    };
    
    std::vector<ConfigChange> getChangeHistory(size_t maxHistory = 100) const;
    
    // Backup and restore
    bool backup(const std::string& filepath) const;
    bool restore(const std::string& filepath);
    
    // Configuration comparison
    bool equals(const EnhancedConfig& other) const;
    
    // Get configuration hash
    std::string getHash() const;
    
    // Configuration encryption (for sensitive values)
    bool setEncryptedKeys(const std::vector<std::string>& keys);
    bool isEncrypted(const std::string& key) const;
    
    // Configuration serialization
    std::string toJSON() const;
    bool fromJSON(const std::string& json);
    
    // Configuration merge strategies
    enum class MergeStrategy {
        Override,        // New values override existing ones
        Merge,           // Merge compatible types
        KeepOlder,       // Keep older values
        KeepNewer,       // Keep newer values
        Conditional      // Only override if different
    };
    
    // Advanced merging
    void merge(const EnhancedConfig& other, MergeStrategy strategy);
    
    // Configuration validation for specific scenarios
    enum class ValidationScenario {
        Default,
        Production,
        Development,
        Testing,
        Benchmark
    };
    
    ValidationResult validateScenario(ValidationScenario scenario) const;
    
    // Configuration health check
    struct HealthReport {
        bool healthy;
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
        double healthScore; // 0.0 to 1.0
        
        HealthReport()
            : healthy(true), healthScore(1.0) {}
    };
    
    HealthReport healthCheck() const;
    
    // Configuration template system
    bool addTemplate(const std::string& name, const EnhancedConfig& templateConfig);
    EnhancedConfig getTemplate(const std::string& name) const;
    bool removeTemplate(const std::string& name);
    std::vector<std::string> getTemplates() const;
    
    // Configuration diffing
    struct ConfigDiff {
        std::vector<std::string> addedKeys;
        std::vector<std::string> removedKeys;
        std::vector<std::string> modifiedKeys;
        std::map<std::string, std::pair<ConfigValue, ConfigValue>> changes;
        
        bool isEmpty() const {
            return addedKeys.empty() && removedKeys.empty() && modifiedKeys.empty() && changes.empty();
        }
    };
    
    ConfigDiff diff(const EnhancedConfig& other) const;
    
    // Configuration statistics and metrics
    struct Metrics {
        std::chrono::system_clock::time_point created;
        std::chrono::system_clock::time_point lastModified;
        size_t loadCount;
        size_t saveCount;
        size_t validationCount;
        double averageValidationTime;
        std::map<std::string, size_t> accessCount;
        
        Metrics()
            : loadCount(0), saveCount(0), validationCount(0), averageValidationTime(0.0) {
            created = std::chrono::system_clock::now();
            lastModified = created;
        }
    };
    
    Metrics getMetrics() const;
    
    // Configuration security
    enum class SecurityLevel {
        Public,
        Internal,
        Confidential,
        Secret
    };
    
    void setSecurityLevel(SecurityLevel level);
    SecurityLevel getSecurityLevel() const;
    
    // Configuration logging
    enum class LogLevel {
        Off,
        Error,
        Warning,
        Info,
        Debug
    };
    
    void setLogLevel(LogLevel level);
    LogLevel getLogLevel() const;
    
    // Get configuration logger
    class Logger {
    public:
        virtual ~Logger() = default;
        virtual void log(const std::string& message, LogLevel level = LogLevel::Info) = 0;
    };
    
    void setLogger(std::unique_ptr<Logger> logger);
    
    // Configuration thread safety
    using LockGuard = std::lock_guard<std::mutex>;
    mutable std::mutex configMutex;
    
    // Configuration observers pattern
    using Observer = std::function<void(const ConfigChange&)>;</n    void addObserver(const Observer& observer);
    void removeObserver(const Observer& observer);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static std::string convertValueToString(const ConfigValue& value);
    static bool stringToValue(const std::string& str, ConfigValue& value);
    static std::string sanitizeKey(const std::string& key);
    static bool isKeyReserved(const std::string& key);
    
    // Environment variable parsing
    static std::string parseEnvironmentVariable(const std::string& name);
    static bool parseEnvironmentBoolean(const std::string& value);
    static int parseEnvironmentInt(const std::string& value);
    static double parseEnvironmentDouble(const std::string& value);
    
    // JSON parsing (simplified implementation)
    static std::string valueToJSONString(const ConfigValue& value);
    static bool stringToValueJSON(const std::string& str, ConfigValue& value);
    
    // Validation helpers
    ValidationResult validateValue(const std::string& key, const ConfigValue& value, ConfigSource source) const;
    bool validateConstraints() const;
    
    // Change tracking
    void recordChange(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue, ConfigSource source, const std::string& description = "");
    
    // Observer notifications
    void notifyObservers(const ConfigChange& change);
    
    // Logger
    std::unique_ptr<Logger> logger;
    LogLevel logLevel;
    
    // Security
    SecurityLevel securityLevel;
    std::vector<std::string> encryptedKeys;
    
    // Templates
    std::map<std::string, EnhancedConfig> templates;
    
    // Metrics
    Metrics metrics;
    
    // Version
    std::string version;
};

// Explicit template instantiations
template std::optional<int> EnhancedConfig::get<int>(const std::string&) const;
template std::optional<int64_t> EnhancedConfig::get<int64_t>(const std::string&) const;
template std::optional<double> EnhancedConfig::get<double>(const std::string&) const;
template std::optional<bool> EnhancedConfig::get<bool>(const std::string&) const;
template std::optional<std::string> EnhancedConfig::get<std::string>(const std::string&) const;

template int EnhancedConfig::getOr<int>(const std::string&, const int&) const;
template int64_t EnhancedConfig::getOr<int64_t>(const std::string&, const int64_t&) const;
template double EnhancedConfig::getOr<double>(const std::string&, const double&) const;
template bool EnhancedConfig::getOr<bool>(const std::string&, const bool&) const;
template std::string EnhancedConfig::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
