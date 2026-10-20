#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <string>

namespace nlm {

// AbstractMemory: Base interface for all memory systems
// Provides common functionality for memory architectures
// Allows different implementations (NeuralWorkingMemory, NeuralEpisodicMemory, NeuralAssociativeMemory)
// to be used interchangeably through strategy pattern

class AbstractMemory {
public:
    virtual ~AbstractMemory() = default;

    // Initialize with brain reference for system coordination
    virtual void initialize(Brain* brain) = 0;

    // Common memory update interface
    // Called every simulation timestep for memory maintenance
    virtual void update(TimestepDuration dt) = 0;

    // Get memory system name/type for debugging and logging
    virtual const char* getName() const = 0;

    // Get memory status information
    virtual std::string getStatus() const = 0;

    // Enable/disable memory system functionality
    virtual void enable(bool enable) = 0;
    virtual bool isEnabled() const = 0;

    // Reset memory system state
    virtual void reset() = 0;

    // Get memory capacity/utilization information
    virtual size_t getCapacity() const = 0;
    virtual float getUtilization() const = 0;

protected:
    AbstractMemory() = default;
};

} // namespace nlm
