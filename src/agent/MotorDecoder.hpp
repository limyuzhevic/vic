#pragma once

#include "../agent/AgentBody.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

/**
 * @class MotorDecoder
 * @brief Decodes neural activity into motor commands with curiosity-based exploration
 * 
 * This component handles the transformation of motor neuron activity into actionable
 * motor commands. It implements a mapping from neural populations to specific
 * motor behaviors (forward, backward, left, right, interact, wait) and adds
 * curiosity-based exploration when appropriate.
 * 
 * Responsibilities:
 * - Calculate activity levels in motor neuron groups
 * - Select highest activity command
 * - Implement curiosity-driven exploration
 * - Validate motor command bounds
 * 
 * Key features:
 * - Event-driven motor command selection
 * - Curiosity-based random exploration
 * - Activity threshold filtering
 * - Backward compatible with existing motor command enumeration
 */
class MotorDecoder {
public:
    /**
     * @brief Constructor for MotorDecoder
     * 
     * @param brain Pointer to the brain for accessing neural data and random generation
     * @throw std::invalid_argument if brain pointer is null
     */
    explicit MotorDecoder(std::shared_ptr<Brain> brain);
    
    /**
     * @brief Destructor
     */
    ~MotorDecoder() = default;
    
    /**
     * @brief Decode neural activity into a motor command
     * 
     * Analyzes the activity levels in motor neuron groups and returns the command
     * with the highest activity. If all activity is below threshold, returns Wait.
     * 
     * @return MotorCommand The decoded motor command
     * @note Thread-safe for concurrent execution
     */
    MotorCommand decodeMotorCommand();
    
    /**
     * @brief Update curiosity level based on external input
     * 
     * @param noveltyLevel Current novelty level (0.0-1.0)
     * @param predictionError Current prediction error value
     * @pre 0.0 <= noveltyLevel <= 1.0
     * @post curiosityLevel_ is clamped to [0.0, 1.0]
     */
    void updateCuriosityLevel(float noveltyLevel, float predictionError);
    
    /**
     * @brief Enable or disable curiosity-based exploration
     * 
     * @param enable True to enable curiosity exploration, false to disable
     */
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    /**
     * @brief Check if curiosity exploration is enabled
     * 
     * @return bool True if curiosity is enabled, false otherwise
     */
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    /**
     * @brief Get the current curiosity level
     * 
     * @return float Current curiosity level (0.0-1.0)
     */
    float getCuriosityLevel() const { return curiosityLevel_; }
    
    /**
     * @brief Reset decoder state
     */
    void reset();
    
    /**
     * @brief Get brain pointer
     * 
     * @return Brain* Pointer to the brain, or nullptr if brain_ is null
     */
    Brain* getBrain() { return brain_.get(); }
    
    /**
     * @brief Set minimum activity threshold for motor commands
     * 
     * Commands with activity below this threshold will be ignored
     * 
     * @param threshold Minimum activity threshold (0.0-1.0)
     * @pre 0.0 <= threshold <= 1.0
     */
    void setActivityThreshold(float threshold) {
        activityThreshold_ = std::clamp(threshold, 0.0f, 1.0f);
    }
    
    /**
     * @brief Set exploration probability modifier for curiosity
     * 
     * @param probability Multiplier for curiosity-based exploration probability
     */
    void setExplorationProbability(float probability) { explorationProbability_ = probability; }

private:
    /**
     * @brief Calculate activity level for a group of neurons
     * 
     * @param neurons Vector of neuron pointers to analyze
     * @return float Activity level (0.0-1.0 scale)
     */
    float calculateActivity(const std::vector<Neuron*>& neurons) const;
    
    /**
     * @brief Select command based on curiosity when curiosity is enabled
     * 
     * @param defaultCmd The default command selected by neural activity
     * @return MotorCommand Selected command (may be random)
     */
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    /**
     * @brief Validate motor command bounds and handle edge cases
     * 
     * @param cmd The command to validate
     * @return MotorCommand Validated command
     */
    MotorCommand validateCommand(MotorCommand cmd) const;
    
    std::shared_ptr<Brain> brain_;  ///< Brain for neural access and random generation
    float curiosityLevel_;           ///< Current curiosity level (0.0-1.0)
    float activityThreshold_;        ///< Minimum activity threshold for commands
    float explorationProbability_;   ///< Multiplier for curiosity exploration
    bool curiosityEnabled_;          ///< Flag to enable/disable curiosity exploration
};

} // namespace nlm
