#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::string details;
};

// Template parameter
struct TemplateParameter {
    std::string name;
    ConfigValue defaultValue;
    std::string type;  // "int", "float", "string", "bool", "vector"
    std::string description;
    std::vector<std::string> allowedValues;  // For string parameters
    bool required;
    std::string validationRegex;  // For string parameters
    std::string units;
    std::pair<double, double> range;  // For numeric parameters
};

// Configuration template for neural network setup
struct ConfigurationTemplate {
    std::string name;
    std::string description;
    std::string category;  // "research", "production", "benchmark", "experimental"
    std::vector<TemplateParameter> parameters;
    std::string defaultScenario;  // "small", "medium", "large", "custom"
    std::string applicationArea;  // "cognitive", "sensory", "motor", "integrative"
    float estimatedMemoryUsage;  // MB
    double estimatedSimulationTime;  // seconds per step
    bool isBuiltIn;
    std::string author;
    std::chrono::system_clock::time_point creationDate;
    std::chrono::system_clock::time_point lastModified;
};

// Template validation result
struct TemplateValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::vector<std::string> suggestedFixes;
};

// Scenario configuration for templates
struct ScenarioConfig {
    std::string name;
    std::string description;
    std::map<std::string, ConfigValue> parameterValues;
    float memoryFootprint;  // MB
    float expectedPerformance;  // Performance score (0-1)
    std::string useCase;  // "debugging", "production", "research", "benchmark"
};

// ConfigurationTemplateManager: Advanced configuration management for power users
class ConfigurationTemplateManager {
public:
    ConfigurationTemplateManager(std::shared_ptr<Config> config);
    ~ConfigurationTemplateManager();
    
    // Template management
    bool createTemplate(const ConfigurationTemplate& templateConfig);
    bool updateTemplate(const ConfigurationTemplate& templateConfig);
    bool deleteTemplate(const std::string& name);
    bool loadTemplate(const std::string& name, ConfigurationTemplate& templateConfig) const;
    bool saveTemplate(const ConfigurationTemplate& templateConfig);
    
    // Template discovery and listing
    std::vector<std::string> listTemplates(const std::string& category = "") const;
    std::vector<ConfigurationTemplate> getTemplates(const std::string& category = "") const;
    ConfigurationTemplate getTemplate(const std::string& name) const;
    bool templateExists(const std::string& name) const;
    
    // Template validation
    TemplateValidationResult validateTemplate(const ConfigurationTemplate& templateConfig) const;
    ValidationResult validateConfig(const Config& config) const;
    
    // Template application
    std::shared_ptr<Config> applyTemplate(const std::string& templateName);
    std::shared_ptr<Config> applyTemplate(const ConfigurationTemplate& templateConfig);
    std::shared_ptr<Config> applyScenario(const std::string& templateName, 
                                         const std::string& scenarioName);
    
    // Template management
    void addBuiltInTemplate(const std::string& name);
    void removeBuiltInTemplate(const std::string& name);
    bool isBuiltInTemplate(const std::string& name) const;
    
    // Parameter management
    bool addParameterToTemplate(const std::string& templateName, 
                               const TemplateParameter& parameter);
    bool updateTemplateParameter(const std::string& templateName,
                                const std::string& parameterName,
                                const TemplateParameter& newParameter);
    bool removeTemplateParameter(const std::string& templateName,
                                const std::string& parameterName);
    
    // Advanced parameter validation
    bool validateParameter(const TemplateParameter& parameter, const ConfigValue& value) const;
    std::vector<std::string> getParameterValidationErrors(const TemplateParameter& parameter,
                                                         const ConfigValue& value) const;
    
    // Dynamic configuration updates
    void enableDynamicUpdates(bool enable);
    bool isDynamicUpdatesEnabled() const;
    void updateTemplateParameters(const std::string& templateName,
                                 const std::map<std::string, ConfigValue>& updates);
    
    // Configuration templates for common scenarios
    void createResearchTemplates();
    void createProductionTemplates();
    void createBenchmarkTemplates();
    void createExperimentalTemplates();
    
    // Template analytics
    std::string generateTemplateReport(const std::string& templateName) const;
    std::string generateCategoryReport(const std::string& category) const;
    std::string generateUsageStatistics() const;
    
    // Integration
    void attachToConfig(std::shared_ptr<Config> config);
    void detachFromConfig();
    bool isAttached() const;
    
    // Export and import
    bool exportTemplates(const std::string& filepath, 
                        const std::vector<std::string>& templateNames = {});
    bool importTemplates(const std::string& filepath);
    bool exportTemplateSet(const std::string& filepath,
                          const std::string& category = "");
    bool importTemplateSet(const std::string& filepath);
    
    // Configuration comparison
    bool compareConfigs(const std::shared_ptr<Config>& config1,
                       const std::shared_ptr<Config>& config2,
                       std::vector<std::string>& differences) const;
    std::string generateDiffReport(const std::shared_ptr<Config>& config1,
                                  const std::shared_ptr<Config>& config2) const;
    
    // Validation and analysis
    std::vector<std::string> analyzeTemplateDependencies(const std::string& templateName) const;
    std::vector<std::string> checkTemplateConflicts(const std::string& templateName) const;
    
    // Cleanup
    void cleanup() noexcept;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
