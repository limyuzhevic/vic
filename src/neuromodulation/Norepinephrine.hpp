#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Forward declaration for Brain
class Brain;

// Norepinephrine: Arousal and vigilance
class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get modulator level
    float getLevel() const;
    void setLevel(float level);
    
    // Get plasticity factor
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Arousal modulation
    void increaseArousal(float arousalLevel);
    void decreaseArousal(float arousalDecay);
    float getArousalLevel() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm