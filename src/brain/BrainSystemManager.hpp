// Brain System Manager - Coordinates all integrated brain systems
// Separates system coordination from the main Brain class

#pragma once

#include "../core/Types/Types.hpp"
#include "Brain.hpp"

namespace nlm {

/**
 * @brief Manages all integrated brain systems and their coordination
 * 
 * This class separates the complex system management logic from the main Brain class,
 * making it more modular and testable. It coordinates:
 * - Working memory updates
 * - Neuromodulation system updates
 * - Prediction system integration
 * - Cognition system updates
 * - Development system management
 * - Structural plasticity application
 * - Memory replay and consolidation
 * - Checkpoint management
 * 
 * @note This is a key part of the refactoring effort to reduce Brain.cpp size
 * and improve code organization by using the manager pattern. It encapsulates
 * the complex Phase 6 integration logic in a dedicated class.
 */
class BrainSystemManager {
public:
    /**
     * @brief Constructor
     * 
     * @param brain Shared pointer to the Brain instance to manage
     */
    explicit BrainSystemManager(std::shared_ptr<Brain> brain)
        : brain_(brain) {}
    
    /**
     * @brief Destructor
     */
    ~BrainSystemManager() = default;
    
    /**
     * @brief Initialize all integrated systems
     * 
     * This method initializes all memory, cognition, and neuromodulation systems
     * with appropriate parameters from configuration.
     */
    void initializeAllSystems();
    
    /**
     * @brief Update working memory for the current simulation step
     * 
     * Applies working memory dynamics based on current neural activity.
     */
    void updateWorkingMemory();
    
    /**
     * @brief Update neuromodulation systems
     * 
     * Updates dopamine, curiosity, and novelty systems based on current activity
     * and applies their effects on neural excitability and plasticity.
     */
    void updateNeuromodulationSystems();
    
    /**
     * @brief Update prediction system
     * 
     * Updates the prediction system with current sensory observations and
     * prediction error signals.
     */
    void updatePredictionSystem();
    
    /**
     * @brief Update cognition systems
     * 
     * Updates attention, concept formation, and planning systems based on
     * current neural activity and working memory contents.
     */
    void updateCognitionSystems();
    
    /**
     * @brief Update development system
     * 
     * Applies developmental stage effects on plasticity rates and neural properties.
     * 
     * @param step Current simulation step
     */
    void updateDevelopmentSystem(SimulationStep step);
    
    /**
     * @brief Apply structural plasticity for the current step
     * 
     * Applies synaptogenesis and pruning based on developmental stage and
     * activity-dependent factors.
     * 
     * @param step Current simulation step
     */
    void applyStructuralPlasticity(SimulationStep step);
    
    /**
     * @brief Replay important memories
     * 
     * Replays episodic memories to strengthen consolidation and integrate
     * learned experiences into ongoing processing.
     * 
     * @param step Current simulation step
     */
    void replayMemories(SimulationStep step);
    
    /**
     * @brief Apply memory consolidation
     * 
     * Consolidates episodic memories, removing weak traces and strengthening
     * important ones.
     * 
     * @param step Current simulation step
     */
    void consolidateMemories(SimulationStep step);
    
    /**
     * @brief Update checkpoint manager
     * 
     * Updates the checkpoint manager to save brain state at appropriate intervals.
     * 
     * @param step Current simulation step
     * @param time Current simulation time
     */
    void updateCheckpointManager(SimulationStep step, Timestamp time);
    
    /**
     * @brief Collect and log system statistics
     * 
     * Collects statistics from all integrated systems and logs them for monitoring.
     */
    void collectSystemStatistics();
    
    /**
     * @brief Set the rest state for the brain
     * 
     * @param resting Whether the brain is in rest state
     */
    void setRestState(bool resting) { integrationState_.isResting = resting; }
    
    /**
     * @brief Get the rest state of the brain
     * 
     * @return true if brain is resting, false otherwise
     */
    bool getRestState() const { return integrationState_.isResting; }
    
private:
    /**
     * @brief Integration state for the brain
     * 
     * Contains state variables for the integrated systems, including rest state,
     * episode tracking, and replay/consolidation intervals.
     */
    struct IntegrationState {
        bool isResting = false;                    ///< Whether the brain is in rest state
        size_t stepsSinceLastEpisode = 0;           ///< Steps since last episode storage
        size_t replayInterval = 100;               ///< Steps between memory replays
        size_t consolidationInterval = 1000;       ///< Steps between memory consolidations
    };
    
    /**
     * @brief Shared pointer to the Brain instance being managed
     */
    std::shared_ptr<Brain> brain_;
    
    /**
     * @brief Current integration state
     */
    IntegrationState integrationState_;
};

} // namespace nlm
