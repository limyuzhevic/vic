#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_set>
#include <functional>
#include <stdexcept>

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
    Runtime,
    SchemaDefault
};

// Configuration validation
enum class ValidationSeverity {
    Info,
    Warning,
    Error,
    Fatal
};

struct ValidationResult {
    bool isValid;
    std::vector<std::pair<ValidationSeverity, std::string>> messages;
    
    bool hasErrors() const {
        return std::any_of(messages.begin(), messages.end(),
                          [](const auto& msg) { return msg.first >= ValidationSeverity::Error; });
    }
    
    bool hasWarnings() const {
        return std::any_of(messages.begin(), messages.end(),
                          [](const auto& msg) { return msg.first == ValidationSeverity::Warning; });
    }
};

// Configuration schema entry
struct ConfigSchema {
    std::string key;
    std::string description;
    ConfigSource defaultSource = ConfigSource::Default;
    std::string defaultValue;
    bool required = false;
    std::string type = "auto";
    std::string validation; // Regex or validation expression
    std::vector<std::string> allowedValues;
    double minValue = std::numeric_limits<double>::lowest();
    double maxValue = std::numeric_limits<double>::max();
    
    ConfigSchema() = default;
    ConfigSchema(const std::string& k, const std::string& desc, bool req = false)
        : key(k), description(desc), required(req) {}
};

// Configuration statistics
struct ConfigStats {
    size_t totalEntries;
    size_t defaultEntries;
    size_t fileEntries;
    size_t commandLineEntries;
    size_t runtimeEntries;
    double memoryUsage;
    double validationTime;
    
    ConfigStats() : totalEntries(0), defaultEntries(0), fileEntries(0), 
                   commandLineEntries(0), runtimeEntries(0), 
                   memoryUsage(0.0), validationTime(0.0) {}
};

// Enhanced configuration class with schema validation and advanced features
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Enhanced loading with validation
    bool loadFromFile(const std::string& filepath, bool validate = true);
    bool loadFromArgs(int argc, char** argv, bool validate = true);
    
    // Save to file with formatting options
    bool saveToFile(const std::string& filepath, bool prettyPrint = false) const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Type-specific getters
    std::optional<int64_t> getInt(const std::string& key) const;
    std::optional<double> getDouble(const std::string& key) const;
    std::optional<bool> getBool(const std::string& key) const;
    std::optional<std::string> getString(const std::string& key) const;
    std::optional<std::vector<int>> getIntList(const std::string& key) const;
    std::optional<std::vector<double>> getDoubleList(const std::string& key) const;
    std::optional<std::vector<std::string>> getStringList(const std::string& key) const;
    
    // Enhanced setters with validation
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime, 
             bool validate = true);
    void set(const std::string& key, const std::string& value, 
             ConfigSource source = ConfigSource::Runtime, 
             bool validate = true);
    void set(const std::string& key, int value, 
             ConfigSource source = ConfigSource::Runtime, 
             bool validate = true);
    void set(const std::string& key, int64_t value, 
             ConfigSource source = ConfigSource::Runtime, 
             bool validate = true);
    void set(const std::string& key, double value, 
             ConfigSource source = ConfigSource::Runtime, 
             bool validate = true);
    void set(const std::string& key, bool value, 
             ConfigSource source = ConfigSource::Runtime, 
             bool validate = true);
    
    // Set with type specification
    template<typename T>
    void setTyped(const std::string& key, const T& value, 
                  ConfigSource source = ConfigSource::Runtime,
                  bool validate = true);
    
    // Configuration schema management
    void addSchema(const ConfigSchema& schema);
    bool validateSchema() const;
    bool validateConfiguration() const;
    ValidationResult validateKey(const std::string& key) const;
    
    // Schema introspection
    std::vector<std::string> getRequiredKeys() const;
    std::vector<std::string> getOptionalKeys() const;
    std::optional<ConfigSchema> getSchema(const std::string& key) const;
    std::map<std::string, ConfigSchema> getAllSchemas() const;
    
    // Configuration statistics and analysis
    ConfigStats getStats() const;
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    double getMemoryUsage() const;
    
    // Configuration operations
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    void remove(const std::vector<std::string>& keys);
    
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeys(ConfigSource source) const;
    
    // Clear all with options
    void clear(ConfigSource source = ConfigSource::Runtime);
    bool clearSection(const std::string& section);
    
    // Get configuration summary with validation info
    std::string summary() const;
    std::string summary(bool includeValidation) const;
    
    // JSON schema generation
    std::string generateJsonSchema() const;
    bool saveJsonSchema(const std::string& filepath) const;
    
    // Advanced configuration features
    bool merge(const Config& other, bool overwrite = true);
    bool patch(const std::map<std::string, ConfigValue>& updates);
    
    // Configuration export/import with compression
    bool exportCompressed(const std::string& filepath) const;
    bool importCompressed(const std::string& filepath);
    
    // Environment variable integration
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    void setEnvironment(const std::string& prefix = "NLM_") const;
    
    // Type conversion and validation helpers
    template<typename T>
    static bool validateValue(const ConfigValue& value, const T& expected);
    
    template<typename T>
    static std::optional<T> convertTo(const ConfigValue& value);
    
    // Callback system for configuration changes
    using ConfigChangeCallback = std::function<void(const std::string&, const ConfigValue&, ConfigSource)>
;    
    void addChangeCallback(const ConfigChangeCallback& callback);
    void removeChangeCallback(const ConfigChangeCallback& callback);
    void notifyChange(const std::string& key, const ConfigValue& value, ConfigSource source);
    
    // JSON schema validation
    static std::string generateJsonSchema(const std::string& title = "NLM Configuration");
    static ValidationResult validateJson(const std::string& json);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isWhitespace(char c);
    static std::string escapeJson(const std::string& str);
    static std::string unescapeJson(const std::string& str);
    
    // Schema validation helpers
    ValidationResult validateAgainstSchema(const std::string& key, const ConfigValue& value) const;
    bool validateType(const std::string& key, const ConfigValue& value, const std::string& type) const;
    bool validateRange(const std::string& key, const ConfigValue& value, double min, double max) const;
    bool validateAllowedValues(const std::string& key, const ConfigValue& value, 
                              const std::vector<std::string>& allowed) const;
    
    // Configuration change notification
    struct ChangeTracker {
        std::vector<ConfigChangeCallback> callbacks;
        std::vector<std::pair<std::string, ConfigValue>> recentChanges;
    };
};

} // namespace nlm
