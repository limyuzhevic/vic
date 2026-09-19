#pragma once

#include "Action.hpp"

namespace nlm {

// Sensory input observation
class EnvironmentObservation {
public:
    EnvironmentObservation();
    EnvironmentObservation(const SensoryInput& input);
    ~EnvironmentObservation();
    
    // Get the sensory input
    const SensoryInput* getSensoryInput() const;
    
    // Check if observation is valid
    bool isValid() const;
    
    // Clone
    std::unique_ptr<EnvironmentObservation> clone() const;
    
private:
    std::unique_ptr<SensoryInput> sensoryInput_;
};

} // namespace nlm
