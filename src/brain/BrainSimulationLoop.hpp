#pragma once

#include "Brain.hpp"
#include "BrainMemoryManager.hpp"
#include "BrainCognitionManager.hpp"
#include "BrainNeuromodulationManager.hpp"
#include "BrainDevelopmentManager.hpp"
#include "BrainPlasticityManager.hpp"
#include <memory>
#include <vector>

namespace nlm {

class BrainSimulationLoop {
public:
    BrainSimulationLoop(Brain* brain);
    ~BrainSimulationLoop();
    
    // Disable copying
    BrainSimulationLoop(const BrainSimulationLoop&) = delete;
    BrainSimulationLoop& operator=(const BrainSimulationLoop&) = delete;
    
    // Perform a complete simulation step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Process pending delayed spike events (deliver synaptic input)
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    
    // Update all neurons (LIF dynamics)
    void updateNeurons(Timestamp currentTime, TimestepDuration timestep);
    
    // Detect spikes and schedule spike events
    void detectSpikes(SimulationStep currentStep, Timestamp currentTime);
    
    // Process immediate spikes
    void processSpikes(SimulationStep currentStep);
    
    // Update neuromodulation effects
    void updateNeuromodulation(Timestamp currentTime, TimestepDuration timestep);
    
    // Apply plasticity rules (STDP and Hebbian)
    void applyPlasticity(Timestamp currentTime, float plasticityMod);
    
    // Update attention system
    void updateAttention(TimestepDuration timestep);
    
    // Update concept formation
    void updateConceptFormation();
    
    // Apply structural plasticity periodically
    void applyStructuralPlasticity(SimulationStep currentStep);
    
    // Replay important memories
    void replayMemories(SimulationStep currentStep);
    
    // Apply development effects
    void applyDevelopment(SimulationStep currentStep, Timestamp currentTime, TimestepDuration timestep);
    
    // Apply memory consolidation
    void applyMemoryConsolidation(SimulationStep currentStep);
    
    // Update checkpoint management
    void updateCheckpoints(SimulationStep currentStep, Timestamp currentTime);
    
    // Collect statistics
    void collectStatistics();
    
    // Get step statistics
    size_t getTotalSpikesThisStep() const;
    size_t getTotalSpikesTotal() const;
    
    // Get manager references
    BrainMemoryManager* getMemoryManager() { return memoryManager.get(); }
    BrainCognitionManager* getCognitionManager() { return cognitionManager.get(); }
    BrainNeuromodulationManager* getNeuromodulationManager() { return neuromodulationManager.get(); }
    BrainDevelopmentManager* getDevelopmentManager() { return developmentManager.get(); }
    BrainPlasticityManager* getPlasticityManager() { return plasticityManager.get(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
