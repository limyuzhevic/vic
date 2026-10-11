#pragma once

#include "PluginBase.hpp"
#include "../brain/Neuron.hpp"

namespace nlm {

enum class NeuromodulatorType {
    DOPAMINE,
    SEROTONIN,
    ACETYLCHOLINE,
    NOREPINEPHRINE,
    GABA,
    CUSTOM
};

class NeuromodulationPlugin : public NLMPlugin {
public:
    // Neuromodulation extension
    virtual void updateNeuromodulation(double deltaTime) = 0;
    virtual void applyNeuromodulationEffects(std::vector<Neuron*>& neurons) = 0;
    virtual float getModulationLevel() const = 0;
    
    // Neuromodulator types
    virtual NeuromodulatorType getNeuromodulatorType() const = 0;
    virtual float getBaselineLevel() const = 0;
    virtual float getModulationRange() const = 0;
};

} // namespace nlm
