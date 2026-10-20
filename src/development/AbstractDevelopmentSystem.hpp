#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <string>

namespace nlm {

// Forward declaration for RandomGenerator
class RandomGenerator;

namespace nlm {

// AbstractDevelopmentSystem: Base interface for neural development
// Provides common functionality for developmental architectures
// Allows different implementations of neural development processes

class AbstractDevelopmentSystem {
public:
    virtual ~AbstractDevelopmentSystem() = default;

    // Initialize with brain reference for system coordination
    virtual void initialize(Brain* brain) = 0;

    // Update developmental processes
    virtual void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) = 0;

    // Get current developmental stage
    virtual DevelopmentalStage getStage() const = 0;
    virtual void setStage(DevelopmentalStage stage) = 0;

    // Get stage name for debugging
    virtual const char* getStageName() const = 0;

    // Get developmental system name/type for debugging and logging
    virtual const char* getName() const = 0;

    // Get development status information
    virtual std::string getStatus() const = 0;

    // Enable/disable development system functionality
    virtual void enable(bool enable) = 0;
    virtual bool isEnabled() const = 0;

    // Reset development system state
    virtual void reset() = 0;

    // Get developmental age (in simulation time)
    virtual double getDevelopmentalAge() const = 0;
    virtual void setDevelopmentalAge(double age) = 0;

    // Get plasticity modifier for current stage
    virtual float getPlasticityModifier() const = 0;

    // Check if in critical period
    virtual bool isCriticalPeriod() const = 0;
    virtual float getCriticalPeriodProgress() const = 0;

protected:
    AbstractDevelopmentSystem() = default;
};

} // namespace nlm
