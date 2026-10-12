#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Forward declarations
class Brain;
class SimulationClock;

// ==========================================
// NEURAL ENGINE - Core Neural Computation
// ==========================================
// 
// Manages: neurons, synapses, spiking, plasticity
// 
// Contains: SpikeSystem, STDP, Hebbian, StructuralPlasticity
// Handles: basic neural dynamics and connectivity

class NeuralEngine {
public:
    NeuralEngine(std::shared_ptr<Config> config);
    ~NeuralEngine();
    
    // Disable copying, enable moving
    NeuralEngine(const NeuralEngine&) = delete;
    NeuralEngine& operator=(const NeuralEngine&) = delete;
    NeuralEngine(NeuralEngine&&) noexcept;
    NeuralEngine& operator=(NeuralEngine&&) noexcept;
    
    // Initialize neural engine with brain reference
    bool initialize(Brain* brain);
    
    // Get core neural computation components
    class SpikeSystem* getSpikeSystem() { return spikeSystem_.get(); }
    const class SpikeSystem* getSpikeSystem() const { return spikeSystem_.get(); }
    
    class STDP* getSTDP() { return stdp_.get(); }
    const class STDP* getSTDP() const { return stdp_.get(); }
    
    class Hebbian* getHebbian() { return hebbian_.get(); }
    const class Hebbian* getHebbian() const { return hebbian_.get(); }
    
    class StructuralPlasticity* getStructuralPlasticity() { return structuralPlasticity_.get(); }
    const class StructuralPlasticity* getStructuralPlasticity() const { return structuralPlasticity_.get(); }
    
    // Get brain reference
    Brain* getBrain() { return brain_; }
    const Brain* getBrain() const { return brain_; }
    
    // Process simulation step
    void step(SimulationStep step, Timestamp time, float timestep);
    
    // Connect neurons with plasticity
    void connectNeurons(NeuronId source, NeuronId destination, float weight, 
                       Delay delay = 1, bool isExcitatory = true,
                       PlasticityFlags plasticity = PlasticityFlags());
    
    // Process synaptic connections
    void processDelayedSpikes(SimulationStep step, Timestamp time);
    void processSpikes(SimulationStep step);
    
    // Update plasticity
    void updatePlasticity();
    
    // Get statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Apply neuromodulation factor to plasticity
    void applyNeuromodulationFactor(float factor);
    
    // Get random generator
    RandomGenerator* getRandomGenerator() { return rng_.get(); }
    
    // Reset neural engine
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::shared_ptr<Config> config_;
    std::unique_ptr<RandomGenerator> rng_;
    
    std::unique_ptr<class SpikeSystem> spikeSystem_;
    std::unique_ptr<class STDP> stdp_;
    std::unique_ptr<class Hebbian> hebbian_;
    std::unique_ptr<class StructuralPlasticity> structuralPlasticity_;
};

} // namespace nlm