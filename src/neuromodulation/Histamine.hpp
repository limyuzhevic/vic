#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Forward declaration for Brain
class Brain;

// Histamine: Wakefulness and cognition regulation
class Histamine {
public:
    Histamine();
    ~Histamine();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get modulator level
    float getLevel() const;
    void setLevel(float level);
    
    // Get plasticity factor
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Wakefulness modulation
    void increaseWakefulness(float wakefulnessLevel);
    void decreaseWakefulness(float wakefulnessDecay);
    float getWakefulnessLevel() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm