#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Neuromodulator: Norepinephrine - arousal and vigilance
// Real implementation - affects arousal, attention, and stress response
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signal arousal changes
    void signalArousal(float arousalLevel);
    
    // Signal stress response
    void signalStress(float stressLevel);
    
    // Get current arousal level
    float getArousalLevel() const;
    
    // Get stress level
    float getStressLevel() const;
    
    // Get vigilance level
    float getVigilanceLevel() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Update internal neuromodulation state
    void updateInternalState();
    
    // Update neural excitability
    void updateNeuralExcitability(float arousalLevel);
    
    // Update memory encoding effects
    void updateMemoryEncoding(float stressLevel);
};

} // namespace nlm