#pragma once

// Phase6IntegratedExperiment.hpp - Phase 6 Integration Experiment Header
// This file defines the Phase 6 integration experiment interface

#include "Phase6Config.hpp"
#include "Brain.hpp"
#include "ExperimentResult.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Forward declarations for integrated systems
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class PredictionSystem;
class Dopamine;
class Curiosity;
class Novelty;
class CheckpointManager;

/**
 * @struct Phase6Result
 * @brief Results from Phase 6 integration experiment run
 */
struct Phase6Result {
    // Performance metrics
    float totalReward;              // Total reward accumulated
    float avgFiringRate;            // Average neural firing rate
    size_t memoryEpisodesStored;   // Number of episodic memory entries
    float dopamineLevel;            // Dopamine concentration level
    
    // Integration verification status
    bool memoryWorkingMemoryIntegrated;
    bool memoryEpisodicMemoryIntegrated;
    bool neuromodulationIntegrated;
    bool predictionIntegrated;
    bool developmentIntegrated;
    bool checkpointingWorks;
    
    // Timing
    double totalWallClockTime;      // Total simulation time in seconds
};

/**
 * @class Phase6Config
 * @brief Configuration for Phase 6 integration experiment
 */
struct Phase6Config {
    // Brain configuration
    size_t neuronCount = 1000;              // Number of neurons
    size_t maxSteps = 5000;                 // Maximum simulation steps
    size_t regionCount = 1;                 // Number of neural regions
    float connectionProbability = 0.1f;     // Connection probability
    
    // System integration
    bool enableCheckpointing = true;        // Enable checkpoint saving
    bool enableReplay = true;               // Enable memory replay
    bool enableDevelopment = true;          // Enable developmental processes
    bool enableWorkingMemory = true;        // Enable working memory
    bool enableEpisodicMemory = true;       // Enable episodic memory
    bool enablePrediction = true;           // Enable prediction system
    bool enableNeuromodulation = true;      // Enable neuromodulation
    bool enableCognition = true;            // Enable cognitive functions
    
    // Development
    DevelopmentalStage developmentalStage = DevelopmentalStage::Adult;
    
    // Performance tuning
    bool verboseLogging = true;             // Enable detailed logging
    bool recordMetrics = true;              // Record performance metrics
};

/**
 * @class Phase6IntegratedExperiment
 * @brief Phase 6 integration experiment for testing complete brain functionality
 * 
 * This experiment verifies that all Phase 6 integrated brain systems work together
 * as a coherent artificial brain. It tests integration of memory systems,
 * neuromodulation, prediction, development, and persistence.
 * 
 * **Phase 6 Integration Tests**:
 * - Integration verification (all systems connected)
 * - Memory integration testing
 * - Neuromodulation integration testing
 * - Checkpoint persistence testing
 * - Memory replay and consolidation testing
 * - Full brain loop execution
 */
class Phase6IntegratedExperiment {
public:
    Phase6IntegratedExperiment();
    ~Phase6IntegratedExperiment();
    
    // Disable copying
    Phase6IntegratedExperiment(const Phase6IntegratedExperiment&) = delete;
    Phase6IntegratedExperiment& operator=(const Phase6IntegratedExperiment&) = delete;
    
    /**
     * @brief Verify integration of all brain systems
     * @return true if all systems are properly integrated and connected
     * 
     * Performs quick verification that all memory, prediction, cognition,
     * and neuromodulation systems are accessible and functional.
     */
    bool verifyIntegration();
    
    /**
     * @brief Test memory system integration
     * 
     * Tests that working memory, episodic memory, and associative memory
     * are properly connected to neural processing.
     */
    void testMemoryIntegration();
    
    /**
     * @brief Test neuromodulation system integration
     * 
     * Tests that neuromodulation systems (dopamine, curiosity, novelty)
     * are properly integrated with neural dynamics and plasticity.
     */
    void testNeuromodulationIntegration();
    
    /**
     * @brief Test checkpoint persistence
     * 
     * Tests that brain state can be saved and loaded via checkpointing.
     */
    void testCheckpointing();
    
    /**
     * @brief Test memory replay and consolidation
     * 
     * Tests that episodic memory replay and consolidation work correctly,
     * simulating sleep/rest cycles and memory strengthening.
     */
    void testReplay();
    
    /**
     * @brief Run full Phase 6 integration experiment
     * @param config Configuration for the experiment
     * @return Results from the experiment run
     * 
     * Executes the complete integrated brain simulation with all systems
     * working together to process sensory input, generate behavior, and
     * learn from experience.
     */
    Phase6Result run(const Phase6Config& config);
    
    /**
     * @brief Get the brain instance used by this experiment
     * @return Pointer to the brain instance
     */
    Brain* getBrain() { return brain_.get(); }
    
    const Brain* getBrain() const { return brain_.get(); }
    
private:
    // Create and configure brain with Phase 6 integration
    void createBrain(const Phase6Config& config);
    
    // Initialize all integrated systems
    void initializeIntegratedSystems();
    
    // Verify specific system integration
    template<typename T>
    bool verifySystemIntegration(T* system, const std::string& systemName);
    
    std::shared_ptr<Brain> brain_;
    bool isInitialized_;
};

} // namespace nlm