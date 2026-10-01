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
    
    // Advanced configuration parameters
    struct AdvancedConfig {
        // Neuromodulation parameters
        float dopamineGain;           // Scaling factor for dopamine effects
        float noveltyThreshold;       // Novelty level needed to trigger exploration
        float curiosityLearningRate;  // How quickly curiosity adapts
        float predictionErrorDecay;   // Decay rate for prediction error
        
        // Developmental parameters
        double developmentalSpeed;    // How fast development progresses
        float plasticityMaturationRate; // Rate of plasticity change during development
        float structuralPlasticityThreshold; // Activity threshold for structural changes
        
        // Behavioral parameters
        float explorationTemperature;  // Temperature parameter for exploration (higher = more random)
        float rewardSensitivity;       // How sensitive the agent is to rewards
        float noveltyAdaptationRate;   // Rate at which novelty detection adapts
        
        // Learning parameters
        float rewardModulationStrength; // Strength of reward modulation effects
        float eligibilityTraceDecay;   // Decay rate for eligibility traces
        float synapticModulationDepth;  // Maximum possible synaptic weight change
        
        // Environmental interaction parameters
        float sensoryIntegrationStrength; // How strongly sensory input affects behavior
        float motorCommandThreshold;     // Minimum activity for action execution
        float actionSelectionTemperature; // Temperature for action selection softmax
        
        // Meta-learning parameters
        float metaLearningRate;        // Rate of meta-learning (learning how to learn)
        float resetFrequency;         // How often to reset for fresh learning
        float consolidationInterval;   // Time between memory consolidation events
        
        // Performance monitoring
        float performanceEvaluationInterval; // How often to evaluate performance
        bool enablePerformanceTracking;     // Whether to track and optimize performance
        
        AdvancedConfig()
            : dopamineGain(1.0f)
            , noveltyThreshold(0.5f)
            , curiosityLearningRate(0.1f)
            , predictionErrorDecay(0.95f)
            , developmentalSpeed(1.0)
            , plasticityMaturationRate(0.5f)
            , structuralPlasticityThreshold(0.7f)
            , explorationTemperature(1.0f)
            , rewardSensitivity(1.0f)
            , noveltyAdaptationRate(0.2f)
            , rewardModulationStrength(1.0f)
            , eligibilityTraceDecay(0.9f)
            , synapticModulationDepth(0.1f)
            , sensoryIntegrationStrength(1.0f)
            , motorCommandThreshold(0.3f)
            , actionSelectionTemperature(1.0f)
            , metaLearningRate(0.01f)
            , resetFrequency(1000.0)
            , consolidationInterval(500.0f)
            , performanceEvaluationInterval(100.0)
            , enablePerformanceTracking(false) {}
    };
    
    AdvancedConfig advancedConfig_;
    
    // Performance monitoring state
    size_t performanceEvaluationCounter_;
    float lastPerformanceScore_;
    size_t stepsWithoutImprovement_;
    
    // Meta-learning state
    float metaLearningProgress_;
    std::vector<float> performanceHistory_;
    std::vector<size_t> optimalActionCounts_;
    
    // Environment adaptation state
    std::vector<float> environmentalStatistics_;
    std::map<std::string, float> learnedResponseRates_;
    std::map<std::string, float> environmentalSensitivity_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

} // namespace nlm
