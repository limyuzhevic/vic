#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "ISubsystem.hpp"
#include <vector>
#include <memory>

namespace nlm {

class NeuralComputationSystem : public ISubsystem {
public:
    NeuralComputationSystem();
    ~NeuralComputationSystem() override;
    
    // ISubsystem interface
    bool initialize(Brain* brain) override;
    void update(float dt) override;
    void reset() override;
    void logStatus() const override;
    
    // Spike system management
    SpikeSystem* getSpikeSystem() const;
    
    // Plasticity rules access
    STDP* getSTDP() const;
    Hebbian* getHebbian() const;
    StructuralPlasticity* getStructuralPlasticity() const;
    
    // Neural computation methods
    void simulateNeurons(SimulationStep step, Timestamp time);
    void processSpikes(SimulationStep step);
    void deliverDelayedSpikes(SimulationStep step, Timestamp time);
    void computeSynapticInputs();
    void detectAndQueueSpikes(SimulationStep step, Timestamp time);
    
    // Configuration and statistics
    void configure(float timestep, float connectionProbability);
    float getTimestep() const;
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    
    // Sensory and motor neuron collections
    const std::vector<Neuron*>& getSensoryNeurons() const;
    const std::vector<Neuron*>& getMotorNeurons() const;
    
    // Input injection methods
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    float getAverageFiringRate() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
