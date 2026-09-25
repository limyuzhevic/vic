#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <variant>
#include "Config.hpp"
#include "ConfigValidator.hpp"
#include "ConfigLoader.hpp"
#include "ConfigManager.hpp"

namespace nlm {

class ConfigBuilder {
public:
    // Configuration parameter definition
    struct Parameter {
        std::string key;
        ConfigValue defaultValue;
        ConfigSource source;
        std::string description;
        std::string category; // e.g., "simulation", "plasticity", "neuromodulation"
        bool required;
        std::string validationRule; // Optional validation expression
        
        Parameter() : source(ConfigSource::Default), required(false) {}
        Parameter(const std::string& k, const ConfigValue& v, const std::string& desc = "")
            : key(k), defaultValue(v), source(ConfigSource::Default), description(desc), category("general"), required(false) {}
    };
    
    // Configuration group for organizing related parameters
    struct ParameterGroup {
        std::string name;
        std::vector<Parameter> parameters;
        std::string description;
        
        ParameterGroup() = default;
        ParameterGroup(const std::string& n, const std::string& desc = "")
            : name(n), description(desc) {}
    };
    
    ConfigBuilder();
    ~ConfigBuilder() = default;
    
    // Add a parameter to the configuration
    void addParameter(const Parameter& param);
    
    // Add a parameter with type helpers
    void addParameter(const std::string& key, int value, const std::string& description = "");
    void addParameter(const std::string& key, int64_t value, const std::string& description = "");
    void addParameter(const std::string& key, double value, const std::string& description = "");
    void addParameter(const std::string& key, bool value, const std::string& description = "");
    void addParameter(const std::string& key, const std::string& value, const std::string& description = "");
    void addParameter(const std::string& key, const std::vector<int>& value, const std::string& description = "");
    void addParameter(const std::string& key, const std::vector<double>& value, const std::string& description = "");
    void addParameter(const std::string& key, const std::vector<std::string>& value, const std::string& description = "");
    
    // Add a parameter group
    void addGroup(const ParameterGroup& group);
    
    // Add a group with type helpers
    void addSimulationGroup();
    void addPlasticityGroup();
    void addNeuromodulationGroup();
    void addDevelopmentGroup();
    void addMemoryGroup();
    void addMotorGroup();
    void addRewardGroup();
    void addEnvironmentGroup();
    void addLoggingGroup();
    void addVisualizationGroup();
    
    // Build configuration from parameters
    std::shared_ptr<Config> buildConfig() const;
    
    // Build configuration with custom values
    std::shared_ptr<Config> buildConfig(const std::map<std::string, ConfigValue>& customValues) const;
    
    // Build configuration and apply to manager
    bool buildAndApply(ConfigManager& manager) const;
    
    // Get all parameters
    const std::vector<Parameter>& getParameters() const { return parameters_; }
    
    // Get all groups
    const std::vector<ParameterGroup>& getGroups() const { return groups_; }
    
    // Get parameter by key
    const Parameter* getParameter(const std::string& key) const;
    
    // Get group by name
    const ParameterGroup* getGroup(const std::string& groupName) const;
    
    // Export configuration to JSON string
    std::string exportToJson() const;
    
    // Import configuration from JSON string
    bool importFromJson(const std::string& jsonString);
    
    // Export configuration to YAML string
    std::string exportToYaml() const;
    
    // Import configuration from YAML string
    bool importFromYaml(const std::string& yamlString);
    
    // Get parameter categories
    std::vector<std::string> getCategories() const;
    
    // Get parameters in a category
    std::vector<Parameter> getParametersInCategory(const std::string& category) const;
    
private:
    std::vector<Parameter> parameters_;
    std::vector<ParameterGroup> groups_;
    
    // Helper methods
    void categorizeParameter(Parameter& param);
    ConfigValue convertValue(const std::string& type, const std::string& value) const;
    std::string valueToString(const ConfigValue& value) const;
    
    // Group definitions
    void defineDefaultGroups();
    void defineSimulationGroup();
    void definePlasticityGroup();
    void defineNeuromodulationGroup();
    void defineDevelopmentGroup();
    void defineMemoryGroup();
    void defineMotorGroup();
    void defineRewardGroup();
    void defineEnvironmentGroup();
    void defineLoggingGroup();
    void defineVisualizationGroup();
};

} // namespace nlm