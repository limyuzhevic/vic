#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// Oxytocin: Social bonding, trust, and social learning
// PLACEHOLDER - Phase 2
class Oxytocin : public Neuromodulator {
public:
    Oxytocin();
    ~Oxytocin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Social bonding signaling
    void signalSocialBonding(float socialCohesion);
    
    // Trust signaling  
    void signalTrust(float trustLevel);
    
    // Social history and trust tracking
    float getSocialBondHistory(size_t index) const;
    float getTrustScore() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm