#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Implemented with biological realism for Phase 2

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    
    // ACh-specific effects
    float getAttentionEffect() const;
    float getMemoryEffect() const;
    float getPlasticityEffect() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
