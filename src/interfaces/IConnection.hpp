// IConnection.hpp
// Abstract interface for synaptic connections
// Manages spike transmission between neurons with synaptic dynamics
// Supports various synapse types and plasticity mechanisms

#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>
#include <functional>
#include <string>
#include <atomic>

namespace nlm {

// Forward declarations
class Neuron;
class Brain;

class IConnection {
public:
    virtual ~IConnection() = default;

    // Initialize connection with brain reference
    virtual bool initialize(Brain* brain) = 0;

    // Process incoming spike from source neuron
    virtual void processSpike(const SpikeEvent& spike) = 0;

    // Get synaptic response to spike (post-synaptic potential)
    virtual float getSynapticResponse(const SpikeEvent& spike) const = 0;

    // Update connection dynamics (post-synaptic potential decay)
    virtual void update(TimestepDuration dt) = 0;

    // Get current post-synaptic potential
    virtual float getPostSynapticPotential() const = 0;

    // Set/Get synaptic weight
    virtual float getWeight() const = 0;
    virtual void setWeight(float weight) = 0;

    // Get/Get synaptic type
    virtual SynapseType getType() const = 0;
    virtual void setType(SynapseType type) = 0;

    // Get/Get connection delay
    virtual Delay getDelay() const = 0;
    virtual void setDelay(Delay delay) = 0;

    // Get/Get source neuron handle
    virtual NeuronId getSourceNeuron() const = 0;
    virtual void setSourceNeuron(NeuronId neuron) = 0;

    // Get/Get target neuron handle
    virtual NeuronId getTargetNeuron() const = 0;
    virtual void setTargetNeuron(NeuronId neuron) = 0;

    // Get connection ID
    virtual SynapseId getId() const = 0;

    // Check if connection is active
    virtual bool isActive() const = 0;
    virtual void setActive(bool active) = 0;

    // Apply plasticity to connection
    virtual void applyPlasticity(float weightChange) = 0;

    // Get plasticity flags for this connection
    virtual const PlasticityFlags& getPlasticityFlags() const = 0;
    virtual PlasticityFlags& getPlasticityFlags() = 0;

    // Enable/disable specific plasticity types
    virtual void enableHebbian(bool enable) = 0;
    virtual void enableSTDP(bool enable) = 0;
    virtual void enableRewardModulation(bool enable) = 0;
    virtual void enableStructuralPlasticity(bool enable) = 0;

    // Get connection strength (effective weight)
    virtual float getConnectionStrength() const = 0;

    // Get transmission probability for stochastic connections
    virtual float getTransmissionProbability() const = 0;
    virtual void setTransmissionProbability(float prob) = 0;

    // Get statistics
    virtual std::string getConnectionStats() const = 0;

    // Interface information
    virtual std::string getInterfaceName() const { return "IConnection"; }
    virtual std::string getVersion() const { return "1.0.0"; }

    // Error handling
    virtual bool hasError() const = 0;
    virtual std::string getErrorMessage() const = 0;
    virtual void clearError() = 0;

    // Callback pattern for spike events
    using SpikeProcessedCallback = std::function<void(const SpikeEvent& spike,
                                                     float synapticResponse)>;
    virtual void setSpikeProcessedCallback(const SpikeProcessedCallback& callback) = 0;

    // Debug information
    virtual std::string getDebugInfo() const = 0;

    // Performance metrics
    virtual float getLastResponseTime() const = 0;
    virtual float getAverageResponse() const = 0;
    virtual uint64_t getTotalSpikesProcessed() const = 0;

protected:
    IConnection() = default;
};

} // namespace nlm
