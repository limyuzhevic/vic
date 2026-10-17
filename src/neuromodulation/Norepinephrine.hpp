#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"
#include <string>

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
    
    // Arousal and vigilance effects
    void signalArousal(float intensity, float contextRelevance);
    void signalVigilance(float taskDemandingness, float novelty);
    
    // Get current effects
    float getArousalLevel() const;
    float getVigilanceLevel() const;
    float getPhasicResponse() const;
    
    // Signal processing
    float getTotalSignal() const;
    
    // Reset
    void reset();

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm