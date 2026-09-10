// BrainSystemUpdater.hpp - Abstract base class for brain system updaters
#pragma once

#include "Brain.hpp"
#include <string>

namespace nlm {

class BrainSystemUpdater {
public:
    virtual ~BrainSystemUpdater() = default;
    
    // Update the system for one simulation step
    virtual void update(Brain& brain, SimulationStep currentStep, 
                       Timestamp currentTime, TimestepDuration timestep) = 0;
    
    // Get human-readable name of this updater
    virtual std::string getName() const = 0;
    
    // Can be overridden to indicate when to run (default: always)
    virtual bool shouldUpdate(const Brain& brain, SimulationStep currentStep) const {
        return true;
    }
};

} // namespace nlm
