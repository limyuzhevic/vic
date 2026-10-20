#pragma once

// ISubsystem.hpp - Base interface for all brain subsystems
// This provides a consistent interface for all subsystems in the refactored brain architecture

#include <functional>
#include <string>

namespace nlm {

// Forward declarations
class Brain;
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;

// Progress callback type for subsystem updates
using ProgressCallback = std::function<void(float)>;

// Basic subsystem interface - ensures all subsystems implement
// the same core methods for initialization, updates, and status reporting
class ISubsystem {
public:
    virtual ~ISubsystem() = default;
    
    // Initialize the subsystem with a reference to the parent brain
    // Returns true if initialization succeeds, false otherwise
    virtual bool initialize(Brain* brain) = 0;
    
    // Update the subsystem for one simulation timestep
    // Parameter dt is the timestep duration in seconds
    virtual void update(float dt) = 0;
    
    // Reset the subsystem to its initial state
    virtual void reset() = 0;
    
    // Log subsystem status for debugging and monitoring
    virtual void logStatus() const = 0;
};

// Convenience typedef for subsystem pointers
using SubsystemPtr = std::unique_ptr<ISubsystem>;

} // namespace nlm
