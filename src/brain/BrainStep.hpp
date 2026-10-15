#pragma once

#include "Brain.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Brain step implementation declarations
// Refactored from Brain.cpp to improve maintainability

class BrainStep {
public:
    // Main step methods
    static void executeStep1_ProcessDelayedSpikes(std::shared_ptr<Brain::Impl> pImpl, 
                                                   SimulationStep step, Timestamp time);
    static void executeStep2_UpdateNeurons(std::shared_ptr<Brain::Impl> pImpl, Timestamp time);
    static void executeStep3_DetectAndScheduleSpikes(std::shared_ptr<Brain::Impl> pImpl, 
                                                     SimulationStep step, Timestamp time);
    static void executeStep4_UpdateWorkingMemory(std::shared_ptr<Brain::Impl> pImpl);
    static void executeStep5_ApplyNeuromodulation(std::shared_ptr<Brain::Impl> pImpl);
    static void executeStep6_ApplyPlasticity(std::shared_ptr<Brain::Impl> pImpl);
    static void executeStep7_UpdateEpisodicMemory(std::shared_ptr<Brain::Impl> pImpl);
    static void executeStep8_UpdatePredictionSystem(std::shared_ptr<Brain::Impl> pImpl);
    static void executeStep9_UpdateAttentionSystem(std::shared_ptr<Brain::Impl> pImpl);
    static void executeStep10_UpdateConceptFormation(std::shared_ptr<Brain::Impl> pImpl);
    static void executeStep11_ApplyStructuralPlasticity(std::shared_ptr<Brain::Impl> pImpl, SimulationStep step);
    static void executeStep12_ReplayMemories(std::shared_ptr<Brain::Impl> pImpl, SimulationStep step);
    static void executeStep13_ApplyDevelopmentEffects(std::shared_ptr<Brain::Impl> pImpl, SimulationStep step);
    static void executeStep14_MemoryConsolidation(std::shared_ptr<Brain::Impl> pImpl, SimulationStep step);
    static void executeStep15_CheckpointManagement(std::shared_ptr<Brain::Impl> pImpl, 
                                                    SimulationStep step, Timestamp time);
    
    static void collectStepStatistics(std::shared_ptr<Brain::Impl> pImpl);
};

}} // namespace nlm