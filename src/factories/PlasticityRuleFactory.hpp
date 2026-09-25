#pragma once

#include "abstraction/AbstractComponents.hpp"
#include "plasticity/PlasticityRule.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/RewardModulatedSTDP.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "core/Config/Config.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace nlm {

class PlasticityRuleFactory {
public:
    // Factory type enum for configuration
    enum class PlasticityRuleType {
        STDP,
        Hebbian,
        RewardModulatedSTDP,
        StructuralPlasticity,
        Custom
    };
    
    // Factory configuration
    struct FactoryConfig {
        std::string name;
        PlasticityRuleType type;
        std::unordered_map<std::string, std::string> parameters;
        bool enabled;
        std::string pluginName;
    };
    
    PlasticityRuleFactory();
    ~PlasticityRuleFactory();
    
    // Create plasticity rule based on type
    std::unique_ptr<AbstractPlasticityRule> createPlasticityRule(
        PlasticityRuleType type, 
        std::shared_ptr<Config> config = nullptr
    );
    
    std::unique_ptr<AbstractPlasticityRule> createPlasticityRule(
        const std::string& ruleName,
        std::shared_ptr<Config> config = nullptr
    );
    
    // Create STDP rule with specific parameters
    std::unique_ptr<STDP> createSTDP(
        std::shared_ptr<Config> config = nullptr
    ) {
        auto stdp = std::make_unique<STDP>();
        if (config) {
            configureFromConfig(stdp.get(), config);
        }
        return stdp;
    }
    
    // Create Hebbian rule with specific parameters
    std::unique_ptr<Hebbian> createHebbian(
        std::shared_ptr<Config> config = nullptr
    ) {
        auto hebbian = std::make_unique<Hebbian>();
        if (config) {
            configureFromConfig(hebbian.get(), config);
        }
        return hebbian;
    }
    
    // Create RewardModulatedSTDP rule
    std::unique_ptr<RewardModulatedSTDP> createRewardModulatedSTDP(
        std::shared_ptr<Config> config = nullptr
    ) {
        auto rewardStdp = std::make_unique<RewardModulatedSTDP>();
        if (config) {
            configureFromConfig(rewardStdp.get(), config);
        }
        return rewardStdp;
    }
    
    // Create StructuralPlasticity rule
    std::unique_ptr<StructuralPlasticity> createStructuralPlasticity(
        std::shared_ptr<Config> config = nullptr
    ) {
        auto structural = std::make_unique<StructuralPlasticity>();
        if (config) {
            configureFromConfig(structural.get(), config);
        }
        return structural;
    }
    
    // Configuration-based factory creation
    std::unique_ptr<AbstractPlasticityRule> createFromConfig(
        const Config& config
    );
    
    // Plugin-based factory creation
    std::unique_ptr<AbstractPlasticityRule> createFromPlugin(
        const std::string& pluginName,
        std::shared_ptr<Config> config = nullptr
    );
    
    // List available plasticity rule types
    static std::vector<PlasticityRuleType> getAvailableTypes() {
        return {
            PlasticityRuleType::STDP,
            PlasticityRuleType::Hebbian,
            PlasticityRuleType::RewardModulatedSTDP,
            PlasticityRuleType::StructuralPlasticity
        };
    }
    
    // Get type name
    static const char* getTypeName(PlasticityRuleType type) {
        switch (type) {
            case PlasticityRuleType::STDP: return "STDP";
            case PlasticityRuleType::Hebbian: return "Hebbian";
            case PlasticityRuleType::RewardModulatedSTDP: return "RewardModulatedSTDP";
            case PlasticityRuleType::StructuralPlasticity: return "StructuralPlasticity";
            default: return "Unknown";
        }
    }
    
    // Configuration helpers
    void configureFromConfig(AbstractPlasticityRule* rule, const Config& config);
    void configureFromConfig(STDP* stdp, const Config& config);
    void configureFromConfig(Hebbian* hebbian, const Config& config);
    void configureFromConfig(RewardModulatedSTDP* rewardStdp, const Config& config);
    void configureFromConfig(StructuralPlasticity* structural, const Config& config);
    
    // Plugin system
    bool registerPlugin(const std::string& name, std::function<std::unique_ptr<AbstractPlasticityRule>(std::shared_ptr<Config>)> factory);
    std::unique_ptr<AbstractPlasticityRule> createFromRegisteredPlugin(const std::string& name, std::shared_ptr<Config> config = nullptr);
    
    // Get registered plugin names
    std::vector<std::string> getRegisteredPlugins() const;
    
    // Statistics and monitoring
    size_t getCreationCount() const { return creationCount_; }
    size_t getPluginCount() const { return plugins_.size(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    mutable size_t creationCount_;
    std::unordered_map<std::string, std::function<std::unique_ptr<AbstractPlasticityRule>(std::shared_ptr<Config>)>> plugins_;
};

} // namespace nlm
