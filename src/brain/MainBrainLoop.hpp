#pragma once

#include "Brain.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <chrono>
#include <functional>

namespace nlm {

class MainBrainLoop {
public:
    MainBrainLoop() = default;
    
    void initialize(Brain* brain);
    
    // Execute the complete 15-step brain integration loop
    void executeStep(Brain& brain, SimulationStep currentStep, Timestamp currentTime);
    void executeFullIntegrationCycle(Brain& brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Step-by-step integration as described in the original implementation
    void step1_ProcessDelayedSpikes(Brain& brain, SimulationStep currentStep, Timestamp currentTime);
    void step2_UpdateNeurons(Brain& brain, SimulationStep currentStep, Timestamp currentTime);
    void step3_DetectAndQueueSpikes(Brain& brain, SimulationStep currentStep, Timestamp currentTime);
    void step4_UpdateWorkingMemory(Brain& brain, TimestepDuration dt);
    void step5_ApplyNeuromodulation(Brain& brain, TimestepDuration dt);
    void step6_ApplyPlasticity(Brain& brain, TimestepDuration dt);
    void step7_UpdateEpisodicMemory(Brain& brain, SimulationStep currentStep, Timestamp currentTime);
    void step8_UpdatePredictionSystem(Brain& brain);
    void step9_UpdateAttentionSystem(Brain& brain, TimestepDuration dt);
    void step10_UpdateConceptFormation(Brain& brain);
    void step11_ApplyStructuralPlasticity(Brain& brain, SimulationStep currentStep);
    void step12_ReplayImportantMemories(Brain& brain, SimulationStep currentStep);
    void step13_ApplyDevelopmentEffects(Brain& brain, SimulationStep currentStep);
    void step14_PeriodicMemoryConsolidation(Brain& brain, SimulationStep currentStep);
    void step15_CheckpointManagement(Brain& brain, SimulationStep currentStep, Timestamp currentTime);
    
    // System coordination
    void coordinateSystems(Brain& brain);
    void manageEventDrivenProcessing(Brain& brain, SimulationStep currentStep);
    void manageTimeSensitiveOperations(Brain& brain, Timestamp currentTime);
    
    // Event-driven processing
    void processEvents(Brain& brain, SimulationStep currentStep);
    void scheduleEvents(Brain& brain, SimulationStep currentStep);
    void handleEmergencyEvents(Brain& brain);
    
    // Statistics and logging
    void updateBrainStatistics(Brain& brain);
    void logIntegrationStatus(Brain& brain, SimulationStep currentStep) const;
    
    // System health monitoring
    void monitorSystemHealth(Brain& brain);
    bool checkSystemIntegrity(Brain& brain) const;
    void handleSystemErrors(Brain& brain, const std::string& error);
    
    void logMainBrainLoopStatus(Brain& brain) const;
    
private:
    // Event-driven computation
    struct ScheduledEvent {
        Timestamp time;
        std::function<void(Brain&)> handler;
        std::string name;
        bool executed;
    };
    
    std::vector<ScheduledEvent> scheduledEvents_;
    std::chrono::steady_clock::time_point lastStepTime_;
    
    // System integration management
    void integrateMemorySystems(Brain& brain);
    void integratePredictionSystem(Brain& brain);
    void integrateCognitionSystems(Brain& brain);
    void integrateNeuromodulationSystems(Brain& brain);
    void integrateDevelopmentSystem(Brain& brain);
    
    // Optimization and performance
    void optimizeNeuralProcessing(Brain& brain);
    void balanceSystemLoad(Brain& brain);
    
    // Error recovery
    void implementErrorRecovery(Brain& brain);
    void performSelfHealing(Brain& brain);
    
    // Timing and coordination
    void syncSystemTimings(Brain& brain);
    void manageResourceAllocation(Brain& brain);
    
    // Validation and verification
    void validateSystemState(Brain& brain);
    void verifyOutputConsistency(Brain& brain);
};

} // namespace nlm
