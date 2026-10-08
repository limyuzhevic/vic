#pragma once

#include "Brain.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../core/Logger/Logger.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <memory>

namespace nlm {

class SimulationController {
public:
    SimulationController(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    
    // Run one complete simulation step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Get current step count
    SimulationStep getCurrentStep() const { return currentStep; }
    
    // Get current time
    Timestamp getCurrentTime() const { return currentTime; }
    
    // Reset simulation
    void reset() {
        currentStep = 0;
        currentTime = 0.0;
        totalSpikesThisStep = 0;
    }
    
    // Get spike statistics
    size_t getTotalSpikesThisStep() const { return totalSpikesThisStep; }
    
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    std::unique_ptr<SimulationClock> clock;
    std::unique_ptr<Logger> logger;
    
    // Simulation state
    SimulationStep currentStep;
    Timestamp currentTime;
    size_t totalSpikesThisStep;
    
    // Execute individual simulation steps
    void executeStep1_SpikeDelivery();  // Process delayed spike events
    void executeStep2_NeuronUpdate();   // Update neuron membrane potentials
    void executeStep3_SpikeDetection(); // Detect and queue spikes
    void executeStep4_WorkingMemory();  // Update working memory
    void executeStep5_Neuromodulation(); // Apply neuromodulation effects
    void executeStep6_Plasticity();    // Apply STDP and Hebbian learning
    void executeStep7_EpisodicMemory(); // Update episodic memory
    void executeStep8_PredictionSystem(); // Update prediction system
    void executeStep9_Attention();      // Update attention system
    void executeStep10_ConceptFormation(); // Update concept formation
    void executeStep11_StructuralPlasticity(); // Apply structural changes
    void executeStep12_MemoryReplay();   // Replay important memories
    void executeStep13_Development();   // Apply development effects
    void executeStep14_Consolidation(); // Memory consolidation
    void executeStep15_Checkpoint();   // Checkpoint management
};

} // namespace nlm