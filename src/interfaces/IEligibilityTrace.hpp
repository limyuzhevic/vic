// IEligibilityTrace.hpp
// Abstract interface for eligibility traces
// Maintain traces of recent neural activity for online learning
// Support various trace types (exponential, delta, etc.)

#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <map>

namespace nlm {

// Forward declarations
class Brain;

// Eligibility trace entry for a neuron or synapse
struct EligibilityEntry {
    NeuronId neuronId;
    float traceValue;
    float weightChangePotential;
    SimulationStep lastUpdated;
    float predictionError;
    bool isActive;
    
    EligibilityEntry() : neuronId(), traceValue(0.0f), weightChangePotential(0.0f),
                        lastUpdated(0), predictionError(0.0f), isActive(false) {}
    EligibilityEntry(NeuronId id, float value, float potential, SimulationStep step)
        : neuronId(id), traceValue(value), weightChangePotential(potential),
          lastUpdated(step), predictionError(0.0f), isActive(true) {}
};

class IEligibilityTrace {
public:
    virtual ~IEligibilityTrace() = default;

    // Initialize eligibility trace system with brain reference
    virtual bool initialize(Brain* brain) = 0;

    // Add eligibility trace for a neuron/synapse
    virtual void addEligibility(NeuronId neuron,
                                float initialValue = 1.0f,
                                float weightPotential = 0.0f) = 0;

    // Update eligibility traces (decay over time)
    virtual void update(TimestepDuration dt) = 0;

    // Get current eligibility trace value
    virtual float getEligibility(NeuronId neuron) const = 0;

    // Set eligibility trace value
    virtual void setEligibility(NeuronId neuron, float value) = 0;

    // Get weight change potential for a neuron
    virtual float getWeightChangePotential(NeuronId neuron) const = 0;

    // Update weight change potential (e.g., from neuromodulators)
    virtual void updateWeightChangePotential(NeuronId neuron, float potential,
                                            float predictionError = 0.0f) = 0;

    // Compute actual weight change using traces
    virtual float computeWeightChange(NeuronId neuron) const = 0;

    // Reset eligibility trace for a neuron
    virtual void resetEligibility(NeuronId neuron) = 0;

    // Clear all eligibility traces
    virtual void clear() = 0;

    // Get trace statistics
    virtual std::string getTraceStats() const = 0;

    // Interface information
    virtual std::string getInterfaceName() const { return "IEligibilityTrace"; }
    virtual std::string getVersion() const { return "1.0.0"; }

    // Error handling
    virtual bool hasError() const = 0;
    virtual std::string getErrorMessage() const = 0;
    virtual void clearError() = 0;

    // Get all active eligibility entries
    virtual std::vector<EligibilityEntry> getActiveEntries() const = 0;

    // Set trace parameters
    virtual float getDecayRate() const = 0;
    virtual void setDecayRate(float rate) = 0;
    virtual float getTraceTimeConstant() const = 0;
    virtual void setTraceTimeConstant(float tc) = 0;

    // Trace type
    enum class TraceType {
        Exponential,
        Delta,
        Incremental,
        Persistent
    };
    virtual TraceType getTraceType() const = 0;
    virtual void setTraceType(TraceType type) = 0;

    // Callback pattern for trace events
    using EligibilityUpdatedCallback = std::function<void(NeuronId neuron, float newValue)>;
    virtual void setEligibilityUpdatedCallback(const EligibilityUpdatedCallback& callback) = 0;

    // Debug information
    virtual std::string getDebugInfo() const = 0;

    // Performance metrics
    virtual float getAverageTraceValue() const = 0;
    virtual float getMaximumTraceValue() const = 0;
    virtual uint64_t getActiveTraceCount() const = 0;

protected:
    IEligibilityTrace() = default;
};

} // namespace nlm
