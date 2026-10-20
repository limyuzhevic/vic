#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// ACh signals attention-grabbing stimuli and enhances memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    // Disable copying
    Acetylcholine(const Acetylcholine&) = delete;
    Acetylcholine& operator=(const Acetylcholine&) = delete;
    
    // Disable moving
    Acetylcholine(Acetylcholine&&) = delete;
    Acetylcholine& operator=(Acetylcholine&&) = delete;
    
    // Get modulator name
    const char* getName() const override;
    
    // Get current concentration/level
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Get ACh-specific plasticity factor
    float getPlasticityFactor() const override;
    
    // Update neuromodulator state
    void update(TimestepDuration dt) override;
    
    // Signal attention-grabbing stimulus
    void signalAttention(float strength);
    
    // Signal memory-relevant event
    void signalMemoryEvent(float importance);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm