#pragma once

#include "../../core/Types/Types.hpp"
#include "../../plasticity/Hebbian.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

// Neuromodulator types for gating Hebbian learning
enum class NeuromodulatorType {
    DOPAMINE,
    ACETYLCHOLINE,
    SEROTONIN,
    NORADRENALINE,
    ENDORPHIN
};

// Neuromodulator effect on Hebbian learning
struct NeuromodulatorEffect {
    NeuromodulatorType type;
    float baseline;
    float gain;
    float decayTimeConstant;
    float activationThreshold;
    float maximumEffect;
    bool enablesSTDP;
    bool enhancesNMDA;
    bool strengthensTagging;
    bool promotesConsolidation;
};

// Neuromodulator signal for regulating neural plasticity
class Neuromodulator {
public:
    Neuromodulator();
    ~Neuromodulator();
    
    // Add neuromodulator type with specific effects
    void addModulator(NeuromodulatorType type, const NeuromodulatorEffect& effect);
    void removeModulator(NeuromodulatorType type);
    
    // Update neuromodulator levels based on neural activity
    void update(float dt, const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                Timestamp currentTime);
    
    // Get current modulation level for a specific type
    float getModulationLevel(NeuromodulatorType type) const;
    
    // Apply neuromodulation effects to Hebbian learning
    float computeModulationGate(NeuromodulatorType type, const Timestamp& spikeTime) const;
    float getEffectMultiplier(NeuromodulatorType type) const;
    
    // Check if neuromodulator enables specific plasticity rules
    bool enablesSTDP(NeuromodulatorType type) const;
    bool enhancesNMDA(NeuromodulatorType type) const;
    bool strengthensTagging(NeuromodulatorType type) const;
    bool promotesConsolidation(NeuromodulatorType type) const;
    
    // Integrate with Hebbian learning system
    void integrateWithHebbian(Hebbian* hebbian);
    
    // State management
    void reset();
    void setTimeScale(float timeScale);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
