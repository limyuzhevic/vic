// PlasticityRulePluginInterface.hpp
#pragma once

#include "../PluginBase.hpp"
#include <memory>
#include <vector>
#include "../../plasticity/PlasticityRule.hpp"

namespace nlm {

class PlasticityRulePluginInterface : public virtual PluginBase {
public:
    virtual ~PlasticityRulePluginInterface() = default;

    // Plasticity rule creation
    virtual std::unique_ptr<PlasticityRule> createRule(const Config& config) = 0;
    virtual std::unique_ptr<PlasticityRule> createRule(PlasticityFlags flags, const Config& config) = 0;

    // Rule configuration
    virtual bool configureRule(PlasticityRule* rule, const Config& config) = 0;
    virtual bool validateRuleConfiguration(const Config& config) const = 0;

    // Plasticity properties
    virtual PlasticityFlags getSupportedPlasticityFlags() const = 0;
    virtual std::string getPlasticityTypeName() const = 0;
    virtual bool supportsCustomLearningRate() const = 0;
    virtual float getDefaultLearningRate() const = 0;
    virtual float getLearningRateRangeMin() const = 0;
    virtual float getLearningRateRangeMax() const = 0;

    // Plasticity dynamics
    virtual bool supportsCustomSTDP() const = 0;
    virtual float getSTDPTimeConstant() const = 0;
    virtual bool supportsCustomHebbian() const = 0;

    // Plugin-specific configuration
    virtual std::string getDefaultConfigurationKey() const = 0;
    virtual Config getDefaultConfiguration() const = 0;

    // Validation and statistics
    virtual float calculateExpectedWeightChange(const PlasticityRule& rule, float preSpikeTime, float postSpikeTime) const = 0;
    virtual float calculateLearningRate(const Config& config) const = 0;
};

} // namespace nlm