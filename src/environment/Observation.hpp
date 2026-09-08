#pragma once

#include "Action.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Sensory input observation
class Observation {
public:
    Observation();
    Observation(const SensoryInput& input);
    ~Observation();
    
    // Get the sensory input
    const SensoryInput* getSensoryInput() const;
    
    // Check if observation is valid
    bool isValid() const;
    
    // Clone
    std::unique_ptr<Observation> clone() const;
    
    // Position-based reward computation helpers
    bool getPositionAvailable() const;
    float getX() const;
    float getY() const;
    const char* getType() const;
    
private:
    std::unique_ptr<SensoryInput> sensoryInput_;
};

} // namespace nlm
