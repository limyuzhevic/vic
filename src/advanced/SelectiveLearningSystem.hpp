#pragma once

/**
 * AdvancedSelectiveLearning - Advanced features for selective pattern learning
 * 
 * Provides advanced capabilities for selective pattern learning, focused attention,
 * and advanced behavioral control. This module extends the basic learning framework
 * with more sophisticated mechanisms for selective learning and pattern recognition.
 * 
 * Features:
 * - Selective attention mechanisms
 * - Pattern-based learning
 * - Advanced reward modulation
 * - Enhanced developmental controls
 * - Behavioral consistency tracking
 * - Advanced metrics and analytics
 */

#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <thread>

namespace nlm {

/**
 * Selective attention mechanism
 */
class SelectiveAttention {
public:
    SelectiveAttention();
    ~SelectiveAttention();
    
    /**
     * Add a pattern to the attention system
     */
    void addPattern(const std::string& patternName, const std::vector<float>& pattern, float priority);
    
    /**
     * Get the most relevant pattern for a given input
     */
    std::string getRelevantPattern(const std::vector<float>& input) const;
    
    /**
     * Update pattern relevance based on feedback
     */
    void updatePatternRelevance(const std::string& patternName, float feedback);
    
    /**
     * Get all patterns sorted by relevance
     */
    std::vector<std::pair<std::string, float>> getPatternRelevance() const;
    
    /**
     * Clear all patterns
     */
    void clearPatterns();
    
    /**
     * Get the current focus area
     */
    std::string getCurrentFocus() const { return currentFocus_; }
    
private:
    std::map<std::string, std::pair<std::vector<float>, float>> patterns_;
    std::string currentFocus_;
    float attentionDecayRate_;
    
    float calculatePatternSimilarity(const std::vector<float>& pattern1, const std::vector<float>& pattern2) const;
};

/**
 * Pattern-based learning system
 */
class PatternBasedLearner {
public:
    PatternBasedLearner();
    ~PatternBasedLearner();
    
    /**
     * Learn a pattern-response association
     */
    void learnPattern(const std::vector<float>& pattern, const std::string& response, float strength);
    
    /**
     * Get the best response for a given pattern
     */
    std::string getBestResponse(const std::vector<float>& pattern) const;
    
    /**
     * Get all learned patterns
     */
    std::map<std::vector<float>, std::string> getLearnedPatterns() const;
    
    /**
     * Update pattern learning based on reward
     */
    void updatePatternLearning(const std::vector<float>& pattern, float reward);
    
    /**
     * Get pattern learning statistics
     */
    struct PatternStats {
        float accuracy;
        float consistency;
        float learningProgress;
        size_t sampleCount;
    };
    
    PatternStats getPatternStats(const std::vector<float>& pattern) const;
    
private:
    std::map<std::vector<float>, std::map<std::string, float>> patternResponses_;
    std::map<std::vector<float>, PatternStats> patternStats_;
    float learningRate_;
    float decayRate_;
};

/**
 * Advanced reward modulation system
 */
class AdvancedRewardModulation {
public:
    AdvancedRewardModulation();
    ~AdvancedRewardModulation();
    
    /**
     * Add a reward rule
     */
    void addRewardRule(const std::string& ruleName, std::function<float(float, const std::vector<float>&)> rewardFunction);
    
    /**
     * Calculate reward using all active rules
     */
    float calculateReward(float baseReward, const std::vector<float>& context) const;
    
    /**
     * Get active reward rules
     */
    std::vector<std::string> getActiveRules() const;
    
    /**
     * Update reward rule based on experience
     */
    void updateRewardRule(const std::string& ruleName, float error, const std::vector<float>& context);
    
    /**
     * Get reward rule statistics
     */
    struct RewardRuleStats {
        float accuracy;
        float precision;
        float recall;
        float f1Score;
    };
    
    RewardRuleStats getRuleStats(const std::string& ruleName) const;
    
private:
    std::map<std::string, std::function<float(float, const std::vector<float>&)>> rewardRules_;
    std::map<std::string, RewardRuleStats> ruleStats_;
    float explorationRate_;
};

/**
 * Enhanced developmental system
 */
class EnhancedDevelopmentalSystem {
public:
    EnhancedDevelopmentalSystem();
    ~EnhancedDevelopmentalSystem();
    
    /**
     * Set developmental stage with custom parameters
     */
    void setDevelopmentalStage(DevelopmentalStage stage, const std::map<std::string, float>& parameters);
    
    /**
     * Get current developmental parameters
     */
    std::map<std::string, float> getDevelopmentalParameters() const;
    
    /**
     * Update developmental state based on experience
     * 
     * @param experience Array of experiences [reward, novelty, prediction_error, ...]
     */
    void updateDevelopment(const std::vector<float>& experience);
    
    /**
     * Get developmental trajectory
     */
    std::vector<std::pair<DevelopmentalStage, double>> getDevelopmentalTrajectory() const;
    
    /**
     * Calculate developmental progress
     */
    float calculateDevelopmentalProgress() const;
    
private:
    std::map<DevelopmentalStage, std::map<std::string, float>> stageParameters_;
    std::vector<std::pair<DevelopmentalStage, double>> developmentalTrajectory_;
    double startTime_;
    float plasticityDecayRate_;
};

/**
 * Behavioral consistency analyzer
 */
class BehavioralConsistencyAnalyzer {
public:
    BehavioralConsistencyAnalyzer();
    ~BehavioralConsistencyAnalyzer();
    
    /**
     * Record a behavioral observation
     */
    void recordBehavior(const std::string& behavior, const std::vector<float>& context, float outcome);
    
    /**
     * Calculate behavioral consistency
     */
    float calculateConsistency(const std::string& behavior) const;
    
    /**
     * Get behavioral patterns
     */
    std::map<std::string, std::vector<std::vector<float>>> getBehavioralPatterns() const;
    
    /**
     * Detect behavioral drifts
     */
    std::vector<std::string> detectBehaviorDrifts() const;
    
    /**
     * Get consistency statistics
     */
    struct ConsistencyStats {
        float overallConsistency;
        float withinBehaviorConsistency;
        float acrossContextConsistency;
        float temporalConsistency;
    };
    
    ConsistencyStats getConsistencyStats() const;
    
private:
    std::map<std::string, std::vector<std::pair<std::vector<float>, float>>> behaviorRecordings_;
    std::map<std::string, std::vector<float>> behavioralContexts_;
    float consistencyThreshold_;
};

/**
 * Advanced learning analytics
 */
class AdvancedLearningAnalytics {
public:
    AdvancedLearningAnalytics();
    ~AdvancedLearningAnalytics();
    
    /**
     * Record learning event
     */
    void recordLearningEvent(const std::string& eventType, float value, const std::vector<float>& context);
    
    /**
     * Calculate learning efficiency
     */
    float calculateLearningEfficiency() const;
    
    /**
     * Get learning trends
     */
    std::vector<std::pair<double, float>> getLearningTrends() const;
    
    /**
     * Detect learning plateaus
     */
    std::vector<double> detectLearningPlateaus() const;
    
    /**
     * Get advanced analytics metrics
     */
    struct AdvancedAnalyticsMetrics {
        float learningRate;
        float retentionRate;
        float transferRate;
        float generalizationAbility;
        float explorationExploitationBalance;
    };
    
    AdvancedAnalyticsMetrics getAdvancedMetrics() const;
    
private:
    std::vector<std::pair<double, float>> learningEvents_;
    std::vector<std::pair<std::string, float>> learningMetrics_;
    float learningThreshold_;
};

/**
 * Comprehensive selective learning system
 * Combines all advanced selective learning features
 */
class SelectiveLearningSystem {
public:
    SelectiveLearningSystem();
    ~SelectiveLearningSystem();
    
    /**
     * Initialize the selective learning system
     */
    bool initialize();
    
    /**
     * Process an experience for selective learning
     */
    void processExperience(const std::vector<float>& sensoryInput, 
                          const std::string& action, 
                          float reward, 
                          const std::vector<float>& context);
    
    /**
     * Get the best action for a given context
     */
    std::string getBestAction(const std::vector<float>& context) const;
    
    /**
     * Get selective learning statistics
     */
    struct SelectiveLearningStats {
        float patternDiscriminationAccuracy;
        float responseSelectivity;
        float learningProgress;
        float behavioralConsistency;
        float attentionFocus;
    };
    
    SelectiveLearningStats getStats() const;
    
    /**
     * Enable/disable specific selective learning features
     */
    void enableFeature(const std::string& featureName, bool enabled);
    bool isFeatureEnabled(const std::string& featureName) const;
    
    /**
     * Get feature status
     */
    std::map<std::string, bool> getFeatureStatus() const;
    
    /**
     * Reset the selective learning system
     */
    void reset();
    
private:
    SelectiveAttention attentionSystem_;
    PatternBasedLearner patternLearner_;
    AdvancedRewardModulation rewardModulator_;
    EnhancedDevelopmentalSystem developmentalSystem_;
    BehavioralConsistencyAnalyzer consistencyAnalyzer_;
    AdvancedLearningAnalytics learningAnalytics_;
    
    std::map<std::string, bool> enabledFeatures_;
    
    // Helper functions
    std::vector<float> extractContextualFeatures(const std::vector<float>& sensoryInput,
                                                 const std::string& action,
                                                 float reward) const;
};

} // namespace nlm
