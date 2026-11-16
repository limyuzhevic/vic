#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Forward declaration for Brain
class Brain;

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get modulator level
    float getLevel() const;
    void setLevel(float level);
    
    // Get plasticity factor
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Mood and behavioral modulation
    void signalMoodChange(float moodLevel);
    float getMoodLevel() const;
    bool isHappy() const;
    bool isAngry() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm