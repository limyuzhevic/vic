// Advanced configuration management utilities implementation
#include "NLMPythonAdvancedConfig.h"
#include "NLMPythonTypes.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <random>

namespace fs = std::filesystem;

// Advanced configuration manager with validation and templates
class AdvancedConfigManager {
public:
    struct ValidationRule {
        std::string parameterName;
        std::string validationType; // "range", "list", "regex", "dependency"
        std::string errorMessage;
        std::vector<std::string> allowedValues;
        std::pair<double, double> range;
        std::string regexPattern;
        std::string dependsOn;
        
        bool validate(const json& config) const {
            if (validationType == "range") {
                if (!config.contains(parameterName)) return true;
                double value = config[parameterName];
                return value >= range.first && value <= range.second;
            } else if (validationType == "list") {
                if (!config.contains(parameterName)) return true;
                std::string value = config[parameterName];
                return std::find(allowedValues.begin(), allowedValues.end(), value) != allowedValues.end();
            } else if (validationType == "regex") {
                if (!config.contains(parameterName)) return true;
                std::string value = config[parameterName];
                // Simple regex validation
                return value.find(regexPattern) != std::string::npos;
            } else if (validationType == "dependency") {
                if (!config.contains(parameterName) && !config.contains(dependsOn)) return true;
                if (!config.contains(parameterName)) return true; // Optional dependency
                // Check if dependent parameter has valid value
                if (config.contains(dependsOn)) {
                    // Additional validation based on dependency
                    return true;
                }
                return false;
            }
            return true; // Default to passing for unknown validation types
        }
    };
    
    struct ParameterTemplate {
        std::string name;
        std::string type; // "int", "float", "string", "bool", "list", "dict"
        std::string description;
        json defaultValue;
        bool required;
        bool readonly;
        std::vector<ValidationRule> validationRules;
        std::string category; // "brain", "plasticity", "neuromodulation", "memory", "development"
        std::string uiHint; // "slider", "dropdown", "checkbox", "textarea", "color"
        
        json getDefaultValue() const { return defaultValue; }
        bool isRequired() const { return required; }
        bool isReadonly() const { return readonly; }
        bool validate(const json& config) const {
            for (const auto& rule : validationRules) {
                if (!rule.validate(config)) {
                    return false;
                }
            }
            return true;
        }
    };
    
    struct ConfigurationProfile {
        std::string name;
        std::string description;
        json parameters;
        std::map<std::string, std::string> tags;
        std::chrono::system_clock::time_point created;
        std::chrono::system_clock::time_point modified;
        std::string createdBy;
        
        std::string getName() const { return name; }
        json getParameters() const { return parameters; }
        std::string getDescription() const { return description; }
    };
    
    struct ConfigurationHistory {
        std::vector<json> changes;
        std::vector<ConfigurationProfile> profiles;
        
        void addChange(const std::string& parameter, const json& oldValue, const json& newValue,
                      const std::string& user = "system", const std::string& reason = "") {
            json change;
            change["parameter"] = parameter;
            change["old_value"] = oldValue;
            change["new_value"] = newValue;
            change["timestamp"] = std::chrono::system_clock::now();
            change["user"] = user;
            change["reason"] = reason;
            changes.push_back(change);
        }
        
        std::vector<json> getChanges() const { return changes; }
        std::vector<ConfigurationProfile> getProfiles() const { return profiles; }
        
        void addProfile(const ConfigurationProfile& profile) {
            profiles.push_back(profile);
        }
    };
    
    class ConfigValidator {
    public:
        static bool validate(const json& config, const std::vector<ValidationRule>& rules) {
            for (const auto& rule : rules) {
                if (!rule.validate(config)) {
                    return false;
                }
            }
            return true;
        }
        
        static std::vector<std::string> getValidationErrors(const json& config,
                                                           const std::vector<ValidationRule>& rules) {
            std::vector<std::string> errors;
            for (const auto& rule : rules) {
                if (!rule.validate(config)) {
                    errors.push_back("Validation failed for parameter " + rule.parameterName + ": " + rule.errorMessage);
                }
            }
            return errors;
        }
    };
    
    class ParameterValidator {
    public:
        static bool validateParameterType(const json& value, const std::string& type) {
            if (type == "int") return value.is_number_integer();
            if (type == "float") return value.is_number_float();
            if (type == "string") return value.is_string();
            if (type == "bool") return value.is_boolean();
            if (type == "list") return value.is_array();
            if (type == "dict") return value.is_object();
            return false;
        }
        
        static json sanitizeValue(const json& value, const std::string& type) {
            if (!validateParameterType(value, type)) {
                return getDefaultForType(type);
            }
            return value;
        }
        
    private:
        static json getDefaultForType(const std::string& type) {
            if (type == "int") return 0;
            if (type == "float") return 0.0;
            if (type == "string") return "";
            if (type == "bool") return false;
            if (type == "list") return json::array();
            if (type == "dict") return json::object();
            return nullptr;
        }
    };
    
    // Advanced configuration manager
    class AdvancedConfig {
    public:
        std::shared_ptr<Config> config;
        std::vector<ParameterTemplate> parameterTemplates;
        ConfigurationHistory history;
        std::string configFile;
        std::map<std::string, std::function<bool(const json&)>> customValidators;
        
        AdvancedConfig(std::shared_ptr<Config> cfg, const std::string& file = "advanced_config.json")
            : config(cfg), configFile(file) {
            loadParameterTemplates();
            loadFromFile();
        }
        
        // Load parameter templates from file
        void loadParameterTemplates() {
            // Load predefined parameter templates
            parameterTemplates = {
                createParameterTemplate("brain.neuron_count", "int", 
                    "Total number of neurons in the brain", 1000, true, false, 
                    createRangeValidation(100, 10000), "brain"),
                createParameterTemplate("brain.synapse_density", "float",
                    "Synapse connectivity density (0.0-1.0)", 0.1f, false, false,
                    createRangeValidation(0.0, 1.0), "brain"),
                createParameterTemplate("plasticity.stdp.enable", "bool",
                    "Enable Spike-Timing-Dependent Plasticity", true, false, {}, "plasticity"),
                createParameterTemplate("plasticity.stdp.learning_rate", "float",
                    "STDP learning rate", 0.001f, false, false,
                    createRangeValidation(0.0, 0.1), "plasticity"),
                createParameterTemplate("neuromod.dopamine.scale", "float",
                    "Dopamine modulation scale", 1.0f, false, false,
                    createRangeValidation(0.0, 10.0), "neuromodulation"),
                createParameterTemplate("memory.working.capacity", "int",
                    "Working memory capacity (number of items)", 10, false, false,
                    createRangeValidation(1, 100), "memory"),
                createParameterTemplate("development.stage", "string",
                    "Developmental stage", "Adult", false, false,
                    createListValidation({"Initial", "CriticalPeriod", "Maturation", "Adult", "Aging"}), "development"),
            };
        }
        
        // Create parameter template
        ParameterTemplate createParameterTemplate(const std::string& name, const std::string& type,
                                                  const std::string& description, const json& defaultValue,
                                                  bool required, bool readonly,
                                                  const std::vector<ValidationRule>& validationRules,
                                                  const std::string& category) {
            ParameterTemplate template;
            template.name = name;
            template.type = type;
            template.description = description;
            template.defaultValue = defaultValue;
            template.required = required;
            template.readonly = readonly;
            template.validationRules = validationRules;
            template.category = category;
            return template;
        }
        
        // Create range validation rule
        ValidationRule createRangeValidation(double min, double max, const std::string& errorMsg = "Value out of range") {
            ValidationRule rule;
            rule.parameterName = "";
            rule.validationType = "range";
            rule.errorMessage = errorMsg;
            rule.range = {min, max};
            return rule;
        }
        
        // Create list validation rule
        ValidationRule createListValidation(const std::vector<std::string>& values, const std::string& errorMsg = "Value not in allowed list") {
            ValidationRule rule;
            rule.parameterName = "";
            rule.validationType = "list";
            rule.errorMessage = errorMsg;
            rule.allowedValues = values;
            return rule;
        }
        
        // Add custom validator
        void addCustomValidator(const std::string& parameterName,
                               std::function<bool(const json&)> validator,
                               const std::string& description = "") {
            customValidators[parameterName] = validator;
        }
        
        // Validate configuration
        std::vector<std::string> validateConfig(const json& config) {
            std::vector<std::string> errors;
            
            // Check required parameters
            for (const auto& template : parameterTemplates) {
                if (template.isRequired() && !config.contains(template.name)) {
                    errors.push_back("Required parameter missing: " + template.name);
                }
            }
            
            // Validate parameter types and values
            for (const auto& template : parameterTemplates) {
                if (config.contains(template.name)) {
                    const json& value = config[template.name];
                    if (!ParameterValidator::validateParameterType(value, template.type)) {
                        errors.push_back("Invalid type for parameter " + template.name + ": expected " + template.type);
                    }
                    
                    if (!template.validate(config)) {
                        errors.push_back("Validation failed for parameter " + template.name + ": " + template.validationRules[0].errorMessage);
                    }
                }
            }
            
            // Run custom validators
            for (const auto& pair : customValidators) {
                if (config.contains(pair.first)) {
                    if (!pair.second(config)) {
                        errors.push_back("Custom validation failed for parameter " + pair.first);
                    }
                }
            }
            
            return errors;
        }
        
        // Get parameter template for a parameter
        ParameterTemplate getParameterTemplate(const std::string& parameterName) const {
            for (const auto& template : parameterTemplates) {
                if (template.name == parameterName) {
                    return template;
                }
            }
            ParameterTemplate emptyTemplate;
            emptyTemplate.name = parameterName;
            emptyTemplate.type = "unknown";
            emptyTemplate.description = "Unknown parameter";
            emptyTemplate.defaultValue = nullptr;
            emptyTemplate.required = false;
            emptyTemplate.readonly = false;
            return emptyTemplate;
        }
        
        // Get all parameter templates for a category
        std::vector<ParameterTemplate> getParameterTemplatesByCategory(const std::string& category) const {
            std::vector<ParameterTemplate> templates;
            for (const auto& template : parameterTemplates) {
                if (template.category == category) {
                    templates.push_back(template);
                }
            }
            return templates;
        }
        
        // Get all categories
        std::vector<std::string> getCategories() const {
            std::set<std::string> categories;
            for (const auto& template : parameterTemplates) {
                categories.insert(template.category);
            }
            return std::vector<std::string>(categories.begin(), categories.end());
        }
        
        // Set parameter with validation
        bool setParameter(const std::string& parameterName, const json& value,
                         const std::string& user = "system", const std::string& reason = "") {
            if (configFile.empty()) {
                return false;
            }
            
            // Get current value
            json currentValue;
            if (config->has(parameterName)) {
                currentValue = config->getOr(parameterName, nullptr);
            } else {
                // Use template default if available
                auto template = getParameterTemplate(parameterName);
                if (template.defaultValue != nullptr) {
                    currentValue = template.defaultValue;
                }
            }
            
            // Validate new value
            if (!validateParameterValue(parameterName, value)) {
                return false;
            }
            
            // Update configuration
            if (!config->set(parameterName, value)) {
                return false;
            }
            
            // Record change in history
            history.addChange(parameterName, currentValue, value, user, reason);
            
            // Save to file
            saveToFile();
            
            return true;
        }
        
        // Validate parameter value
        bool validateParameterValue(const std::string& parameterName, const json& value) const {
            // Check parameter type
            auto template = getParameterTemplate(parameterName);
            if (!ParameterValidator::validateParameterType(value, template.type)) {
                return false;
            }
            
            // Check validation rules
            if (!template.validate(createJsonFromMap({{parameterName, value}}))) {
                return false;
            }
            
            // Run custom validators
            auto it = customValidators.find(parameterName);
            if (it != customValidators.end() && !it->second(createJsonFromMap({{parameterName, value}}))) {
                return false;
            }
            
            return true;
        }
        
        // Create JSON from parameter map
        json createJsonFromMap(const std::map<std::string, json>& map) const {
            json result;
            for (const auto& pair : map) {
                result[pair.first] = pair.second;
            }
            return result;
        }
        
        // Load configuration from file
        bool loadFromFile() {
            if (configFile.empty()) {
                return false;
            }
            
            std::ifstream inFile(configFile);
            if (!inFile.is_open()) {
                return false;
            }
            
            try {
                json configJson;
                inFile >> configJson;
                
                // Apply configuration
                for (auto it = configJson.begin(); it != configJson.end(); ++it) {
                    config->set(it.key(), *it);
                }
                
                return true;
            } catch (...) {
                return false;
            }
        }
        
        // Save configuration to file
        bool saveToFile() {
            if (configFile.empty()) {
                return false;
            }
            
            try {
                json configJson;
                
                // Get all configuration parameters
                auto keys = config->getKeys();
                for (const auto& key : keys) {
                    configJson[key] = config->getOr(key, nullptr);
                }
                
                std::ofstream outFile(configFile);
                outFile << configJson.dump(2);
                
                return true;
            } catch (...) {
                return false;
            }
        }
        
        // Export configuration as JSON string
        std::string exportToJSON() const {
            json configJson;
            
            auto keys = config->getKeys();
            for (const auto& key : keys) {
                configJson[key] = config->getOr(key, nullptr);
            }
            
            return configJson.dump(2);
        }
        
        // Import configuration from JSON string
        bool importFromJSON(const std::string& jsonString) {
            try {
                json configJson = json::parse(jsonString);
                
                // Validate configuration
                auto errors = validateConfig(configJson);
                if (!errors.empty()) {
                    return false;
                }
                
                // Apply configuration
                for (auto it = configJson.begin(); it != configJson.end(); ++it) {
                    config->set(it.key(), *it);
                }
                
                // Save to file
                saveToFile();
                
                return true;
            } catch (...) {
                return false;
            }
        }
        
        // Create configuration profile
        ConfigurationProfile createProfile(const std::string& name, const std::string& description,
                                          const std::map<std::string, std::string>& tags = {}) {
            ConfigurationProfile profile;
            profile.name = name;
            profile.description = description;
            profile.tags = tags;
            profile.created = std::chrono::system_clock::now();
            profile.modified = profile.created;
            profile.createdBy = "system";
            
            // Get current parameters
            auto keys = config->getKeys();
            for (const auto& key : keys) {
                profile.parameters[key] = config->getOr(key, nullptr);
            }
            
            history.addProfile(profile);
            saveProfileFile(profile);
            
            return profile;
        }
        
        // Save profile to file
        bool saveProfileFile(const ConfigurationProfile& profile) {
            try {
                std::string filename = "profiles/" + profile.name + ".json";
                if (!fs::exists("profiles")) {
                    fs::create_directories("profiles");
                }
                
                std::ofstream outFile(filename);
                
                json profileJson;
                profileJson["name"] = profile.name;
                profileJson["description"] = profile.description;
                profileJson["parameters"] = profile.parameters;
                profileJson["tags"] = profile.tags;
                
                outFile << profileJson.dump(2);
                return true;
            } catch (...) {
                return false;
            }
        }
        
        // Load configuration profile
        ConfigurationProfile loadProfile(const std::string& name) {
            std::string filename = "profiles/" + name + ".json";
            
            if (!fs::exists(filename)) {
                ConfigurationProfile emptyProfile;
                emptyProfile.name = "unknown";
                return emptyProfile;
            }
            
            try {
                std::ifstream inFile(filename);
                json profileJson;
                inFile >> profileJson;
                
                ConfigurationProfile profile;
                profile.name = profileJson["name"];
                profile.description = profileJson["description"];
                profile.parameters = profileJson["parameters"];
                profile.tags = profileJson["tags"];
                profile.created = std::chrono::system_clock::now();
                profile.modified = profile.created;
                profile.createdBy = "system";
                
                // Apply profile
                for (auto it = profile.parameters.begin(); it != profile.parameters.end(); ++it) {
                    config->set(it.key(), it.value());
                }
                
                saveToFile();
                return profile;
            } catch (...) {
                ConfigurationProfile emptyProfile;
                emptyProfile.name = "unknown";
                return emptyProfile;
            }
        }
        
        // Get all configuration profiles
        std::vector<ConfigurationProfile> getAllProfiles() const {
            std::vector<ConfigurationProfile> profiles;
            
            if (fs::exists("profiles")) {
                for (const auto& entry : fs::directory_iterator("profiles")) {
                    if (entry.is_regular_file() && entry.path().extension() == ".json") {
                        profiles.push_back(loadProfile(entry.path().stem().string()));
                    }
                }
            }
            
            return profiles;
        }
        
        // Get configuration history
        ConfigurationHistory getHistory() const { return history; }
        
        // Get configuration statistics
        json getConfigurationStats() const {
            json stats;
            
            auto keys = config->getKeys();
            stats["total_parameters"] = keys.size();
            stats["required_parameters"] = 0;
            
            for (const auto& template : parameterTemplates) {
                if (template.isRequired()) {
                    stats["required_parameters"]++;
                }
            }
            
            stats["categories"] = json::array();
            auto categories = getCategories();
            for (const auto& category : categories) {
                stats["categories"].push_back(category);
            }
            
            stats["history_size"] = history.getChanges().size();
            stats["profiles_count"] = history.getProfiles().size();
            
            return stats;
        }
        
        // Get the underlying config
        std::shared_ptr<Config> getConfig() const { return config; }
        
        // Set config file
        void setConfigFile(const std::string& file) { configFile = file; }
        
        // Get config file
        std::string getConfigFile() const { return configFile; }
    };

void registerAdvancedConfigUtils(py::module_& m) {
    // Advanced configuration manager
    py::class_<AdvancedConfigManager::AdvancedConfig>(m, "AdvancedConfig",
                                                     "Advanced configuration manager with validation and templates")
        .def(py::init<std::shared_ptr<Config>, std::string>(),
             py::arg("config"), py::arg("file") = "advanced_config.json",
             "Initialize advanced configuration manager")
        .def("set_parameter", &AdvancedConfigManager::AdvancedConfig::setParameter,
             py::arg("parameter_name"), py::arg("value"),
             py::arg("user") = "system", py::arg("reason") = "",
             "Set configuration parameter with validation")
        .def("get_parameter_template", &AdvancedConfigManager::AdvancedConfig::getParameterTemplate,
             py::arg("parameter_name"),
             "Get parameter template for parameter name")
        .def("get_parameter_templates_by_category", &AdvancedConfigManager::AdvancedConfig::getParameterTemplatesByCategory,
             py::arg("category"),
             "Get all parameter templates for a category")
        .def("get_categories", &AdvancedConfigManager::AdvancedConfig::getCategories,
             "Get all parameter categories")
        .def("validate_config", &AdvancedConfigManager::AdvancedConfig::validateConfig,
             py::arg("config"),
             "Validate configuration")
        .def("load_from_file", &AdvancedConfigManager::AdvancedConfig::loadFromFile,
             "Load configuration from file")
        .def("save_to_file", &AdvancedConfigManager::AdvancedConfig::saveToFile,
             "Save configuration to file")
        .def("export_to_json", &AdvancedConfigManager::AdvancedConfig::exportToJSON,
             "Export configuration as JSON string")
        .def("import_from_json", &AdvancedConfigManager::AdvancedConfig::importFromJSON,
             py::arg("json_string"),
             "Import configuration from JSON string")
        .def("create_profile", &AdvancedConfigManager::AdvancedConfig::createProfile,
             py::arg("name"), py::arg("description"), py::arg("tags") = std::map<std::string, std::string>(),
             "Create configuration profile")
        .def("load_profile", &AdvancedConfigManager::AdvancedConfig::loadProfile,
             py::arg("name"),
             "Load configuration profile")
        .def("get_all_profiles", &AdvancedConfigManager::AdvancedConfig::getAllProfiles,
             "Get all configuration profiles")
        .def("get_configuration_stats", &AdvancedConfigManager::AdvancedConfig::getConfigurationStats,
             "Get configuration statistics")
        .def("get_config", &AdvancedConfigManager::AdvancedConfig::getConfig,
             "Get the underlying config")
        .def("set_config_file", &AdvancedConfigManager::AdvancedConfig::setConfigFile,
             py::arg("file"),
             "Set configuration file")
        .def("get_config_file", &AdvancedConfigManager::AdvancedConfig::getConfigFile,
             "Get configuration file");
    
    // Parameter template
    py::class_<AdvancedConfigManager::ParameterTemplate>(m, "ParameterTemplate",
                                                        "Template for configuration parameter")
        .def_readonly("name", &AdvancedConfigManager::ParameterTemplate::name)
        .def_readonly("type", &AdvancedConfigManager::ParameterTemplate::type)
        .def_readonly("description", &AdvancedConfigManager::ParameterTemplate::description)
        .def_readonly("default_value", &AdvancedConfigManager::ParameterTemplate::defaultValue)
        .def_readonly("required", &AdvancedConfigManager::ParameterTemplate::required)
        .def_readonly("readonly", &AdvancedConfigManager::ParameterTemplate::readonly)
        .def_readonly("category", &AdvancedConfigManager::ParameterTemplate::category)
        .def("is_required", &AdvancedConfigManager::ParameterTemplate::isRequired,
             "Check if parameter is required")
        .def("is_readonly", &AdvancedConfigManager::ParameterTemplate::isReadonly,
             "Check if parameter is readonly");
    
    // Validation rule
    py::class_<AdvancedConfigManager::ValidationRule>(m, "ValidationRule",
                                                     "Rule for validating configuration parameters")
        .def_readonly("validation_type", &AdvancedConfigManager::ValidationRule::validationType)
        .def_readonly("error_message", &AdvancedConfigManager::ValidationRule::errorMessage);
    
    // Configuration profile
    py::class_<AdvancedConfigManager::ConfigurationProfile>(m, "ConfigurationProfile",
                                                         "Configuration profile")
        .def_readonly("name", &AdvancedConfigManager::ConfigurationProfile::name)
        .def_readonly("description", &AdvancedConfigManager::ConfigurationProfile::description)
        .def_readonly("parameters", &AdvancedConfigManager::ConfigurationProfile::parameters)
        .def_readonly("tags", &AdvancedConfigManager::ConfigurationProfile::tags);
    
    // Configuration history
    py::class_<AdvancedConfigManager::ConfigurationHistory>(m, "ConfigurationHistory",
                                                         "History of configuration changes")
        .def("get_changes", &AdvancedConfigManager::ConfigurationHistory::getChanges,
             "Get configuration changes")
        .def("get_profiles", &AdvancedConfigManager::ConfigurationHistory::getProfiles,
             "Get configuration profiles");
    
    // Configuration validator
    py::class_<AdvancedConfigManager::ConfigValidator>(m, "ConfigValidator",
                                                     "Utility class for configuration validation")
        .def_static("validate", &AdvancedConfigManager::ConfigValidator::validate,
                   py::arg("config"), py::arg("rules"),
                   "Validate configuration with rules")
        .def_static("get_validation_errors", &AdvancedConfigManager::ConfigValidator::getValidationErrors,
                   py::arg("config"), py::arg("rules"),
                   "Get validation errors for configuration")
        .def_static("validate_parameter_type", &AdvancedConfigManager::ParameterValidator::validateParameterType,
                   py::arg("value"), py::arg("type"),
                   "Validate parameter type");
    
    // Convenience functions for advanced configuration
    m.def("create_advanced_config", [](std::shared_ptr<Config> config, const std::string& file) {
        return AdvancedConfigManager::AdvancedConfig(config, file);
    }, py::arg("config"), py::arg("file") = "advanced_config.json",
       "Create advanced configuration manager (convenience function)");

    m.def("create_parameter_template", [](const std::string& name, const std::string& type,
                                          const std::string& description, const json& defaultValue,
                                          bool required, bool readonly,
                                          const std::vector<AdvancedConfigManager::ValidationRule>& validationRules,
                                          const std::string& category) {
        AdvancedConfigManager::ParameterTemplate template;
        template.name = name;
        template.type = type;
        template.description = description;
        template.defaultValue = defaultValue;
        template.required = required;
        template.readonly = readonly;
        template.validationRules = validationRules;
        template.category = category;
        return template;
    }, py::arg("name"), py::arg("type"), py::arg("description"), py::arg("default_value"),
       py::arg("required"), py::arg("readonly"), py::arg("validation_rules"),
       py::arg("category") = "general",
       "Create parameter template (convenience function)");

    // Range validation helper
    m.def("create_range_validation", [](double min, double max, const std::string& errorMsg) {
        AdvancedConfigManager::ValidationRule rule;
        rule.validationType = "range";
        rule.errorMessage = errorMsg;
        rule.range = {min, max};
        return rule;
    }, py::arg("min"), py::arg("max"), py::arg("error_msg") = "Value out of range",
       "Create range validation rule (convenience function)");

    // List validation helper
    m.def("create_list_validation", [](const std::vector<std::string>& values, const std::string& errorMsg) {
        AdvancedConfigManager::ValidationRule rule;
        rule.validationType = "list";
        rule.errorMessage = errorMsg;
        rule.allowedValues = values;
        return rule;
    }, py::arg("values"), py::arg("error_msg") = "Value not in allowed list",
       "Create list validation rule (convenience function)");

    // Configuration export/import helpers
    m.def("export_config_to_json", [](std::shared_ptr<Config> config) {
        AdvancedConfigManager::AdvancedConfig advConfig(config);
        return advConfig.exportToJSON();
    }, py::arg("config"),
       "Export configuration to JSON string (convenience function)");

    m.def("import_config_from_json", [](std::shared_ptr<Config> config, const std::string& jsonString) {
        AdvancedConfigManager::AdvancedConfig advConfig(config);
        return advConfig.importFromJSON(jsonString);
    }, py::arg("config"), py::arg("json_string"),
       "Import configuration from JSON string (convenience function)");
}
