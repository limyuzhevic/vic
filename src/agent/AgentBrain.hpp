#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <random>

namespace nlm {

// Motor command selection strategy
enum class MotorDistributionStrategy {
    Uniform,          // Equal distribution across all motor groups
    Weighted,         // Custom weights for each motor group
    Hierarchical,     // Priority-based distribution
    Random,           // Random assignment
    Custom           // User-defined distribution function
};

// Sensory distribution strategy
enum class SensoryDistributionStrategy {
    Uniform,          // Equal distribution across sensory modalities
    Priority,         // Priority-based (vision first)
    Balanced,         // Balanced approach
    Custom           // User-defined distribution function
};

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding with enhanced configurability
class AgentBrain {
public:
    // Configuration structure for advanced users
    struct DistributionConfig {
        MotorDistributionStrategy motorStrategy = MotorDistributionStrategy::Uniform;
        SensoryDistributionStrategy sensoryStrategy = SensoryDistributionStrategy::Priority;
        
        // Motor weights for weighted distribution (size 6 for 6 motor actions)
        std::vector<float> motorWeights;
        
        // Sensory weights for weighted distribution (size 4 for 4 modalities)
        std::vector<float> sensoryWeights;
        
        // Development parameters
        double developmentalAge = 0.0;
        float plasticityModifier = 1.0f;
        
        // Neuromodulation parameters
        float dopamineDecay = 0.95f;
        float noveltyDecay = 0.99f;
        float curiosityThreshold = 0.3f;
        
        // Advanced features
        bool enableNoiseInjection = false;
        float noiseLevel = 0.1f;
        bool enableAdaptiveWeights = false;
        
        DistributionConfig() {
            motorWeights = std::vector<float>(6, 1.0f);  // Equal weights
            sensoryWeights = std::vector<float>(4, 1.0f);  // Equal weights
        }
    };
    
    AgentBrain(std::shared_ptr<Brain> brain, const DistributionConfig& config = DistributionConfig());
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
    
    // Enhanced configuration methods
    void setDistributionConfig(const DistributionConfig& config) { distributionConfig_ = config; }
    void setMotorDistributionStrategy(MotorDistributionStrategy strategy);
    void setSensoryDistributionStrategy(SensoryDistributionStrategy strategy);
    void setMotorWeights(const std::vector<float>& weights);
    void setSensoryWeights(const std::vector<float>& weights);
    void enableAdvancedFeatures(bool enable, float noiseLevel = 0.1f);
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    bool areAdvancedFeaturesEnabled() const { return advancedFeaturesEnabled_; }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Enhanced distribution methods
    void distributeMotorNeurons();
    void distributeSensoryNeurons();
    void applyNoiseToSensoryInput(std::vector<float>& signals) const;
    
    // Utility functions
    float calculateWeightedActivity(const std::vector<Neuron*>& neurons) const;
    void normalizeWeights(std::vector<float>& weights) const;
    
    std::shared_ptr<Brain> brain_;
    
    // Distribution configuration
    DistributionConfig distributionConfig_;
    
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
    bool advancedFeaturesEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Random number generator for distribution
    mutable std::mt19937 rng_;
};

} // namespace nlm
