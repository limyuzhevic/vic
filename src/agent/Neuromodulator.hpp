#pragma once

#include "../agent/AgentBody.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>
#include <cmath>

namespace nlm {

/**
 * @class Neuromodulator
 * @brief Manages neuromodulation, reward processing, and developmental changes
 * 
 * This component handles the complex system of neuromodulators that influence
 * neural plasticity, reward processing, and developmental stages. It integrates
 * reward signals with prediction errors, dopamine-like signals, and developmental
 * timing to modulate brain function throughout the agent's lifecycle.
 * 
 * Responsibilities:
 * - Process reward signals and compute prediction errors
 * - Apply neuromodulation to synaptic plasticity
 * - Manage developmental stages and age-based modulation
 * - Handle structural plasticity changes
 * - Maintain neuromodulation state variables
 * 
 * Key features:
 * - Reward-prediction error computation
 * - Dopamine-like neuromodulation
 * - Multi-stage developmental progression
 * - Activity-dependent plasticity
 * - Thread-safe neuromodulation application
 */
class Neuromodulator {
public:
    /**
     * @brief Constructor for Neuromodulator
     * 
     * @param brain Pointer to the brain for neural access
     * @param world Pointer to the world for developmental context
     * @throw std::invalid_argument if brain or world pointers are null
     */
    Neuromodulator(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world);
    
    /**
     * @brief Destructor
     */
    ~Neuromodulator() = default;
    
    /**
     * @brief Apply reward-based neuromodulation
     * 
     * Computes prediction error and applies dopamine-like signals to modulate
     * synaptic plasticity throughout the neural network.
     * 
     * @param reward Current reward signal
     * @param predictedReward Predicted reward based on expectations
     * @pre 0.0 <= reward and 0.0 <= predictedReward
     * @post Dopamine level and prediction error are updated
     */
    void applyRewardModulation(float reward, float predictedReward);
    
    /**
     * @brief Update developmental stage based on age
     * 
     * Progresses through developmental stages (Initial -> CriticalPeriod -> Maturation -> Adult)
     * and modulates plasticity based on the current stage.
     * 
     * @param timestep Time since last update
     * @pre timestep >= 0.0
     * @post Developmental stage is updated based on age
     */
    void updateDevelopment(double timestep);
    
    /**
     * @brief Get current developmental stage
     * 
     * @return DevelopmentalStage Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const { return developmentalStage_; }
    
    /**
     * @brief Get current neuromodulation level (dopamine)
     * 
     * @return float Neuromodulation level (-1.0 to 1.0)
     */
    float getNeuromodulationLevel() const { return dopamineLevel_; }
    
    /**
     * @brief Get current prediction error
     * 
     * @return float Prediction error (can be positive or negative)
     */
    float getPredictionError() const { return predictionError_; }
    
    /**
     * @brief Get expected reward (running average)
     * 
     * @return float Expected reward
     */
    float getExpectedReward() const { return expectedReward_; }
    
    /**
     * @brief Get developmental age in simulation time
     * 
     * @return double Developmental age
     */
    double getDevelopmentalAge() const { return developmentalAge_; }
    
    /**
     * @brief Get current plasticity modifier
     * 
     * @return float Plasticity modifier (0.1 to 2.0)
     */
    float getPlasticityModifier() const { return plasticityModifier_; }
    
    /**
     * @brief Enable or disable structural plasticity
     * 
     * @param enable True to enable structural plasticity, false to disable
     */
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    
    /**
     * @brief Check if structural plasticity is enabled
     * 
     * @return bool True if structural plasticity is enabled
     */
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    
    /**
     * @brief Reset neuromodulation state
     */
    void reset();
    
    /**
     * @brief Set neuromodulation range limits
     * 
     * @param minMin Minimum possible neuromodulation level
     * @param maxMax Maximum possible neuromodulation level
     * @pre minMin < maxMax
     */
    void setRange(float minMin, float maxMax) {
        neuromodulationMin_ = minMin;
        neuromodulationMax_ = maxMax;
    }
    
    /**
     * @brief Set developmental stage durations
     * 
     * @param initial Initial stage duration (seconds)
     * @param critical Critical period duration (seconds)
     * @param maturation Maturation period duration (seconds)
     * @pre All durations >= 0.0
     */
    void setStageDurations(double initial, double critical, double maturation) {
        initialDuration_ = std::max(0.0, initial);
        criticalPeriodDuration_ = std::max(0.0, critical);
        maturationDuration_ = std::max(0.0, maturation);
    }

private:
    /**
     * @brief Compute prediction error from reward and expectation
     * 
     * @param reward Current reward
     * @param predictedReward Expected reward
     * @return float Prediction error (reward - predictedReward)
     */
    float computePredictionError(float reward, float predictedReward);
    
    /**
     * @brief Apply dopamine-like modulation to plasticity
     * 
     * @param dopamineLevel Neuromodulation level (-1.0 to 1.0)
     */
    void applyDopamineModulation(float dopamineLevel);
    
    /**
     * @brief Update plasticity based on developmental stage
     * 
     * @param stage Current developmental stage
     */
    void updatePlasticityForStage(DevelopmentalStage stage);
    
    /**
     * @brief Apply structural plasticity changes
     * 
     * @param stage Current developmental stage
     */
    void applyStructuralPlasticity(DevelopmentalStage stage);
    
    /**
     * @brief Clamp neuromodulation value to valid range
     * 
     * @param value Value to clamp
     * @return float Clamped value
     */
    float clampNeuromodulation(float value) const {
        return std::clamp(value, neuromodulationMin_, neuromodulationMax_);
    }
    
    std::shared_ptr<Brain> brain_;           ///< Brain for neural access and plasticity control
    std::shared_ptr<SimpleWorld> world_;      ///< World for developmental context
    
    // Neuromodulation state
    float dopamineLevel_;              ///< Dopamine-like neuromodulation level
    float predictionError_;            ///< Reward prediction error
    float expectedReward_;             ///< Running average of expected reward
    float neuromodulationMin_;          ///< Minimum neuromodulation level
    float neuromodulationMax_;          ///< Maximum neuromodulation level
    
    // Development state
    double developmentalAge_;          ///< Age in simulation time
    DevelopmentalStage developmentalStage_; ///< Current developmental stage
    float plasticityModifier_;         ///< Plasticity multiplier (0.1-2.0)
    
    // Configuration
    bool structuralPlasticityEnabled_; ///< Flag to enable/disable structural plasticity
    double initialDuration_;           ///< Initial stage duration
    double criticalPeriodDuration_;    ///< Critical period duration
    double maturationDuration_;        ///< Maturation period duration
};

} // namespace nlm
