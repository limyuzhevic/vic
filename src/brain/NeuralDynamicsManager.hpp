#pragma once

#include "Brain.hpp"
#include <unordered_map>
#include <memory>

namespace nlm {

class NeuralDynamicsManager {
public:
    NeuralDynamicsManager(Brain* brain);
    ~NeuralDynamicsManager();
    
    bool initialize();
    void step(SimulationStep currentStep, Timestamp currentTime);
    void reset();
    
    // Accessors for backward compatibility
    SpikeSystem* getSpikeSystem() { return spikeSystem.get(); }
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Neural processing
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    void detectAndScheduleSpikes(SimulationStep currentStep, Timestamp currentTime);
    void processSpikes(SimulationStep currentStep);
    void updateAllNeurons(SimulationStep currentStep, Timestamp currentTime);
    
private:
    Brain* brain_;
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unordered_map<uint64_t, Neuron*> neuronById;
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    bool spikeHandlersRegistered;
};

} // namespace nlm
