#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Implemented with biological realism for Phase 2

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    
    // NE-specific effects
    float getArousalEffect() const;
    float getVigilanceEffect() const;
    float getPlasticityEffect() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
