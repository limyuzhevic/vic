#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "NeuralRegion.hpp"
#include <memory>
#include <string>

namespace nlm {

// Forward declarations
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;

// Neural computation, LIF dynamics, spike processing, and plasticity
class BrainNeural {
public:
    BrainNeural() = default;
    ~BrainNeural() = default;
    
    // Main neural computation step (LIF dynamics, spike processing)
    void step(double timestep, SimulationStep currentStep, Timestamp currentTime,
              const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    
    // Receive sensory input from environment
    void receiveSensoryInput(const SensoryInput& input,
                           std::vector<Neuron*>& sensoryNeurons,
                           std::shared_ptr<class NeuralWorkingMemory> workingMemory);
    
    // Inject current into specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current,
                      const std::unordered_map<uint64_t, Neuron*>& neuronById);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current,
                               const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    
    // Spike system management
    void processSpikes(SimulationStep currentStep, SpikeSystem* spikeSystem);
    
    // Plasticity processing
    void applyPlasticity(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                       STDP* stdp, Hebbian* hebbian, float plasticityMod,
                       Timestamp currentTime);
    
    // Working memory updates
    void updateWorkingMemory(double timestep,
                           std::shared_ptr<class NeuralWorkingMemory> workingMemory,
                           const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    
    // Produce motor action based on neural activity
    std::unique_ptr<Action> produceAction(const std::vector<Neuron*>& motorNeurons);
    
    // Get firing neuron count
    size_t getFiringNeuronCount(const std::vector<Neuron*>& motorNeurons);
    
    // Get spike system accessor
    SpikeSystem* getSpikeSystem() const;
    
    // Get plasticity systems
    STDP* getSTDP() const;
    Hebbian* getHebbian() const;
    StructuralPlasticity* getStructuralPlasticity() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
