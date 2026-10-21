#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention signaling
    void signalAttentionBoost(float boost);
    void triggerMemoryConsolidation();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
