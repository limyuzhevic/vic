#pragma once

#include "Action.hpp"

namespace nlm {

// Sensory input observation
class Observation {
public:
    Observation(nullptr);
    Observation(const SensoryInput& input);
    ~Observation(nullptr);
    
    // Get the sensory input
    const SensoryInput* getSensoryInput(nullptr) const;
    
    // Check if observation is valid
    bool isValid(nullptr) const;
    
    // Clone
    std::unique_ptr<Observation> clone(nullptr) const;
    
private:
    std::unique_ptr<SensoryInput> sensoryInput_;
};

} // namespace nlm
