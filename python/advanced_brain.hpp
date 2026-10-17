#pragma once

// Advanced brain configuration for NLM
// Provides programmatic configuration creation with schema validation,
// configuration presets and templates, runtime configuration modification,
// and configuration inheritance and merging

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <functional>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {
namespace python {

// Configuration schema validation
struct ConfigurationSchema {
    std::string name;
    std::string description;
    std::map<std::string, std::string> fieldTypes;
    std::map<std::string, std::pair<float, float>> ranges;
    std::map<std::string, std::string> patterns;
    std::vector<std::string> requiredFields;
    std::vector<std::string> optionalFields;
    bool isValidated;
    std::string version;
};

// Configuration template
struct ConfigurationTemplate {
    std::string name;
    std::string description;
    std::map<std::string, std::variant<int, float, bool, std::string>> parameters;
    std::vector<std::string> inheritanceChain;
    bool isActive;
    double priority;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point lastModified;
};

// Runtime configuration modification context
struct ModificationContext {
    std::string key;
    std::variant<int, float, bool, std::string> oldValue;
    std::variant<int, float, bool, std::string> newValue;
    std::chrono::system_clock::time_point timestamp;
    std::string modifier;
    std::string reason;
    bool isValidated;
};

// Advanced brain configuration class
class AdvancedBrainConfiguration {
public:
    // Programmatic configuration creation with schema validation
    static std::shared_ptr<Config> createConfiguration(const std::map<std::string, std::variant<int, float, bool, std::string>>& params);
    static std::shared_ptr<Config> createConfigurationFromJSON(const std::string& jsonString);
    static std::shared_ptr<Config> createConfigurationFromFile(const std::string& filepath);
    static std::shared_ptr<Config> createConfigurationFromTemplate(const std::string& templateName);
    
    // Configuration presets and templates
    static bool registerPreset(const std::string& name, const std::string& description, 
                               const std::map<std::string, std::variant<int, float, bool, std::string>>& parameters);
    static bool applyPreset(const std::string& presetName);
    static bool applyPresetToConfig(const std::string& presetName, std::shared_ptr<Config> config);
    static bool removePreset(const std::string& presetName);
    static std::vector<std::string> getAvailablePresets();
    static std::map<std::string, std::string> getPresetInfo(const std::string& presetName);
    static bool registerTemplate(const ConfigurationTemplate& templateData);
    static ConfigurationTemplate getTemplate(const std::string& templateName);
    static std::vector<ConfigurationTemplate> getAllTemplates();
    
    // Runtime configuration modification
    static bool modifyParameter(const std::string& key, const std::variant<int, float, bool, std::string>& value);
    static bool modifyParameter(std::shared_ptr<Config> config, const std::string& key, 
                               const std::variant<int, float, bool, std::string>& value);
    static std::vector<ModificationContext> getModificationHistory(size_t limit = 100);
    static bool validateModification(const std::string& key, const std::variant<int, float, bool, std::string>& value);
    static std::map<std::string, std::variant<int, float, bool, std::string>> getModificationConstraints();
    
    // Configuration inheritance and merging
    static std::shared_ptr<Config> mergeConfigurations(const std::vector<std::shared_ptr<Config>>& configs);
    static std::shared_ptr<Config> inheritConfiguration(const std::shared_ptr<Config>& baseConfig, 
                                                       const std::map<std::string, std::variant<int, float, bool, std::string>>& overrides);
    static std::shared_ptr<Config> inheritConfiguration(const std::string& baseConfigName,
                                                       const std::map<std::string, std::variant<int, float, bool, std::string>>& overrides);
    static bool setInheritanceChain(const std::vector<std::string>& configNames);
    static std::vector<std::string> getInheritanceChain();
    static void clearInheritanceChain();
    
    // Schema validation
    static bool validateConfiguration(const std::shared_ptr<Config>& config);
    static bool validateConfigurationAgainstSchema(const std::shared_ptr<Config>& config, 
                                                  const ConfigurationSchema& schema);
    static ConfigurationSchema getConfigurationSchema(const std::string& schemaName = "default");
    static std::vector<ConfigurationSchema> getAvailableSchemas();
    static bool updateSchema(const ConfigurationSchema& schema);
    static std::map<std::string, std::variant<int, float, bool, std::string>> getConfigurationMetadata();
    
    // Export and import
    static bool exportConfiguration(const std::string& filepath, const std::string& format = "json");
    static std::shared_ptr<Config> importConfiguration(const std::string& filepath, const std::string& format = "auto");
    static std::string exportConfigurationToString(const std::string& format = "json");
    static bool importConfigurationFromString(const std::string& data, const std::string& format = "auto");
    
    // Configuration analysis
    static std::map<std::string, std::variant<int, float, bool, std::string>> analyzeConfiguration(const std::shared_ptr<Config>& config);
    static std::vector<std::string> getUsedParameters(const std::shared_ptr<Config>& config);
    static std::vector<std::string> getUnusedParameters(const std::shared_ptr<Config>& config);
    static double calculateConfigurationCompatibility(const std::shared_ptr<Config>& config1, const std::shared_ptr<Config>& config2);
    
    // Configuration management
    static void clearConfiguration();
    static bool isConfigurationLoaded();
    static std::shared_ptr<Config> getCurrentConfiguration();
    static std::shared_ptr<Config> getDefaultConfiguration();
    static size_t getConfigurationSize(const std::shared_ptr<Config>& config);
    
    // Statistics and monitoring
    static std::map<std::string, double> getConfigurationStatistics();
    static std::vector<std::pair<std::string, double>> getParameterUsage();
    static double getConfigurationValidityScore(const std::shared_ptr<Config>& config);
    
private:
    static std::map<std::string, std::shared_ptr<Config>> s_presets;
    static std::map<std::string, ConfigurationTemplate> s_templates;
    static std::map<std::string, ConfigurationSchema> s_schemas;
    static std::vector<ModificationContext> s_modificationHistory;
    static std::vector<std::string> s_inheritanceChain;
    static std::shared_ptr<Config> s_currentConfig;
    static std::mutex s_mutex;
};

} // namespace python
} // namespace nlm