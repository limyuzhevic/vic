#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <functional>
#include <vector>
#include <string>
#include <chrono>

namespace nlm {

// Forward declarations
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;

class BrainOrchestrator {
public:
    // Create orchestrator for a brain
    explicit BrainOrchestrator(std::shared_ptr<Brain> brain);
    ~BrainOrchestrator();
    
    // Disable copying
    BrainOrchestrator(const BrainOrchestrator&) = delete;
    BrainOrchestrator& operator=(const BrainOrchestrator&) = delete;
    
    // Enable moving
    BrainOrchestrator(BrainOrchestrator&&) noexcept;
    BrainOrchestrator& operator=(BrainOrchestrator&&) noexcept;
    
    // Initialize all systems
    bool initialize();
    
    // Execute one complete simulation step
    void executeStep(SimulationStep currentStep, Timestamp currentTime);
    
    // System-specific orchestration methods
    void orchestrateMemorySystems(SimulationStep currentStep);
    void orchestratePredictionSystem(SimulationStep currentStep, Timestamp currentTime);
    void orchestrateCognitionSystems(SimulationStep currentStep, Timestamp currentTime);
    void orchestrateNeuromodulation(SimulationStep currentStep, Timestamp currentTime);
    void orchestrateDevelopment(SimulationStep currentStep, Timestamp currentTime);
    void orchestratePlasticity(SimulationStep currentStep, float plasticityMod);
    void orchestrateReplayAndConsolidation(SimulationStep currentStep);
    
    // Get orchestrator's brain reference
    Brain* getBrain() { return brain_.get(); }
    const Brain* getBrain() const { return brain_.get(); }
    
    // Configuration and settings
    void setReplayInterval(size_t interval) { replayInterval_ = interval; }
    void setConsolidationInterval(size_t interval) { consolidationInterval_ = interval; }
    size_t getReplayInterval() const { return replayInterval_; }
    size_t getConsolidationInterval() const { return consolidationInterval_; }
    
    // Performance and metrics
    float getAverageStepTime() const { return averageStepTime_; }
    size_t getTotalStepCount() const { return totalStepsExecuted_; }
    float getSpikesPerStep() const { return averageSpikesPerStep_; }
    
    // Statistics and monitoring
    void logSystemStatus() const;
    void resetMetrics();
    
    // Custom orchestration hooks for plugins
    void addPreStepHook(std::function<void(SimulationStep, Timestamp)> hook) {
        preStepHooks_.push_back(std::move(hook));
    }
    void addPostStepHook(std::function<void(SimulationStep, Timestamp)> hook) {
        postStepHooks_.push_back(std::move(hook));
    }
    void addSystemHook(std::function<void(std::string, SimulationStep)> hook) {
        systemHooks_.push_back(std::move(hook));
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::shared_ptr<Brain> brain_;
    
    // Orchestration configuration
    size_t replayInterval_;
    size_t consolidationInterval_;
    
    // Performance metrics
    float averageStepTime_;
    size_t totalStepsExecuted_;
    float averageSpikesPerStep_;
    
    // Custom hooks for extensibility
    std::vector<std::function<void(SimulationStep, Timestamp)>> preStepHooks_;
    std::vector<std::function<void(SimulationStep, Timestamp)>> postStepHooks_;
    std::vector<std::function<void(std::string, SimulationStep)>> systemHooks_;
};

} // namespace nlm
