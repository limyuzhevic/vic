// Neuromodulation.hpp - Comprehensive reward-modulated plasticity

#pragma once

#include "../../brain/Synapse.hpp"
#include <string>

namespace nlm {

class Neuromodulation {
public:
    Neuromodulation();
    ~Neuromodulation();
    
    // Disable copying, enable moving
    Neuromodulation(const Neuromodulation&) = delete;
    Neuromodulation& operator=(const Neuromodulation&) = delete;
    Neuromodulation(Neuromodulation&&) noexcept;
    Neuromodulation& operator=(Neuromodulation&&) noexcept;
    
    // Main update function for reward-modulated plasticity
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                float rewardPredictionError,
                float neuromodulatorLevel,
                float learningRate,
                TimestepDuration dt);
    
    // Apply weight change with bounds
    void applyWeightChange(Synapse* synapse, SynapticWeight delta, float neuromodulatorLevel);
    
    // Configuration
    void setLearningRate(float rate);
    void setModulationFactor(float factor);
    void setEligibilityTraceDecay(float decay);
    void setTraceBoostFactor(float factor);
    void setWeightClipping(float max);
    
    // Biological parameters
    void enableDopamineModulation(bool enable);
    void enableAChModulation(bool enable);
    void enableSerotoninModulation(bool enable);
    void enableNorepinephrineModulation(bool enable);
    
    // Statistics
    size_t getTotalNeuromodulatedUpdates() const;
    float getAverageNeuromodulatedChange() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
