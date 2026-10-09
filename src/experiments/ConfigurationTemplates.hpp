#pragma once

#include "../core/Config/Config.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>

namespace nlm {

/**
 * Configuration Templates
 * Preset configurations for different experimental scenarios
 */
class ConfigurationTemplates {
public:
    struct Template {
        std::string name;                    // Template name
        std::string description;             // Template description
        std::map<std::string, std::any> parameters; // Template parameters
        bool advancedFeatures;               // Whether template enables advanced features
        std::vector<std::string> requiredPlugins; // Plugins required for this template
        std::map<std::string, std::any> defaultParameters; // Default parameters
        std::map<std::string, std::function<void(std::map<std::string, std::any>&)>> applyFunction; // Apply function
        bool validated;                      // Whether template is validated
    };
    
    ConfigurationTemplates();
    ~ConfigurationTemplates() = default;
    
    // Register a template
    void registerTemplate(const Template& template);
    
    // Get a template by name
    const Template* getTemplate(const std::string& name) const;
    
    // Get all available templates
    std::vector<std::string> getAvailableTemplates() const;
    
    // Create a configuration from a template
    std::shared_ptr<Config> createConfig(const std::string& templateName,
                                         const std::map<std::string, std::any>& overrides = {}) const;
    
    // Validate a configuration against a template
    bool validateConfig(const std::shared_ptr<Config>& config,
                       const Template& template) const;
    
    // Get default templates
    static Template getDefaultTemplate() {
        Template t;
        t.name = "Default";
        t.description = "Standard NLM configuration with basic features";
        t.advancedFeatures = false;
        t.validated = true;
        t.parameters = {
            {"neuron_count", 1000},
            {"region_count", 1},
            {"simulation_timestep", 0.001},
            {"random_seed", 42}
        };
        t.defaultParameters = t.parameters;
        return t;
    }
    
    static Template getAdvancedTemplate() {
        Template t;
        t.name = "Advanced";
        t.description = "Advanced configuration with enhanced features";
        t.advancedFeatures = true;
        t.validated = true;
        t.parameters = {
            {"neuron_count", 5000},
            {"region_count", 4},
            {"simulation_timestep", 0.0001},
            {"random_seed", 42},
            {"enable_advanced_plasticity", true},
            {"enable_multiple_modulators", true},
            {"enable_memory_replay", true},
            {"enable_structural_plasticity", true},
            {"enable_custom_neuron_models", true},
            {"enable_dynamic_stdp", true}
        };
        t.defaultParameters = t.parameters;
        return t;
    }
    
    static Template getResearchTemplate() {
        Template t;
        t.name = "Research";
        t.description = "Research configuration with maximum flexibility";
        t.advancedFeatures = true;
        t.validated = true;
        t.parameters = {
            {"neuron_count", 20000},
            {"region_count", 8},
            {"simulation_timestep", 0.00001},
            {"random_seed", 42},
            {"enable_advanced_plasticity", true},
            {"enable_multiple_modulators", true},
            {"enable_memory_replay", true},
            {"enable_structural_plasticity", true},
            {"enable_parallel_processing", true},
            {"enable_custom_neuron_models", true},
            {"enable_dynamic_stdp", true},
            {"enable_cross_modal_memory", true},
            {"enable_advanced_reward_functions", true},
            {"enable_performance_monitoring", true}
        };
        t.defaultParameters = t.parameters;
        return t;
    }
    
    // Template validation
    bool validateTemplate(const Template& template) const;
    
    // Parameter validation
    bool validateParameters(const std::map<std::string, std::any>& parameters,
                           const Template& template) const;
    
    // Apply template parameters
    void applyTemplateParameters(std::shared_ptr<Config>& config,
                                 const Template& template) const;
    
private:
    std::map<std::string, Template> templates_;
    
    // Parameter validation
    bool validateParameter(const std::string& name,
                          const std::any& value,
                          const Template& template) const;
    
    // Type checking utilities
    template<typename T>
    bool checkType(const std::any& value) const {
        return value.type() == typeid(T);
    }
};

} // namespace nlm
