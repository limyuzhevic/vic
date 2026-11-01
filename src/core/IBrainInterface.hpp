// Brain interface for loose coupling between systems
#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

class IBrainInterface {
public:
    virtual ~IBrainInterface() = default;
    
    // === Core Brain Information ===
    virtual uint64_t getNeuronCount() const = 0;
    virtual uint64_t getSynapseCount() const = 0;
    virtual uint64_t getRegionCount() const = 0;
    virtual SimulationStep getCurrentStep() const = 0;
    virtual Timestamp getCurrentTime() const = 0;
    
    // === Neural System Access ===
    virtual SpikeSystem* getSpikeSystem() = 0;
    virtual STDP* getSTDP() = 0;
    virtual Hebbian* getHebbian() = 0;
    virtual StructuralPlasticity* getStructuralPlasticity() = 0;
    
    // === System Integration ===
    virtual void registerForSpikes(std::function<void(const DetailedSpikeEvent&)> handler) = 0;
    virtual void registerForDelayedSpikes(std::function<void(const DelayedSpikeEvent&)> handler) = 0;
    
    // === State Management ===
    virtual void advanceStep(SimulationStep step, Timestamp time) = 0;
    virtual void reset() = 0;
    
    // === Statistics ===
    virtual size_t getTotalSpikeCount() const = 0;
    virtual size_t getFiringNeuronCount() const = 0;
    virtual float getAverageFiringRate() const = 0;
    
    // === Event Notification ===
    virtual void notifyNeuronFiring(const NeuronId& neuron) = 0;
    virtual void notifySynapseUpdated(const SynapseId& synapse, float oldWeight, float newWeight) = 0;
    virtual void notifyMemoryUpdated(const MemoryEvent& event) = 0;
    
    // === Configuration ===
    virtual std::shared_ptr<const Config> getConfig() const = 0;
};

} // namespace nlm
