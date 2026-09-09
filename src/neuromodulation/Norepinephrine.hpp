#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Real pharmacological implementation

class Brain;

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain) override;
    
    // Get NE level
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Update NE dynamics
    void update(TimestepDuration dt) override;
    
    // Get NE effects on plasticity
    float getPlasticityFactor() const override;
    
    // Signal events that should trigger NE release
    void signalStartle();
    void signalSustainedAttention();
    
    // Get modulated parameters
    void setArousalLevel(float arousal);
    float getArousalLevel() const;
    float getVigilance() const;
    float getSignalToNoiseRatio() const;
    
    // History tracking
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
