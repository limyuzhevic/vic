#pragma once

#include "Brain.hpp"
#include "../core/Types/Types.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <memory>
#include <vector>

namespace nlm {

class BrainIntegration {
public:
    BrainIntegration() = default;
    ~BrainIntegration() = default;
    
    // System integration and initialization
    void initializeIntegrationSystems(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    void setupInterSystemConnections(std::shared_ptr<Brain> brain);
    
    // Integrated memory systems management
    void initializeMemorySystems(std::shared_ptr<Brain> brain, size_t neuronCount);
    void updateMemorySystems(std::shared_ptr<Brain> brain, SimulationStep currentStep, 
                           Timestamp currentTime);
    void integrateMemoryWithNeurons(std::shared_ptr<Brain> brain);
    
    // Prediction system integration
    void initializePredictionSystem(std::shared_ptr<Brain> brain);
    void updatePredictionSystem(std::shared_ptr<Brain> brain);
    void integratePredictionWithLearning(std::shared_ptr<Brain> brain);
    
    // Cognition systems integration
    void initializeCognitionSystems(std::shared_ptr<Brain> brain);
    void updateCognitionSystems(std::shared_ptr<Brain> brain, SimulationStep currentStep);
    void integrateCognitionWithMemory(std::shared_ptr<Brain> brain);
    
    // Neuromodulation integration
    void initializeNeuromodulationSystems(std::shared_ptr<Brain> brain);
    void updateNeuromodulationSystems(std::shared_ptr<Brain> brain);
    void integrateNeuromodulationWithPlasticity(std::shared_ptr<Brain> brain);
    
    // Development system integration
    void initializeDevelopmentSystem(std::shared_ptr<Brain> brain);
    void updateDevelopmentSystem(std::shared_ptr<Brain> brain, SimulationStep currentStep, 
                                Timestamp currentTime, TimestepDuration timestep);
    void integrateDevelopmentWithPlasticity(std::shared_ptr<Brain> brain, DevelopmentStage stage);
    
    // Checkpoint and persistence
    void setupCheckpointSystem(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    bool saveState(std::shared_ptr<Brain> brain, const std::string& filepath) const;
    bool loadState(std::shared_ptr<Brain> brain, const std::string& filepath);
    
    // Memory replay and consolidation
    void updateMemoryReplay(std::shared_ptr<Brain> brain, SimulationStep currentStep);
    void updateMemoryConsolidation(std::shared_ptr<Brain> brain, SimulationStep currentStep);
    void integrateSleepCycle(std::shared_ptr<Brain> brain, SimulationStep currentStep);
    
    // Statistics and monitoring
    void updateIntegrationStatistics(std::shared_ptr<Brain> brain);
    void logIntegrationStatus(const Brain* brain) const;
    
    // State management
    void resetAllIntegratedSystems(std::shared_ptr<Brain> brain);
    void updatePlasticityFromNeuromodulation(std::shared_ptr<Brain> brain);
    void applyDevelopmentEffectsToPlasticity(std::shared_ptr<Brain> brain, DevelopmentalStage stage);
    
private:
    // Helper methods
    void setupMemoryNeurons(std::shared_ptr<Brain> brain);
    void setupPredictionNeurons(std::shared_ptr<Brain> brain);
    void setupCognitionNeurons(std::shared_ptr<Brain> brain);
    void setupNeuromodulationNeurons(std::shared_ptr<Brain> brain);
    
    void calculateAttentionWeights(std::shared_ptr<Brain> brain);
    void calculatePredictionErrors(std::shared_ptr<Brain> brain);
    void calculateNoveltySignals(std::shared_ptr<Brain> brain);
    void calculateCuriositySignals(std::shared_ptr<Brain> brain);
    
    // Development stage updates
    void updateCriticalPeriodEffects(std::shared_ptr<Brain> brain);
    void updateMaturationEffects(std::shared_ptr<Brain> brain);
    void updateAdultStabilityEffects(std::shared_ptr<Brain> brain);
    
    // Configuration helpers
    size_t getReplayInterval(std::shared_ptr<Config> config) const;
    size_t getConsolidationInterval(std::shared_ptr<Config> config) const;
    float getSynaptogenesisRate(std::shared_ptr<Config> config, DevelopmentalStage stage) const;
    float getPruningRate(std::shared_ptr<Config> config, DevelopmentalStage stage) const;
};

} // namespace nlm