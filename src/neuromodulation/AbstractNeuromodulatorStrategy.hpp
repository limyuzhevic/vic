#pragma once

#include "Neuromodulator.hpp"
#include <string>

namespace nlm {

// AbstractNeuromodulatorStrategy: Extended base interface for neuromodulatory strategies
// Extends the existing Neuromodulator base class
// Provides common functionality for neuromodulatory architectures
// Allows different implementations (Dopamine, Curiosity, Novelty, PredictionError)
// to be used interchangeably through strategy pattern

class AbstractNeuromodulatorStrategy : public Neuromodulator {
public:
    virtual ~AbstractNeuromodulatorStrategy() = default;

    // Enhanced initialization with brain reference for system coordination
    virtual void initialize(Brain* brain) = 0;

    // Get neuromodulator strategy name/type for debugging and logging
    virtual const char* getName() const = 0;

    // Get enhanced neuromodulator status information
    virtual std::string getStatus() const = 0;

    // Enable/disable neuromodulator strategy functionality
    virtual void enable(bool enable) = 0;
    virtual bool isEnabled() const = 0;

    // Reset neuromodulator strategy state
    virtual void reset() = 0;

    // Get neuromodulation source (what triggers this neuromodulator)
    virtual const char* getModulationSource() const = 0;

    // Get neuromodulation target (what this affects)
    virtual const char* getModulationTarget() const = 0;

protected:
    AbstractNeuromodulatorStrategy() = default;
};

} // namespace nlm
