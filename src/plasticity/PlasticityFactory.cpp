#include "PlasticityFactory.hpp"
#include "PlasticityParameters.hpp"
#include "core/Config/Config.hpp"
#include <stdexcept>
#include <fstream>
#include <nlohmann/json.hpp> // Assuming nlohmann/json is available for JSON serialization

namespace nlm {

// Registry implementation
std::unordered_map<std::string, PlasticityFactory::PlasticityRuleFactory>& 
PlasticityFactory::getRegistry() {
    static std::unordered_map<std::string, PlasticityRuleFactory> registry;
    return registry;
}

void PlasticityFactory::registerPlasticityRule(const std::string& name,
                                             PlasticityRuleFactory factory) {
    if (factory == nullptr) {
        throw std::invalid_argument("Factory function cannot be null");
    }
    getRegistry()[name] = std::move(factory);
}

std::unique_ptr<PlasticityRule> PlasticityFactory::createPlasticityRule(const std::string& name) {
    auto& registry = getRegistry();
    auto it = registry.find(name);
    if (it == registry.end()) {
        throw std::runtime_error("Unknown plasticity rule: " + name);
    }
    return it->second();
}

bool PlasticityFactory::hasPlasticityRule(const std::string& name) {
    return getRegistry().find(name) != getRegistry().end();
}

std::vector<std::string> PlasticityFactory::getAvailableRuleNames() {
    std::vector<std::string> names;
    for (const auto& pair : getRegistry()) {
        names.push_back(pair.first);
    }
    return names;
}

std::unique_ptr<PlasticityRule> PlasticityFactory::createFromConfig(const Config& config) {
    std::string ruleName = config.getOr<std::string>("plasticity.rule_name", "STDP");
    return createWithParams(ruleName, config.getOr<PlasticityParameters>("plasticity.params", {}));
}

std::unique_ptr<PlasticityRule> PlasticityFactory::createWithParams(const std::string& name,
                                                                  const PlasticityParameters& params) {
    if (!validateParameters(name, params)) {
        throw std::invalid_argument("Invalid parameters for plasticity rule: " + name);
    }
    
    auto rule = createPlasticityRule(name);
    
    // Apply parameters based on rule type (in a real implementation, this would be more sophisticated)
    if (name == "STDP" || name == "R-STDP") {
        float ltpWeight = params.getFloat("ltp_weight", 0.01f);
        float ltdWeight = params.getFloat("ltd_weight", 0.012f);
        float timeConstant = params.getFloat("time_constant", 20.0f);
        
        // In a real implementation, we would cast to the specific rule type and set parameters
        // For now, this is a demonstration of the factory pattern
    } else if (name == "Hebbian") {
        float learningRate = params.getFloat("learning_rate", 0.01f);
        float maxWeight = params.getFloat("max_weight", 1.0f);
        float minWeight = params.getFloat("min_weight", -1.0f);
        float covarianceThreshold = params.getFloat("covariance_threshold", 0.0f);
        
        // Similar casting and parameter setting would go here
    }
    
    return rule;
}

PlasticityParameters PlasticityFactory::getDefaultParameters(const std::string& name) {
    PlasticityParameters params;
    
    if (name == "STDP") {
        params.set("ltp_weight", 0.01f);
        params.set("ltd_weight", 0.012f);
        params.set("time_constant", 20.0f);
    } else if (name == "R-STDP") {
        params.set("ltp_weight", 0.01f);
        params.set("ltd_weight", 0.012f);
        params.set("time_constant", 20.0f);
        params.set("dopamine_scale", 1.0f);
    } else if (name == "TripletSTDP") {
        params.set("a_plus", 0.025f);
        params.set("a_minus", 0.0275f);
        params.set("a_u", 0.005f);
        params.set("tau_plus", 20.0f);
        params.set("tau_minus", 20.0f);
        params.set("tau_u", 200.0f);
    } else if (name == "Hebbian") {
        params.set("learning_rate", 0.01f);
        params.set("max_weight", 1.0f);
        params.set("min_weight", -1.0f);
        params.set("covariance_threshold", 0.0f);
    }
    
    return params;
}

bool PlasticityFactory::validateParameters(const std::string& name,
                                         const PlasticityParameters& params) {
    if (!hasPlasticityRule(name)) {
        return false;
    }
    
    // Basic validation - in a real implementation, would validate each parameter
    // against the specific rule's constraints
    return true;
}

std::unique_ptr<PlasticityRule> PlasticityFactory::createConfiguredRule(const Config& config) {
    if (!isPlasticityEnabled(config)) {
        return nullptr; // Plasticity disabled
    }
    
    // Check if rule is specified in config
    std::string ruleName = config.getOr<std::string>(CONFIG_RULE_NAME, "STDP");
    
    // Check if rule type is enabled
    if (ruleName == "Hebbian" && !isHebbianEnabled(config)) {
        return nullptr;
    } else if (ruleName == "STDP" && !isSTDPEnabled(config)) {
        return nullptr;
    } else if (ruleName == "R-STDP" && !isRewardModulatedEnabled(config)) {
        return nullptr;
    } else if (ruleName == "TripletSTDP" && !isTripletEnabled(config)) {
        return nullptr;
    }
    
    // Create rule from config
    return createFromConfig(config);
}

std::vector<std::unique_ptr<PlasticityRule>> PlasticityFactory::createPlasticitySystem(const Config& config) {
    std::vector<std::unique_ptr<PlasticityRule>> rules;
    
    if (!isPlasticityEnabled(config)) {
        return rules; // No plasticity rules
    }
    
    // Get enabled rule names from config
    std::vector<std::string> enabledRules = getEnabledRuleNames(config);
    
    // Create each enabled rule
    for (const std::string& ruleName : enabledRules) {
        try {
            auto rule = createConfiguredRule(config);
            if (rule != nullptr) {
                rules.push_back(std::move(rule));
            }
        } catch (const std::exception& e) {
            // Log error but continue with other rules
            // In a real implementation, would log this error
        }
    }
    
    return rules;
}

std::vector<std::string> PlasticityFactory::getEnabledRuleNames(const Config& config) {
    std::vector<std::string> enabledRules;
    
    // Check each rule type based on config
    if (isHebbianEnabled(config)) {
        enabledRules.push_back("Hebbian");
    }
    if (isSTDPEnabled(config)) {
        enabledRules.push_back("STDP");
    }
    if (isRewardModulatedEnabled(config)) {
        enabledRules.push_back("R-STDP");
    }
    if (isTripletEnabled(config)) {
        enabledRules.push_back("TripletSTDP");
    }
    
    return enabledRules;
}

bool PlasticityFactory::isHebbianEnabled(const Config& config) {
    return config.getOr<bool>(CONFIG_HEBBIAN_ENABLED, true);
}

bool PlasticityFactory::isSTDPEnabled(const Config& config) {
    return config.getOr<bool>(CONFIG_STDP_ENABLED, true);
}

bool PlasticityFactory::isRewardModulatedEnabled(const Config& config) {
    return config.getOr<bool>(CONFIG_REWARD_MODULATED_ENABLED, true);
}

bool PlasticityFactory::isTripletEnabled(const Config& config) {
    return config.getOr<bool>(CONFIG_TRIPLET_ENABLED, true);
}

bool PlasticityFactory::applyConfigToRule(std::unique_ptr<PlasticityRule>& rule,
                                         const Config& config) {
    if (rule == nullptr) {
        return false;
    }
    
    // In a real implementation, would cast to specific rule type and apply config
    // For now, just a stub showing the concept
    return true;
}

PlasticityParameters PlasticityFactory::getConfigurationTemplate(const std::string& ruleName) {
    return getDefaultParameters(ruleName);
}

bool PlasticityFactory::saveRuleConfiguration(const std::string& ruleName,
                                             const PlasticityParameters& params,
                                             const std::string& filepath) {
    try {
        // Convert parameters to JSON (assuming nlohmann/json is available)
        nlohmann::json jsonData;
        
        // In a real implementation, would serialize all parameters
        if (ruleName == "STDP") {
            jsonData["rule_type"] = "STDP";
            jsonData["ltp_weight"] = params.getFloat("ltp_weight", 0.01f);
            jsonData["ltd_weight"] = params.getFloat("ltd_weight", 0.012f);
            jsonData["time_constant"] = params.getFloat("time_constant", 20.0f);
        } else if (ruleName == "Hebbian") {
            jsonData["rule_type"] = "Hebbian";
            jsonData["learning_rate"] = params.getFloat("learning_rate", 0.01f);
            jsonData["max_weight"] = params.getFloat("max_weight", 1.0f);
            jsonData["min_weight"] = params.getFloat("min_weight", -1.0f);
            jsonData["covariance_threshold"] = params.getFloat("covariance_threshold", 0.0f);
        }
        
        // Write to file
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << jsonData.dump(2);
        file.close();
        
        return true;
    } catch (const std::exception& e) {
        // In a real implementation, would log this error
        return false;
    }
}

PlasticityParameters PlasticityFactory::loadRuleConfiguration(const std::string& ruleName,
                                                             const std::string& filepath) {
    try {
        PlasticityParameters params;
        
        // Read JSON from file (assuming nlohmann/json is available)
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filepath);
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        file.close();
        
        // In a real implementation, would deserialize based on rule type
        // For now, return empty params
        return params;
    } catch (const std::exception& e) {
        // In a real implementation, would log this error
        return PlasticityParameters();
    }
}

// Default configuration values
const PlasticityParameters& PlasticityFactory::getDefaultSTDPConfig() {
    static const PlasticityParameters config = getDefaultParameters("STDP");
    return config;
}

const PlasticityParameters& PlasticityFactory::getDefaultHebbianConfig() {
    static const PlasticityParameters config = getDefaultParameters("Hebbian");
    return config;
}

const PlasticityParameters& PlasticityFactory::getDefaultR_STDPConfig() {
    static const PlasticityParameters config = getDefaultParameters("R-STDP");
    return config;
}

const PlasticityParameters& PlasticityFactory::getDefaultTripletSTDPConfig() {
    static const PlasticityParameters config = getDefaultParameters("TripletSTDP");
    return config;
}

// Configuration parameter names for each rule type
const std::vector<std::string>& PlasticityFactory::getSTDPConfigKeys() {
    static const std::vector<std::string> keys = {"ltp_weight", "ltd_weight", "time_constant"};
    return keys;
}

const std::vector<std::string>& PlasticityFactory::getHebbianConfigKeys() {
    static const std::vector<std::string> keys = {"learning_rate", "max_weight", "min_weight", "covariance_threshold"};
    return keys;
}

const std::vector<std::string>& PlasticityFactory::getR_STDPConfigKeys() {
    static const std::vector<std::string> keys = {"ltp_weight", "ltd_weight", "time_constant", "dopamine_scale"};
    return keys;
}

const std::vector<std::string>& PlasticityFactory::getTripletSTDPConfigKeys() {
    static const std::vector<std::string> keys = {"a_plus", "a_minus", "a_u", "tau_plus", "tau_minus", "tau_u"};
    return keys;
}

// Initialize built-in rules on first use
namespace {
    struct PlasticityFactoryInitializer {
        PlasticityFactoryInitializer() {
            registerBuiltInPlasticityRules();
        }
    } initializer;
}

} // namespace nlm