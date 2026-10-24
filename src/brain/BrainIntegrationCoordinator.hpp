#pragma once

/**
 * Brain Integration Coordinator
 * 
 * This class serves as the central coordinator for all brain systems,
 * ensuring proper integration and communication between different systems.
 * It manages the brain loop and coordinates the interaction between:
 * - Memory systems (working memory, episodic memory, associative memory)
 * - Neuromodulation systems (dopamine, curiosity, novelty, prediction error)
 * - Prediction system
 * - Cognition systems (planner, concept formation, attention)
 * - Development system
 * - Performance systems (memory pools, event queues, SIMD)
 */

#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "development/DevelopmentSystem.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

namespace nlm {

/**
 * Exception thrown when integration coordinator fails to initialize
 */
class IntegrationCoordinatorException : public std::runtime_error {
public:
    explicit IntegrationCoordinatorException(const std::string& message)
        : std::runtime_error(message) {}
};

class BrainIntegrationCoordinator {
public:
    BrainIntegrationCoordinator(std::shared_ptr<Brain> brain);
    ~BrainIntegrationCoordinator();
    
    /**
     * Initialize the integration coordinator
     * 
     * @return true if initialization successful, false otherwise
     * @throws IntegrationCoordinatorException if initialization fails
     */
    bool initialize();
    
    /**
     * Process a complete brain step
     * This is the main integration entry point that coordinates all systems
     * 
     * @param step Current simulation step
     * @param time Current simulation time
     */
    void processStep(SimulationStep step, Timestamp time);
    
    /**
     * Get integration status as a formatted string
     * 
     * @return Integration status string
     */
    std::string getIntegrationStatus() const;
    
    /**
     * Enable or disable specific integration components
     * 
     * @param enabled True to enable, false to disable
     */
    void enableMemoryIntegration(bool enabled);
    void enablePredictionIntegration(bool enabled);
    void enableCognitionIntegration(bool enabled);
    void enableNeuromodulationIntegration(bool enabled);
    void enableDevelopmentIntegration(bool enabled);
    void enablePerformanceIntegration(bool enabled);
    
    /**
     * Configure integration parameters
     * 
     * @param capacity Working memory capacity
     */
    void setWorkingMemoryCapacity(size_t capacity);
    
    /**
     * Configure integration parameters
     * 
     * @param maxEpisodes Maximum number of episodic memory episodes
     */
    void setEpisodicMemoryMaxEpisodes(size_t maxEpisodes);
    
    /**
     * Configure integration parameters
     * 
     * @param interval Prediction update interval in steps
     */
    void setPredictionUpdateInterval(size_t interval);
    
    /**
     * Configure integration parameters
     * 
     * @param depth Planning depth (number of future steps to consider)
     */
    void setPlanningDepth(size_t depth);
    
    /**
     * Configure integration parameters
     * 
     * @param inhibition Attention inhibition strength
     * @param excitation Attention excitation strength
     */
    void setAttentionParameters(float inhibition, float excitation);
    
    /**
     * Get integration metrics
     * 
     * @return Current memory activity level
     */
    float getMemoryActivity() const;
    
    /**
     * Get integration metrics
     * 
     * @return Current prediction accuracy
     */
    float getPredictionAccuracy() const;
    
    /**
     * Get integration metrics
     * 
     * @return Current cognitive load
     */
    float getCognitiveLoad() const;
    
    /**
     * Get integration metrics
     * 
     * @return Current neuromodulation level
     */
    float getNeuromodulationLevel() const;
    
    /**
     * Test integration of specific components
     * 
     * @return true if integration test passes, false otherwise
     */
    bool testMemoryIntegration() const;
    bool testPredictionIntegration() const;
    bool testCognitionIntegration() const;
    bool testNeuromodulationIntegration() const;
    bool testDevelopmentIntegration() const;
    
    /**
     * Configure integration for different modes
     */
    void configureForLearningMode();
    void configureForReplayMode();
    void configureForDevelopmentMode();
    void configureForStandardMode();

private:
    // Core components
    std::shared_ptr<Brain> brain_;
    
    // Integration state
    bool memoryIntegrationEnabled_;
    bool predictionIntegrationEnabled_;
    bool cognitionIntegrationEnabled_;
    bool neuromodulationIntegrationEnabled_;
    bool developmentIntegrationEnabled_;
    bool performanceIntegrationEnabled_;
    
    // Configuration
    size_t predictionUpdateInterval_;
    size_t planningDepth_;
    float attentionInhibitionStrength_;
    float attentionExcitationStrength_;
    
    // Integration counters and metrics
    SimulationStep lastPredictionUpdateStep_;
    SimulationStep lastMemoryUpdateStep_;
    SimulationStep lastDevelopmentUpdateStep_;
    SimulationStep lastConsolidationStep_;
    
    // For replay mechanism
    std::vector<std::vector<float>> memoryReplayBuffer_;
    bool replayModeEnabled_;
    
    // Internal helper methods
    void integrateMemorySystems(SimulationStep step, Timestamp time);
    void integratePredictionSystem(SimulationStep step, Timestamp time);
    void integrateCognitionSystems(SimulationStep step, Timestamp time);
    void integrateNeuromodulationSystems(SimulationStep step, Timestamp time);
    void integrateDevelopmentSystem(SimulationStep step, Timestamp time);
    void integratePerformanceSystems(SimulationStep step, Timestamp time);
    
    void updateWorkingMemory(SimulationStep step, Timestamp time);
    void updateEpisodicMemory(SimulationStep step, Timestamp time);
    void updateAssociativeMemory(SimulationStep step, Timestamp time);
    
    void updatePrediction(SimulationStep step, Timestamp time);
    void updatePlanning(SimulationStep step, Timestamp time);
    void updateConceptFormation(SimulationStep step, Timestamp time);
    void updateAttention(SimulationStep step, Timestamp time);
    
    void updateNeuromodulation(SimulationStep step, Timestamp time);
    void updateDevelopment(SimulationStep step, Timestamp time);
    
    // Cross-system communication
    void communicateMemoryToPrediction();
    void communicatePredictionToCognition();
    void communicateCognitionToAction();
    void communicateNeuromodulationToPlasticity();
    
    // Memory consolidation and replay
    void consolidateMemories();
    void replayMemories();
    
    // Development progression
    void updateDevelopmentalStage();
    
    // Statistics and metrics
    struct IntegrationMetrics {
        float memoryActivity_;
        float predictionAccuracy_;
        float cognitiveLoad_;
        float neuromodulationLevel_;
        SimulationStep stepsSinceLastConsolidation_;
        SimulationStep stepsSinceLastPredictionUpdate_;
        size_t memoryReplayCount_;
        
        IntegrationMetrics()
            : memoryActivity_(0.0f), predictionAccuracy_(0.0f), cognitiveLoad_(0.0f)
            , neuromodulationLevel_(0.0f), stepsSinceLastConsolidation_(0)
            , stepsSinceLastPredictionUpdate_(0), memoryReplayCount_(0) {}
    };
    
    IntegrationMetrics metrics_;
};

} // namespace nlm