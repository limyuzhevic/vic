#pragma once

// Phase6Config.hpp - Configuration for Phase 6 integration experiment
// This file defines the configuration structure for Phase 6 experiments

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

/**
 * @struct Phase6Config
 * @brief Configuration for Phase 6 integration experiment
 * 
 * This structure defines all configuration parameters for Phase 6 experiments,
 * including brain settings, system integration options, and performance tuning.
 * 
 * **Phase 6 Integration Features**:
 * - All memory systems (working, episodic, associative)
 * - Neuromodulation systems (dopamine, curiosity, novelty)
 * - Prediction system
 * - Cognition systems (planning, attention, concept formation)
 * - Development system
 * - Checkpoint persistence
 * - Memory replay and consolidation
 */
struct Phase6Config {
    // === BRAIN CONFIGURATION ===
    size_t neuronCount = 1000;              // Number of neurons in the brain
    size_t maxSteps = 5000;                 // Maximum number of simulation steps
    size_t regionCount = 1;                 // Number of neural regions
    float connectionProbability = 0.1f;     // Probability of synaptic connections
    
    // === SYSTEM INTEGRATION ===
    bool enableCheckpointing = true;        // Enable checkpoint saving/loading
    bool enableReplay = true;               // Enable episodic memory replay
    bool enableDevelopment = true;          // Enable developmental processes
    bool enableWorkingMemory = true;        // Enable working memory system
    bool enableEpisodicMemory = true;       // Enable episodic memory system
    bool enablePrediction = true;           // Enable prediction system
    bool enableNeuromodulation = true;      // Enable neuromodulation systems
    bool enableCognition = true;            // Enable cognitive functions
    
    // === DEVELOPMENT ===
    DevelopmentalStage developmentalStage = DevelopmentalStage::Adult;
    
    // === PERFORMANCE TUNING ===
    bool verboseLogging = true;             // Enable detailed logging output
    bool recordMetrics = true;              // Record experiment performance metrics
};

} // namespace nlm