#pragma once

#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    NeuromodulatorType getType() const override;
    void applyToNeuron(Neuron* neuron, float factor) override;
    void applyToPlasticity(float currentLevel, float& ltpWeight, float& ltdWeight) override;
    
    // ACh-specific functions
    void enhanceAttention();
    void facilitateMemoryEncoding();
    void modulateSensoryProcessing(float gain);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
