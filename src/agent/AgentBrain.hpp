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
    // ========== ADVANCED FEATURE EXTRACTION ==========
    
    // Extract and process vision features
    void extractVisionFeatures(const std::vector<float>& vision);
    
    // Extract and process touch features
    void extractTouchFeatures(const std::vector<float>& touch);
    
    // Extract and process internal features
    void extractInternalFeatures(const std::vector<float>& intern);
    
    // Extract and process proprioception features
    void extractProprioceptionFeatures(const std::vector<float>& proprio);
    
    // Fuse features from all sensory modalities
    std::vector<float> fuseSensoryFeatures();
    
    // Apply temporal smoothing for stability
    void applyTemporalSmoothing(std::vector<float>& features);
    
    // Compute novelty from feature differences
    float computeFeatureNovelty(const std::vector<float>& current, const std::vector<float>& previous);
    
    // Update action values using learning
    void updateActionValues();
    
    // Compute curiosity signal for exploration
    float computeCuriositySignal();
    
    // Inject processed features into brain neural system
    void injectFeaturesIntoBrain(const std::vector<float>& features);
    
    // ========== ADVANCED ACTION SELECTION ==========
    
    // Select action using value-based learning with exploration
    MotorCommand decodeActionWithValueLearning();
    
    // Select action based on curiosity and exploration
    MotorCommand selectActionWithCuriosity(MotorCommand defaultCmd);
    
    // Compute action preferences using softmax
    std::vector<float> computeActionPreferences() const;
    
    // Sample action according to preference distribution
    MotorCommand sampleActionFromPreferences(const std::vector<float>& preferences);
    
    // ========== REWARD AND LEARNING ==========
    
    // Apply reward using temporal difference learning
    void applyRewardWithTDLearning(float reward, float predictedReward);
    
    // Update prediction errors for each action
    void updatePredictionErrors(MotorCommand action, float actualReward);
    
    // Compute expected future rewards
    float computeExpectedFutureRewards(MotorCommand action) const;
    
    // ========== DEVELOPMENT AND ADAPTATION ==========
    
    // Update developmental state based on experience
    void updateDevelopmentalAdaptation();
    
    // Adapt behavior based on learning progress
    void adaptBasedOnLearningProgress();
    
    // Modulate exploration based on developmental stage
    void modulateExplorationByDevelopment();
    
    // ========== MEMORY AND HISTORY ==========
    
    // Store action and outcome in memory
    void storeExperience(MotorCommand action, float reward, const std::vector<float>& state);
    
    // Retrieve similar experiences for pattern completion
    std::vector<std::pair<MotorCommand, float>> retrieveSimilarExperiences(const std::vector<float>& state, size_t maxCount) const;
    
    // Compute action statistics from experience
    void computeActionStatistics();
    
    // ========== UTILITY AND CONFIGURATION ==========
    
    // Set exploration parameters
    void setExplorationEpsilon(float epsilon) { explorationEpsilon_ = epsilon; }
    void setLearningRate(float rate) { learningRate_ = rate; }
    void setDiscountFactor(float factor) { discountFactor_ = factor; }
    
    // Get current exploration level
    float getExplorationLevel() const { return explorationEpsilon_; }
    
    // Get action value estimates
    const std::vector<float>& getActionValues() const { return actionValues_; }
    
    // Get curiosity and novelty levels
    float getCuriosityLevel() const { return curiosityLevel_; }
    float getNoveltyLevel() const { return noveltyLevel_; }
    
    // Get development progress
    float getDevelopmentalProgress() const { return developmentalAge_ / 1000.0f; } // Normalized to [0,1]
    
    // Get experience count
    size_t getExperienceCount() const { return actionHistory_.size(); }
    
    // Clear old experiences to save memory
    void trimExperienceHistory(size_t maxHistory = 0) {
        if (maxHistory == 0) maxHistory_ = maxActionHistory_;
        else maxHistory_ = maxHistory;
        
        while (actionHistory_.size() > maxHistory_) {
            actionHistory_.pop_front();
        }
    }
};

} // namespace nlm

// Experience and statistics structures for learning
struct Experience {
    MotorCommand action;
    float reward;
    std::vector<float> state;
    float timestamp;
};

struct ActionStats {
    MotorCommand action;
    float meanReward;
    float stdDevReward;
    float frequency;
};
