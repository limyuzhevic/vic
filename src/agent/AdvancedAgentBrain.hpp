#pragma once

#include "../agent/AgentBrain.hpp"
#include "Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>

namespace nlm {

// AdvancedAgentBrain: Enhanced version with advanced cognitive capabilities
// Extends AgentBrain with predictive coding, meta-cognition, and advanced learning
class AdvancedAgentBrain : public AgentBrain {
public:
    // Constructor with advanced capabilities
    AdvancedAgentBrain(std::shared_ptr<Brain> brain);
    
    // Enhanced initialization with prediction and learning systems
    void initialize(const SimpleWorld& world) override;
    
    // Enhanced sensory processing with prediction error
    void processSensoryInput(const SensoryPercept& percept) override;
    
    // Advanced action selection with planning and meta-cognition
    MotorCommand decodeMotorCommand() override;
    
    // Enhanced reward processing with prediction error
    void applyRewardModulation(float reward, float predictedReward) override;
    
    // Update development and cognitive systems
    void updateDevelopment(double timestep) override;
    
    // Advanced cognitive functions
    void updatePredictionSystem();
    void updateCognitiveSystems(double timestep);
    void updateMetaCognitiveSystems(double timestep);
    
    // Learning enhancement capabilities
    void enableEnhancedLearning(bool enable) { enhancedLearningEnabled_ = enable; }
    bool isEnhancedLearningEnabled() const { return enhancedLearningEnabled_; }
    
    // Meta-cognitive awareness
    float getPredictionError() const override { return predictionError_; }
    float getNoveltyLevel() const override { return noveltyLevel_; }
    float getCuriosityLevel() const override { return curiosityLevel_; }
    
    // Advanced monitoring
    float getPredictionAccuracy() const { return predictionAccuracy_; }
    float getCognitiveLoad() const { return cognitiveLoad_; }
    double getAttentionSpan() const { return attentionSpan_; }
    
    // Meta-cognitive control
    void regulateAttention(float focusLevel);
    void adjustCognitiveLoad(float loadLevel);
    void updateMetaAwareness(double timestep);
    
    // Enhanced state management
    void reset() override;
    std::string getAdvancedState() const;
    
    // Performance monitoring
    void logAdvancedMetrics() const;
    
private:
    // Enhanced neuromodulation with prediction error
    void applyEnhancedNeuromodulation(float reward, float predictedReward, float predictionError);
    
    // Predictive processing pipeline
    void runPredictiveProcessing();
    void updatePredictionErrors();
    
    // Meta-cognitive functions
    void evaluateMetaCognitiveState(double timestep);
    void optimizeCognitiveArchitecture(double timestep);
    
    // Advanced action selection
    MotorCommand selectWithMetaCognition(MotorCommand defaultCmd);
    MotorCommand planActionSequence();
    
    // State for advanced functions
    float predictionError_;
    float predictionAccuracy_;
    float cognitiveLoad_;
    double attentionSpan_;
    double metaAwareness_;
    float explorationBonus_;
    
    // Enhanced learning system
    bool enhancedLearningEnabled_;
    float learningRateAdaptation_;
    size_t successfulPredictions_;
    size_t totalPredictions_;
    
    // Planning and meta-cognition
    std::vector<MotorCommand> actionSequence_;
    size_t currentActionIndex_;
    float planningHorizon_;
    
    // Advanced monitoring
    std::vector<float> predictionHistory_;
    std::vector<float> curiosityHistory_;
    double lastMetaUpdateTime_;
};

} // namespace nlm
