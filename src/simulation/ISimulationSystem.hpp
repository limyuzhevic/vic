#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <string>

namespace nlm {

// ISimulationSystem: Common interface for all simulation systems
// Provides abstraction for system lifecycle management and state control
class ISimulationSystem {
public:
    virtual ~ISimulationSystem() = default;
    
    // Initialize the system with brain reference for dependency injection
    // Parameter: brain - Pointer to the brain that owns this system
    virtual void Initialize(Brain* brain) = 0;
    
    // Update system state with given timestep
    // Parameter: timestep - Time elapsed since last update (seconds)
    virtual void Update(double timestep) = 0;
    
    // Reset system to initial state
    virtual void Reset() = 0;
    
    // Check if system is enabled
    // Returns: true if system is enabled, false if disabled
    virtual bool IsEnabled() const = 0;
    
    // Enable system for operation
    virtual void Enable() = 0;
    
    // Disable system from operation
    virtual void Disable() = 0;
    
    // Get system name for logging and identification
    // Returns: String identifier for the system
    virtual const char* GetName() const = 0;
    
    // Get system status for debugging
    // Returns: String representation of system state
    virtual std::string GetStatus() const = 0;
};

} // namespace nlm
