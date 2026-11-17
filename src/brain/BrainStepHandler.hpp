// Brain Step Handler
// Phase 2: Main simulation step logic

#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <chrono>

namespace nlm {

class BrainStepHandler {
public:
    BrainStepHandler();
    ~BrainStepHandler();
    
    // Main simulation step with timestamp
    void step(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Main simulation step (time derived from step)
    void step(Brain* brain, SimulationStep currentStep);
    
    // Process spike events for current step
    void processSpikeEvents(Brain* brain);
    
    // Update plasticity rules
    void updatePlasticity(Brain* brain);
    
    // Update development
    void updateDevelopment(Brain* brain, Timestamp timestep);
    
    // Update neuromodulation
    void updateNeuromodulation(Brain* brain, Timestamp timestep);
    
    // Update integrated systems
    void updateIntegratedSystems(Brain* brain, SimulationStep currentStep);
    
    // Reset step state
    void resetStepState(Brain* brain);
    
    // Log step statistics
    void logStepStats(Brain* brain, SimulationStep currentStep) const;
    
    // Validate step execution
    bool validateStepExecution(Brain* brain, SimulationStep currentStep) const;
    
private:
    // Step timing and state management
    struct StepState {
        Timestamp currentTime = 0.0;
        size_t totalSpikesProcessed = 0;
        size_t plasticityUpdates = 0;
        size_t developmentUpdates = 0;
        size_t neuromodulationUpdates = 0;
        double stepTime = 0.0;
        bool stepCompleted = false;
    };
    
    StepState stepState_;
    
    // Performance tracking
    struct PerformanceStats {
        double totalStepTime = 0.0;
        size_t stepsProcessed = 0;
        double averageStepTime = 0.0;
        size_t spikesProcessedPerStep = 0;
    };
    
    PerformanceStats perfStats_;
    
    // Event processing
    void processDelayedSpikes(Brain* brain);
    void processSynapticDelays(Brain* brain);
    void handleStructuralPlasticity(Brain* brain);
    
    // Integration system updates
    void updatePredictionSystem(Brain* brain, SimulationStep currentStep);
    void updateMemorySystems(Brain* brain);
    void updateCognitionSystems(Brain* brain);
    void updateWorldInterface(Brain* brain);
    
    // Statistics and metrics
    void calculatePerformanceMetrics(Brain* brain);
    void trackPlasticityEfficiency(Brain* brain);
    void trackNeuralDynamics(Brain* brain);
    
    // Error handling and recovery
    bool handleStepErrors(Brain* brain, const std::exception& e);
    void recoverFromStepErrors(Brain* brain);
    
    // Configuration-based step behavior
    bool shouldUpdatePlasticity(const Config* config) const;
    bool shouldUpdateDevelopment(const Config* config, Timestamp timestep) const;
    bool shouldUpdateNeuromodulation(const Config* config, Timestamp timestep) const;
    
    // Helper functions
    void setupStepEnvironment(Brain* brain, SimulationStep currentStep);
    void cleanupStepEnvironment(Brain* brain);
    Timestamp calculateCurrentTime(SimulationStep step, Timestamp timestep) const;
    
    // Statistics tracking
    struct StepMetrics {
        size_t spikesGenerated = 0;
        size_t neuronsFired = 0;
        size_t plasticityEvents = 0;
        size_t developmentEvents = 0;
        size_t neuromodulationEvents = 0;
        double synapticActivity = 0.0;
        float avgFiringRate = 0.0f;
        float excitationInhibitionRatio = 1.0f;
    };
    
    StepMetrics stepMetrics_;
};

} // namespace nlm