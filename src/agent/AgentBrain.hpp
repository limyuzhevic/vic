#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <algorithm>
#include <random>

namespace nlm {

// Motor distribution strategies
enum class DistributionStrategy {
    Random,        // Pure random distribution
    Balanced,      // Equal emphasis on important actions
    Learned,       // Based on experience and learned patterns
    Adaptive       // Dynamic based on current state
};

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
    
    // Motor distribution configuration
    void setMotorDistributionStrategy(DistributionStrategy strategy) { 
        motorDistributionStrategy_ = strategy; 
    }
    void setSensoryDistributionStrategy(DistributionStrategy strategy) { 
        sensoryDistributionStrategy_ = strategy; 
    }
    DistributionStrategy getMotorDistributionStrategy() const { return motorDistributionStrategy_; }
    DistributionStrategy getSensoryDistributionStrategy() const { return sensoryDistributionStrategy_; }
    
    // Re-distribute neurons (useful when brain is reconfigured)
    void redistributeNeurons() {
        distributeNeurons();
    }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Neuron distribution methods
    void distributeNeurons();
    void distributeMotorNeurons(size_t totalMotorNeurons);
    void distributeMotorRandom(size_t totalMotorNeurons);
    void distributeMotorBalanced(size_t totalMotorNeurons);
    void distributeMotorLearned(size_t totalMotorNeurons);
    void distributeMotorAdaptive(size_t totalMotorNeurons);
    void distributeMotorWeighted(size_t totalMotorNeurons, 
                                 double forwardWeight, double backwardWeight,
                                 double leftWeight, double rightWeight,
                                 double interactWeight, double waitWeight);
    
    void distributeSensoryNeurons(size_t totalSensoryNeurons);
    void distributeSensoryRandom(size_t totalSensoryNeurons);
    void distributeSensoryOrganized(size_t totalSensoryNeurons);
    void distributeSensoryExperienceBased(size_t totalSensoryNeurons);
    void distributeSensoryAdaptive(size_t totalSensoryNeurons);
    void distributeSensoryWeighted(size_t totalSensoryNeurons,
                                 double visionWeight, double touchWeight,
                                 double internalWeight, double proprioceptionWeight);
    
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
    
    // Distribution strategies
    DistributionStrategy motorDistributionStrategy_;
    DistributionStrategy sensoryDistributionStrategy_;
    
    // Random number generator for distribution
    std::mt19937 rng_;
};

} // namespace nlm
