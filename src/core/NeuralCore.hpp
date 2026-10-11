// NeuralCore - Handles neuron updates, spike detection, and LIF dynamics
// Extracted from Brain::step() method
#pragma once

#include "../brain/Brain.hpp"
#include "../dynamics/NeuralDynamics.hpp"
#include <memory>

namespace nlm {

class NeuralCore {
public:
    NeuralCore(std::shared_ptr<Brain> brain);
    ~NeuralCore();
    
    // Initialize the neural core
    bool initialize();
    
    // Process one timestep of neural computation
    void step(SimulationStep currentStep, Timestamp currentTime, TimestepDuration timestep);
    
    // Get spike system for event handling
    SpikeSystem* getSpikeSystem() const { return spikeSystem_.get(); }
    
    // Get statistics
    size_t getTotalSpikeCount() const { return totalSpikes_; }
    size_t getActiveNeuronCount() const { return activeNeurons_; }
    
    // Access to brain regions and neurons
    Brain* getBrain() { return brain_.get(); }
    const Brain* getBrain() const { return brain_.get(); }
    
private:
    std::shared_ptr<Brain> brain_;
    std::unique_ptr<SpikeSystem> spikeSystem_;
    std::unique_ptr<IntegrateAndFireDynamics> neuralDynamics_;
    
    size_t totalSpikes_;
    size_t activeNeurons_;
    
    // Helper methods
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    void updateAllNeurons(TimestepDuration timestep);
    void detectAndScheduleSpikes(SimulationStep currentStep, Timestamp currentTime);
    void recordPlasticityData(SimulationStep currentStep, Timestamp currentTime);
};

} // namespace nlm
