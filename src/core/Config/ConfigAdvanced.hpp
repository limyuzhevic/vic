#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <variant>
#include <optional>
#include <type_traits>
#include <chrono>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <toml.hpp>
#include "Config.hpp"

namespace nlm {

// Configuration schema validation errors
class ConfigValidationError {
public:
    enum class ErrorType {
        SchemaViolation,
        TypeMismatch,
        ConstraintViolation,
        MissingRequired,
       InvalidFormat,
        DependencyError,
       CircularDependency,
        UnknownError
    };
    
    ConfigValidationError(
        ErrorType type,
        const std::string& message,
        const std::string& key = "",
        const std::string& path = ""
    ) : errorType(type), message(message), key(key), path(path), timestamp(std::chrono::system_clock::now()) {}
    
    ErrorType getType() const { return errorType; }
    const std::string& getMessage() const { return message; }
    const std::string& getKey() const { return key; }
    const std::string& getPath() const { return path; }
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp; }
    std::string getFormattedError() const {
        std::ostringstream oss;
        oss << "[ConfigValidationError] " << path << "." << key << ": " << message;
        return oss.str();
    }
    
private:
    ErrorType errorType;
    std::string message;
    std::string key;
    std::string path;
    std::chrono::system_clock::time_point timestamp;
};

// Configuration schema definition
struct ConfigSchema {
    struct Field {
        std::string name;
        std::string type;
        std::string description;
        bool required = false;
        std::variant<int, double, std::string> defaultValue;
        std::vector<std::string> dependencies;
        std::function<bool(const ConfigValue&)> validator;
        
        Field() = default;
        Field(const std::string& name, const std::string& type,
              const std::string& description = "", bool required = false)
            : name(name), type(type), description(description), required(required) {}
    };
    
    std::string name;
    std::string version;
    std::string description;
    std::vector<Field> fields;
    std::vector<std::string> extends;
    std::string platform;
    std::string environment;
    
    ConfigSchema() = default;
    ConfigSchema(const std::string& name, const std::string& version = "1.0.0")
        : name(name), version(version) {}
    
    bool validate(const Config& config, std::vector<ConfigValidationError>& errors) const;
    bool validateField(const Field& field, const ConfigValue& value,
                     std::vector<ConfigValidationError>& errors, const std::string& path) const;
    const Field* getField(const std::string& fieldName) const;
    std::string getFieldType(const std::string& fieldName) const;
    std::string getFieldDescription(const std::string& fieldName) const;
    bool isFieldRequired(const std::string& fieldName) const;
    std::optional<ConfigValue> getDefaultValue(const std::string& fieldName) const;
};

// JSON/YAML/TOML parser and serializer
class ConfigFormatParser {
public:
    static std::unordered_map<std::string, ConfigValue> parseJSON(const std::string& content);
    static std::string serializeJSON(const std::unordered_map<std::string, ConfigValue>& config);
    
    static std::unordered_map<std::string, ConfigValue> parseYAML(const std::string& content);
    static std::string serializeYAML(const std::unordered_map<std::string, ConfigValue>& config);
    
    static std::unordered_map<std::string, ConfigValue> parseTOML(const std::string& content);
    static std::string serializeTOML(const std::unordered_map<std::string, ConfigValue>& config);
    
    static bool detectFormat(const std::string& content, std::string& detectedFormat);
    static std::unordered_map<std::string, ConfigValue> parse(const std::string& content, const std::string& format);
    static std::string serialize(const std::unordered_map<std::string, ConfigValue>& config, const std::string& format);
};

// Advanced configuration with schema support
class ConfigAdvanced : public Config {
public:
    ConfigAdvanced();
    ~ConfigAdvanced() override;
    
    // Disable copying, enable moving
    ConfigAdvanced(const ConfigAdvanced&) = delete;
    ConfigAdvanced& operator=(const ConfigAdvanced&) = delete;
    ConfigAdvanced(ConfigAdvanced&&) noexcept;
    ConfigAdvanced& operator=(ConfigAdvanced&&) noexcept;
    
    // Schema management
    bool setSchema(const ConfigSchema& schema);
    const ConfigSchema* getSchema() const { return currentSchema.get(); }
    bool validate() const;
    std::vector<ConfigValidationError> getValidationErrors() const { return validationErrors; }
    
    // Configuration inheritance and merging
    bool inheritFrom(const ConfigAdvanced& parent, bool overrideDefaults = true);
    bool merge(const ConfigAdvanced& other, bool overrideExisting = true);
    
    // Template system
    bool setTemplate(const std::string& name, const ConfigAdvanced& templateConfig);
    bool applyTemplate(const std::string& templateName);
    bool removeTemplate(const std::string& templateName);
    std::vector<std::string> getTemplateNames() const;
    
    // Environment and platform support
    void setEnvironment(const std::string& env) { environment = env; }
    const std::string& getEnvironment() const { return environment; }
    void setPlatform(const std::string& platform) { currentPlatform = platform; }
    const std::string& getPlatform() const { return currentPlatform; }
    
    // Configuration profiles
    bool createProfile(const std::string& profileName, const std::string& description = "");
    bool applyProfile(const std::string& profileName);
    bool removeProfile(const std::string& profileName);
    bool renameProfile(const std::string& oldName, const std::string& newName);
    std::vector<std::string> getProfileNames() const;
    
    // Runtime modification tracking
    void enableModificationTracking(bool enable) { trackModifications = enable; }
    bool isModificationTrackingEnabled() const { return trackModifications; }
    std::vector<ConfigEntry> getModificationHistory(size_t maxEntries = 100) const;
    
    // Performance optimization
    void enableCache(bool enable) { cacheEnabled = enable; }
    bool isCacheEnabled() const { return cacheEnabled; }
    void clearCache();
    
    // Configuration comparison and diffing
    std::string diff(const ConfigAdvanced& other) const;
    bool equals(const ConfigAdvanced& other) const;
    
    // Configuration history and versioning
    std::string getCurrentVersion() const { return currentVersion; }
    bool saveVersion(const std::string& versionName, const std::string& comment = "");
    bool restoreVersion(const std::string& versionName);
    std::vector<std::string> getVersionNames() const;
    
    // Configuration rollback
    bool enableRollback(bool enable) { canRollback = enable; return canRollback; }
    bool isRollbackEnabled() const { return canRollback; }
    bool rollback(size_t steps);
    
    // Backward compatibility
    bool enableBackwardCompatibility(bool enable) { backwardCompatEnabled = enable; return backwardCompatEnabled; }
    bool isBackwardCompatibilityEnabled() const { return backwardCompatEnabled; }
    std::string getCompatibilityVersion() const { return compatibilityVersion; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    mutable std::vector<ConfigValidationError> validationErrors;
    std::unique_ptr<ConfigSchema> currentSchema;
    std::string environment;
    std::string currentPlatform;
    std::string currentVersion;
    bool trackModifications = false;
    bool cacheEnabled = true;
    bool canRollback = true;
    bool backwardCompatEnabled = true;
    std::string compatibilityVersion = "1.0.0";
    
    // Modification tracking
    struct ModificationRecord {
        std::chrono::system_clock::time_point timestamp;
        std::string key;
        ConfigValue oldValue;
        ConfigValue newValue;
        std::string modifiedBy;
    };
    std::vector<ModificationRecord> modificationHistory;
    
    // Version control
    struct VersionRecord {
        std::string name;
        std::string comment;
        std::chrono::system_clock::time_point timestamp;
        std::unordered_map<std::string, ConfigValue> configSnapshot;
    };
    std::vector<VersionRecord> versionHistory;
    
    // Template storage
    std::unordered_map<std::string, ConfigAdvanced> templates;
    
    // Profile storage
    std::unordered_map<std::string, std::pair<std::string, std::string>> profiles;
    
    // Internal helpers
    void trackModification(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue, const std::string& modifiedBy = "");
    void updateCache() const;
    const ConfigValue* getCachedValue(const std::string& key) const;
    void addVersionRecord(const std::string& versionName, const std::string& comment = "");
    bool validateDependencies() const;
    bool checkCircularDependencies(const std::string& key,
                                 const std::unordered_map<std::string, std::vector<std::string>>& graph,
                                 std::unordered_set<std::string>& visiting,
                                 std::unordered_set<std::string>& visited) const;
};

// Configuration validation utilities
class ConfigValidation {
public:
    // Type validation
    static bool validateType(const std::string& typeName, const ConfigValue& value);
    static bool validateInt(const ConfigValue& value, int min = INT_MIN, int max = INT_MAX);
    static bool validateDouble(const ConfigValue& value, double min = -std::numeric_limits<double>::max(),
                              double max = std::numeric_limits<double>::max());
    static bool validateString(const ConfigValue& value, size_t minLength = 0, size_t maxLength = SIZE_MAX);
    static bool validateBool(const ConfigValue& value);
    static bool validateVector(const ConfigValue& value, const std::string& elementType = "any");
    
    // Constraint validation
    static bool validateRange(const std::string& fieldName, const ConfigValue& value,
                             const std::string& constraint);
    static bool validateFormat(const std::string& fieldName, const ConfigValue& value,
                              const std::string& format);
    static bool validateRegex(const std::string& fieldName, const ConfigValue& value,
                             const std::string& pattern);
    
    // Performance validation
    static bool validatePerformance(const std::string& fieldName, const ConfigValue& value);
    
    // Security validation
    static bool validateSecurity(const std::string& fieldName, const ConfigValue& value);
    
    // Conditional validation
    static bool validateConditional(const std::string& fieldName, const ConfigValue& value,
                                   const std::string& condition, const Config& config);
    
    // Cross-field validation
    static bool validateDependencies(const std::string& fieldName, const ConfigValue& value,
                                    const std::vector<std::string>& dependencies, const Config& config);
    
    // Configuration comparison
    static bool compareValues(const ConfigValue& value1, const ConfigValue& value2);
    static std::string formatDiff(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue);
};

// Configuration management tools
class ConfigTools {
public:
    // Configuration validation tools
    static bool validateConfigFile(const std::string& filepath, const std::string& format = "auto");
    static bool validateConfigJSON(const std::string& content);
    static bool validateConfigYAML(const std::string& content);
    static bool validateConfigTOML(const std::string& content);
    
    // Configuration diffing tools
    static std::string diffConfigFiles(const std::string& filepath1, const std::string& filepath2);
    static std::string diffConfig(const ConfigAdvanced& config1, const ConfigAdvanced& config2);
    
    // Configuration migration tools
    static bool migrateConfig(const ConfigAdvanced& source, ConfigAdvanced& target,
                            const std::string& targetVersion);
    static bool migrateConfigFile(const std::string& sourceFile, const std::string& targetFile,
                                const std::string& targetVersion);
    
    // Configuration optimization tools
    static bool optimizeConfig(ConfigAdvanced& config);
    static bool removeUnusedEntries(ConfigAdvanced& config);
    static bool compressConfig(ConfigAdvanced& config);
    
    // Configuration backup and recovery
    static bool backupConfig(const ConfigAdvanced& config, const std::string& backupFile);
    static bool restoreConfig(ConfigAdvanced& config, const std::string& backupFile);
};

} // namespace nlm