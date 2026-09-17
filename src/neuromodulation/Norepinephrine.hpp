#pragma once

#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    NeuromodulatorType getType() const override;
    void applyToNeuron(Neuron* neuron, float factor) override;
    void applyToPlasticity(float currentLevel, float& ltpWeight, float& ltdWeight) override;
    
    // NE-specific functions
    void increaseArousal();
    void enhanceFocus();
    void modulateLiking();
    void improveWorkingMemory(float capacityIncrease);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
