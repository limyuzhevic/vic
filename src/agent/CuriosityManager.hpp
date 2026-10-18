#pragma once

#include "../agent/SensoryPercept.hpp"
#include "../agent/AgentBody.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Exploration mode enumeration for agent behavior
enum class ExplorationMode {
    Random,              // Pure random exploration
    EpsilonGreedy,       // Epsilon-greedy exploration based on curiosity level
    UCB,                 // Upper Confidence Bound for exploration-exploitation tradeoff
    ThompsonSampling     // Thompson sampling for uncertainty-based exploration
};

// Curiosity strategy enumeration for curiosity computation
enum class CuriosityStrategy {
    NoveltyBased,        // Curiosity based primarily on sensory novelty
    PredErrorBased,      // Curiosity based on prediction error
    Combined,            // Combined novelty and prediction error curiosity
    Intrinsic            // Intrinsic motivation-based curiosity
};

class CuriosityManager {
public:
    CuriosityManager();
    ~CuriosityManager();
    
    // Set previous sensory state for novelty computation
    void setPreviousVision(const std::vector<float>& vision) { previousVision_ = vision; }
    
    // Compute novelty based on sensory input differences
    float computeNovelty(const std::vector<float>& vision) const;
    
    // Compute curiosity level from novelty and prediction error
    float computeCuriosity(float novelty, float predictionError) const;
    
    // Clamp curiosity to valid range [0, 1]
    float clampCuriosity(float curiosity) const;
    
    // Check if exploration should be triggered based on curiosity
    bool shouldExplore(float curiosityLevel, float explorationThreshold = 0.3f) const;
    
    // Get random motor command for exploration
    MotorCommand selectRandomMotorCommand(class RandomGenerator& rng) const;
    
    // Set curiosity behavior parameters
    void setCuriosityParameters(float curiosityGain, float explorationRate, float noveltyDecay) {
        curiosityGain_ = curiosityGain;
        explorationRate_ = explorationRate;
        noveltyDecay_ = noveltyDecay;
    }
    
    // Get curiosity parameters
    float getCuriosityGain() const { return curiosityGain_; }
    float getExplorationRate() const { return explorationRate_; }
    float getNoveltyDecay() const { return noveltyDecay_; }
    
// Get curiosity parameters
    float getCuriosityGain() const { return curiosityGain_; }
    float getExplorationRate() const { return explorationRate_; }
    float getNoveltyDecay() const { return noveltyDecay_; }
    
    // Set exploration mode
    void setExplorationMode(ExplorationMode mode) { explorationMode_ = mode; }
    
    // Set curiosity strategy
    void setCuriosityStrategy(CuriosityStrategy strategy) { curiosityStrategy_ = strategy; }
    
    // Enable/disable learning mode
    void enableLearning(bool enable) { learningEnabled_ = enable; }
    
    // Get behavior history
    std::vector<MotorCommand> getBehaviorHistory() const { return behaviorHistory_; }
    
    // Get action selection statistics
    std::vector<std::pair<MotorCommand, int>> getActionSelectionStats() const;
    
    // Get curiosity behavior parameters
    float getCuriosityGain() const { return curiosityGain_; }
    float getExplorationRate() const { return explorationRate_; }
    float getNoveltyDecay() const { return noveltyDecay_; }
    
    // Reset internal state
    void reset();
    
    // Get current state values
    float getNoveltyLevel() const { return noveltyLevel_; }
    float getCuriosityLevel() const { return curiosityLevel_; }
    
private:
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    
    // Novelty and curiosity state
    float noveltyLevel_;
    float curiosityLevel_;
    
    // Parameters for curiosity computation
    float curiosityGain_;           // Weight of novelty in curiosity computation
    float explorationRate_;         // Maximum probability of exploration based on curiosity
    float noveltyDecay_;             // Decay factor for novelty (0 < decay < 1)
    
    // Exploration and curiosity strategy
    ExplorationMode explorationMode_;  // Exploration strategy
    CuriosityStrategy curiosityStrategy_;  // Curiosity computation strategy
    bool learningEnabled_;             // Whether learning mechanisms are enabled
    
    // Novelty decay counter
    float noveltyDecayCounter_;
    
    // Behavior tracking
    std::vector<MotorCommand> behaviorHistory_;
};

} // namespace nlm
