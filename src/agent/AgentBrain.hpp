#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../performance/Performance.hpp"
#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>
#include <numeric>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding with advanced capabilities
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
    
    // Get spatial context from brain
    std::pair<float, float> getSpatialContext() const;

    // Get spatial representation status
    bool hasSpatialContext() const;

    // Get temporal context for sequence prediction
    std::vector<float> getTemporalContext() const;
    
    // Get temporal representation status
    bool hasTemporalContext() const;
    
    // Get self-model context for self-awareness
    std::vector<float> getSelfModelContext() const;
    
    // Get self-model status
    bool hasSelfModel() const;

    // Get body awareness (how accurately can predict consequences)
    float getBodyAwareness() const;
    
    // Get current capability level (0-1)
    float getCapabilityLevel() const;
    
    // Get social learning context for social cognition
    std::vector<float> getSocialContext() const;
    
    // Get social learning status
    bool hasSocialLearning() const;
    
    // Get social observation count
    size_t getSocialObservationCount() const;
    
    // Check if can imitate observed actions
    bool canImitate(ActionType observedAction) const;
    
    // Get imitation action based on current state
    ActionType getImitationAction(const std::vector<float>& currentState) const;
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Enhanced motor command generation with context awareness
    MotorCommand decodeMotorCommandWithContext(const std::vector<float>& context = {});
    MotorCommand decodeMotorCommandWithCuriosity(float curiosityThreshold = 0.5f);
    MotorCommand decodeMotorCommandWithExploration(float explorationRate = 0.1f);
    
    // Enhanced sensory processing
    std::vector<float> preprocessVision(const std::vector<float>& rawVision) const;
    std::vector<float> preprocessTouch(const std::vector<float>& rawTouch) const;
    std::vector<float> preprocessInternal(const std::vector<float>& rawInternal) const;
    std::vector<float> preprocessProprioception(const std::vector<float>& rawProprioception) const;
    
    // Novelty and pattern detection
    float detectVisualNovelty(const std::vector<float>& currentVision) const;
    float detectPattern(const std::vector<float>& signal) const;
    std::vector<float> extractFeatures(const std::vector<float>& signal) const;
    
    // Advanced reward-based learning
    void applyTemporalDifferenceLearning(float reward, float predictedReward, float gamma = 0.95f);
    void applyPolicyGradient(const std::vector<float>& state, float reward, float learningRate = 0.01f);
    void applyEligibilityTrace(const std::vector<float>& state, float decay = 0.99f);
    
    // Performance monitoring
    void updatePerformanceStats(double timestep);
    float getAverageFrameTime() const;
    float getAverageProcessingTime() const;
    size_t getProcessedSensoryEvents() const;
    float getCPUUsage() const;
    float getMemoryUsage() const;
    
    // Debug and visualization tools
    std::string generateDebugReport() const;
    void logState() const;
    void visualizeState() const;
    void exportState(const std::string& filename) const;
    
    // Agent control and configuration commands
    void setBehaviorMode(int mode);
    void setExplorationRate(float rate);
    void setCuriosityThreshold(float threshold);
    void setRewardWeight(float weight);
    void setNoveltyThreshold(float threshold);
    
    int getBehaviorMode() const;
    float getExplorationRate() const;
    float getCuriosityThreshold() const;
    float getRewardWeight() const;
    float getNoveltyThreshold() const;
    
    // Advanced learning and adaptation
    void enableLearning(bool enable);
    void enableAdaptivePlasticity(bool enable);
    void enableMetaLearning(bool enable);
    
    bool isLearningEnabled() const;
    bool isAdaptivePlasticityEnabled() const;
    bool isMetaLearningEnabled() const;
    
    // Performance optimization features
    void enableOptimization(bool enable);
    void setTargetFrameRate(float fps);
    void setMemoryBudget(size_t bytes);
    void enableSIMD(bool enable);
    void enableParallelProcessing(bool enable);
    
    bool isOptimizationEnabled() const;
    float getTargetFrameRate() const;
    size_t getMemoryBudget() const;
    bool isSIMDEnabled() const;
    bool isParallelProcessingEnabled() const;
    
    // Simulation and control commands
    void step(double timestep);
    void pause() { isPaused_ = true; }
    void resume() { isPaused_ = false; }
    bool isPaused() const { return isPaused_; }
    
    // Command interface
    void executeCommand(const std::string& command, const std::vector<float>& params = {});
    bool isCommandAvailable(const std::string& command) const;
    
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
