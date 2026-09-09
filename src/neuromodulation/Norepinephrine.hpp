#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Apply attention effects (NE increases signal-to-noise ratio)
    void applyAttentionEffect(Brain* brain) override;
    
    // Apply memory effects (NE enhances consolidation of salient events)
    void applyMemoryEffect(Brain* brain) override;
    
    // Apply arousal effects (NE increases arousal, attention, and readiness)
    void applyArousalEffect(Brain* brain) override;
    
    // Increase vigilance and alertness
    void increaseVigilance(float amount);
    
    // Enhance encoding of novel/emerging stimuli
    void enhanceNoveltyEncoding(const std::vector<float>& pattern);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
