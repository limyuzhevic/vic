#pragma once

#include "../src/core/Config/Config.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <variant>

namespace nlm {

// Schema validation
struct SchemaValidationError {
    std::string key;
    std::string errorType;
    std::string message;
    std::string expectedType;
    std::string actualType;
    
    SchemaValidationError() : errorType("invalid_type"), expectedType(""), actualType("") {}
    SchemaValidationError(const std::string& k, const std::string& e, const std::string& m,
                         const std::string& et = "", const std::string& at = "")
        : key(k), errorType(e), message(m), expectedType(et), actualType(at) {}
};

// Configuration validation rule
struct ValidationRule {
    std::string key;
    std::string type;  // "range", "choice", "regex", "required", "custom"
    std::vector<std::string> allowedValues;
    std::string regexPattern;
    std::function<bool(const ConfigValue&)> customValidator;
    std::string errorMessage;
    
    ValidationRule() : type("custom") {}
    ValidationRule(const std::string& k, const std::string& t) : key(k), type(t) {}
};

// Configuration inheritance hierarchy
struct InheritanceConfig {
    std::string baseConfig;
    std::map<std::string, ConfigValue> overrides;
    std::map<std::string, ConfigValue> additions;
    std::vector<std::string> inheritFrom;
    std::string inheritanceStrategy;  // "override", "merge", "extend"
    
    InheritanceConfig() : inheritanceStrategy("merge") {}
};

// Configuration template
struct ConfigurationTemplate {
    std::string name;
    std::string description;
    std::map<std::string, ConfigValue> defaultValues;
    std::vector<ValidationRule> validationRules;
    std::map<std::string, InheritanceConfig> inheritanceHierarchy;
    std::string templateType;  // "brain", "simulation", "experiment", "custom"
    std::map<std::string, std::string> metadata;
    
    ConfigurationTemplate() : templateType("custom") {}
};

// Parameter optimization configuration
struct OptimizationConfig {
    enum class OptimizationMethod {
        GridSearch,
        RandomSearch,
        BayesianOptimization,
        GeneticAlgorithm,
        GradientDescent,
        SimulatedAnnealing
    };
    
    enum class ObjectiveFunction {
        MaximizeAccuracy,
        MinimizeError,
        MaximizeStability,
        MinimizeResourceUsage,
        BalanceAll
    };
    
    struct ParameterConstraint {
        std::string parameterName;
        float minValue;
        float maxValue;
        std::string constraintType;  // "hard", "soft"
    };
    
    OptimizationMethod method;
    ObjectiveFunction objective;
    std::vector<ParameterConstraint> constraints;
    std::map<std::string, float> hyperparameters;
    size_t populationSize;
    size_t generations;
    float convergenceThreshold;
    bool parallelOptimization;
    
    OptimizationConfig() : method(OptimizationMethod::RandomSearch),
                           objective(ObjectiveFunction::BalanceAll),
                           populationSize(50), generations(100),
                           convergenceThreshold(0.001f),
                           parallelOptimization(true) {}
};

// Configuration versioning
struct VersionInfo {
    std::string version;
    std::string description;
    std::string createdBy;
    std::chrono::system_clock::time_point creationTime;
    std::vector<std::string> changes;
    std::string previousVersion;
    
    VersionInfo() : version("1.0.0") {}
    VersionInfo(const std::string& v, const std::string& d) : version(v), description(d) {}
};

// Advanced Configuration Engine
class AdvancedConfig {
public:
    AdvancedConfig();
    ~AdvancedConfig();
    
    // Disable copying, enable moving
    AdvancedConfig(const AdvancedConfig&) = delete;
    AdvancedConfig& operator=(const AdvancedConfig&) = delete;
    AdvancedConfig(AdvancedConfig&&) noexcept = default;
    AdvancedConfig& operator=(AdvancedConfig&&) noexcept = default;
    
    // Configuration inheritance
    bool loadBaseConfiguration(const std::string& configPath);
    bool inheritConfiguration(const InheritanceConfig& inheritance);
    std::shared_ptr<Config> resolveInheritance(const std::string& baseName) const;
    
    // Schema validation
    bool validateConfiguration(const Config& config) const;
    bool validateConfiguration(const Config& config, const std::vector<ValidationRule>& rules) const;
    std::vector<SchemaValidationError> validateAgainstSchema(const Config& config) const;
    bool validateSchema(const std::string& schemaName, const Config& config) const;
    
    // Configuration templates
    bool createTemplate(const ConfigurationTemplate& templateConfig);
    bool loadTemplate(const std::string& templateName);
    bool applyTemplate(const std::string& templateName, Config& config) const;
    ConfigurationTemplate getTemplate(const std::string& templateName) const;
    std::vector<std::string> getAvailableTemplates() const;
    
    // Parameter optimization
    std::map<std::string, ConfigValue> optimizeParameters(const Config& config,
                                                          const OptimizationConfig& optConfig,
                                                          const std::function<float(const Config&)>& objectiveFunction);
    std::map<std::string, ConfigValue> gridSearch(const Config& baseConfig,
                                                 const std::map<std::string, std::vector<ConfigValue>>& parameterGrid,
                                                 const std::function<float(const Config&)>& objectiveFunction);
    std::map<std::string, ConfigValue> bayesianOptimization(const Config& baseConfig,
                                                           const OptimizationConfig& optConfig,
                                                           const std::function<float(const Config&)>& objectiveFunction);
    
    // Configuration versioning
    bool createVersion(const std::string& configPath, const std::string& description);
    bool restoreVersion(const std::string& configPath, const std::string& version);
    std::vector<VersionInfo> getVersionHistory(const std::string& configPath) const;
    bool deleteVersion(const std::string& configPath, const std::string& version);
    VersionInfo getVersionInfo(const std::string& configPath, const std::string& version) const;
    
    // Schema management
    bool createSchema(const std::string& schemaName, const std::vector<ValidationRule>& rules);
    bool updateSchema(const std::string& schemaName, const std::vector<ValidationRule>& rules);
    bool deleteSchema(const std::string& schemaName);
    std::vector<ValidationRule> getSchema(const std::string& schemaName) const;
    std::vector<std::string> getAvailableSchemas() const;
    
    // Configuration comparison
    bool compareConfigurations(const Config& config1, const Config& config2) const;
    std::map<std::string, std::pair<ConfigValue, ConfigValue>> diffConfigurations(const Config& config1,
                                                                                 const Config& config2) const;
    float calculateConfigurationDistance(const Config& config1, const Config& config2) const;
    
    // Advanced configuration features
    bool enableAdvancedFeatures(const std::map<std::string, bool>& features);
    bool isAdvancedFeatureEnabled(const std::string& featureName) const;
    std::map<std::string, bool> getEnabledFeatures() const;
    
    // Validation utilities
    static bool validateValueType(const ConfigValue& value, const std::string& expectedType) const;
    static std::string getValueTypeName(const ConfigValue& value) const;
    static bool validateValueRange(const ConfigValue& value, float min, float max) const;
    static bool validateValueChoices(const ConfigValue& value, const std::vector<std::string>& choices) const;
    static bool validateValuePattern(const ConfigValue& value, const std::string& pattern) const;
    static bool validateCustom(const ConfigValue& value, const std::function<bool(const ConfigValue&)>& validator) const;
    
    // Configuration export/import
    bool exportConfiguration(const Config& config, const std::string& filepath) const;
    bool importConfiguration(Config& config, const std::string& filepath);
    bool exportConfigurationWithSchema(const Config& config, const std::string& filepath, const std::string& schemaName) const;
    bool importConfigurationWithSchema(Config& config, const std::string& filepath, const std::string& schemaName);
    
    // Get current configuration
    const Config& getConfiguration() const;
    void updateConfiguration(const Config& config);
    
    // Schema validation statistics
    struct ValidationStatistics {
        size_t totalValidations;
        size_t successfulValidations;
        size_t failedValidations;
        std::map<std::string, size_t> errorCounts;
        float successRate;
    };
    
    ValidationStatistics getValidationStatistics() const;
    
    // Optimization statistics
    struct OptimizationStatistics {
        size_t evaluations;
        size_t iterations;
        float bestScore;
        float convergenceRate;
        std::chrono::duration<double> totalTime;
    };
    
    OptimizationStatistics getOptimizationStatistics() const;
    
    // Logging and monitoring
    void enableLogging(bool enable);
    bool isLoggingEnabled() const;
    void logConfiguration(const std::string& message) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
