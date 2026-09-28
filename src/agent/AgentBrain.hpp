#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialize with world
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
    float getCuriosityLevel() const;
    
    // Get novelty level
    float getNoveltyLevel() const;
    
// Get novelty level
    float getNoveltyLevel() const;
    
    // Get prediction error
    float getPredictionError() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // ========== ADVANCED AGENT CONTROL ==========
    
    // Fine-tune agent behavior parameters
    void setActionThreshold(float threshold);
    void setExplorationRate(float rate);
    void setLearningRate(float rate);
    void setRewardScale(float scale);
    void setMemoryCapacity(size_t capacity);
    
    // Configuration presets
    void configureForSpeed();
    void configureForExploration();
    void configureForLearning();
    void configureForPerformance();
    
    // Development and learning helpers
    void resetDevelopmentalProgress();
    std::tuple<float, float, float> getDevelopmentStats() const;
    
    // Memory system helpers (if available)
    std::tuple<size_t, float, float> getMemoryStats() const;
    
    // ========== CONVENIENCE METHODS ==========
    
    // Run a complete episode with optional learning
    void runEpisode(const SimpleWorld& world, size_t numSteps, bool enableLearning = true);
    
    // Helper function to run a complete simulation with agent
    static AgentBrain runSimulation(const SimpleWorld& world, 
                                    const std::shared_ptr<Brain>& brain,
                                    size_t numSteps);
    
    // Sensory processing convenience methods
    std::tuple<float, size_t, size_t> computeSensoryStats(const SensoryPercept& percept) const;
    void normalizeSensoryInput(const std::vector<float>& targetRange);
    
private:
    // Internal parameters
    float actionThreshold_;
    float explorationRate_;
    float learningRate_;
    float rewardScale_;
    size_t memoryCapacity_;
    
    // Development reset helper
    void resetDevelopmentalHelper();
};

} // namespace nlm
