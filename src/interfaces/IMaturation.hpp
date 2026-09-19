// IMaturation.hpp
// Abstract interface for maturation systems
// Manages neuronal and synaptic maturation processes
// Refines connections and adjusts response properties

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
class Neuron;
class Synapse;

// Maturation parameter
struct MaturationParameter {
    std::string parameterName;
    float currentValue;
    float targetValue;
    float rate;
    SimulationStep startTime;
    SimulationStep endTime;
    bool isComplete;
    
    MaturationParameter() : currentValue(0), targetValue(0), rate(0), startTime(0), endTime(0), isComplete(false) {}
    MaturationParameter(const std::string& name, float start, float target, float r)
        : parameterName(name), currentValue(start), targetValue(target), rate(r),
          startTime(0), endTime(0), isComplete(false) {}
};

// Synaptic maturity state
struct SynapticMaturationState {
    SynapseId synapseId;
    NeuronId preNeuron;
    NeuronId postNeuron;
    float currentMaturity;
    float targetMaturity;
    float conductanceScale;
    float weightScale;
    SimulationStep lastUpdated;
    bool isMature;
    
    SynapticMaturationState() : currentMaturity(0), targetMaturity(1), conductanceScale(1),
                                weightScale(1), lastUpdated(0), isMature(false) {}
};

// Neuronal maturity state
struct NeuronalMaturationState {
    NeuronId neuronId;
    float membranePotentialMaturity;
    float thresholdMaturity;
    float refractoryPeriodMaturity;
    float excitabilityMaturity;
    SimulationStep lastUpdated;
    bool isMature;
    
    NeuronalMaturationState() : membranePotentialMaturity(0), thresholdMaturity(0),
                                 refractoryPeriodMaturity(0), excitabilityMaturity(0),
                                 lastUpdated(0), isMature(false) {}
};

class IMaturation {
public:
    virtual ~IMaturation() = default;

    // Initialize maturation system with brain reference
    virtual bool initialize(Brain* brain) = 0;

    // Update maturation processes
    virtual void update(TimestepDuration dt) = 0;

    // Mature a specific synapse
    virtual void matureSynapse(SynapseId synapse) = 0;

    // Mature a specific neuron
    virtual void matureNeuron(NeuronId neuron) = 0;

    // Check if synapse is mature
    virtual bool isSynapseMature(SynapseId synapse) const = 0;

    // Check if neuron is mature
    virtual bool isNeuronMature(NeuronId neuron) const = 0;

    // Get synapse maturation state
    virtual SynapticMaturationState getSynapticMaturationState(SynapseId synapse) const = 0;

    // Get neuron maturation state
    virtual NeuronalMaturationState getNeuronalMaturationState(NeuronId neuron) const = 0;

    // Set maturation parameters for synapse
    virtual void setSynapseMaturationParameters(SynapseId synapse,
                                                const MaturationParameter& conductance,
                                                const MaturationParameter& weight) = 0;

    // Set maturation parameters for neuron
    virtual void setNeuronMaturationParameters(NeuronId neuron,
                                               const MaturationParameter& threshold,
                                               const MaturationParameter& refractory,
                                               const MaturationParameter& excitability) = 0;

    // Get all immature synapses
    virtual std::vector<SynapseId> getImmatureSynapses() const = 0;

    // Get all immature neurons
    virtual std::vector<NeuronId> getImmatureNeurons() const = 0;

    // Reset maturation system
    virtual void reset() = 0;

    // Get maturation statistics
    virtual std::string getMaturationStats() const = 0;

    // Interface information
    virtual std::string getInterfaceName() const { return "IMaturation"; }
    virtual std::string getVersion() const { return "1.0.0"; }

    // Error handling
    virtual bool hasError() const = 0;
    virtual std::string getErrorMessage() const = 0;
    virtual void clearError() = 0;

    // Callback pattern for maturation events
    using SynapseMaturedCallback = std::function<void(SynapseId synapse, float maturity)>;
    virtual void setSynapseMaturedCallback(const SynapseMaturedCallback& callback) = 0;

    using NeuronMaturedCallback = std::function<void(NeuronId neuron, float maturity)>;
    virtual void setNeuronMaturedCallback(const NeuronMaturedCallback& callback) = 0;

    // Debug information
    virtual std::string getDebugInfo() const = 0;

    // Maturation parameters
    virtual float getSynapticMaturationTime() const = 0;
    virtual void setSynapticMaturationTime(float time) = 0;
    virtual float getNeuronalMaturationTime() const = 0;
    virtual void setNeuronalMaturationTime(float time) = 0;

    // Performance metrics
    virtual float getAverageSynapticMaturity() const = 0;
    virtual float getAverageNeuronalMaturity() const = 0;
    virtual uint64_t getTotalSynapsesMatured() const = 0;
    virtual uint64_t getTotalNeuronsMatured() const = 0;

    // Get current maturation progress (0.0 to 1.0)
    virtual float getMaturationProgress() const = 0;

protected:
    IMaturation() = default;
};

} // namespace nlm
