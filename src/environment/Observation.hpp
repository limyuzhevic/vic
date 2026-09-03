#pragma once

#include "Action.hpp"

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
    
private:
    std::unique_ptr<SensoryInput> sensoryInput_;
};

} // namespace nlm
