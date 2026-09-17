#pragma once

#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    NeuromodulatorType getType() const override;
    void applyToNeuron(Neuron* neuron, float factor) override;
    void applyToPlasticity(float currentLevel, float& ltpWeight, float& ltdWeight) override;
    
    // 5-HT specific functions
    void improveMood();
    void decreaseImpulsivity();
    void modulateSocialBehavior(float sociality);
    void facilitateSensoryGating();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
