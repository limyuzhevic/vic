// IDevelopmentSystem.hpp
// Abstract interface for development systems
// Manages brain development, maturation, and synaptic refinement
// Controls plasticity during critical periods

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

// Development stage
struct DevelopmentStage {
    DevelopmentalStage stage;
    SimulationStep startTime;
    SimulationStep endTime;
    float plasticityModifier;  // Factor affecting plasticity strength
    bool isActive;
    
    DevelopmentStage() : stage(DevelopmentalStage::Initial), startTime(0), endTime(0),
                         plasticityModifier(1.0f), isActive(false) {}
};

// Synaptogenesis event
struct SynaptogenesisEvent {
    SynapseId synapseId;
    NeuronId preNeuron;
    NeuronId postNeuron;
    SimulationStep timestamp;
    float initialStrength;
    float maturityFactor;
    bool completed;
    
    SynaptogenesisEvent() : synapseId(), initialStrength(0), maturityFactor(1), completed(false) {}
};

// Pruning event
struct PruningEvent {
    SynapseId synapseId;
    NeuronId preNeuron;
    NeuronId postNeuron;
    SimulationStep timestamp;
    float strength;
    float importanceScore;
    bool wasPruned;
    
    PruningEvent() : synapseId(), strength(0), importanceScore(0), wasPruned(false) {}
};

class IDevelopmentSystem {
public:
    virtual ~IDevelopmentSystem() = default;

    // Initialize development system with brain reference
    virtual bool initialize(Brain* brain) = 0;

    // Update development (synaptogenesis, pruning, maturation)
    virtual void update(TimestepDuration dt) = 0;

    // Get current developmental stage
    virtual DevelopmentalStage getCurrentStage() const = 0;

    // Set developmental stage
    virtual void setDevelopmentalStage(DevelopmentalStage stage) = 0;

    // Advance to next developmental stage
    virtual void advanceStage() = 0;

    // Get stage duration
    virtual float getStageDuration(DevelopmentalStage stage) const = 0;
    virtual void setStageDuration(DevelopmentalStage stage, float duration) = 0;

    // Get plasticity modifier for current stage
    virtual float getCurrentPlasticityModifier() const = 0;

    // Schedule synaptogenesis event
    virtual SynaptogenesisEvent scheduleSynaptogenesis(NeuronId pre, NeuronId post,
                                                         float initialStrength = 0.1f) = 0;

    // Schedule pruning event
    virtual PruningEvent schedulePruning(SynapseId synapse, float importance = 0.5f) = 0;

    // Get all scheduled events
    virtual std::vector<SynaptogenesisEvent> getScheduledSynaptogenesis() const = 0;
    virtual std::vector<PruningEvent> getScheduledPruning() const = 0;

    // Get completed events
    virtual std::vector<SynaptogenesisEvent> getCompletedSynaptogenesis() const = 0;
    virtual std::vector<PruningEvent> getCompletedPruning() const = 0;

    // Reset development system
    virtual void reset() = 0;

    // Get development statistics
    virtual std::string getDevelopmentStats() const = 0;

    // Interface information
    virtual std::string getInterfaceName() const { return "IDevelopmentSystem"; }
    virtual std::string getVersion() const { return "1.0.0"; }

    // Error handling
    virtual bool hasError() const = 0;
    virtual std::string getErrorMessage() const = 0;
    virtual void clearError() = 0;

    // Callback pattern for development events
    using SynaptogenesisCompletedCallback = std::function<void(const SynaptogenesisEvent& event)>;
    virtual void setSynaptogenesisCompletedCallback(const SynaptogenesisCompletedCallback& callback) = 0;

    using PruningCompletedCallback = std::function<void(const PruningEvent& event)>;
    virtual void setPruningCompletedCallback(const PruningCompletedCallback& callback) = 0;

    // Debug information
    virtual std::string getDebugInfo() const = 0;

    // Development parameters
    virtual float getSynaptogenesisRate() const = 0;
    virtual void setSynaptogenesisRate(float rate) = 0;
    virtual float getPruningRate() const = 0;
    virtual void setPruningRate(float rate) = 0;
    virtual float getMaturationRate() const = 0;
    virtual void setMaturationRate(float rate) = 0;

    // Performance metrics
    virtual uint64_t getTotalSynaptogenesisEvents() const = 0;
    virtual uint64_t getTotalPruningEvents() const = 0;
    virtual float getCurrentBrainMaturity() const = 0;  // 0.0 to 1.0

protected:
    IDevelopmentSystem() = default;
};

} // namespace nlm
