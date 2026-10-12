#pragma once

#include "Action.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../brain/Neuron.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <algorithm>

namespace nlm {

// Motor system: converts neural activity to actions
// PLACEHOLDER - Phase 2 will implement real action selection

/**
 * @class MotorSystem
 * @brief Converts neural activity into motor actions
 * @details The motor system bridges the gap between neural computation and
 *         real-world interaction. It takes neural activity patterns from
 *         motor neuron populations and transforms them into executable
 *         actions that can be applied to the environment.
 * 
 * @note This is a Phase 2 implementation placeholder. The current version
 *       provides the basic framework for action selection but requires
 *       more sophisticated algorithms for:
 *       - Real action selection based on neural dynamics
 *       - Motor planning and sequencing
 *       - Sensor-motor integration
 *       - Behavioral flexibility and adaptation
 * 
 * @author NLM Research Team
 * @date Version 0.1.0
 * @ingroup motor
 * @implements action_selection_system
 */

class MotorSystem {
public:
    /**
     * @brief Construct a new MotorSystem object
     * @details Creates a motor system with default action types and preferences.
     *         The system starts with no actions configured and must be initialized
     *         with available actions before it can select actions.
     */
    MotorSystem();
    
    /**
     * @brief Destroy the MotorSystem object
     * @details Cleans up any allocated action history and internal buffers.
     *         The motor system will be in an uninitialized state after destruction.
     */
    ~MotorSystem();
    
    // ========== ACTION SELECTION ==========
    
    /**
     * @brief Select action based on neural activity
     * @param motorRegion Neural region containing motor neurons
     * @return std::unique_ptr<Action> Selected action
     * @details Implements the core action selection process. This method:
     *         - Analyzes neural activity in the motor region
     *         - Calculates action scores based on neural properties
     *         - Applies action selection algorithms
     *         - Returns the best action based on neural activity
     * 
     * @note This is a Phase 2 placeholder. Real implementation will use:
     *       - Competitive selection among motor neuron populations
     *       - Action policy gradients for learning
     *       - Predictive coding for action planning
     *       - Hierarchical action selection
     * 
     * @param motorRegion Neural region containing motor neurons
     * @return Selected action with associated confidence and strength
     * @see calculateActionScores() for score calculation
     * @see selectBestAction() for action selection algorithm
     */
    std::unique_ptr<Action> selectAction(const class NeuralRegion& motorRegion);
    
    // Get available actions
    /**
     * @brief Get available actions
     * @return const std::vector<ActionType>& List of available action types
     * @details Returns the list of action types that the motor system can
     *         produce. This is used for:
     *         - Understanding available behaviors
     *         - Action space definition
     *         - Policy learning
     * 
     * @return List of available action types
     */
    const std::vector<ActionType>& getAvailableActions() const;
    
    /**
     * @brief Set action preferences (for exploration)
     * @param action Action type to set preference for
     * @param preference Preference strength (0.0 to 1.0)
     * @details Sets a learned preference for a specific action type.
     *         Preferences are used during action selection to bias toward
     *         preferred actions, implementing exploration-exploitation trade-offs.
     * 
     * @note Preferences decay over time to allow for adaptation:
     *       - Default decay rate: 0.95 per timestep
     *       - Can be overridden by setDecayRate()
     * 
     * @param action Action type to set preference for
     * @param preference Preference strength (0.0 = avoid, 1.0 = strongly prefer)
     * @see decayPreferences() for preference decay
     * @see calculateActionScores() for using preferences in selection
     */
    void setActionPreference(ActionType action, float preference);
    
    // Get action history
    /**
     * @brief Get action history
     * @return const std::vector<const Action*>& History of selected actions
     * @details Returns the history of actions selected by the motor system.
     *         This is useful for:
     *         - Analyzing behavioral patterns
     *         - Learning from past actions
     *         - Policy evaluation
     *         - Debugging action selection
     * 
     * @return Action history vector
     * @see clearHistory() for clearing action history
     */
    const std::vector<const Action*>& getActionHistory() const;
    
    /**
     * @brief Clear action history
     * @details Clears the action history, removing all previously selected
     *         actions. This is useful for:
     *         - Resetting behavioral memory
     *         - Starting new learning episodes
     *         - Memory management
     * 
     * @see getActionHistory() for accessing action history
     */
    void clearHistory();
    
    // ========== HELPER METHODS ==========
    
    /**
     * @struct ActionScore
     * @brief Represents the score for a candidate action
     * @details Contains all information needed to evaluate and select actions:
     *         - Type of action
     *         - Strength from neural activity
     *         - Confidence in selection
     *         - Neural coherence supporting action
     *         - Learned preference (exploration bias)
     * 
     * @note Action scores are used in the action selection process to
     *       determine which action is most appropriate for the current
     *       neural state. The scores are combined using different selection
     *       algorithms (softmax, argmax, etc.).
     * 
     * @see calculateActionScores() for score computation
     * @see selectBestAction() for action selection using scores
     */
    struct ActionScore {
        ActionType type;           // Overall action strength from neural activity
        float strength;      // Overall action strength from neural activity
        float confidence;    // Confidence in the action selection
        float coherence;     // Neural coherence supporting this action
        float preference;    // Learned preference (exploration bias)
    };
    
    /**
     * @brief Calculate action scores based on neural activity
     * @param motorRegion Neural region containing motor neurons
     * @return std::vector<ActionScore> Action scores for all candidate actions
     * @details Computes scores for all available actions based on the current
     *         neural activity in the motor region. Each action score includes:
     *         - Neural activity strength
     *         - Confidence based on firing consistency
     *         - Neural coherence
     *         - Learned preferences
     * 
     * @note The scoring process involves:
     *       1. Calculating neural activity statistics
     *       2. Computing action strength from firing patterns
     *       3. Calculating confidence from firing consistency
     *       4. Computing neural coherence
     *       5. Applying learned preferences
     * 
     * @param motorRegion Neural region containing motor neurons
     * @return Action scores for all candidate actions
     * @see selectBestAction() for action selection
     * @see ActionScore struct for score components
     */
    std::vector<ActionScore> calculateActionScores(const class NeuralRegion& motorRegion) const;
    
    /**
     * @brief Select best action from scored options
     * @param scores Vector of action scores
     * @return ActionType Best action to execute
     * @details Selects the best action from a set of scored options using
     *         a selection algorithm. The selection algorithm can be:
     *         - Deterministic (argmax): Choose action with highest score
     *         - Probabilistic (softmax): Sample based on scores
     *         - Epsilon-greedy: Sometimes explore random actions
     * 
     * @note The action selection algorithm balances exploration and exploitation:
     *       - High scores favor exploitation (choose best action)
     *       - Low scores encourage exploration (try other actions)
     *       - Temperature parameter controls selection randomness
     * 
     * @param scores Action scores for candidate actions
     * @return Selected action type
     * @see calculateActionScores() for score computation
     * @see selectAction() for end-to-end action selection
     */
    ActionType selectBestAction(const std::vector<ActionScore>& scores) const;
    
    /**
     * @brief Calculate neural coherence for a population of neurons
     * @param neurons Vector of neurons to analyze
     * @return float Neural coherence (0.0 to 1.0)
     * @details Computes coherence among a population of neurons, measuring how
     *         synchronized their activity is. High coherence indicates coordinated
     *         neural activity, which is often associated with strong action selection.
     * 
     * @note Coherence is calculated as the correlation of firing rates across
     *       neurons in the population. Values range from:
     *       - 0.0: Completely uncorrelated activity
     *       - 1.0: Perfect correlation
     * 
     * @param neurons Neurons to analyze
     * @return Neural coherence measure
     * @see calculateActionScores() for using coherence in scoring
     */
    float calculateNeuralCoherence(const std::vector<const Neuron*>& neurons) const;
    
    /**
     * @brief Calculate action strength based on neural firing patterns
     * @param action Action type to evaluate
     * @param neurons Neurons contributing to action evaluation
     * @return float Action strength (0.0 to 1.0)
     * @details Computes how well a specific action is supported by the
     *         neural activity. This is based on:
     *         - Firing rates of motor neurons
     *         - Neuron types related to the action
     *         - Synaptic weights and connections
     *         - Neural synchrony
     * 
     * @note Action strength represents the neural drive to perform an action.
     *       Higher values indicate stronger motivation or readiness to act.
     * 
     * @param action Action type to evaluate
     * @param neurons Neurons contributing to evaluation
     * @return Action strength measure
     * @see calculateActionScores() for using action strength
     */
    float calculateActionStrength(ActionType action, const std::vector<const Neuron*>& neurons) const;
    
    /**
     * @brief Calculate confidence based on firing consistency and population size
     * @param neurons Neurons to analyze
     * @return float Confidence measure (0.0 to 1.0)
     * @details Computes confidence in action selection based on:
     *         - Firing consistency (how stable the firing rates are)
     *         - Population size (number of contributing neurons)
     *         - Activity diversity
     *         - Signal-to-noise ratio
     * 
     * @note Higher confidence indicates more reliable action selection:
     *       - Consistent firing across neurons = higher confidence
     *       - Larger population = higher confidence
     *       - Stable activity patterns = higher confidence
     * 
     * @param neurons Neurons to analyze
     * @return Confidence measure
     * @see calculateActionScores() for using confidence in scoring
     */
    float calculateConfidence(const std::vector<const Neuron*>& neurons) const;
    
    // Decay old action preferences over time
    /**
     * @brief Decay old action preferences over time
     * @param decayRate Decay rate per timestep (default: 0.95)
     * @details Gradually reduces all action preferences over time to allow
     *         for behavioral adaptation and learning. This implements
     *         forgetting of previously learned preferences.
     * 
     * @note Preference decay follows exponential decay:
     *       preference(t) = preference(t-1) * decayRate
     *       - decayRate = 1.0: No decay (preferences persist)
     *       - decayRate = 0.95: Slow decay (10% loss per timestep)
     *       - decayRate = 0.9: Fast decay (10% loss per timestep)
     * 
     * @param decayRate Decay factor per timestep
     * @see setActionPreference() for setting preferences
     */
    void decayPreferences(float decayRate = 0.95f);
    
    // Get neural activity summary statistics
    /**
     * @struct NeuralActivityStats
     * @brief Summary statistics of neural activity in a population
     * @details Contains statistical measures of neural activity:
     *         - Mean firing rate
     *         - Firing variance
     *         - Fraction of active neurons
     *         - Total active neurons
     *         - Total neurons in population
     * 
     * @note These statistics are used for:
     *       - Action selection decisions
     *       - Activity monitoring
     *       - Learning and adaptation
     *       - Performance evaluation
     */
    struct NeuralActivityStats {
        float meanFiringRate;
        float firingVariance;
        float activeFraction;
        float totalActiveNeurons;
        float totalNeurons;
    };
    
    /**
     * @brief Get neural activity summary statistics
     * @param motorRegion Neural region to analyze
     * @return NeuralActivityStats Statistical summary of neural activity
     * @details Computes summary statistics for the motor neuron population
     *         in the specified region. This includes measures of:
     *         - Central tendency (mean firing rate)
     *         - Variability (firing variance)
     *         - Engagement (fraction active)
     *         - Scale (population size)
     * 
     * @note Neural activity statistics are used for:
     *       - Action selection decisions
     *       - Monitoring neural state
     *       - Learning and adaptation
     *       - Performance evaluation
     * 
     * @param motorRegion Neural region containing motor neurons
     * @return Summary statistics of neural activity
     * @see calculateActionStrength() for using neural activity
     * @see calculateConfidence() for using neural variability
     */
    NeuralActivityStats getNeuralActivityStats(const class NeuralRegion& motorRegion) const;
    
    /**
     * @brief Reset internal state
     * @details Resets all internal motor system state to initial values:
     *         - Clears action history
     *         - Resets all preferences to default
     *         - Clears accumulated scores
     *         - Resets any learned behavior
     * 
     * @note This is useful for:
     *       - Starting new simulations
     *       - Resetting behavioral state
     *       - Learning new behaviors
     *       - Experimental control
     * 
     * @see selectAction() for action selection after reset
     * @see getActionHistory() for checking history after reset
     */
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
