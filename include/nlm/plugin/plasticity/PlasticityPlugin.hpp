#pragma once

#include "NLMPlugin.hpp"
#include "../plasticity/PlasticityRule.hpp"
#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>

namespace nlm {

/**
 * @class PlasticityPlugin
 * @brief Plugin interface for custom plasticity rules
 */
class PlasticityPlugin : public virtual NLMPlugin {
public:
    virtual ~PlasticityPlugin() = default;
    
    // Type check
    bool isPlasticityPlugin() const override { return true; }
    
    // Plasticity factory methods
    virtual void* getPlasticityFactory() override {
        return static_cast<void*>(this);
    }
    
    // Abstract plugin interface for plasticity
    virtual PlasticityRule* createPlasticityRule() = 0;
    virtual bool canCreatePlasticityRule(const std::string& ruleType) const = 0;
    virtual std::vector<std::string> getSupportedPlasticityRuleTypes() const = 0;
    
    // Plasticity configuration
    virtual bool configurePlasticityRule(PlasticityRule& rule, const std::string& config) = 0;
    virtual std::string getPlasticityRuleConfig(const PlasticityRule& rule) const = 0;
};

} // namespace nlm