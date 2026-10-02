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
    
    // Optimized sensory processing
    void processVisionInput(const std::vector<float>& vision);
    void processTouchInput(const std::vector<float>& touch);
    void processInternalInput(const std::vector<float>& internal);
    void processProprioceptionInput(const std::vector<float>& proprio);
    void processSmellInput(const std::vector<float>& smell);
    void processTasteInput(const std::vector<float>& taste);
    
    // Optimized novelty detection
    void computeNoveltyLevel(const std::vector<float>& vision);
    
    // Random action selection for exploration
    MotorCommand getRandomMotorCommand() const;
    
    // Additional helper methods for new sensory modalities
    void processAudioInput(const std::vector<float>& audio);
    
    // Development methods
    void applyRewardModulation(float reward, float predictedReward);
    void updateDevelopment(double timestep);
    DevelopmentalStage getDevelopmentalStage() const;
    float getNeuromodulationLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Learning rate control
    void adaptiveLearningRate(float error);
    void scheduleLearningRateChange(float newRate, double timestep);
    
    // Configuration and export
    // Network topology export
    std::string exportNetworkTopology() const;
    
    // Behavioral pattern recording
    void recordBehaviorPattern(const std::string& patternName);
    
    // State serialization formats
    bool saveState(const std::string& filepath) const;
    bool loadState(const std::string& filepath);
    
    // Configuration
    void setLearningRate(float rate) { learningRate_ = rate; }
    void setWeightDecay(float decay) { weightDecay_ = decay; }
    void setExplorationRate(float rate) { explorationRate_ = rate; }
    void setTargetNetworkUpdateInterval(int interval) { targetNetworkUpdateInterval_ = interval; }
    
    float getLearningRate() const { return learningRate_; }
    float getWeightDecay() const { return weightDecay_; }
    float getExplorationRate() const { return explorationRate_; }
    int getTargetNetworkUpdateInterval() const { return targetNetworkUpdateInterval_; }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
// Helper methods for neuron distribution
    void distributeMotorNeurons(const NeuralPopulation* population);
    void distributeSensoryNeurons(const NeuralPopulation* population);
    
    // Optimized sensory processing
    void processVisionInput(const std::vector<float>& vision);
    void processTouchInput(const std::vector<float>& touch);
    void processInternalInput(const std::vector<float>& internal);
    void processProprioceptionInput(const std::vector<float>& proprio);
    void processSmellInput(const std::vector<float>& smell);
    void processTasteInput(const std::vector<float>& taste);
    
    // Optimized novelty detection
    void computeNoveltyLevel(const std::vector<float>& vision);
    
    // Random action selection for exploration
    MotorCommand getRandomMotorCommand() const;

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
    std::vector<Neuron*> sensorySmell_;
    std::vector<Neuron*> sensoryTaste_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Learning rate control
    float learningRate_;
    float weightDecay_;
    float explorationRate_;
    int targetNetworkUpdateInterval_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Constants
    static constexpr float DEFAULT_SENSORY_NOVELTY_DECAY_ = 0.99f;
    static constexpr float NEURON_SCALE_FACTOR_VISION_ = 5.0f;
    static constexpr float NEURON_SCALE_FACTOR_TOUCH_ = 8.0f;
    static constexpr float NEURON_SCALE_FACTOR_INTERNAL_ = 5.0f;
    static constexpr float NEURON_SCALE_FACTOR_PROPRIOCEPTION_ = 3.0f;
    static constexpr float NEURON_SCALE_FACTOR_SMELL_ = 4.0f;
    static constexpr float NEURON_SCALE_FACTOR_TASTE_ = 4.0f;
    static constexpr float NEURON_ACTIVITY_THRESHOLD_ = 0.5f;
    static constexpr float NEURON_SCALE_FACTOR_INITIAL_ = 1.0f;
    static constexpr float NEURON_SCALE_FACTOR_CRITICAL_ = 0.8f;
    static constexpr float NEURON_SCALE_FACTOR_MATURE_ = 0.5f;
    static constexpr float NEURON_SCALE_FACTOR_ADULT_ = 0.2f;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Optimized configuration constants
    static constexpr size_t MAX_VISION_DIMENSIONS_ = 256;
    static constexpr size_t MAX_TOUCH_DIMENSIONS_ = 8;
    static constexpr size_t MAX_INTERNAL_DIMENSIONS_ = 4;
    static constexpr size_t MAX_PROPRIOCEPTION_DIMENSIONS_ = 6;
    static constexpr size_t MAX_SMELL_DIMENSIONS_ = 16;
    static constexpr size_t MAX_TASTE_DIMENSIONS_ = 8;
    static constexpr size_t MOTOR_COMMAND_COUNT_ = 6;
    static constexpr float NOVELTY_THRESHOLD_ = 0.001f;
    static constexpr float PREDICTION_ERROR_NOISE_ = 0.005f;
    
    // Developmental stage constants
    static constexpr double DEVELOPMENTAL_STAGE_INITIAL_ = 60.0;
    static constexpr double DEVELOPMENTAL_STAGE_CRITICAL_ = 300.0;
    static constexpr double DEVELOPMENTAL_STAGE_MATURATION_ = 900.0;
};

} // namespace nlm
