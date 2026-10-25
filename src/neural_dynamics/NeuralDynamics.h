#ifndef NEURAL_DYNAMICS_H
#define NEURAL_DYNAMICS_H

#include "brain/Neuron.hpp"
#include "brain/NeuralRegion.hpp"
#include "dynamics/SpikeSystem.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "core/SimulationClock/SimulationClock.hpp"

namespace nlm {

class NeuralDynamics {
public:
    virtual ~NeuralDynamics() = default;
    
    virtual void updateNeurons(const std::vector<std::unique_ptr<NeuralRegion>>& regions, 
                             Timestamp currentTime, TimestepDuration timestep) = 0;
    
    virtual void detectAndProcessSpikes(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                      SimulationStep currentStep, Timestamp currentTime,
                                      SpikeSystem& spikeSystem,
                                      std::unique_ptr<NeuralWorkingMemory>& workingMemory) = 0;
    
    virtual size_t countTotalSpikes() const = 0;
    
    // Spike handler registration
    virtual void registerSpikeHandler(std::function<void(const SpikeEvent&)> handler) = 0;
    virtual void registerDelayedSpikeHandler(std::function<void(const DelayedSpikeEvent&)> handler) = 0;
};

} // namespace nlm

#endif // NEURAL_DYNAMICS_H