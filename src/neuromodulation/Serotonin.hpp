#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood and social behavior modulation
    void modulateMood(float moodStrength);
    void modulateSocialBehavior(float strength);
    void reduceImpulsivity(float reduction);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm