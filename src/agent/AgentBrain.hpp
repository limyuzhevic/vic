#pragma once

#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../sensory/SensoryPercept.hpp"
#include "../motor/Action.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <memory>
#include <vector>

namespace nlm {

/**
 * AgentBrain: Connects NLM brain to the world
 * Handles sensory transduction and motor decoding
 */
class AgentBrain {
public:
    // Disable copying, enable moving
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&&) noexcept;
    AgentBrain& operator=(AgentBrain&&) noexcept;
    
    /**
     * Initialize agent brain with world
     */
    void initialize(const SimpleWorld& world);
    
    /**
     * Get expected sensory input size
     */
    size_t getSensoryInputSize() const;
    
    /**
     * Get expected motor output size
     */
    size_t getMotorOutputSize() const;
    
    /**
     * Process sensory percept and inject into brain
     */
    void processSensoryInput(const SensoryPercept& percept);
    
    /**
     * Decode brain motor activity into motor command
     */
    MotorCommand decodeMotorCommand();
    
    /**
     * Apply reward-based neuromodulation
     */
    void applyRewardModulation(float reward, float predictedReward);
    
    /**
     * Update development system
     */
    void updateDevelopment(double timestep);
    
    /**
     * Get current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * Get current neuromodulation level
     */
    float getNeuromodulationLevel() const;
    
    /**
     * Get curiosity level
     */
    float getCuriosityLevel() const;
    
    /**
     * Get novelty level
     */
    float getNoveltyLevel() const;
    
    /**
     * Get prediction error
     */
    float getPredictionError() const;
    
    /**
     * Reset agent brain for new episode
     */
    void reset();
    
    /**
     * Get underlying brain
     */
    std::shared_ptr<Brain> getBrain() const;
    
    /**
     * Enable/disable reward modulation
     */
    void enableRewardModulation(bool enable);
    
    /**
     * Enable/disable structural plasticity
     */
    void enableStructuralPlasticity(bool enable);
    
    /**
     * Enable/disable development
     */
    void enableDevelopment(bool enable);
    
    /**
     * Enable/disable curiosity
     */
    void enableCuriosity(bool enable);
    
    /**
     * Check if reward modulation is enabled
     */
    bool isRewardModulationEnabled() const;
    
    /**
     * Check if structural plasticity is enabled
     */
    bool isStructuralPlasticityEnabled() const;
    
    /**
     * Check if development is enabled
     */
    bool isDevelopmentEnabled() const;
    
    /**
     * Check if curiosity is enabled
     */
    bool isCuriosityEnabled() const;
    
    /**
     * Log agent brain status
     */
    void logStatus() const;
    
    // Private implementation details
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
