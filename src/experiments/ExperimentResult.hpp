#pragma once

// ExperimentResult.hpp - Results from experiment runs
// This file defines the structure for storing experiment results

namespace nlm {

/**
 * @struct ExperimentResult
 * @brief Base structure for experiment results
 * 
 * This structure provides a common interface for storing results from
 * different types of experiments within the NLM framework.
 */
struct ExperimentResult {
    // Common experiment metrics
    size_t totalSteps = 0;                 // Total number of simulation steps executed
    double simulationTime = 0.0;           // Total simulation time in seconds
    bool completedSuccessfully = false;    // Whether experiment completed successfully
};

/**
 * @struct Phase6Result
 * @brief Results from Phase 6 integration experiment
 * 
 * This structure contains all performance metrics and integration status
 * from Phase 6 experiments, tracking the complete functionality of
 * the integrated artificial brain.
 */
struct Phase6Result : public ExperimentResult {
    // Performance metrics
    float totalReward = 0.0f;              // Total reward accumulated during experiment
    float avgFiringRate = 0.0f;            // Average neural firing rate across all neurons
    size_t memoryEpisodesStored = 0;      // Number of episodic memory entries created
    float dopamineLevel = 0.0f;            // Average dopamine level during experiment
    
    // Integration verification status
    bool memoryWorkingMemoryIntegrated = false;
    bool memoryEpisodicMemoryIntegrated = false;
    bool neuromodulationIntegrated = false;
    bool predictionIntegrated = false;
    bool developmentIntegrated = false;
    bool checkpointingWorks = false;
    
    // Additional metrics for comprehensive analysis
    size_t totalNeuronsProcessed = 0;     // Total number of neurons processed
    size_t totalConnectionsMade = 0;     // Total synaptic connections established
    float learningRate = 0.0f;           // Average learning rate during experiment
    float memoryConsolidationScore = 0.0f; // Score indicating memory consolidation effectiveness
};

} // namespace nlm