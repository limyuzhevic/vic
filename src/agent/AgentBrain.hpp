#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <unordered_map>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    // Configuration structure for neuron group sizes
    struct NeuronConfig {
        size_t visionSize = 256;           // Vision input size (16x16)
        size_t touchSize = 8;              // Touch input size
        size_t internalSize = 4;           // Internal signal size
        size_t proprioceptionSize = 6;    // Proprioception size
        size_t motorSize = 6;              // Motor output size
        
        // Scaling factors for sensory input processing
        float visionScaleFactor = 5.0f;    // Vision scaling
        float touchScaleFactor = 8.0f;     // Touch scaling
        float internalCenterFactor = 2.0f; // Internal center
        float proprioceptionCenterFactor = 2.0f; // Proprioception center
        float visionScaleOffset = 5.0f;    // Vision scale offset
        float internalScaleOffset = 5.0f;  // Internal scale offset
        float proprioceptionScaleOffset = 3.0f; // Proprioception scale offset
        
        // Curioisty and exploration parameters
        float curiosityThreshold = 0.3f;   // Curiosity threshold
        float explorationChance = 0.3f;    // Max exploration probability
        float noveltyDecay = 0.99f;        // Novelty decay factor
        
        // Plasticity and development parameters
        float plasticityModifier = 1.0f;   // Initial plasticity
        float developmentStage1 = 60.0;    // Stage 1 threshold
        float developmentStage2 = 300.0;   // Stage 2 threshold
        float developmentStage3 = 900.0;   // Stage 3 threshold
        
        // Reward and neuromodulation parameters
        float predictionErrorWeight = 0.5f; // Prediction error weight
        float rewardModulationWeight = 1.0f; // Reward modulation weight
        float plasticityFactorRange = 2.0f; // Plasticity factor range
    };
    
    AgentBrain(std::shared_ptr<Brain> brain, const NeuronConfig& config = NeuronConfig{});
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
    
    // Validation and utility methods
    bool validateNeuronGroups() const;
    size_t getTotalNeuronCount() const;
    void logNeuronDistribution() const;
    
    // Configuration loading
    void loadFromConfig(const std::shared_ptr<Config>& config);
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Helper methods for initialization
    void distributeNeurons(const Brain* brain);
    void initializeMotorGroups();
    void initializeSensoryGroups();
    
    // Validation helpers
    void validateArrayBounds(const std::vector<float>& array, const std::vector<float>& input) const;
    bool validateNeuronExists(const Neuron* neuron) const;
    
    // Configuration helpers
    void applyConfiguration();
    
    std::shared_ptr<Brain> brain_;
    NeuronConfig config_;
    
    // Neuron groups using std::reference_wrapper for memory safety
    using NeuronRef = std::reference_wrapper<Neuron>;
    using NeuronRefVec = std::vector<NeuronRef>;
    
    // Motor neuron groups
    NeuronRefVec motorForward_;
    NeuronRefVec motorBackward_;
    NeuronRefVec motorTurnLeft_;
    NeuronRefVec motorTurnRight_;
    NeuronRefVec motorInteract_;
    NeuronRefVec motorWait_;
    
    // Sensory neuron groups
    NeuronRefVec sensoryVision_;
    NeuronRefVec sensoryTouch_;
    NeuronRefVec sensoryInternal_;
    NeuronRefVec sensoryProprioception_;
    
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
    
    // Helper for tracking active neurons
    std::unordered_map<Neuron*, bool> activeNeuronTracker_;
};

} // namespace nlm
