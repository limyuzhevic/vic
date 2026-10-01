#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <functional>
#include <algorithm>
#include <regex>
#include <nlohmann/json.hpp>
#include "../core/Config/Config.hpp"

namespace nlm {

// ===============================================================
// ADVANCED CONFIGURATION VALIDATION TYPES
// ===============================================================

enum class ValidationLevel {
    Strict,      // All validation errors must be fixed
    Warning,     // Validation warnings are issued but not errors
    Lenient,     // Only critical errors are caught
    Custom      // User-defined validation rules
};

enum class ValidationRuleType {
    Required,                // Key must be present
    Forbidden,               // Key must not be present
    Type,                    // Value must be of specific type
    Range,                   // Value must be within range
    Pattern,                 // Value must match pattern
    Length,                  // String length constraints
    Unique,                  // All values must be unique
    Dependency,              // One key's presence depends on another
    Format,                  // Value must match specific format
    List,                    // Value must be a valid list
    Schema,                  // Value must conform to schema
    Custom                   // User-defined validation
};

enum class ConfigurationScope {
    Global,      // Applies to entire configuration
    Brain,       // Applies to brain configuration only
    Agent,       // Applies to agent configuration only
    World,       // Applies to world configuration only
    System,      // Applies to system-level settings
    Plugin,      // Applies to plugin-specific settings
    Component,   // Applies to component-specific settings
    Custom       // User-defined scope
};

enum class ValidationSeverity {
    Error,       // Validation failed - configuration is invalid
    Warning,     // Validation warning - configuration may have issues
    Info,        // Information - configuration may be suboptimal
    Hint,        // Hint - suggestion for improvement
};

// ===============================================================
// VALIDATION RULE DEFINITIONS
// ===============================================================

struct ValidationRule {
    std::string id;                           // Unique rule identifier
    std::string name;                         // Human-readable name
    ValidationRuleType type;                  // Type of validation rule
    ConfigurationScope scope;                 // Scope of the rule
    ValidationSeverity severity;              // Severity of violation
    std::string description;                  // Description of the rule
    bool isRequired;                          // Whether this rule is mandatory
    std::unordered_map<std::string, std::string> parameters;  // Rule-specific parameters
    
    // Custom validation function
    std::function<bool(const Config&, const ValidationRule&)> customValidator;
    
    // Error messages
    std::string errorMessage;                 // Message when validation fails
    std::string warningMessage;               // Message when validation warns
    std::string hintMessage;                  // Message when providing hint
    
    // Tags for categorization
    std::vector<std::string> tags;
    std::unordered_map<std::string, std::string> metadata;
};

struct ValidationResult {
    bool isValid;                             // Overall validation status
    std::vector<ValidationIssue> issues;      // List of validation issues
    std::unordered_map<std::string, int> ruleCounts;  // Count of rules by severity
    double validationTime;                    // Time taken for validation (seconds)
    std::string sessionId;                    // Session identifier for this validation
    
    ValidationResult() : isValid(true), validationTime(0.0) {}
};

struct ValidationIssue {
    std::string ruleId;                       // ID of the rule that failed
    std::string ruleName;                     // Name of the rule
    ValidationSeverity severity;              // Severity of the issue
    std::string message;                      // Human-readable message
    std::string key;                          // Configuration key that failed validation
    std::string value;                        // Value that failed validation (may be truncated)
    std::string expected;                     // What was expected
    std::string file;                         // File where issue was detected
    int line;                                 // Line number where issue was detected
    std::string function;                     // Function where issue was detected
    
    std::chrono::system_clock::time_point timestamp;  // When the issue was detected
    std::unordered_map<std::string, std::string> context;  // Additional context
    
    // Location information
    std::string category;                     // Category of the issue
    std::string subcategory;                  // Subcategory of the issue
    std::vector<std::string> suggestedFixes;  // Suggested fixes for the issue
    
    ValidationIssue() : line(0), severity(ValidationSeverity::Error) {}
    ValidationIssue(const std::string& ruleId, const std::string& message, 
                   ValidationSeverity severity, const std::string& key = "",
                   const std::string& value = "", const std::string& expected = "")
        : ruleId(ruleId), ruleName(""), severity(severity), message(message),
          key(key), value(value), expected(expected), line(0) {}
};

struct ValidationContext {
    std::string sessionId;                    // Session identifier
    std::string environment;                  // Environment (development, production, testing)
    std::string configurationSource;          // Source of configuration (file, command-line, API)
    std::unordered_map<std::string, std::string> environmentVariables;  // Environment variables
    
    // Configuration scope
    std::unordered_map<ConfigurationScope, std::unordered_set<std::string>> activeScopes;
    
    // Validation settings
    ValidationLevel validationLevel;           // Overall validation level
    bool strictTypeChecking;                   // Whether to enforce strict type checking
    bool validateDependencies;                 // Whether to validate dependencies
    bool checkCircularReferences;              // Whether to check for circular references
    
    // Performance settings
    size_t maxConfigSize;                      // Maximum configuration size
    size_t timeout;                            // Validation timeout in seconds
    
    // Logging settings
    bool logIssues;                           // Whether to log validation issues
    std::string logFile;                      // Log file path
    
    // Repair settings
    bool autoRepair;                          // Whether to attempt auto-repair
    bool suggestFixes;                        // Whether to suggest fixes
    
    ValidationContext() : validationLevel(ValidationLevel::Strict), 
                         strictTypeChecking(true), validateDependencies(true),
                         checkCircularReferences(true), maxConfigSize(1024 * 1024), // 1MB
                         timeout(30), logIssues(true), autoRepair(false), suggestFixes(true) {}
};

// ===============================================================
// CONFIGURATION SCHEMA DEFINITIONS
// ===============================================================

struct ConfigurationSchema {
    std::string name;                         // Schema name
    std::string description;                  // Schema description
    std::string version;                      // Schema version
    ConfigurationScope scope;                 // Schema scope
    
    // Schema rules
    std::vector<ValidationRule> rules;        // Validation rules
    std::vector<std::string> requiredKeys;    // Required configuration keys
    std::vector<std::string> forbiddenKeys;   // Forbidden configuration keys
    
    // Type definitions
    std::unordered_map<std::string, std::string> typeDefinitions;  // Type definitions
    
    // Schema constraints
    size_t minKeys;                           // Minimum number of keys
    size_t maxKeys;                           // Maximum number of keys
    
    // Validation settings
    ValidationLevel validationLevel;           // Schema validation level
    bool allowUnknownKeys;                     // Whether to allow unknown keys
    bool strictTypeChecking;                   // Whether to enforce strict type checking
    
    // Schema metadata
    std::unordered_map<std::string, std::string> metadata;
    std::vector<std::string> tags;            // Tags for categorization
    
    // Dependencies
    std::vector<std::pair<std::string, std::string>> dependencies;  // Key dependencies
    
    // Examples
    std::vector<nlohmann::json> examples;     // Example configurations
    
    ConfigurationSchema() : scope(ConfigurationScope::Global), 
                           minKeys(0), maxKeys(1000), 
                           validationLevel(ValidationLevel::Strict),
                           allowUnknownKeys(false), strictTypeChecking(true) {}
};

// ===============================================================
// CONFIGURATION VALIDATOR CLASSES
// ===============================================================

class IValidator {
public:
    virtual ~IValidator() = default;
    
    // Core validation methods
    virtual ValidationResult validate(const Config& config) = 0;
    virtual ValidationResult validate(const Config& config, const ValidationContext& context) = 0;
    virtual ValidationResult validate(const std::string& configJson) = 0;
    
    // Rule management
    virtual void addRule(const ValidationRule& rule) = 0;
    virtual void removeRule(const std::string& ruleId) = 0;
    virtual void clearRules() = 0;
    virtual std::vector<ValidationRule> getRules() const = 0;
    
    // Schema management
    virtual void addSchema(const ConfigurationSchema& schema) = 0;
    virtual void removeSchema(const std::string& schemaName) = 0;
    virtual void clearSchemas() = 0;
    virtual std::vector<ConfigurationSchema> getSchemas() const = 0;
    
    // Validation context management
    virtual void setContext(const ValidationContext& context) = 0;
    virtual ValidationContext getContext() const = 0;
    
    // Validation options
    virtual void setValidationLevel(ValidationLevel level) = 0;
    virtual ValidationLevel getValidationLevel() const = 0;
    virtual void enableAutoRepair(bool enable) = 0;
    virtual bool isAutoRepairEnabled() const = 0;
    
    // Statistics
    virtual size_t getTotalRules() const = 0;
    virtual size_t getTotalIssues() const = 0;
    virtual size_t getErrorsCount() const = 0;
    virtual size_t getWarningsCount() const = 0;
    virtual double getValidationTime() const = 0;
};

class AdvancedValidator : public IValidator {
public:
    AdvancedValidator();
    ~AdvancedValidator() override;
    
    // Core validation methods
    ValidationResult validate(const Config& config) override;
    ValidationResult validate(const Config& config, const ValidationContext& context) override;
    ValidationResult validate(const std::string& configJson) override;
    
    // Rule management
    void addRule(const ValidationRule& rule) override;
    void removeRule(const std::string& ruleId) override;
    void clearRules() override;
    std::vector<ValidationRule> getRules() const override;
    
    // Schema management
    void addSchema(const ConfigurationSchema& schema) override;
    void removeSchema(const std::string& schemaName) override;
    void clearSchemas() override;
    std::vector<ConfigurationSchema> getSchemas() const override;
    
    // Validation context management
    void setContext(const ValidationContext& context) override;
    ValidationContext getContext() const override;
    
    // Validation options
    void setValidationLevel(ValidationLevel level) override;
    ValidationLevel getValidationLevel() const override;
    void enableAutoRepair(bool enable) override;
    bool isAutoRepairEnabled() const override;
    
    // Statistics
    size_t getTotalRules() const override;
    size_t getTotalIssues() const override;
    size_t getErrorsCount() const override;
    size_t getWarningsCount() const override;
    double getValidationTime() const override;
    
    // Configuration validation methods
    ValidationResult validateConfig(const Config& config) const;
    ValidationResult validateConfig(const Config& config, const ConfigurationSchema& schema) const;
    
    // Advanced validation methods
    ValidationResult validateDependencies(const Config& config) const;
    ValidationResult validateTypes(const Config& config) const;
    ValidationResult validateRanges(const Config& config) const;
    ValidationResult validatePatterns(const Config& config) const;
    ValidationResult validateSchemas(const Config& config) const;
    
    // Repair methods
    bool attemptAutoRepair(Config& config) const;
    std::vector<ValidationIssue> suggestFixes(const Config& config) const;
    
    // Configuration management
    Config applyFixes(const Config& config) const;
    Config applySchemaFixes(const Config& config, const ConfigurationSchema& schema) const;
    
    // Export/import
    bool exportRules(const std::string& filepath) const;
    bool importRules(const std::string& filepath);
    bool exportSchemas(const std::string& filepath) const;
    bool importSchemas(const std::string& filepath);
    
    // Schema utilities
    ConfigurationSchema createBrainSchema();
    ConfigurationSchema createWorldSchema();
    ConfigurationSchema createAgentSchema();
    ConfigurationSchema createSystemSchema();
    
    ConfigurationSchema createPhase6Schema();
    ConfigurationSchema createDevelopmentSchema();
    ConfigurationSchema createNeuromodulationSchema();
    
    // Predefined rule sets
    void createDefaultRules();
    void createBrainRules();
    void createWorldRules();
    void createAgentRules();
    void createSystemRules();
    void createNeuromodulationRules();
    void createMemoryRules();
    void createPredictionRules();
    void createCognitionRules();
    void createDevelopmentRules();
    
    // Statistics and analysis
    std::unordered_map<std::string, size_t> getRuleStatistics() const;
    std::unordered_map<std::string, size_t> getSchemaStatistics() const;
    std::string getValidationSummary() const;
    
    // Report generation
    std::string generateValidationReport(const ValidationResult& result) const;
    std::string generateSchemaReport(const ConfigurationSchema& schema) const;
    std::string generateRuleReport(const ValidationRule& rule) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void initializeDefaultRules();
    void initializeSchemaValidations();
    void setupRuleEngine();
    
    ValidationResult validateAgainstSchema(const Config& config, const ConfigurationSchema& schema) const;
    ValidationResult validateRule(const ValidationRule& rule, const Config& config) const;
    
    bool matchesRuleConditions(const ValidationRule& rule, const Config& config) const;
    bool checkType(const std::string& key, const std::string& value, const std::string& type) const;
    bool checkRange(const std::string& key, const std::string& value, const std::string& range) const;
    bool checkPattern(const std::string& key, const std::string& value, const std::string& pattern) const;
    bool checkDependency(const std::string& key, const std::string& value, const std::string& dependency) const;
    
    std::vector<ValidationIssue> collectIssues(const Config& config, const ValidationContext& context) const;
    void classifyIssues(ValidationResult& result) const;
    
    bool attemptAutoFix(const ValidationIssue& issue, Config& config) const;
    std::string getTypeName(const std::string& value) const;
    
    bool validateConfigJson(const std::string& jsonString, Config& config) const;
    std::string configToJson(const Config& config) const;
    bool jsonToConfig(const std::string& jsonString, Config& config) const;
    
    void writeRulesToJson(const std::string& filepath) const;
    bool readRulesFromJson(const std::string& filepath);
    void writeSchemasToJson(const std::string& filepath) const;
    bool readSchemasFromJson(const std::string& filepath);
    
    void generateRuleDocumentation(std::ostream& out) const;
    void generateSchemaDocumentation(std::ostream& out) const;
    void generateValidationDocumentation(std::ostream& out) const;
};

// ===============================================================
// VALIDATION FACTORY AND UTILITIES
// ===============================================================

class ValidationFactory {
public:
    static std::shared_ptr<IValidator> createStandardValidator();
    static std::shared_ptr<IValidator> createStrictValidator();
    static std::shared_ptr<IValidator> createLenientValidator();
    static std::shared_ptr<IValidator> createCustomValidator(const std::vector<ValidationRule>& customRules);
    
    static void setupGlobalValidator(std::shared_ptr<IValidator> validator);
    static std::shared_ptr<IValidator> getGlobalValidator();
    
    static ValidationRule createRequiredRule(const std::string& key, const std::string& description = "");
    static ValidationRule createTypeRule(const std::string& key, const std::string& type, const std::string& description = "");
    static ValidationRule createRangeRule(const std::string& key, const std::string& min, const std::string& max, const std::string& description = "");
    static ValidationRule createPatternRule(const std::string& key, const std::string& pattern, const std::string& description = "");
    static ValidationRule createLengthRule(const std::string& key, size_t minLength, size_t maxLength, const std::string& description = "");
    static ValidationRule createDependencyRule(const std::string& key, const std::string& dependsOn, const std::string& description = "");
    static ValidationRule createUniqueRule(const std::string& key, const std::string& description = "");
    
    static ConfigurationSchema createBrainConfigurationSchema();
    static ConfigurationSchema createWorldConfigurationSchema();
    static ConfigurationSchema createAgentConfigurationSchema();
    static ConfigurationSchema createSystemConfigurationSchema();
    static ConfigurationSchema createNeuromodulationConfigurationSchema();
    static ConfigurationSchema createMemoryConfigurationSchema();
    static ConfigurationSchema createPredictionConfigurationSchema();
    static ConfigurationSchema createCognitionConfigurationSchema();
    static ConfigurationSchema createDevelopmentConfigurationSchema();
    
    static void setupDevelopmentValidator();
    static void setupProductionValidator();
    static void setupResearchValidator();
    
    static std::string getValidatorVersion();
    static std::vector<std::string> getAvailableValidatorTypes();
    static bool isValidatorAvailable(const std::string& type);
    
    static std::string getDefaultConfigurationSchema();
    static std::string getBrainConfigurationSchema();
    static std::string getWorldConfigurationSchema();
    static std::string getAgentConfigurationSchema();
    static std::string getSystemConfigurationSchema();
};

// ===============================================================
// VALIDATION MACROS AND STREAMS
// ===============================================================

// Validation macros
#define NLM_VALIDATE_CONFIG(config) nlm::ValidationFactory::getGlobalValidator()->validate(config)
#define NLM_VALIDATE_CONFIG_JSON(json) nlm::ValidationFactory::getGlobalValidator()->validate(json)
#define NLM_VALIDATE_CONFIG_STRICT(config) nlm::ValidationFactory::createStrictValidator()->validate(config)
#define NLM_VALIDATE_CONFIG_LENIENT(config) nlm::ValidationFactory::createLenientValidator()->validate(config)

#define NLM_VALIDATE_AND_FIX(config) nlm::ValidationFactory::getGlobalValidator()->isAutoRepairEnabled() ? nlm::ValidationFactory::getGlobalValidator()->attemptAutoFix(nlm::ValidationFactory::getGlobalValidator()->validate(config), config) : false

// Validation stream for detailed reporting
class ValidationStream {
public:
    ValidationStream(const std::string& operation, std::shared_ptr<IValidator> validator);
    ~ValidationStream();
    
    template<typename T>
    ValidationStream& operator<<(const T& value) {
        data_ << value;
        return *this;
    }
    
    ValidationStream& withRule(const ValidationRule& rule);
    ValidationStream& withContext(const ValidationContext& context);
    ValidationStream& withAutoRepair(bool enable);
    
    ValidationResult getResult() const;
    std::string getReport() const;

private:
    std::string operation_;
    std::shared_ptr<IValidator> validator_;
    std::ostringstream data_;
    
    ValidationContext context_;
    bool autoRepair_ = false;
    ValidationResult result_;
};

// Validation stream macro
#define NLM_VALIDATE_STREAM(operation) nlm::ValidationStream operation##_stream(operation, nlm::ValidationFactory::getGlobalValidator())

// ===============================================================
// VALIDATION HELP AND UTILITIES
// ===============================================================

void showValidationHelp();
void showValidationSetupGuide();
void showValidationRuleGuide();
void showValidationSchemaGuide();
void showValidationReportGuide();

} // namespace nlm

// Include implementation header
#include "Validator.ipp"
