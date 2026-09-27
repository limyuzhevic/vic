#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <optional>
#include <vector>
#include "../core/Types/Types.hpp"
#include "PlasticityRule.hpp"
#include "STDP.hpp"
#include "Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "AntiHebbianRule.hpp"
#include "BCMRule.hpp"

namespace nlm {

// PlasticityRule factory class with plugin architecture
class PlasticityRuleFactory {
public:
    // Factory function type for plasticity rule creation
    using PlasticityRuleCreator = std::function<std::unique_ptr<PlasticityRule>(std::shared_ptr<Config>, TimestepDuration)>;
    
    // Plugin registration function type
    using PluginRegistration = std::function<void(PlasticityRuleFactory&)>;
    
    // Plasticity rule type information
    struct PlasticityRuleInfo {
        std::string name;
        std::string description;
        std::unordered_map<std::string, std::string> configOptions;
        bool isBuiltIn;
        bool isStub;
    };
    
    // Create a plasticity rule instance based on type name and configuration
    static std::unique_ptr<PlasticityRule> create(const std::string& ruleType, 
                                                  std::shared_ptr<Config> config,
                                                  TimestepDuration timestep = 0.001);
    
    // Get all available plasticity rule types
    static std::vector<PlasticityRuleInfo> getAvailableTypes();
    
    // Check if a plasticity rule type is available
    static bool hasType(const std::string& ruleType);
    
    // Check if a plasticity rule type is a stub (placeholder)
    static bool isStub(const std::string& ruleType);
    
    // Register a built-in plasticity rule creator
    static void registerBuiltInCreator(const std::string& typeName, 
                                       PlasticityRuleCreator creator,
                                       const std::string& description,
                                       bool isStub = false);
    
    // Register a plugin
    static void registerPlugin(const std::string& pluginName, PluginRegistration registrationFunc);
    
    // Load all built-in plasticity rules
    static void loadBuiltInPlasticityRules();
    
    // Initialize plugin system
    static void initializePlugins();
    
    // Graceful fallback for missing implementations
    static std::unique_ptr<PlasticityRule> createWithFallback(const std::string& ruleType,
                                                              std::shared_ptr<Config> config,
                                                              TimestepDuration timestep);
    
private:
    static std::unordered_map<std::string, PlasticityRuleCreator> builtInCreators_;
    static std::vector<PlasticityRuleInfo> builtInTypes_;
    static std::mutex mutex_;
    static std::unordered_set<std::string> loadedPlugins_;
    
    // Thread-safe registration helper
    static void registerCreator(const std::string& typeName, 
                                PlasticityRuleCreator creator,
                                const std::string& description,
                                bool isStub = false);
    
    // Create default stub implementation
    static std::unique_ptr<PlasticityRule> createStub(const std::string& typeName, 
                                                      std::shared_ptr<Config> config);
};

} // namespace nlm

