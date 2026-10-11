#pragma once

#include "PluginBase.hpp"
#include "../development/DevelopmentStage.hpp"

namespace nlm {

class DevelopmentPlugin : public NLMPlugin {
public:
    // Development extension
    virtual void updateDevelopment(double deltaTime) = 0;
    virtual void applyDevelopmentalChanges(NeuralRegion* region) = 0;
    virtual DevelopmentalStage getDevelopmentalStage() const = 0;
    
    // Developmental parameters
    virtual double getPlasticityRate() const = 0;
    virtual double getExcitabilityFactor() const = 0;
    virtual double getMaturationTime() const = 0;
};

} // namespace nlm
