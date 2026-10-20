#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <map>
#include <functional>

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
    
    // Get prediction error
    float getPredictionError() const;
    
    // Reset agent for new episode
    void reset();
    
    // ========== ADVANCED BRAIN CONTROL ==========
    
    // Direct developmental stage control
    void setDevelopmentStage(DevelopmentalStage stage);
    
    // Set plasticity modifier (0.1-2.0)
    void setPlasticityModifier(float modifier);
    
    // Set target firing rate for all neurons
    void setTargetFiringRate(float rate);
    
    // Set learning rate for plasticity rules
    void setLearningRate(float learningRate);
    
    // Set curiosity threshold for exploration (0.0-1.0)
    void setCuriosityThreshold(float threshold);
    
    // ========== SIMULATION CONTROL ==========
    
    // Run a complete episode with world interaction
    bool runEpisode(std::shared_ptr<SimpleWorld> world, size_t maxSteps);
    
    // Train brain using reward function
    bool trainWithRewards(std::shared_ptr<SimpleWorld> world, size_t numEpisodes, 
                         std::function<float(const std::vector<float>&)> rewardFn);
    
    // ========== STATISTICAL ANALYSIS ==========
    
    // Get comprehensive network statistics
    std::map<std::string, float> getNetworkStats() const;
    
    // Get activity profile over time window
    std::vector<float> getActivityProfile(size_t timeWindow) const;
    
    // Get memory system statistics
    std::map<std::string, float> getMemoryStats() const;
    
    // Get prediction system statistics
    std::map<std::string, float> getPredictionStats() const;
    
    // ========== SENSORY PROCESSING ==========
    
    // Set sensory input weighting
    void setSensoryWeight(SensoryInputType inputType, float weight);
    
    // Get current sensory input weighting
    float getSensoryWeight(SensoryInputType inputType) const;
    
    // ========== CONFIGURATION MANAGEMENT ==========
    
    // Import configuration from file
    bool importConfig(const std::string& filepath);
    
    // Export configuration to file
    bool exportConfig(const std::string& filepath) const;
    
    // Validate current configuration
    bool validateConfig() const;
    
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
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

} // namespace nlm
