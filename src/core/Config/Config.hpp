#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <regex>
#include <any>
#include <set>
#include <unordered_map>
#include <type_traits>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <rapidjson/document.h>
#include <yaml-cpp/yaml.h>
#include <ini.h>
#include <toml.hpp>

namespace nlm {

// Forward declarations
class Config;
class ConfigSchema;

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

// Validation result
struct ValidationResult {
    bool valid;
    std::string message;
    std::vector<std::string> errors;
    
    ValidationResult() : valid(true), message("Valid") {}
    ValidationResult(bool v, const std::string& msg) : valid(v), message(msg) {}
    ValidationResult(bool v, const std::string& msg, const std::vector<std::string>& errs) 
        : valid(v), message(msg), errors(errs) {}
    
    static ValidationResult Valid() { return ValidationResult(true, "Valid"); }
    static ValidationResult Invalid(const std::string& msg) { return ValidationResult(false, msg); }
    static ValidationResult Invalid(const std::string& msg, const std::vector<std::string>& errs) {
        return ValidationResult(false, msg, errs);
    }
};

// Parameter metadata for schema definition
struct ParameterMetadata {
    std::string name;
    std::string type;
    std::string description;
    bool required = false;
    std::any defaultValue;
    
    // Validation rules
    bool hasMin = false;
    double minValue;
    bool hasMax = false;
    double maxValue;
    
    // Format validation
    std::string pattern; // Regex pattern
    std::string format;  // Format type (email, url, etc.)
    
    // Conditional validation
    std::vector<std::string> dependsOn;
    std::function<ValidationResult(const std::any&, const std::map<std::string, std::any>&)> customValidator;
    
    // Parameter relationships
    std::function<ValidationResult(const std::map<std::string, std::any>&)> crossValidator;
    
    // Help and documentation
    std::string helpText;
    std::string category;
    int order = 0;
    
    ParameterMetadata() = default;
    
    ParameterMetadata(const std::string& name, const std::string& type, const std::string& description = "",
                      bool required = false, const std::any& defaultValue = std::any(),
                      const std::string& helpText = "")
        : name(name), type(type), description(description), required(required), 
          defaultValue(defaultValue), helpText(helpText) {}
};

// Schema validation result with detailed error information
struct SchemaValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::map<std::string, ValidationResult> parameterResults;
    
    SchemaValidationResult() : valid(true) {}
    SchemaValidationResult(bool v, const std::vector<std::string>& errs) 
        : valid(v), errors(errs) {}
    
    static SchemaValidationResult Valid() { return SchemaValidationResult(true, {}); }
    static SchemaValidationResult Invalid(const std::vector<std::string>& errs) {
        return SchemaValidationResult(false, errs);
    }
    
    void addError(const std::string& error) {
        errors.push_back(error);
        valid = false;
    }
    
    void addParameterError(const std::string& param, const std::string& error) {
        parameterResults[param].addError(error);
    }
};

// Configuration schema for validation and documentation
class ConfigSchema {
public:
    ConfigSchema(const std::string& name, const std::string& description = "")
        : name(name), description(description) {}
    
    // Parameter management
    void addParameter(const ParameterMetadata& param);
    bool hasParameter(const std::string& name) const;
    const ParameterMetadata* getParameter(const std::string& name) const;
    
    // Schema hierarchy
    void setParent(const std::shared_ptr<ConfigSchema>& parent);
    const std::shared_ptr<ConfigSchema>& getParent() const { return parent; }
    void addChild(const std::shared_ptr<ConfigSchema>& child);
    const std::vector<std::shared_ptr<ConfigSchema>>& getChildren() const { return children; }
    
    // Validation
    SchemaValidationResult validate(const std::map<std::string, std::any>& values) const;
    ValidationResult validateParameter(const std::string& name, const std::any& value) const;
    
    // Template support
    void addTemplate(const std::string& name, const std::map<std::string, std::any>& values);
    const std::map<std::string, std::map<std::string, std::any>>& getTemplates() const { return templates; }
    
    // Schema merging
    bool merge(const ConfigSchema& other);
    
    // Get all parameters recursively from parent schemas
    std::map<std::string, ParameterMetadata> getAllParameters() const;
    
    // Schema information
    const std::string& getName() const { return name; }
    const std::string& getDescription() const { return description; }
    
private:
    std::string name;
    std::string description;
    std::map<std::string, ParameterMetadata> parameters;
    std::shared_ptr<ConfigSchema> parent;
    std::vector<std::shared_ptr<ConfigSchema>> children;
    std::map<std::string, std::map<std::string, std::any>> templates;
    
    friend class Config;
};

// Configuration template system
struct TemplateMetadata {
    std::string name;
    std::string description;
    std::map<std::string, std::any> parameters;
    std::map<std::string, std::string> dependencies;
    
    TemplateMetadata() = default;
    TemplateMetadata(const std::string& name, const std::string& description = "")
        : name(name), description(description) {}
};

// Configuration change tracking
struct ConfigChange {
    std::string timestamp;
    std::string key;
    std::any oldValue;
    std::any newValue;
    ConfigSource source;
    std::string reason;
    
    ConfigChange(const std::string& k, const std::any& oldVal, const std::any& newVal, 
                 ConfigSource s, const std::string& reason = "")
        : key(k), oldValue(oldVal), newValue(newVal), source(s), reason(reason) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        timestamp = std::string(std::ctime(&time_t));
        // Remove trailing newline from ctime
        if (!timestamp.empty() && timestamp.back() == '\n') {
            timestamp.pop_back();
        }
    }
};

// Main enhanced configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Schema management
    void registerSchema(const std::string& category, const std::shared_ptr<ConfigSchema>& schema);
    const std::shared_ptr<ConfigSchema>& getSchema(const std::string& category) const;
    bool hasSchema(const std::string& category) const;
    std::vector<std::string> getSchemaCategories() const;
    
    // Schema-based loading/saving
    bool loadFromFile(const std::string& filepath, const std::string& category = "");
    bool saveToFile(const std::string& filepath, const std::string& category = "") const;
    bool loadFromSchemaFile(const std::string& filepath, const std::string& category = "");
    bool saveToSchemaFile(const std::string& filepath, const std::string& category = "") const;
    
    // Schema validation
    SchemaValidationResult validateSchema(const std::string& category = "") const;
    
    // Advanced loading/saving with format support
    enum class FileFormat { JSON, YAML, INI, TOML, Auto };
    
    // Get values with validation
    template<typename T>
    std::optional<T> get(const std::string& key, const std::string& category = "") const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue, const std::string& category = "") const;
    
    // Set values with validation and history
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& reason = "", const std::string& category = "");
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& reason = "", const std::string& category = "");
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime,
             const std::string& reason = "", const std::string& category = "");
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime,
             const std::string& reason = "", const std::string& category = "");
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime,
             const std::string& reason = "", const std::string& category = "");
    
    // Configuration history and change tracking
    void enableHistory(bool enabled = true);
    bool isHistoryEnabled() const;
    std::vector<ConfigChange> getHistory() const;
    void clearHistory();
    std::vector<ConfigChange> getHistory(const std::string& key) const;
    
    // Configuration comparison and diffing
    std::map<std::string, std::pair<std::any, std::any>> diff(const Config& other) const;
    std::string diffToString(const Config& other) const;
    
    // Configuration snapshots and rollback
    std::string createSnapshot(const std::string& name = "");
    bool rollbackToSnapshot(const std::string& name);
    std::vector<std::string> getSnapshotNames() const;
    
    // Configuration migration
    void addMigration(const std::string& fromVersion, const std::string& toVersion,
                      const std::function<void(Config&)>& migrationFunc);
    bool migrate(const std::string& fromVersion, const std::string& toVersion);
    
    // Configuration templates
    void applyTemplate(const std::string& templateName, const std::string& category = "");
    std::vector<std::string> getAvailableTemplates(const std::string& category = "") const;
    
    // Template management
    void addTemplate(const std::string& name, const std::map<std::string, std::any>& parameters,
                     const std::string& description = "", const std::string& category = "");
    
    // Advanced configuration operations
    void merge(const Config& other, const std::string& category = "");
    void mergeFromFile(const std::string& filepath, const std::string& category = "");
    
    // File format detection and auto-selection
    static FileFormat detectFileFormat(const std::string& filepath);
    
    // File format parsing methods
    static std::map<std::string, std::any> parseJSON(const std::string& content);
    static std::map<std::string, std::any> parseYAML(const std::string& content);
    static std::map<std::string, std::any> parseINI(const std::string& content);
    static std::map<std::string, std::any> parseTOML(const std::string& content);
    static std::map<std::string, std::any> parseFile(const std::string& filepath);
    
    // File format conversion
    static std::string toJSON(const std::map<std::string, std::any>& data);
    static std::string toYAML(const std::map<std::string, std::any>& data);
    static std::string toINI(const std::map<std::string, std::any>& data);
    static std::string toTOML(const std::map<std::string, std::any>& data);
    
    // Configuration information
    std::string getCategory() const;
    
    // Legacy API (backward compatibility)
    bool loadFromFile(const std::string& filepath); // Uses default schema
    bool saveToFile(const std::string& filepath) const; // Uses default schema
    template<typename T>
    std::optional<T> get(const std::string& key) const { return get<T>(key, ""); } // Default category
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const { return getOr<T>(key, defaultValue, ""); }
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime) {
        set(key, value, source, ""); }
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) {
        set(key, value, source, ""); }
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) {
        set(key, value, source, ""); }
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) {
        set(key, value, source, ""); }
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) {
        set(key, value, source, ""); }
    
    // Check existence
    bool has(const std::string& key) const;
    bool has(const std::string& key, const std::string& category) const;
    
    // Remove key
    void remove(const std::string& key);
    void remove(const std::string& key, const std::string& category);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeys(const std::string& category) const;
    
    // Get all keys with metadata
    std::vector<std::pair<std::string, ConfigEntry>> getAllEntries() const;
    std::vector<std::pair<std::string, ConfigEntry>> getAllEntries(const std::string& category) const;
    
    // Clear all
    void clear();
    void clear(const std::string& category);
    
    // Get configuration summary
    std::string summary() const;
    std::string summary(const std::string& category) const;
    
    // Legacy API accessors
    const std::vector<ConfigEntry>& getLegacyEntries() const { return legacyEntries; }
    std::string getLegacyCategory() const { return "legacy"; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Legacy support
    std::vector<ConfigEntry> legacyEntries;
    
    // Legacy helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    friend class ConfigSchema;
};

} // namespace nlm