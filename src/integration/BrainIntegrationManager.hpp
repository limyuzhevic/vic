// Brain Integration Manager - Core Integration Logic
// 
// This file provides comprehensive integration of all brain systems
// addressing the critical gaps identified in Phase 6 Final Audit.

#ifndef BRAIN_INTEGRATION_MANAGER_HPP
#define BRAIN_INTEGRATION_MANAGER_HPP

#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "development/DevelopmentSystem.hpp"
#include "performance/CheckpointSystem.hpp"

namespace nlm {

class BrainIntegrationManager {
public:
    BrainIntegrationManager(std::shared_ptr<Brain> brain,
                           std::shared_ptr<CheckpointSystem> checkpointSystem);
    
    // Main integration setup
    bool initializeIntegration();
    
    // Core integration updates (called in Brain::step)
    void updateIntegration(double currentTime, SimulationStep step);
    
    // System-specific integration methods
    void integrateMemorySystems(double currentTime);
    void integratePredictionSystem(double currentTime);
    void integrateCognitionSystems(double currentTime);
    void integrateNeuromodulation(double currentTime);
    void integrateDevelopment(double currentTime);
    void integratePersistence(double currentTime);
    
    // Integration verification
    bool verifyIntegration() const;
    IntegrationReport getIntegrationReport() const;
    
    // Configuration
    void enableMemoryStorage(bool enable);
    void enablePredictionTraining(bool enable);
    void enableCognitiveProcessing(bool enable);
    void enableFullNeuromodulation(bool enable);
    void enableAdvancedDevelopment(bool enable);
    void enablePersistence(bool enable);
    
    // Integration status
    bool isMemoryIntegrated() const { return memoryIntegrated; }
    bool isPredictionIntegrated() const { return predictionIntegrated; }
    bool isCognitiveIntegrated() const { return cognitiveIntegrated; }
    bool isNeuromodulationIntegrated() const { return neuromodulationIntegrated; }
    bool isDevelopmentIntegrated() const { return developmentIntegrated; }
    bool isPersistenceIntegrated() const { return persistenceIntegrated; }
    
    // Get integrated components
    std::shared_ptr<NeuralWorkingMemory> getWorkingMemory() const { return workingMemory; }
    std::shared_ptr<NeuralEpisodicMemory> getEpisodicMemory() const { return episodicMemory; }
    std::shared_ptr<PredictionSystem> getPredictionSystem() const { return predictionSystem; }
    std::shared_ptr<NeuralPlanner> getNeuralPlanner() const { return neuralPlanner; }
    std::shared_ptr<ConceptFormation> getConceptFormation() const { return conceptFormation; }
    
private:
    // Brain reference
    std::shared_ptr<Brain> brain;
    
    // Checkpoint system
    std::shared_ptr<CheckpointSystem> checkpointSystem;
    
    // Integrated components (owned by manager)
    std::shared_ptr<NeuralWorkingMemory> workingMemory;
    std::shared_ptr<NeuralEpisodicMemory> episodicMemory;
    std::shared_ptr<PredictionSystem> predictionSystem;
    std::shared_ptr<NeuralPlanner> neuralPlanner;
    std::shared_ptr<ConceptFormation> conceptFormation;
    
    // Integration flags
    bool memoryIntegrated;
    bool predictionIntegrated;
    bool cognitiveIntegrated;
    bool neuromodulationIntegrated;
    bool developmentIntegrated;
    bool persistenceIntegrated;
    
    // Integration state
    double lastIntegrationTime;
    IntegrationStepCount integrationSteps;
    
    // Helper methods
    void setupMemoryIntegration();
    void setupPredictionIntegration();
    void setupCognitiveIntegration();
    void setupNeuromodulationIntegration();
    void setupDevelopmentIntegration();
    void setupPersistenceIntegration();
    
    void processSensoryToMemory(const SensoryPercept& percept, double currentTime);
    void processMemoryToAttention(double currentTime);
    void processAttentionToConceptFormation(double currentTime);
    void processConceptFormationToPlanning(double currentTime);
    void processPlanningToAction(double currentTime);
    void processActionToSelfModel(double currentTime);
    void processExperienceToEpisodicMemory(double currentTime);
    void processEpisodicMemoryToConsolidation(double currentTime);
    
    // Memory consolidation and replay
    void runMemoryConsolidation(double currentTime);
    void runMemoryReplay(double currentTime);
    
    // Integration validation
    bool validateMemoryConnections() const;
    bool validatePredictionConnections() const;
    bool validateCognitiveConnections() const;
    bool validateNeuromodulationConnections() const;
    bool validateDevelopmentConnections() const;
    bool validatePersistenceConnections() const;
    
    // Statistics and reporting
    void updateIntegrationStatistics(double currentTime);
    IntegrationMetrics getCurrentMetrics() const;
};

// Integration report structure
struct IntegrationReport {
    // System status
    bool memorySystemStatus;
    bool predictionSystemStatus;
    bool cognitiveSystemStatus;
    bool neuromodulationSystemStatus;
    bool developmentSystemStatus;
    bool persistenceSystemStatus;
    
    // Performance metrics
    double memoryRetentionRate;
    double predictionAccuracy;
    double cognitiveProcessingSpeed;
    double neuromodulationEffectiveness;
    double developmentalProgress;
    double persistenceReliability;
    
    // Integration quality
    int connectivityScore;
    int functionalityScore;
    int stabilityScore;
    
    // Issues and warnings
    std::vector<std::string> warnings;
    std::vector<std::string> criticalIssues;
    
    // Summary
    std::string getSummary() const;
    bool isFullyIntegrated() const;
};

// Integration step counter
struct IntegrationStepCount {
    SimulationStep memoryUpdates;
    SimulationStep predictionUpdates;
    SimulationStep cognitiveUpdates;
    SimulationStep neuromodulationUpdates;
    SimulationStep developmentUpdates;
    SimulationStep persistenceUpdates;
    
    void incrementAll() {
        memoryUpdates++;
        predictionUpdates++;
        cognitiveUpdates++;
        neuromodulationUpdates++;
        developmentUpdates++;
        persistenceUpdates++;
    }
};

// Configuration for integration
struct IntegrationConfig {
    // Memory integration
    bool enableWorkingMemory;
    bool enableEpisodicMemory;
    bool enableSemanticMemory;
    bool enableProceduralMemory;
    
    // Prediction integration
    bool enablePredictionTraining;
    bool enableMultiStepPrediction;
    bool enableActionConsequencePrediction;
    
    // Cognitive integration
    bool enableNeuralPlanning;
    bool enableConceptFormation;
    bool enableSelfModel;
    bool enableSocialLearning;
    
    // Neuromodulation integration
    bool enableFullNeuromodulation;
    bool enableAChAttention;
    bool enableNEN arousal;
    bool enable5HT mood;
    
    // Development integration
    bool enableAdvancedDevelopment;
    bool enableBehavioralMaturation;
    bool enableCognitiveMaturation;
    
    // Persistence
    bool enablePersistence;
    bool enableAutoSave;
    bool enableVersionControl;
    
    // Integration parameters
    double memoryConsolidationInterval;
    double predictionTrainingInterval;
    double replayDuration;
    double developmentStageDuration;
    
    // Configuration validation
    bool validate() const;
};

inline namespace IntegrationPresets {
    // Preset configurations for different use cases
    IntegrationConfig CreateMinimalIntegration();
    IntegrationConfig CreateStandardIntegration();
    IntegrationConfig CreateAdvancedIntegration();
    IntegrationConfig CreateResearchIntegration();
}

} // namespace nlm

#endif // BRAIN_INTEGRATION_MANAGER_HPP
