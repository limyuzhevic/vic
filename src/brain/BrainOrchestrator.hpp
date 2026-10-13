#pragma once

#include "../brain/Brain.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <chrono>

namespace nlm {

/**
 * BrainOrchestrator - Orchestrates brain step processing by delegating to specialized systems
 * This refactoring reduces Brain.cpp complexity from ~1117 to ~200 lines by separating concerns
 * into focused controller classes for each major responsibility.
 */
class BrainOrchestrator {
public:
    BrainOrchestrator(std::shared_ptr<Brain> brain);
    ~BrainOrchestrator();
    
    // Disable copying, enable moving
    BrainOrchestrator(const BrainOrchestrator&) = delete;
    BrainOrchestrator& operator=(const BrainOrchestrator&) = delete;
    BrainOrchestrator(BrainOrchestrator&&) noexcept;
    BrainOrchestrator& operator=(BrainOrchestrator&&) noexcept;
    
    // Main orchestration logic - delegates to specialized controllers
    void executeStep(SimulationStep step, Timestamp time);
    
    // Get orchestration statistics
    const StepStatistics& getStepStatistics() const { return stats_; }
    void resetStatistics() { stats_ = StepStatistics(); }
    
    // Get total spikes for this step
    size_t getTotalSpikesThisStep() const { return stats_.totalSpikesThisStep; }
    
private:
    // Forward declarations for controllers
    class NeuralStepController;
    class MemoryIntegrationController;
    class NeuromodulationController;
    class PlasticityController;
    class CognitionController;
    class DevelopmentController;
    class CheckpointController;
    
    std::shared_ptr<Brain> brain_;
    
    // Controllers for each major responsibility
    NeuralStepController neuralController_;
    MemoryIntegrationController memoryController_;
    NeuromodulationController neuromodController_;
    PlasticityController plasticityController_;
    CognitionController cognitionController_;
    DevelopmentController developmentController_;
    CheckpointController checkpointController_;
    
    // Statistics tracking
    struct StepStatistics {
        size_t neuralStepsCompleted = 0;
        size_t memoryUpdatesCompleted = 0;
        size_t neuromodulationUpdatesCompleted = 0;
        size_t plasticityUpdatesCompleted = 0;
        size_t cognitionUpdatesCompleted = 0;
        size_t developmentUpdatesCompleted = 0;
        size_t checkpointUpdatesCompleted = 0;
        size_t totalSpikesThisStep = 0;
        double orchestrationTime = 0.0;
        Timestamp stepTime = 0.0;
    };
    
    StepStatistics stats_;
    std::chrono::high_resolution_clock::time_point startTime_;
};

} // namespace nlm