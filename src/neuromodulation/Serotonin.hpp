#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <memory>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    void initialize();
    
    // Get current level
    float getLevel() const;
    
    // Set level
    void setLevel(float level);
    
    // Update state
    void update(double dt);
    
    // Enhance mood
    float enhanceMood() const;
    
    // Reduce impulsivity
    float reduceImpulsivity() const;
    
    // Correct prediction errors
    float correctPredictionErrors() const;
    
    // Clear state
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    float mood_;
    float impulsivity_;
    float errorCorrection_;
    float baseline_;
};

} // namespace nlm
