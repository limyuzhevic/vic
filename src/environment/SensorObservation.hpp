#pragma once

#include "SensoryInput.hpp"

namespace nlm {

// Sensory input observation
class SensorObservation {
public:
    SensorObservation();
    SensorObservation(const SensoryInput& input);
    ~SensorObservation();
    
    // Get the sensory input
    const SensoryInput* getSensoryInput() const;
    
    // Check if observation is valid
    bool isValid() const;
    
    // Clone
    std::unique_ptr<SensorObservation> clone() const;
    
private:
    std::unique_ptr<SensoryInput> sensoryInput_;
};

// Backward compatibility alias
using Observation = SensorObservation;

} // namespace nlm
