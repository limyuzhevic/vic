#include "PlasticityRuleFactory.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <stdexcept>

namespace nlm {

std::unordered_map<std::string, PlasticityRuleFactory::PlasticityRuleCreator> PlasticityRuleFactory::builtInCreators_;
std::vector<PlasticityRuleFactory::PlasticityRuleInfo> PlasticityRuleFactory::builtInTypes_;
std::mutex PlasticityRuleFactory::mutex_;
std::unordered_set<std::string> PlasticityRuleFactory::loadedPlugins_;

// Static initializer to load built-in plasticity rules
static void initializePlasticityRuleFactory() {
    PlasticityRuleFactory::loadBuiltInPlasticityRules();
}

// Initialize on first use
void ensureInitialized() {
    static bool initialized = []() {
        initializePlasticityRuleFactory();
        return true;
    }();
}

void PlasticityRuleFactory::loadBuiltInPlasticityRules() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!builtInCreators_.empty()) {
        return; // Already loaded
    }
    
    // Register all built-in plasticity rules
    registerBuiltInCreator("STDP", 
        [](std::shared_ptr<Config> config, TimestepDuration timestep) {
            auto rule = std::make_unique<STDP>();
            return rule;
        },
        "Spike-Timing-Dependent Plasticity: learns based on precise timing of pre- and post-synaptic spikes",
        false);
    
    registerBuiltInCreator("Hebbian",
        [](std::shared_ptr<Config> config, TimestepDuration timestep) {
            auto rule = std::make_unique<Hebbian>();
            return rule;
        },
        "Hebbian learning: 'neurons that fire together, wire together'",
        false);
    
    registerBuiltInCreator("StructuralPlasticity",
        [](std::shared_ptr<Config> config, TimestepDuration timestep) {
            auto rule = std::make_unique<StructuralPlasticity>();
            return rule;
        },
        "Structural plasticity: creation and removal of synapses and neurons",
        false);
    
    // Register stub plasticity rules for future extension
    registerBuiltInCreator("AntiHebbianRule",
        [](std::shared_ptr<Config> config, TimestepDuration timestep) {
            return createStub("AntiHebbianRule", config);
        },
        "Anti-Hebbian rule: decreases weight when neurons fire together",
        true);
    
    registerBuiltInCreator("BCMRule",
        [](std::shared_ptr<Config> config, TimestepDuration timestep) {
            return createStub("BCM", config);
        },
        "BCM rule: implements Bienenstock-Cooper-Munro sliding threshold plasticity",
        true);
}

void PlasticityRuleFactory::registerBuiltInCreator(const std::string& typeName,
                                                   PlasticityRuleCreator creator,
                                                   const std::string& description,
                                                   bool isStub) {
    registerCreator(typeName, creator, description, isStub);
}

void PlasticityRuleFactory::registerCreator(const std::string& typeName,
                                            PlasticityRuleCreator creator,
                                            const std::string& description,
                                            bool isStub) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    builtInCreators_[typeName] = std::move(creator);
    
    PlasticityRuleInfo info;
    info.name = typeName;
    info.description = description;
    info.isBuiltIn = !isStub;
    info.isStub = isStub;
    
    // Add common config options
    info.configOptions["enabled"] = "Enable/disable this plasticity rule";
    info.configOptions["learning_rate"] = "Learning rate (0.0-1.0)";
    
    builtInTypes_.push_back(std::move(info));
    
    NLM_LOG_INFO("Registered plasticity rule: " + typeName + " - " + description);
}

std::unique_ptr<PlasticityRule> PlasticityRuleFactory::create(const std::string& ruleType,
                                                             std::shared_ptr<Config> config,
                                                             TimestepDuration timestep) {
    ensureInitialized();
    
    std::string typeName = ruleType;
    // Convert to lowercase for case-insensitive comparison
    std::transform(typeName.begin(), typeName.end(), typeName.begin(), ::tolower);
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = builtInCreators_.find(typeName);
    if (it == builtInCreators_.end()) {
        throw std::runtime_error("Unknown plasticity rule type: " + ruleType);
    }
    
    try {
        return it->second(config, timestep);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to create plasticity rule " + ruleType + ": " + std::string(e.what()));
    }
}

bool PlasticityRuleFactory::hasType(const std::string& ruleType) {
    ensureInitialized();
    
    std::string typeName = ruleType;
    std::transform(typeName.begin(), typeName.end(), typeName.begin(), ::tolower);
    
    std::lock_guard<std::mutex> lock(mutex_);
    return builtInCreators_.find(typeName) != builtInCreators_.end();
}

bool PlasticityRuleFactory::isStub(const std::string& ruleType) {
    ensureInitialized();
    
    for (const auto& info : builtInTypes_) {
        if (info.name == ruleType) {
            return info.isStub;
        }
    }
    return false;
}

void PlasticityRuleFactory::registerPlugin(const std::string& pluginName, 
                                           PluginRegistration registrationFunc) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (loadedPlugins_.find(pluginName) != loadedPlugins_.end()) {
        NLM_LOG_WARN("Plugin already loaded: " + pluginName);
        return;
    }
    
    try {
        registrationFunc(*this);
        loadedPlugins_.insert(pluginName);
        NLM_LOG_INFO("Loaded plugin: " + pluginName);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load plugin " + pluginName + ": " + std::string(e.what()));
    }
}

void PlasticityRuleFactory::initializePlugins() {
    ensureInitialized();
    // Plugins would be automatically initialized when registered
    NLM_LOG_INFO("PlasticityRule plugin system initialized");
}

std::unique_ptr<PlasticityRule> PlasticityRuleFactory::createWithFallback(const std::string& ruleType,
                                                                          std::shared_ptr<Config> config,
                                                                          TimestepDuration timestep) {
    try {
        return create(ruleType, config, timestep);
    } catch (const std::runtime_error& e) {
        NLM_LOG_WARN(std::string("Failed to create plasticity rule ") + ruleType + 
                   ". Using stub implementation: " + std::string(e.what()));
        
        // Try to create a stub if available
        std::string typeName = ruleType;
        std::transform(typeName.begin(), typeName.end(), typeName.begin(), ::tolower);
        
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = builtInCreators_.find(typeName);
        if (it != builtInCreators_.end()) {
            // Check if it's a stub creator
            for (const auto& info : builtInTypes_) {
                if (info.name == typeName && info.isStub) {
                    return it->second(config, timestep);
                }
            }
        }
        
        // If no stub found, create a generic placeholder
        return createStub(ruleType, config);
    }
}

std::unique_ptr<PlasticityRule> PlasticityRuleFactory::createStub(const std::string& typeName,
                                                                 std::shared_ptr<Config> config) {
    auto rule = std::make_unique<AntiHebbianRule>();
    rule->setName("Generic Stub " + typeName);
    rule->setEnabled(false);
    NLM_LOG_INFO("Created stub plasticity rule: " + typeName);
    return rule;
}

std::vector<PlasticityRuleFactory::PlasticityRuleInfo> PlasticityRuleFactory::getAvailableTypes() {
    ensureInitialized();
    
    std::lock_guard<std::mutex> lock(mutex_);
    return builtInTypes_;
}

} // namespace nlm
