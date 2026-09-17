#pragma once

/**
 * Phase6IntegratedExperiment - Phase 6 final integration test
 * 
 * This experiment demonstrates that the integrated brain systems work together:
 * - Memory systems are connected to neural processing
 * - Neuromodulation affects plasticity and neural dynamics
 * - Development affects plasticity rates
 * - Prediction system is integrated
 * - Replay and consolidation are functional
 * - Checkpoint save/load works
 * 
 * The experiment runs a complete lifetime simulation and verifies
 * that all systems interact properly.
 */

#include "../experiments/Experiment.hpp"
#include "../experiments/Metrics.hpp"
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

namespace nlm {

/**
 * Phase 6 integration test result
 */
// Simple sync mechanism between master and slave brains
    bool syncComplete;
    double syncError;
    double syncIterations;
    
    // Integration status - various systems connected?
    bool neuralWorkingMemoryIntegrated;
    bool neuralEpisodicMemoryIntegrated;
    bool neuralAssociativeMemoryIntegrated;
    
    // Connection metrics
    float memoryConnectionStrength;
    float predictionConnectionStrength;
    float neuromodulationConnectionStrength;
    float developmentConnectionStrength;
    
    // Developmental stage comparison
    DevelopmentalStage masterDevelopmentalStage;
    DevelopmentalStage slaveDevelopmentalStage;
    
    // Integration verification
    bool verifyIntegration();
    
    // Integration testing
    bool testMemoryIntegration();
    bool testNeuromodulationIntegration();
    bool testPredictionIntegration();
    bool testDevelopmentIntegration();
    
    // Actual integration execution
    bool integrateWorkingMemory(NeuralWorkingMemory* master, NeuralWorkingMemory* slave);
    bool integrateEpisodicMemory(NeuralEpisodicMemory* master, NeuralEpisodicMemory* slave);
    bool integrateAssociativeMemory(NeuralAssociativeMemory* master, NeuralAssociativeMemory* slave);
    bool integratePredictionSystem(PredictionSystem* master, PredictionSystem* slave);
    bool integrateNeuromodulation(Neuromodulator* master, Neuromodulator* slave);
    bool integrateDevelopment(DevelopmentSystem* master, DevelopmentSystem* slave);
    
    // Integration metrics
    double computeIntegrationMetrics();
    double computeSyncMetrics();
    double computeConnectionMetrics();
    
    // Data collection for integration
    IntegrationData collectMasterData();
    IntegrationData collectSlaveData();
    IntegrationData computeDifference(const IntegrationData& master, const IntegrationData& slave);
    
    // Output and visualization
    void outputIntegrationReport();
    void outputSyncReport();
    void outputConnectionReport();
    
    // Debugging and monitoring
    void monitorIntegration();
    bool checkIntegrationHealth();
    
    // Phase 6 specific integration tests
    bool runPhase6StressTest();
    bool runPhase6ConsistencyTest();
    bool runPhase6PerformanceTest();
    
    // Cross-system validation
    bool validateNeuralConsistency();
    bool validateMemoryConsistency();
    bool validatePredictionConsistency();
    bool validateDevelopmentConsistency();
    
    // Learning and adaptation
    void learnFromMasterSlaveDifference();
    void adaptConnectionWeights();
    void updateIntegrationParameters();
    
    // Maintenance and cleanup
    void cleanupSlaveBrain();
    void resetIntegration();
    void shutdownIntegration();
};

} // namespace nlm
