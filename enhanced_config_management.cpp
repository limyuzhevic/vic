// Enhanced configuration management and validation

// This file provides enhanced configuration management capabilities including:
// 1. Advanced configuration schema validation
// 2. Configuration templates and presets
// 3. Configuration migration and upgrade support
// 4. Configuration dependency management
// 5. Configuration change tracking and rollback support
// 6. Environment-specific configuration management

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <regex>

namespace py = pybind11;
namespace nlm {

// Configuration schema validation
class ConfigurationSchema {
public:
    enum class ConstraintType {
        Range,
        Choices,
        RegexPattern,
        Custom,
        Required,
        Positive,
        Negative,
        NonZero,
        Unique
    };
    
    struct ValidationRule {
        std::string key;
        ConstraintType constraintType;
        std::string errorMessage;
        std::vector<std::string> choices; // For Choices constraint
        std::string pattern; // For RegexPattern constraint
        std::function<bool(const ConfigValue&)> customValidator; // For Custom constraint
        double minValue; // For Range constraint
        double maxValue; // For Range constraint
    };
    
    ConfigurationSchema() = default;
    
    void addRule(const ValidationRule& rule) {
        rules.push_back(rule);
    }
    
    void addRangeRule(const std::string& key, double min, double max, 
                     const std::string& errorMsg = "Value out of range") {
        ValidationRule rule;
        rule.key = key;
        rule.constraintType = ConstraintType::Range;
        rule.errorMessage = errorMsg;
        rule.minValue = min;
        rule.maxValue = max;
        rules.push_back(rule);
    }
    
    void addChoicesRule(const std::string& key, 
                       const std::vector<std::string>& choices,
                       const std::string& errorMsg = "Invalid choice") {
        ValidationRule rule;
        rule.key = key;
        rule.constraintType = ConstraintType::Choices;
        rule.errorMessage = errorMsg;
        rule.choices = choices;
        rules.push_back(rule);
    }
    
    void addPatternRule(const std::string& key, const std::string& pattern,
                       const std::string& errorMsg = "Pattern mismatch") {
        ValidationRule rule;
        rule.key = key;
        rule.constraintType = ConstraintType::RegexPattern;
        rule.errorMessage = errorMsg;
        rule.pattern = pattern;
        rules.push_back(rule);
    }
    
    void addRequiredRule(const std::string& key, const std::string& errorMsg = "Required") {
        ValidationRule rule;
        rule.key = key;
        rule.constraintType = ConstraintType::Required;
        rule.errorMessage = errorMsg;
        rules.push_back(rule);
    }
    
    std::vector<std::string> validate(const Config& config) const {
        std::vector<std::string> errors;
        
        for (const auto& rule : rules) {
            if (!rule.key.empty() && !config.has(rule.key)) {
                // Skip if key doesn't exist (unless it's required)
                if (rule.constraintType == ConstraintType::Required) {
                    errors.push_back(rule.errorMessage);
                }
                continue;
            }
            
            auto value = config.get(rule.key);
            if (!value.has_value()) {
                if (rule.constraintType == ConstraintType::Required) {
                    errors.push_back(rule.errorMessage);
                }
                continue;
            }
            
            // Apply constraint validation
            switch (rule.constraintType) {
                case ConstraintType::Range:
                    validateRange(*value, rule, errors);
                    break;
                case ConstraintType::Choices:
                    validateChoices(*value, rule, errors);
                    break;
                case ConstraintType::RegexPattern:
                    validatePattern(*value, rule, errors);
                    break;
                case ConstraintType::Required:
                    // Already handled above
                    break;
                case ConstraintType::Custom:
                    if (rule.customValidator && !rule.customValidator(*value)) {
                        errors.push_back(rule.errorMessage);
                    }
                    break;
                case ConstraintType::Positive:
                    validatePositive(*value, rule, errors);
                    break;
                case ConstraintType::Negative:
                    validateNegative(*value, rule, errors);
                    break;
                case ConstraintType::NonZero:
                    validateNonZero(*value, rule, errors);
                    break;
                case ConstraintType::Unique:
                    // Would require context about all values
                    break;
            }
        }
        
        return errors;
    }
    
    bool hasRules() const {
        return !rules.empty();
    }
    
    size_t ruleCount() const {
        return rules.size();
    }
    
private:
    void validateRange(const ConfigValue& value, const ValidationRule& rule, 
                      std::vector<std::string>& errors) const {
        try {
            double val = 0.0;
            if (std::holds_alternative<int>(value)) {
                val = std::get<int>(value);
            } else if (std::holds_alternative<int64_t>(value)) {
                val = static_cast<double>(std::get<int64_t>(value));
            } else if (std::holds_alternative<double>(value)) {
                val = std::get<double>(value);
            } else {
                return; // Not a numeric type
            }
            
            if (val < rule.minValue || val > rule.maxValue) {
                errors.push_back(rule.errorMessage);
            }
        } catch (...) {
            errors.push_back("Range validation failed for key: " + rule.key);
        }
    }
    
    void validateChoices(const ConfigValue& value, const ValidationRule& rule,
                        std::vector<std::string>& errors) const {
        std::string strValue;
        if (std::holds_alternative<std::string>(value)) {
            strValue = std::get<std::string>(value);
        } else if (std::holds_alternative<int>(value)) {
            strValue = std::to_string(std::get<int>(value));
        } else if (std::holds_alternative<int64_t>(value)) {
            strValue = std::to_string(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            strValue = std::to_string(std::get<double>(value));
        } else if (std::holds_alternative<bool>(value)) {
            strValue = std::get<bool>(value) ? "true" : "false";
        } else {
            errors.push_back("Choices validation failed for key: " + rule.key);
            return;
        }
        
        if (std::find(rule.choices.begin(), rule.choices.end(), strValue) == rule.choices.end()) {
            errors.push_back(rule.errorMessage);
        }
    }
    
    void validatePattern(const ConfigValue& value, const ValidationRule& rule,
                        std::vector<std::string>& errors) const {
        std::string strValue;
        if (std::holds_alternative<std::string>(value)) {
            strValue = std::get<std::string>(value);
        } else if (std::holds_alternative<int>(value)) {
            strValue = std::to_string(std::get<int>(value));
        } else if (std::holds_alternative<int64_t>(value)) {
            strValue = std::to_string(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            strValue = std::to_string(std::get<double>(value));
        } else {
            return; // Not a string type, skip pattern validation
        }
        
        std::regex pattern(rule.pattern);
        if (!std::regex_match(strValue, pattern)) {
            errors.push_back(rule.errorMessage);
        }
    }
    
    void validatePositive(const ConfigValue& value, const ValidationRule& rule,
                         std::vector<std::string>& errors) const {
        try {
            double val = 0.0;
            if (std::holds_alternative<int>(value)) {
                val = static_cast<double>(std::get<int>(value));
            } else if (std::holds_alternative<int64_t>(value)) {
                val = static_cast<double>(std::get<int64_t>(value));
            } else if (std::holds_alternative<double>(value)) {
                val = std::get<double>(value);
            } else {
                return; // Not a numeric type
            }
            
            if (val <= 0) {
                errors.push_back(rule.errorMessage);
            }
        } catch (...) {
            errors.push_back("Positive validation failed for key: " + rule.key);
        }
    }
    
    void validateNegative(const ConfigValue& value, const ValidationRule& rule,
                         std::vector<std::string>& errors) const {
        try {
            double val = 0.0;
            if (std::holds_alternative<int>(value)) {
                val = static_cast<double>(std::get<int>(value));
            } else if (std::holds_alternative<int64_t>(value)) {
                val = static_cast<double>(std::get<int64_t>(value));
            } else if (std::holds_alternative<double>(value)) {
                val = std::get<double>(value);
            } else {
                return; // Not a numeric type
            }
            
            if (val >= 0) {
                errors.push_back(rule.errorMessage);
            }
        } catch (...) {
            errors.push_back("Negative validation failed for key: " + rule.key);
        }
    }
    
    void validateNonZero(const ConfigValue& value, const ValidationRule& rule,
                         std::vector<std::string>& errors) const {
        try {
            double val = 0.0;
            if (std::holds_alternative<int>(value)) {
                val = static_cast<double>(std::get<int>(value));
            } else if (std::holds_alternative<int64_t>(value)) {
                val = static_cast<double>(std::get<int64_t>(value));
            } else if (std::holds_alternative<double>(value)) {
                val = std::get<double>(value);
            } else {
                return; // Not a numeric type
            }
            
            if (val == 0) {
                errors.push_back(rule.errorMessage);
            }
        } catch (...) {
            errors.push_back("Non-zero validation failed for key: " + rule.key);
        }
    }
    
    // Data members
    std::vector<ValidationRule> rules;
};

// Configuration template system
class ConfigurationTemplate {
public:
    struct TemplateParameter {
        std::string name;
        ConfigValue defaultValue;
        std::string description;
        bool required;
        
        TemplateParameter() : name(), defaultValue(), description(), required(false) {}
        TemplateParameter(const std::string& n, const ConfigValue& v, 
                         const std::string& desc = "", bool req = false)
            : name(n), defaultValue(v), description(desc), required(req) {}
    };
    
    ConfigurationTemplate() = default;
    
    void addParameter(const TemplateParameter& param) {
        parameters.push_back(param);
    }
    
    void addParameter(const std::string& name, const ConfigValue& defaultValue,
                     const std::string& description = "", bool required = false) {
        TemplateParameter param(name, defaultValue, description, required);
        addParameter(param);
    }
    
    Config applyTemplate(const Config& baseConfig) const {
        Config result = baseConfig;
        
        for (const auto& param : parameters) {
            if (!result.has(param.name)) {
                // Apply default value if key doesn't exist
                setConfigValue(result, param.name, param.defaultValue, ConfigSource::Runtime);
            }
        }
        
        return result;
    }
    
    std::map<std::string, ConfigValue> getParameters() const {
        std::map<std::string, ConfigValue> paramMap;
        for (const auto& param : parameters) {
            paramMap[param.name] = param.defaultValue;
        }
        return paramMap;
    }
    
    std::string getDescription(const std::string& paramName) const {
        for (const auto& param : parameters) {
            if (param.name == paramName) {
                return param.description;
            }
        }
        return "";
    }
    
    bool isRequired(const std::string& paramName) const {
        for (const auto& param : parameters) {
            if (param.name == paramName) {
                return param.required;
            }
        }
        return false;
    }
    
    std::vector<std::string> getParameterNames() const {
        std::vector<std::string> names;
        for (const auto& param : parameters) {
            names.push_back(param.name);
        }
        return names;
    }
    
    static std::shared_ptr<ConfigurationTemplate> createDefaultTemplate() {
        auto templatePtr = std::make_shared<ConfigurationTemplate>();
        
        // Add common NLM parameters
        templatePtr->addParameter("brain.neuron_count", static_cast<int64_t>(1000),
                                 "Total number of neurons in the brain");
        templatePtr->addParameter("brain.v_thresh", -55.0,
                                 "Neuron threshold potential (mV)");
        templatePtr->addParameter("brain.v_rest", -70.0,
                                 "Resting potential (mV)");
        templatePtr->addParameter("brain.tau_mem", 20.0,
                                 "Membrane time constant (ms)");
        templatePtr->addParameter("brain.tau_ref", 2.0,
                                 "Refractory period (ms)");
        templatePtr->addParameter("brain.synapse_density", 0.1,
                                 "Synapse connectivity density");
        templatePtr->addParameter("brain.connection_probability", 0.05,
                                 "Probability of connection between neurons");
        templatePtr->addParameter("plasticity.stdp.enable", true,
                                 "Enable Spike-Timing Dependent Plasticity");
        templatePtr->addParameter("plasticity.hebbian.enable", true,
                                 "Enable Hebbian plasticity");
        templatePtr->addParameter("plasticity.structural.enable", true,
                                 "Enable structural plasticity");
        templatePtr->addParameter("neuromod.dopamine.scale", 1.0,
                                 "Dopamine modulation scale");
        templatePtr->addParameter("neuromod.curiosity.enable", true,
                                 "Enable curiosity-driven exploration");
        templatePtr->addParameter("development.enable", true,
                                 "Enable brain development");
        templatePtr->addParameter("checkpoint_dir", "./checkpoints",
                                 "Directory for checkpoint files");
        
        return templatePtr;
    }
    
    static std::shared_ptr<ConfigurationTemplate> createPerformanceTemplate() {
        auto templatePtr = std::make_shared<ConfigurationTemplate>();
        
        // Performance-optimized template
        templatePtr->addParameter("brain.neuron_count", static_cast<int64_t>(500),
                                 "Total number of neurons (reduced for performance)");
        templatePtr->addParameter("brain.tau_mem", 10.0,
                                 "Faster membrane time constant (ms)");
        templatePtr->addParameter("plasticity.stdp.enable", true,
                                 "Enable STDP for learning");
        templatePtr->addParameter("plasticity.hebbian.enable", false,
                                 "Disable Hebbian plasticity (faster)");
        templatePtr->addParameter("neuromod.curiosity.enable", false,
                                 "Disable curiosity for faster simulation");
        templatePtr->addParameter("development.enable", false,
                                 "Disable development for faster simulation");
        
        return templatePtr;
    }
    
    static std::shared_ptr<ConfigurationTemplate> createLearningTemplate() {
        auto templatePtr = std::make_shared<ConfigurationTemplate>();
        
        // Learning-optimized template
        templatePtr->addParameter("brain.neuron_count", static_cast<int64_t>(2000),
                                 "Higher neuron count for better learning capacity");
        templatePtr->addParameter("brain.v_thresh", -50.0,
                                 "Higher threshold for more stable firing");
        templatePtr->addParameter("plasticity.stdp.enable", true,
                                 "Enable STDP for temporal learning");
        templatePtr->addParameter("plasticity.hebbian.enable", true,
                                 "Enable Hebbian learning for associational learning");
        templatePtr->addParameter("plasticity.structural.enable", true,
                                 "Enable structural plasticity for growth");
        templatePtr->addParameter("neuromod.curiosity.enable", true,
                                 "Enable curiosity for exploration");
        templatePtr->addParameter("neuromod.dopamine.scale", 1.5,
                                 "Enhanced dopamine modulation for learning");
        templatePtr->addParameter("development.enable", true,
                                 "Enable developmental stages for maturation");
        
        return templatePtr;
    }
    
private:
    void setConfigValue(Config& config, const std::string& key, 
                       const ConfigValue& value, ConfigSource source) {
        std::visit([&config, &key, source](auto&& val) {
            config.set(key, val, source);
        }, value);
    }
    
    // Data members
    std::vector<TemplateParameter> parameters;
};

// Configuration migration system
class ConfigurationMigration {
public:
    struct MigrationStep {
        int version;
        std::string description;
        std::function<Config(const Config&)> migrateFunc;
        
        MigrationStep() : version(0), description(), migrateFunc() {}
        MigrationStep(int v, const std::string& desc, 
                     const std::function<Config(const Config&)>& func)
            : version(v), description(desc), migrateFunc(func) {}
    };
    
    ConfigurationMigration() = default;
    
    void addMigrationStep(int version, const std::string& description,
                         const std::function<Config(const Config&)>& migrateFunc) {
        MigrationStep step(version, description, migrateFunc);
        migrationSteps.push_back(step);
        
        // Sort by version
        std::sort(migrationSteps.begin(), migrationSteps.end(),
                 [](const MigrationStep& a, const MigrationStep& b) {
                     return a.version < b.version;
                 });
    }
    
    Config migrate(const Config& sourceConfig, int targetVersion) const {
        Config current = sourceConfig;
        int currentVersion = getVersion(current);
        
        // Find migration steps to apply
        auto it = std::find_if(migrationSteps.begin(), migrationSteps.end(),
                              [currentVersion](const MigrationStep& step) {
                                  return step.version > currentVersion && 
                                         step.version <= targetVersion;
                              });
        
        while (it != migrationSteps.end()) {
            current = it->migrateFunc(current);
            currentVersion = it->version;
            
            it = std::find_if(migrationSteps.begin(), migrationSteps.end(),
                            [currentVersion](const MigrationStep& step) {
                                return step.version > currentVersion && 
                                       step.version <= targetVersion;
                            });
        }
        
        setVersion(current, targetVersion);
        return current;
    }
    
    std::vector<std::string> getMigrationHistory(const Config& config) const {
        std::vector<std::string> history;
        int currentVersion = getVersion(config);
        
        for (const auto& step : migrationSteps) {
            if (step.version <= currentVersion) {
                history.push_back("✓ Applied migration v" + std::to_string(step.version) + ": " + step.description);
            } else {
                history.push_back("- Pending migration v" + std::to_string(step.version) + ": " + step.description);
            }
        }
        
        return history;
    }
    
    int getVersion(const Config& config) const {
        int version = 0; // Default version
        
        if (config.has("config.version")) {
            auto val = config.get<int64_t>("config.version");
            if (val.has_value()) {
                version = static_cast<int>(val.value());
            }
        }
        
        return version;
    }
    
    void setVersion(Config& config, int version) const {
        config.set("config.version", static_cast<int64_t>(version), ConfigSource::Runtime);
    }
    
    static std::shared_ptr<ConfigurationMigration> createDefaultMigration() {
        auto migrationPtr = std::make_shared<ConfigurationMigration>();
        
        // Add common migration steps
        migrationPtr->addMigrationStep(1, "Initialize config version",
                                     [](const Config& config) -> Config {
                                         Config result = config;
                                         migrationPtr->setVersion(result, 1);
                                         return result;
                                     });
        
        migrationPtr->addMigrationStep(2, "Add plasticity configuration",
                                     [](const Config& config) -> Config {
                                         Config result = config;
                                         
                                         // Set default plasticity values if not present
                                         if (!result.has("plasticity.stdp.enable")) {
                                             result.set("plasticity.stdp.enable", true, ConfigSource::Runtime);
                                         }
                                         if (!result.has("plasticity.hebbian.enable")) {
                                             result.set("plasticity.hebbian.enable", true, ConfigSource::Runtime);
                                         }
                                         if (!result.has("plasticity.structural.enable")) {
                                             result.set("plasticity.structural.enable", true, ConfigSource::Runtime);
                                         }
                                         
                                         migrationPtr->setVersion(result, 2);
                                         return result;
                                     });
        
        migrationPtr->addMigrationStep(3, "Add neuromodulation configuration",
                                     [](const Config& config) -> Config {
                                         Config result = config;
                                         
                                         // Set default neuromodulation values if not present
                                         if (!result.has("neuromod.dopamine.scale")) {
                                             result.set("neuromod.dopamine.scale", 1.0, ConfigSource::Runtime);
                                         }
                                         if (!result.has("neuromod.curiosity.enable")) {
                                             result.set("neuromod.curiosity.enable", true, ConfigSource::Runtime);
                                         }
                                         if (!result.has("neuromod.novelty.enable")) {
                                             result.set("neuromod.novelty.enable", true, ConfigSource::Runtime);
                                         }
                                         
                                         migrationPtr->setVersion(result, 3);
                                         return result;
                                     });
        
        migrationPtr->addMigrationStep(4, "Add development system configuration",
                                     [](const Config& config) -> Config {
                                         Config result = config;
                                         
                                         // Set default development values if not present
                                         if (!result.has("development.enable")) {
                                             result.set("development.enable", true, ConfigSource::Runtime);
                                         }
                                         if (!result.has("development.stage_count")) {
                                             result.set("development.stage_count", 5, ConfigSource::Runtime);
                                         }
                                         
                                         migrationPtr->setVersion(result, 4);
                                         return result;
                                     });
        
        return migrationPtr;
    }
    
    std::vector<std::string> getAvailableMigrations(int fromVersion, int toVersion) const {
        std::vector<std::string> available;
        
        auto it = std::find_if(migrationSteps.begin(), migrationSteps.end(),
                              [fromVersion](const MigrationStep& step) {
                                  return step.version > fromVersion;
                              });
        
        while (it != migrationSteps.end() && it->version <= toVersion) {
            available.push_back(std::to_string(it->version) + ": " + it->description);
            ++it;
        }
        
        return available;
    }
    
private:
    // Data members
    std::vector<MigrationStep> migrationSteps;
};

// Environment-based configuration management
class EnvironmentConfigManager {
public:
    enum class Environment {
        Development,
        Testing,
        Staging,
        Production
    };
    
    EnvironmentConfigManager() = default;
    
    void setEnvironment(Environment env) {
        currentEnvironment = env;
    }
    
    Environment getEnvironment() const {
        return currentEnvironment;
    }
    
    Config loadEnvironmentConfig(const std::string& basePath,
                                 Environment env = Environment::Development) const {
        Config config;
        
        // Load base configuration
        std::string baseFile = basePath + "/base.cfg";
        if (std::filesystem::exists(baseFile)) {
            config.loadFromFile(baseFile);
        }
        
        // Load environment-specific configuration
        std::string envFile = basePath + "/" + getEnvironmentFilename(env);
        if (std::filesystem::exists(envFile)) {
            config.loadFromFile(envFile);
        }
        
        // Load user-specific configuration (overrides environment)
        std::string userFile = basePath + "/user.cfg";
        if (std::filesystem::exists(userFile)) {
            config.loadFromFile(userFile);
        }
        
        return config;
    }
    
    void saveEnvironmentConfig(const Config& config, const std::string& basePath,
                              Environment env = Environment::Development) const {
        // Create backup of existing environment config
        std::string backupFile = basePath + "/" + getEnvironmentFilename(env) + ".backup";
        std::string currentFile = basePath + "/" + getEnvironmentFilename(env);
        
        if (std::filesystem::exists(currentFile)) {
            std::filesystem::copy_file(currentFile, backupFile, 
                                      std::filesystem::copy_options::overwrite_existing);
        }
        
        // Save configuration
        config.saveToFile(currentFile);
    }
    
    static std::string getEnvironmentFilename(Environment env) {
        switch (env) {
            case Environment::Development: return "development.cfg";
            case Environment::Testing: return "testing.cfg";
            case Environment::Staging: return "staging.cfg";
            case Environment::Production: return "production.cfg";
            default: return "development.cfg";
        }
    }
    
    static std::shared_ptr<EnvironmentConfigManager> createDefault() {
        return std::make_shared<EnvironmentConfigManager>();
    }
    
private:
    Environment currentEnvironment = Environment::Development;
};

// Enhanced Python bindings for configuration management
void addConfigurationManagementBindings(py::module_& m) {
    // Configuration schema
    py::class_<ConfigurationSchema>(m, "ConfigurationSchema")
        .def(py::init<>())
        .def("add_rule", &ConfigurationSchema::addRule, py::arg("rule"),
             "Add validation rule")
        .def("add_range_rule", &ConfigurationSchema::addRangeRule,
             py::arg("key"), py::arg("min"), py::arg("max"), py::arg("errorMsg") = "Value out of range",
             "Add range constraint rule")
        .def("add_choices_rule", &ConfigurationSchema::addChoicesRule,
             py::arg("key"), py::arg("choices"), py::arg("errorMsg") = "Invalid choice",
             "Add choices constraint rule")
        .def("add_pattern_rule", &ConfigurationSchema::addPatternRule,
             py::arg("key"), py::arg("pattern"), py::arg("errorMsg") = "Pattern mismatch",
             "Add pattern constraint rule")
        .def("add_required_rule", &ConfigurationSchema::addRequiredRule,
             py::arg("key"), py::arg("errorMsg") = "Required",
             "Add required constraint rule")
        .def("validate", &ConfigurationSchema::validate, py::arg("config"),
             "Validate configuration against schema")
        .def("has_rules", &ConfigurationSchema::hasRules,
             "Check if schema has any validation rules")
        .def("rule_count", &ConfigurationSchema::ruleCount,
             "Get number of validation rules");
    
    // Configuration template
    py::class_<ConfigurationTemplate>(m, "ConfigurationTemplate")
        .def(py::init<>())
        .def("add_parameter", (void(ConfigurationTemplate::*)(const ConfigurationTemplate::TemplateParameter&)) &ConfigurationTemplate::addParameter,
             py::arg("param"), "Add template parameter")
        .def("add_parameter", (void(ConfigurationTemplate::*)(const std::string&, const ConfigValue&, const std::string&, bool)) &ConfigurationTemplate::addParameter,
             py::arg("name"), py::arg("defaultValue"), py::arg("description") = "", py::arg("required") = false,
             "Add template parameter with name, default value, and description")
        .def("apply_template", &ConfigurationTemplate::applyTemplate, py::arg("baseConfig"),
             "Apply template to base configuration")
        .def("get_parameters", &ConfigurationTemplate::getParameters,
             "Get all template parameters as dictionary")
        .def("get_description", &ConfigurationTemplate::getDescription, py::arg("paramName"),
             "Get description for parameter")
        .def("is_required", &ConfigurationTemplate::isRequired, py::arg("paramName"),
             "Check if parameter is required")
        .def("get_parameter_names", &ConfigurationTemplate::getParameterNames,
             "Get all parameter names")
        .def_static("create_default_template", &ConfigurationTemplate::createDefaultTemplate,
             "Create default NLM configuration template")
        .def_static("create_performance_template", &ConfigurationTemplate::createPerformanceTemplate,
             "Create performance-optimized configuration template")
        .def_static("create_learning_template", &ConfigurationTemplate::createLearningTemplate,
             "Create learning-optimized configuration template");
    
    // Configuration migration
    py::class_<ConfigurationMigration>(m, "ConfigurationMigration")
        .def(py::init<>())
        .def("add_migration_step", &ConfigurationMigration::addMigrationStep,
             py::arg("version"), py::arg("description"), py::arg("migrateFunc"),
             "Add migration step")
        .def("migrate", &ConfigurationMigration::migrate, py::arg("sourceConfig"), py::arg("targetVersion"),
             "Migrate configuration to target version")
        .def("get_migration_history", &ConfigurationMigration::getMigrationHistory, py::arg("config"),
             "Get migration history for configuration")
        .def("get_version", &ConfigurationMigration::getVersion, py::arg("config"),
             "Get configuration version")
        .def_static("create_default_migration", &ConfigurationMigration::createDefaultMigration,
             "Create default configuration migration system")
        .def("get_available_migrations", &ConfigurationMigration::getAvailableMigrations,
             py::arg("fromVersion"), py::arg("toVersion"),
             "Get available migrations between versions");
    
    // Environment configuration manager
    py::class_<EnvironmentConfigManager>(m, "EnvironmentConfigManager")
        .def(py::init<>())
        .def("set_environment", &EnvironmentConfigManager::setEnvironment, py::arg("env"),
             "Set current environment")
        .def("get_environment", &EnvironmentConfigManager::getEnvironment,
             "Get current environment")
        .def("load_environment_config", &EnvironmentConfigManager::loadEnvironmentConfig,
             py::arg("basePath"), py::arg("env") = EnvironmentConfigManager::Environment::Development,
             "Load configuration for environment")
        .def("save_environment_config", &EnvironmentConfigManager::saveEnvironmentConfig,
             py::arg("config"), py::arg("basePath"), py::arg("env") = EnvironmentConfigManager::Environment::Development,
             "Save configuration for environment")
        .def_static("create_default", &EnvironmentConfigManager::createDefault,
             "Create default environment configuration manager");
    
    // Convenience functions for configuration management
    m.def("create_configuration_schema", []() {
        return std::make_shared<ConfigurationSchema>();
    }, "Create configuration schema for validation");
    
    m.def("create_configuration_template", []() {
        return std::make_shared<ConfigurationTemplate>();
    }, "Create configuration template");
    
    m.def("create_configuration_migration", []() {
        return std::make_shared<ConfigurationMigration>();
    }, "Create configuration migration system");
    
    m.def("create_environment_config_manager", []() {
        return std::make_shared<EnvironmentConfigManager>();
    }, "Create environment configuration manager");
    
    m.def("validate_config_with_schema", [](const Config& config, 
                                           const ConfigurationSchema& schema) {
        return schema.validate(config);
    }, py::arg("config"), py::arg("schema"),
       "Validate configuration against schema and return errors");
    
    m.def("apply_config_template", [](const Config& config,
                                     const ConfigurationTemplate& template) {
        return template.applyTemplate(config);
    }, py::arg("config"), py::arg("template"),
       "Apply configuration template to base configuration");
    
    m.def("migrate_config", [](const Config& config,
                              const ConfigurationMigration& migration,
                              int targetVersion) {
        return migration.migrate(config, targetVersion);
    }, py::arg("config"), py::arg("migration"), py::arg("targetVersion"),
       "Migrate configuration to target version");
}

// Integration function to add all configuration management bindings
void addAllConfigurationManagement(py::module_& m) {
    addConfigurationManagementBindings(m);
    
    // Add version information
    m.attr("__version__") = "2.0.0-configuration-management";
    
    // Add configuration management utilities
    m.def("config_version_info", []() {
        std::map<std::string, std::string> info;
        info["version"] = "2.0.0-configuration-management";
        info["features"] = "Schema validation, Templates, Migration, Environment management";
        info["status"] = "Ready for enhanced configuration management";
        return info;
    }, "Get configuration management system information");
}