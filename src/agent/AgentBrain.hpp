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
    // Agent configuration structure
    struct Config {
        float visionNoiseLevel = 0.01f;           // Vision input noise
        float motorThreshold = 0.5f;              // Minimum activity to trigger action
        float curiosityThreshold = 0.3f;         // Minimum curiosity for exploration
        float rewardPredictionWindow = 10.0f;     // Window for reward prediction
        float developmentStageDuration[4] = {60.0f, 240.0f, 660.0f, 3660.0f};  // Developmental stage durations
        float plasticityModifiers[4] = {1.0f, 0.8f, 0.5f, 0.2f};            // Plasticity per developmental stage
        bool enableExploration = true;           // Enable curiosity-driven exploration
        bool enableRewardPrediction = true;      // Enable reward prediction
        bool enableActionPlanning = true;        // Enable action planning
        bool enableMemoryIntegration = true;      // Enable memory integration
        int maxActions = 8;                      // Maximum number of actions supported
        float actionSelectionTemperature = 1.0f; // Temperature for softmax action selection
        float memoryConsolidationThreshold = 0.7f; // Threshold for memory consolidation
    };

    AgentBrain(std::shared_ptr<Brain> brain, const Config& config = Config());
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
    
    // Get expected reward
    float getExpectedReward() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    void setConfig(const Config& config) { config_ = config; }
    const Config& getConfig() const { return config_; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Action planning methods
    void enableActionPlanning(bool enable) { config_.enableActionPlanning = enable; }
    void enableMemoryIntegration(bool enable) { config_.enableMemoryIntegration = enable; }
    
    // Statistics and debugging
    float getActionSelectionEntropy() const { return actionSelectionEntropy_; }
    void logStatus() const;
    
    // Event-driven processing
    void registerActionCallback(std::function<void(MotorCommand)> callback) {
        actionCallback_ = callback;
    }
    
    // Reward prediction
    void updateRewardPrediction(float reward) {
        if (config_.enableRewardPrediction) {
            rewardHistory_.push_back(reward);
            if (rewardHistory_.size() > static_cast<size_t>(config_.rewardPredictionWindow)) {
                rewardHistory_.pop_front();
            }
        }
    }
    
    float getPredictedReward() const {
        if (rewardHistory_.empty()) return 0.0f;
        float sum = 0.0f;
        for (float reward : rewardHistory_) {
            sum += reward;
        }
        return sum / static_cast<float>(rewardHistory_.size());
    }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Action planning methods
    MotorCommand planAction();
    MotorCommand selectActionWithSoftmax(const std::vector<std::pair<MotorCommand, float>>& actions);
    
    // Memory integration
    void integrateMemory();
    void consolidateMemory(float threshold);
    
    std::shared_ptr<Brain> brain_;
    Config config_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    std::vector<Neuron*> motorLookLeft_;
    std::vector<Neuron*> motorLookRight_;
    std::vector<Neuron*> motorEat_;
    std::vector<Neuron*> motorSleep_;
    std::vector<Neuron*> motorDrink_;
    
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
    
    // Reward prediction system
    std::deque<float> rewardHistory_;
    
    // Action planning system
    float actionSelectionEntropy_;
    std::function<void(MotorCommand)> actionCallback_;
    
    // Event system for extensibility
    std::vector<std::function<void()>> eventListeners_;
};

} // namespace nlm
