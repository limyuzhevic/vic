#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "AdvancedAgentFeatures.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding with advanced features
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();  // Added destructor for memory cleanup
    
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
    
    // Advanced features integration
    void setExplorationStrategy(ExplorationStrategy strategy) {
        if (explorationSystem_) {
            explorationStrategy_ = strategy;
        }
    }
    
    void setUseEntropyBonus(bool use) { useEntropyBonus_ = use; }
    
    void setUCBExplorationFactor(double factor) { ucbExplorationFactor_ = factor; }
    
    void enableMetacognitiveMonitoring(bool enable) { metacognitiveMonitoringEnabled_ = enable; }
    
    void setCurriculumSkill(int skill) {
        if (enhancedLearning_) {
            enhancedLearning_->setCurrentSkill(skill);
        }
    }
    
    bool advanceCurriculumSkill() {
        if (enhancedLearning_) {
            return enhancedLearning_->advanceSkill();
        }
        return false;
    }
    
    void enableHierarchicalStateAbstraction(bool enable) { hierarchicalStateEnabled_ = enable; }
    
    void enableWorkingMemoryConsolidation(bool enable) { workingMemoryEnabled_ = enable; }
    
    void enablePredictiveCoding(bool enable) { predictiveCodingEnabled_ = enable; }
    
    void enableHomeostaticRegulation(bool enable) { homeostaticEnabled_ = enable; }
    
    void enableCrossModalIntegration(bool enable) { crossModalIntegrationEnabled_ = enable; }
    
    void enableBatchProcessing(bool enable) { batchProcessingEnabled_ = enable; }
    
    void enableMemoryPooling(bool enable) { memoryPoolingEnabled_ = enable; }
    
    void enableEarlyTermination(bool enable) { earlyTerminationEnabled_ = enable; }
    
    // Get advanced feature statistics
    const std::vector<float>& getActionValues() const {
        return explorationSystem_ ? explorationSystem_->getActionValues() : std::vector<float>{};
    }
    
    const std::vector<int>& getActionCounts() const {
        return explorationSystem_ ? explorationSystem_->getActionCounts() : std::vector<int>{};
    }
    
    const EnhancedLearningMechanisms::LearningMetrics& getLearningMetrics() const {
        return currentLearningMetrics_;
    }
    
    const ImprovedStateManagement::StressLevel& getStressLevel() const {
        return stateManagement_ ? stateManagement_->assessStress() : ImprovedStateManagement::StressLevel{};
    }
    
    const AdvancedNeuromodulation::NeuromodulatorState& getNeuromodulatorState() const {
        return neuromodulation_ ? neuromodulation_->getNeuromodulatorState() : AdvancedNeuromodulation::NeuromodulatorState{};
    }
    
    float getMemoryUsage() const {
        return performanceOpt_ ? performanceOpt_->getMemoryUsage() : 0.0f;
    }
    
    size_t getBatchSize() const {
        return performanceOpt_ ? performanceOpt_->getBatchSize() : 1;
    }

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
    
    // Advanced features
    ExplorationStrategy explorationStrategy_ = ExplorationStrategy::ThompsonSampling;
    bool useEntropyBonus_ = true;
    double ucbExplorationFactor_ = 2.0;
    bool metacognitiveMonitoringEnabled_ = true;
    
    // Advanced feature instances
    std::unique_ptr<AdaptiveExplorationSystem> explorationSystem_;
    std::unique_ptr<EnhancedLearningMechanisms> enhancedLearning_;
    std::unique_ptr<ImprovedStateManagement> stateManagement_;
    std::unique_ptr<AdvancedNeuromodulation> neuromodulation_;
    std::unique_ptr<PerformanceOptimization> performanceOpt_;
    
    // Hierarchical state abstraction and related flags
    bool hierarchicalStateEnabled_ = false;
    bool workingMemoryEnabled_ = false;
    bool predictiveCodingEnabled_ = false;
    bool homeostaticEnabled_ = false;
    bool crossModalIntegrationEnabled_ = false;
    bool batchProcessingEnabled_ = false;
    bool memoryPoolingEnabled_ = false;
    bool earlyTerminationEnabled_ = false;
    
    // Metacognitive metrics
    EnhancedLearningMechanisms::LearningMetrics currentLearningMetrics_;
    
    // Constants for better maintainability
    static constexpr float DEFAULT_NOVELTY_DECAY = 0.99f;
    static constexpr float MAX_CURIOUSNESS_THRESHOLD = 0.5f;
    static constexpr float EXPLORATION_CHANCE = 0.3f;
    static constexpr float PLASTICITY_MIN = 0.1f;
    static constexpr float PLASTICITY_MAX = 2.0f;
    static constexpr float NOISE_THRESHOLD = 0.5f;
};

} // namespace nlm
