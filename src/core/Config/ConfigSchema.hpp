#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <optional>
#include <variant>
#include <any>
#include <memory>
#include <regex>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <iomanip>

namespace nlm {

// Forward declarations
class Config;
class ConfigValidator;
class ConfigSchema;

// Configuration error types
enum class ConfigErrorType {
    TypeMismatch,
    RangeViolation,
    MissingDependency,
    DependencyCycle,
    SchemaValidation,
    MigrationError,
    ImportExportError
};

// Configuration error details
struct ConfigError {
    ConfigErrorType type;
    std::string key;
    std::string expected;
    std::string actual;
    std::string message;
    
    ConfigError(ConfigErrorType t, const std::string& k, const std::string& exp, const std::string& act, const std::string& msg)
        : type(t), key(k), expected(exp), actual(act), message(msg) {}
};

// Parameter types for schema definition
enum class ParameterType {
    Int,
    Int64,
    Float,
    Double,
    Bool,
    String,
    StringList,
    IntList,
    FloatList,
    Choice,
    Range,
    Vector2D,
    Vector3D,
    Matrix,
    Custom
};

// Range specification
struct RangeSpec {
    std::string min;
    std::string max;
    bool inclusive = true;
    
    RangeSpec(const std::string& min_val, const std::string& max_val, bool inc = true)
        : min(min_val), max(max_val), inclusive(inc) {}
};

// Choice specification
struct ChoiceSpec {
    std::vector<std::string> choices;
    
    ChoiceSpec(const std::vector<std::string>& c) : choices(c) {}
};

// Vector dimensions
struct VectorSpec {
    int dimensions;
    std::vector<RangeSpec> componentRanges;
    
    VectorSpec(int dim, const std::vector<RangeSpec>& ranges) : dimensions(dim), componentRanges(ranges) {}
};

// Matrix specification
struct MatrixSpec {
    int rows;
    int cols;
    std::vector<RangeSpec> valueRanges;
    
    MatrixSpec(int r, int c, const std::vector<RangeSpec>& ranges) : rows(r), cols(c), valueRanges(ranges) {}
};

// Parameter metadata for documentation
struct ParameterInfo {
    std::string name;
    std::string description;
    ParameterType type;
    std::optional<RangeSpec> range;
    std::optional<ChoiceSpec> choice;
    std::optional<VectorSpec> vector;
    std::optional<MatrixSpec> matrix;
    std::optional<ParameterInfo> dependency;
    std::vector<std::string> tags;
    bool required = false;
    bool readOnly = false;
    std::string defaultValue;
    std::string category;
    int order = 0;
    std::string unit;
    
    ParameterInfo(const std::string& n, const std::string& desc = "", ParameterType t = ParameterType::String)
        : name(n), description(desc), type(t) {}
};

// Configuration schema
class ConfigSchema {
public:
    ConfigSchema() = default;
    
    // Add parameter definition
    void addParameter(const ParameterInfo& param);
    
    // Get parameter by name
    const ParameterInfo* getParameter(const std::string& name) const;
    
    // Get all parameters
    const std::vector<ParameterInfo>& getParameters() const { return parameters; }
    
    // Get required parameters
    std::vector<std::string> getRequiredParameters() const;
    
    // Get parameters by category
    std::vector<ParameterInfo> getParametersByCategory(const std::string& category) const;
    
    // Get ordered parameters (by order field)
    std::vector<ParameterInfo> getOrderedParameters() const;
    
    // Validate against schema
    std::vector<ConfigError> validate(const std::map<std::string, std::any>& config) const;
    
    // Get documentation string
    std::string getDocumentation() const;
    
    // Add built-in schemas
    static ConfigSchema createQuickSchema();
    static ConfigSchema createBalancedSchema();
    static ConfigSchema createResearchSchema();
    
private:
    std::vector<ParameterInfo> parameters;
    std::unordered_map<std::string, size_t> paramMap;
};

// Configuration change notification
class ConfigChangeListener {
public:
    virtual ~ConfigChangeListener() = default;
    virtual void onConfigChanged(const std::string& key, const std::any& oldValue, const std::any& newValue) = 0;
    virtual void onConfigAdded(const std::string& key, const std::any& value) = 0;
    virtual void onConfigRemoved(const std::string& key) = 0;
    virtual void onValidationFailed(const ConfigError& error) = 0;
};

// Built-in presets for different use cases
class ConfigPresets {
public:
    struct Preset {
        std::string name;
        std::string description;
        std::map<std::string, std::any> values;
        std::string schemaName = "default";
        
        Preset(const std::string& n, const std::string& desc) : name(n), description(desc) {}
    };
    
    static std::vector<Preset> getQuickPreset();
    static std::vector<Preset> getBalancedPreset();
    static std::vector<Preset> getResearchPreset();
    static std::vector<Preset> getMinimalPreset();
    static std::vector<Preset> getMaximumPreset();
};

// Configuration migration support
struct MigrationStep {
    int version;
    std::string description;
    std::function<void(std::map<std::string, std::any>&)> migrationLogic;
    
    MigrationStep(int ver, const std::string& desc, const std::function<void(std::map<std::string, std::any>&)>& logic)
        : version(ver), description(desc), migrationLogic(logic) {}
};

class ConfigMigration {
public:
    ConfigMigration() = default;
    
    void addMigrationStep(int fromVersion, int toVersion, const std::string& description,
                          const std::function<void(std::map<std::string, std::any>&)>& migrationLogic);
    
    std::vector<ConfigError> applyMigrations(std::map<std::string, std::any>& config) const;
    
    void setCurrentVersion(int version) { currentVersion = version; }
    int getCurrentVersion() const { return currentVersion; }
    
private:
    std::vector<MigrationStep> migrationSteps;
    int currentVersion = 1;
};

// Configuration watcher
class ConfigWatcher {
public:
    ConfigWatcher() = default;
    
    void addListener(std::shared_ptr<ConfigChangeListener> listener);
    void removeListener(std::shared_ptr<ConfigChangeListener> listener);
    void notifyChange(const std::string& key, const std::any& oldValue, const std::any& newValue);
    void notifyAdd(const std::string& key, const std::any& value);
    void notifyRemove(const std::string& key);
    
    void setWatchInterval(std::chrono::milliseconds interval) { watchInterval = interval; }
    std::chrono::milliseconds getWatchInterval() const { return watchInterval; }
    
private:
    std::vector<std::shared_ptr<ConfigChangeListener>> listeners;
    std::chrono::milliseconds watchInterval{100};
};