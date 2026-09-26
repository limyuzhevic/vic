#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "MotorDecoder.hpp"
#include "SensoryProcessor.hpp"
#include "Neuromodulator.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

/**
 * @class AgentBrain
 * @brief Connects NLM brain to the world using component-based architecture
 * 
 * This class serves as the main interface between the neural system and the environment.
 * It has been refactored to use composition with focused, single-responsibility
 * components for improved maintainability, testability, and modularity.
 * 
 * The AgentBrain now delegates its responsibilities to:
 * - MotorDecoder: Handles motor neuron decoding and curiosity-based exploration
 * - SensoryProcessor: Handles sensory input processing and distribution
 * - Neuromodulator: Handles neuromodulation, development, and reward processing
 * 
 * This component-based design provides:
 * - Clean separation of concerns
 * - Easier unit testing
 * - Better maintainability
 * - Enhanced modularity
 * - Backward compatibility through unchanged public API
 */
class AgentBrain {
public:
    /**
     * @brief Constructor for AgentBrain
     * 
     * @param brain Pointer to the brain for neural access
     * @param world Pointer to the world for environmental context
     * @throw std::invalid_argument if brain pointer is null
     */
    AgentBrain(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world);
    
    /**
     * @brief Destructor
     */
    ~AgentBrain();
    
    /**
     * @brief Initialize agent with world context
     * 
     * @param world The world to initialize with
     * @pre world pointer must not be null
     */
    void initialize(const SimpleWorld& world);
    
    /**
     * @brief Get sensory input size expected by brain
     * 
     * @return size_t Expected sensory input size
     */
    size_t getSensoryInputSize() const;
    
    /**
     * @brief Get motor output size expected
     * 
     * @return size_t Expected motor output size
     */
    size_t getMotorOutputSize() const;
    
    /**
     * @brief Process sensory percept and inject into brain
     * 
     * @param percept The sensory percept to process
     * @pre percept must contain valid sensory data
     */
    void processSensoryInput(const SensoryPercept& percept);
    
    /**
     * @brief Decode brain motor activity into motor command
     * 
     * @return MotorCommand The decoded motor command
     */
    MotorCommand decodeMotorCommand();
    
    /**
     * @brief Apply neuromodulation based on reward
     * 
     * @param reward Current reward signal
     * @param predictedReward Predicted reward based on expectations
     */
    void applyRewardModulation(float reward, float predictedReward);
    
    /**
     * @brief Update development system
     * 
     * @param timestep Time since last update
     */
    void updateDevelopment(double timestep);
    
    /**
     * @brief Get current developmental stage
     * 
     * @return DevelopmentalStage Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Get neuromodulation level (dopamine)
     * 
     * @return float Neuromodulation level (-1.0 to 1.0)
     */
    float getNeuromodulationLevel() const;
    
    /**
     * @brief Get curiosity level
     * 
     * @return float Current curiosity level (0.0-1.0)
     */
    float getCuriosityLevel() const;
    
    /**
     * @brief Get novelty level
     * 
     * @return float Current novelty level (0.0-1.0)
     */
    float getNoveltyLevel() const;
    
    /**
     * @brief Get prediction error
     * 
     * @return float Current prediction error
     */
    float getPredictionError() const;
    
    /**
     * @brief Reset agent for new episode
     */
    void reset();
    
    /**
     * @brief Get brain pointer
     * 
     * @return Brain* Pointer to the brain, or nullptr if brain_ is null
     */
    Brain* getBrain() { return brain_.get(); }
    
    /**
     * @brief Enable or disable reward modulation
     * 
     * @param enable True to enable reward modulation, false to disable
     */
    void enableRewardModulation(bool enable) { neuromodulator_->enableRewardModulation(enable); }
    
    /**
     * @brief Enable or disable structural plasticity
     * 
     * @param enable True to enable structural plasticity, false to disable
     */
    void enableStructuralPlasticity(bool enable) { neuromodulator_->enableStructuralPlasticity(enable); }
    
    /**
     * @brief Enable or disable development
     * 
     * @param enable True to enable development, false to disable
     */
    void enableDevelopment(bool enable) { neuromodulator_->enableDevelopment(enable); }
    
    /**
     * @brief Enable or disable curiosity
     * 
     * @param enable True to enable curiosity, false to disable
     */
    void enableCuriosity(bool enable) { motorDecoder_->enableCuriosity(enable); }
    
    /**
     * @brief Check if reward modulation is enabled
     * 
     * @return bool True if reward modulation is enabled
     */
    bool isRewardModulationEnabled() const { return neuromodulator_->isRewardModulationEnabled(); }
    
    /**
     * @brief Check if structural plasticity is enabled
     * 
     * @return bool True if structural plasticity is enabled
     */
    bool isStructuralPlasticityEnabled() const { return neuromodulator_->isStructuralPlasticityEnabled(); }
    
    /**
     * @brief Check if development is enabled
     * 
     * @return bool True if development is enabled
     */
    bool isDevelopmentEnabled() const { return neuromodulator_->isDevelopmentEnabled(); }
    
    /**
     * @brief Check if curiosity is enabled
     * 
     * @return bool True if curiosity is enabled
     */
    bool isCuriosityEnabled() const { return motorDecoder_->isCuriosityEnabled(); }

private:
    // Core components
    std::shared_ptr<Brain> brain_;            ///< Brain for neural computation
    std::shared_ptr<SimpleWorld> world_;       ///< World for environmental context
    std::unique_ptr<MotorDecoder> motorDecoder_; ///< Motor decoding and curiosity
    std::unique_ptr<SensoryProcessor> sensoryProcessor_; ///< Sensory input processing
    std::unique_ptr<Neuromodulator> neuromodulator_; ///< Neuromodulation and development
};

} // namespace nlm
