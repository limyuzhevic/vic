#pragma once

#include "Neuromodulator.hpp"
#include "core/Config/Config.hpp"

namespace nlm {

/**
 * @class Reward
 * @brief Reward signal for reinforcement learning
 *
 * Implements a multi-component reward system based on:
 * - Survival reward (40%): Energy, health, and safety
 * - Efficiency reward (30%): Successful action and goal progress
 * - Learning reward (20%): Novelty and adaptation
 * - Exploration reward (10%): Curiosity and seeking
 *
 * @note Thread-safe implementation with mutex protection
 * @note Configurable through the Config system
 * @note Extensible for future reward components
 */

class Reward : public Neuromodulator {
public:
    Reward();
    ~Reward() override;
    
    // Neuromodulator interface
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    /**
     * @brief Compute reward from observation and configuration
     *
     * Implements a multi-component reward system based on:
     * - Survival reward (40%): Energy, health, and safety
     * - Efficiency reward (30%): Successful action and goal progress
     * - Learning reward (20%): Novelty and adaptation
     * - Exploration reward (10%): Curiosity and seeking
     *
     * @param observation Current sensory observation
     * @param config Configuration parameters
     * @return float Total reward value [0.0, 1.0]
     * @throws std::invalid_argument if observation is invalid
     * @note Thread-safe implementation with mutex protection
     */
    float computeReward(const Observation& observation, std::shared_ptr<const Config> config);
    
    /**
     * @brief Compute individual reward components
     *
     * These methods allow access to individual reward components for
     * debugging and analysis purposes.
     */
    
    /**
     * @brief Compute survival component reward (40%)
     * @param observation Current sensory observation
     * @param config Configuration parameters
     * @return float Survival reward value [0.0, 1.0]
     */
    static float computeSurvivalReward(const Observation& observation, std::shared_ptr<const Config> config);
    
    /**
     * @brief Compute efficiency component reward (30%)
     * @param observation Current sensory observation
     * @param config Configuration parameters
     * @return float Efficiency reward value [0.0, 1.0]
     */
    static float computeEfficiencyReward(const Observation& observation, std::shared_ptr<const Config> config);
    
    /**
     * @brief Compute learning component reward (20%)
     * @param observation Current sensory observation
     * @param config Configuration parameters
     * @return float Learning reward value [0.0, 1.0]
     */
    static float computeLearningReward(const Observation& observation, std::shared_ptr<const Config> config);
    
    /**
     * @brief Compute exploration component reward (10%)
     * @param observation Current sensory observation
     * @param config Configuration parameters
     * @return float Exploration reward value [0.0, 1.0]
     */
    static float computeExplorationReward(const Observation& observation, std::shared_ptr<const Config> config);
    
    // Legacy methods for backward compatibility
    float getValue() const;
    void setValue(float value);
    void add(float delta);
    void reset();
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
