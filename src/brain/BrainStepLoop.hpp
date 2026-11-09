#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include "BrainUtils.hpp"

namespace nlm {

// BrainStepLoop implementation - Main 15-step Phase 6 integration loop
// Extracted from Brain.cpp lines 343-692

class BrainStepLoop {
public:
    // Phase 6 integration loop - simplified from original 1591 line Brain.cpp
    static void executeIntegrationLoop(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime);
    
private:
    static void processDelayedSpikes(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime);
    static void updateNeurons(Brain::Impl* impl, Timestamp currentTime, Timestamp timestep);
    static void detectAndScheduleSpikes(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime);
    static void updateWorkingMemory(Brain::Impl* impl);
    static void applyNeuromodulationEffects(Brain::Impl* impl);
    static void applyPlasticity(Brain::Impl* impl, Timestamp currentTime);
    static void updateEpisodicMemory(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime);
    static void updatePredictionSystem(Brain::Impl* impl);
    static void updateAttentionSystem(Brain::Impl* impl);
    static void updateConceptFormation(Brain::Impl* impl);
    static void applyStructuralPlasticity(Brain::Impl* impl, SimulationStep currentStep);
    static void replayMemories(Brain::Impl* impl, SimulationStep currentStep);
    static void applyDevelopmentEffects(Brain::Impl* impl, SimulationStep currentStep);
    static void collectStatistics(Brain::Impl* impl);
};

} // namespace nlm
