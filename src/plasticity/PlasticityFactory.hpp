#pragma once

#include "PlasticityRule.hpp"
#include "STDP.hpp"
#include "Hebbian.hpp"
#include "RewardModulatedSTDP.hpp"
#include "TripletSTDP.hpp"
#include "PlasticityParameters.hpp"
#include "../../core/Config/Config.hpp"
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include <vector>

namespace nlm {

// Factory for creating plasticity rule instances
// Improves extensibility by allowing new plasticity rules to be registered
// without modifying core code

class PlasticityFactory {
public:
    // Factory function type for plasticity rule creators
    using PlasticityRuleFactory = std::function<std::unique_ptr<PlasticityRule>()>;
    
    // Configuration key for plasticity rule name
    static constexpr const char* CONFIG_RULE_NAME = "plasticity.rule_name";
    
    // Configuration key for plasticity rule parameters
    static constexpr const char* CONFIG_RULE_PARAMS = "plasticity.params";
    
    // Configuration key for enabling/disabling plasticity
    static constexpr const char* CONFIG_PLATIVITY_ENABLED = "plasticity.enabled";
    
    // Configuration key for enabling specific plasticity mechanisms
    static constexpr const char* CONFIG_HEBBIAN_ENABLED = "plasticity.hebbian_enabled";
    static constexpr const char* CONFIG_STDP_ENABLED = "plasticity.stdp_enabled";
    static constexpr const char* CONFIG_REWARD_MODULATED_ENABLED = "plasticity.reward_modulated_enabled";
    static constexpr const char* CONFIG_TRIPLET_ENABLED = "plasticity.triplet_enabled";
    
public:
    // Register a plasticity rule factory by name
    static void registerPlasticityRule(const std::string& name, 
                                       PlasticityRuleFactory factory);
    
    // Create a plasticity rule by name
    static std::unique_ptr<PlasticityRule> createPlasticityRule(const std::string& name);
    
    // Check if a plasticity rule type exists
    static bool hasPlasticityRule(const std::string& name);
    
    // Get list of available plasticity rule names
    static std::vector<std::string> getAvailableRuleNames();
    
    // Create plasticity rule from configuration
    static std::unique_ptr<PlasticityRule> createFromConfig(const Config& config);
    
    // Create plasticity rule with parameters
    static std::unique_ptr<PlasticityRule> createWithParams(const std::string& name,
                                                           const PlasticityParameters& params);
    
    // Get default parameters for a plasticity rule type
    static PlasticityParameters getDefaultParameters(const std::string& name);
    
    // Validate plasticity rule parameters
    static bool validateParameters(const std::string& name, 
                                   const PlasticityParameters& params);
    
    // Create configured plasticity rule based on configuration
    static std::unique_ptr<PlasticityRule> createConfiguredRule(const Config& config);
    
    // Create plasticity rule system from configuration (returns multiple rules)
    static std::vector<std::unique_ptr<PlasticityRule>> createPlasticitySystem(const Config& config);
    
    // Get enabled plasticity rule names from configuration
    static std::vector<std::string> getEnabledRuleNames(const Config& config);
    
    // Check if specific plasticity mechanism is enabled
    static bool isHebbianEnabled(const Config& config);
    static bool isSTDPEnabled(const Config& config);
    static bool isRewardModulatedEnabled(const Config& config);
    static bool isTripletEnabled(const Config& config);
    
    // Apply configuration to existing plasticity rule
    static bool applyConfigToRule(std::unique_ptr<PlasticityRule>& rule, 
                                 const Config& config);
    
    // Get configuration template for a plasticity rule type
    static PlasticityParameters getConfigurationTemplate(const std::string& ruleName);
    
    // Save configuration for a plasticity rule type
    static bool saveRuleConfiguration(const std::string& ruleName,
                                     const PlasticityParameters& params,
                                     const std::string& filepath);
    
    // Load configuration for a plasticity rule type
    static PlasticityParameters loadRuleConfiguration(const std::string& ruleName,
                                                     const std::string& filepath);
    
private:
    // Registry mapping rule names to factory functions
    static std::unordered_map<std::string, PlasticityRuleFactory>& getRegistry();
    
    // Default configuration values
    static const PlasticityParameters& getDefaultSTDPConfig();
    static const PlasticityParameters& getDefaultHebbianConfig();
    static const PlasticityParameters& getDefaultR_STDPConfig();
    static const PlasticityParameters& getDefaultTripletSTDPConfig();
    
    // Configuration parameter names for each rule type
    static const std::vector<std::string>& getSTDPConfigKeys();
    static const std::vector<std::string>& getHebbianConfigKeys();
    static const std::vector<std::string>& getR_STDPConfigKeys();
    static const std::vector<std::string>& getTripletSTDPConfigKeys();
};

// Helper function to register built-in plasticity rules
inline void registerBuiltInPlasticityRules() {
    // Register STDP
    PlasticityFactory::registerPlasticityRule("STDP", []() {
        return std::make_unique<STDP>();
    });
    
    // Register Hebbian
    PlasticityFactory::registerPlasticityRule("Hebbian", []() {
        return std::make_unique<Hebbian>();
    });
    
    // Register Reward-Modulated STDP
    PlasticityFactory::registerPlasticityRule("R-STDP", []() {
        return std::make_unique<RewardModulatedSTDP>();
    });
    
    // Register Triplet STDP
    PlasticityFactory::registerPlasticityRule("TripletSTDP", []() {
        return std::make_unique<TripletSTDP>();
    });
}

// Convenience functions for common use cases
inline std::unique_ptr<PlasticityRule> createDefaultSTDP() {
    return std::make_unique<STDP>();
}

inline std::unique_ptr<PlasticityRule> createDefaultHebbian() {
    return std::make_unique<Hebbian>();
}

inline std::unique_ptr<PlasticityRule> createDefaultR_STDP() {
    return std::make_unique<RewardModulatedSTDP>();
}

inline std::unique_ptr<PlasticityRule> createDefaultTripletSTDP() {
    return std::make_unique<TripletSTDP>();
}

inline std::unique_ptr<PlasticityRule> createPlasticityRuleByName(const std::string& name) {
    return PlasticityFactory::createPlasticityRule(name);
}

inline bool isValidPlasticityRule(const std::string& name) {
    return PlasticityFactory::hasPlasticityRule(name);
}

inline std::vector<std::string> getAvailablePlasticityRules() {
    return PlasticityFactory::getAvailableRuleNames();
}

inline std::unique_ptr<PlasticityRule> createFromConfig(const Config& config) {
    return PlasticityFactory::createFromConfig(config);
}

inline std::vector<std::unique_ptr<PlasticityRule>> createPlasticitySystem(const Config& config) {
    return PlasticityFactory::createPlasticitySystem(config);
}

inline bool isPlasticityEnabled(const Config& config) {
    return config.getOr<bool>(PlasticityFactory::CONFIG_PLATIVITY_ENABLED, true);
}

inline bool isHebbianEnabled(const Config& config) {
    return config.getOr<bool>(PlasticityFactory::CONFIG_HEBBIAN_ENABLED, true);
}

inline bool isSTDPEnabled(const Config& config) {
    return config.getOr<bool>(PlasticityFactory::CONFIG_STDP_ENABLED, true);
}

inline bool isRewardModulatedEnabled(const Config& config) {
    return config.getOr<bool>(PlasticityFactory::CONFIG_REWARD_MODULATED_ENABLED, true);
}

inline bool isTripletEnabled(const Config& config) {
    return config.getOr<bool>(PlasticityFactory::CONFIG_TRIPLET_ENABLED, true);
}

} // namespace nlm