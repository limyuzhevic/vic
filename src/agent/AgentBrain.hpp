#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../environment/Environment.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>

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
    
    // Get expected reward
    float getExpectedReward() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    const Brain* getBrain() const { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Enhanced agent brain functionality
    
    // Agent body interface
    void setBody(std::shared_ptr<AgentBody> body) { body_ = body; }
    AgentBody* getBody() { return body_.get(); }
    const AgentBody* getBody() const { return body_.get(); }
    
    // World interface
    void setWorld(std::shared_ptr<SimpleWorld> world) { world_ = world; }
    SimpleWorld* getWorld() { return world_.get(); }
    const SimpleWorld* getWorld() const { return world_.get(); }
    
    // Environment interface
    void setEnvironment(std::shared_ptr<Environment> environment) { environment_ = environment; }
    Environment* getEnvironment() { return environment_.get(); }
    const Environment* getEnvironment() const { return environment_.get(); }
    
    // Action execution
    ActionResult executeAction(const MotorCommand& command);
    ActionResult executeAction(const std::string& actionName);
    
    // Sensory processing
    SensoryPercept observe() const;
    void updateSensoryExpectations();
    
    // Reward processing
    void provideReward(float reward);
    void provideIntrinsicReward(float reward);
    void applyCreditAssignment(float reward, float predictedReward);
    
    // Learning and adaptation
    void learnFromExperience(const std::vector<float>& experience);
    void consolidateMemory();
    void resetForLearning();
    
    // State management
    std::string serializeState() const;
    bool deserializeState(const std::string& stateData);
    bool saveStateToFile(const std::string& filename) const;
    bool loadStateFromFile(const std::string& filename);
    
    // Performance monitoring
    void startPerformanceMonitoring();
    void stopPerformanceMonitoring();
    double getAverageStepTime() const;
    size_t getStepsExecuted() const;
    
    // Configuration management
    void setLearningRate(float rate) { learningRate_ = rate; }
    void setExplorationRate(float rate) { explorationRate_ = rate; }
    void setMemoryCapacity(size_t capacity) { memoryCapacity_ = capacity; }
    void setPlanningHorizon(size_t horizon) { planningHorizon_ = horizon; }
    
    float getLearningRate() const { return learningRate_; }
    float getExplorationRate() const { return explorationRate_; }
    size_t getMemoryCapacity() const { return memoryCapacity_; }
    size_t getPlanningHorizon() const { return planningHorizon_; }
    
    // Callbacks and event handlers
    using RewardCallback = std::function<void(float, float)>;
    using StateChangeCallback = std::function<void()>;
    using ActionCallback = std::function<ActionResult(const MotorCommand&)>;
    
    void setRewardCallback(const RewardCallback& callback) { rewardCallback_ = callback; }
    void setStateChangeCallback(const StateChangeCallback& callback) { stateChangeCallback_ = callback; }
    void setActionCallback(const ActionCallback& callback) { actionCallback_ = callback; }
    
    // Advanced features
    void enableMetaLearning(bool enable) { metaLearningEnabled_ = enable; }
    void enableTransferLearning(bool enable) { transferLearningEnabled_ = enable; }
    void enableSelfMonitoring(bool enable) { selfMonitoringEnabled_ = enable; }
    
    bool isMetaLearningEnabled() const { return metaLearningEnabled_; }
    bool isTransferLearningEnabled() const { return transferLearningEnabled_; }
    bool isSelfMonitoringEnabled() const { return selfMonitoringEnabled_; }
    
    // Metrics and statistics
    float getTotalReward() const { return totalReward_; }
    float getAverageReward() const { return stepsExecuted_ > 0 ? totalReward_ / stepsExecuted_ : 0.0f; }
    size_t getActionCount(MotorCommand cmd) const;
    float getActionEfficiency(MotorCommand cmd) const;
    
    // Debug and analysis
    std::string getDebugInfo() const;
    void printStatus() const;
    void printPerformanceStats() const;
    void printLearningStats() const;
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Sensory neuron group
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBody> body_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<Environment> environment_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    float learningRate_;
    float explorationRate_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    bool metaLearningEnabled_;
    bool transferLearningEnabled_;
    bool selfMonitoringEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Performance monitoring
    bool performanceMonitoringActive_;
    double totalCPUTime_;
    double totalWallTime_;
    size_t stepsExecuted_;
    double averageStepTime_;
    
    // Learning and memory
    size_t memoryCapacity_;
    size_t planningHorizon_;
    float totalReward_;
    
    // Statistics
    std::unordered_map<MotorCommand, size_t> actionCounts_;
    std::unordered_map<MotorCommand, float> actionEfficiencies_;
    
    // Callbacks
    RewardCallback rewardCallback_;
    StateChangeCallback stateChangeCallback_;
    ActionCallback actionCallback_;
}; // namespace nlm

} // namespace nlm