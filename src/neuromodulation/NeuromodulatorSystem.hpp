#pragma once

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace nlm {

/**
 * @class NeuromodulatorSystem
 * @brief Unified system for managing all neuromodulators
 * 
 * This class provides a centralized interface for:
 * - Managing multiple neuromodulators (Dopamine, Curiosity, Novelty, PredictionError)
 * - Applying neuromodulatory effects to brain systems
 * - Integrating neuromodulation with plasticity and learning
 * - Coordinating neuromodulator interactions
 * 
 * Neuromodulators are chemical signals that modulate neural activity,
 * influencing plasticity, attention, memory, and learning processes.
 */
class NeuromodulatorSystem {
public:
    NeuromodulatorSystem();
    ~NeuromodulatorSystem();
    
    // Disable copying and moving
    NeuromodulatorSystem(const NeuromodulatorSystem&) = delete;
    NeuromodulatorSystem& operator=(const NeuromodulatorSystem&) = delete;
    NeuromodulatorSystem(NeuromodulatorSystem&&) noexcept = default;
    NeuromodulatorSystem& operator=(NeuromodulatorSystem&&) noexcept = default;
    
    /**
     * @brief Initialize the neuromodulator system
     * 
     * @param config Configuration object
     * @return true if initialization succeeded
     */
    bool initialize(std::shared_ptr<Config> config);
    
    /**
     * @brief Update neuromodulators with current simulation state
     * 
     * @param dt Timestep duration
     * @param brain Pointer to brain for system interaction
     */
    void update(TimestepDuration dt, Brain* brain);
    
    /**
     * @brief Get neuromodulator by name
     * 
     * @param name Name of neuromodulator (e.g., "DA", "Curiosity", "Novelty", "PredictionError")
     * @return Pointer to neuromodulator or nullptr if not found
     */
    Neuromodulator* getNeuromodulator(const std::string& name);
    
    /**
     * @brief Get neuromodulator level
     * 
     * @param name Name of neuromodulator
     * @return Level value (0.0-1.0 typical range)
     */
    float getLevel(const std::string& name) const;
    
    /**
     * @brief Set neuromodulator level
     * 
     * @param name Name of neuromodulator
     * @param level Level value (clamped to valid range)
     */
    void setLevel(const std::string& name, float level);
    
    /**
     * @brief Get plasticity modulation factor
     * 
     * @param name Name of neuromodulator
     * @return Plasticity modulation factor
     */
    float getPlasticityFactor(const std::string& name) const;
    
    /**
     * @brief Signal a reward event
     * 
     * @param reward Reward value (positive or negative)
     */
    void signalReward(float reward);
    
    /**
     * @brief Signal prediction error
     * 
     * @param predictionError Prediction error value
     */
    void signalPredictionError(float predictionError);
    
    /**
     * @brief Signal novelty detection
     * 
     * @param novelty Novelty value
     */
    void signalNovelty(float novelty);
    
    /**
     * @brief Signal curiosity drive
     * 
     * @param curiosity Curiosity value
     */
    void signalCuriosity(float curiosity);
    
    /**
     * @brief Get list of all neuromodulator names
     * 
     * @return Vector of neuromodulator names
     */
    std::vector<std::string> getNeuromodulatorNames() const;
    
    /**
     * @brief Configure neuromodulators from config
     * 
     * @param config Configuration object
     */
    void configureFromConfig(const std::shared_ptr<Config>& config);
    
    /**
     * @brief Get system statistics
     * 
     * @return Statistics string
     */
    std::string getStatistics() const;
    
    /**
     * @brief Reset all neuromodulators
     */
    void reset();
    
    /**
     * @brief Register callback for neuromodulator changes
     * 
     * @param callback Function to call when neuromodulator levels change
     */
    void registerLevelChangeCallback(std::function<void(const std::string&, float)> callback);
    
    /**
     * @brief Remove all level change callbacks
     */
    void clearLevelChangeCallbacks();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm