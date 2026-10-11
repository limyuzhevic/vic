#pragma once

#include "PluginBase.hpp"
#include "../plasticity/PlasticityRule.hpp"

namespace nlm {

class PlasticityPlugin : public NLMPlugin {
public:
    // Plasticity extension
    virtual void updatePlasticity(std::vector<Synapse*>& synapses, double deltaTime) = 0;
    virtual void applyLearningRule(const class LearningRuleContext& context) = 0;
    virtual PlasticityRule getPlasticityRule() const = 0;
    
    // Plasticity parameters
    virtual double getLearningRate() const = 0;
    virtual double getDecayRate() const = 0;
    virtual double getBaselineStrength() const = 0;
};

} // namespace nlm
