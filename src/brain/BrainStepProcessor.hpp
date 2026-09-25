#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// BrainStepProcessor manages the main simulation step logic
class BrainStepProcessor {
public:
    BrainStepProcessor() = default;
    ~BrainStepProcessor() = default;
    
    // Main simulation step (full version with time)
    void processStep(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Main simulation step (simplified version)
    void processStep(Brain* brain, SimulationStep currentStep);
    
    // Process pending delayed spikes
    void processDelayedSpikes(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Update all neurons (LIF dynamics)
    void updateNeurons(Brain* brain, Timestamp currentTime, TimestepDuration timestep);
    
    // Detect spikes and schedule events
    void detectAndQueueSpikes(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Update working memory
    void updateWorkingMemory(Brain* brain, TimestepDuration timestep);
    
    // Apply neuromodulation effects
    void applyNeuromodulation(Brain* brain, TimestepDuration timestep);
    
    // Apply plasticity rules
    void applyPlasticity(Brain* brain, Timestamp currentTime, TimestepDuration timestep);
    
    // Update episodic memory
    void updateEpisodicMemory(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Update prediction system
    void updatePredictionSystem(Brain* brain);
    
    // Update attention system
    void updateAttentionSystem(Brain* brain, TimestepDuration timestep);
    
    // Update concept formation
    void updateConceptFormation(Brain* brain);
    
    // Apply structural plasticity periodically
    void applyStructuralPlasticity(Brain* brain, SimulationStep currentStep);
    
    // Replay important memories
    void replayMemories(Brain* brain, SimulationStep currentStep);
    
    // Apply development effects
    void applyDevelopmentEffects(Brain* brain, SimulationStep currentStep, TimestepDuration timestep);
    
    // Memory consolidation
    void consolidateMemories(Brain* brain, SimulationStep currentStep);
    
    // Update checkpoint system
    void updateCheckpoints(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Collect statistics
    void collectStatistics(Brain* brain);
};

} // namespace nlm
