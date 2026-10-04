#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <unordered_set>
#include <functional>

#include "AdvancedAgentLearning.hpp"
#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Experience replay with prioritized sampling
class ExperienceReplay {
public:
    ExperienceReplay(size_t capacity = 100000, float alpha = 0.6f, float beta = 0.4f,
                    float epsilon = 1e-6f);
    ~ExperienceReplay();
    
    // Store transition
    void store(const LearningState& state, const Action& action,
               const LearningState& nextState, float reward, bool done,
               float priority = 1.0f);
    
    // Sample batch
    std::vector<Transition> sample(size_t batchSize, bool prioritize = true);
    
    // Get current size
    size_t size() const { return experiences_.size(); }
    
    // Get capacity
    size_t capacity() const { return capacity_; }
    
    // Clear all experiences
    void clear();
    
    // Update priorities (for TD error)
    void updatePriorities(const std::vector<size_t>& indices,
                         const std::vector<float>& priorities);
    
    // Enable/disable episodic memory
    void enableEpisodicMemory(bool enabled) { episodicMemoryEnabled_ = enabled; }
    void enableHindsightReplay(bool enabled) { hindsightReplayEnabled_ = enabled; }
    
    // Store episode for episodic memory
    void storeEpisode(const std::vector<Transition>& episode);
    
    // Get experiences from episodic memory
    std::vector<Transition> sampleFromEpisodicMemory(size_t maxEpisodes = 5);
    
    // Dimensionality reduction for storage
    void enableDimensionalityReduction(bool enabled) { dimReductionEnabled_ = enabled; }
    void setReductionRatio(float ratio) { reductionRatio_ = ratio; }
    
    // Get statistics
    struct Statistics {
        size_t totalTransitions;
        size_t sampledTransitions;
        float averagePriority;
        float maxPriority;
        float minPriority;
        
        Statistics() : totalTransitions(0), sampledTransitions(0),
                      averagePriority(0.0f), maxPriority(0.0f),
                      minPriority(1.0f) {}
    };
    
    Statistics getStatistics() const { return stats_; }
    
private:
    size_t capacity_;
    float alpha_;
    float beta_;
    float epsilon_;
    
    // Main experience buffer
    std::vector<Transition> experiences_;
    
    // Priority weights
    std::vector<float> priorities_;
    
    // Episodic memory
    std::vector<std::vector<Transition>> episodicMemory_;
    bool episodicMemoryEnabled_;
    
    // Hindsight replay enabled
    bool hindsightReplayEnabled_;
    
    // Dimensionality reduction
    bool dimReductionEnabled_;
    float reductionRatio_;
    
    // Statistics
    Statistics stats_;
    
    // Helper functions
    float computePriority(float tdError) const {
        return std::pow(std::abs(tdError) + epsilon_, alpha_);
    }
    
    float normalizePriority(float priority) const {
        if (maxPriority_ - minPriority_ > 0.0f) {
            return (priority - minPriority_) / (maxPriority_ - minPriority_);
        }
        return 0.5f;
    }
    
    float maxPriority_;
    float minPriority_;
};

// Advanced curiosity mechanisms
class CuriosityDrivenLearning {
public:
    CuriosityDrivenLearning(size_t stateDim, size_t actionDim,
                           float curiosityWeight = 1.0f, float noveltyThreshold = 0.5f);
    ~CuriosityDrivenLearning();
    
    // Initialize with networks
    void initialize(std::unique_ptr<LearningNetwork> encoder,
                    std::unique_ptr<LearningNetwork> dynamicsPredictor,
                    std::unique_ptr<LearningNetwork> intrinsicRewardEstimator);
    
    // Compute intrinsic motivation
    float computeIntrinsicReward(const std::vector<float>& state,
                                const std::vector<float>& nextState,
                                const std::vector<float>& action);
    
    // Get curiosity level
    float getCuriosityLevel(const std::vector<float>& state) const;
    
    // Update curiosity model
    void update(const std::vector<float>& state,
                const std::vector<float>& nextState,
                const std::vector<float>& action,
                float extrinsicReward);
    
    // Get representation of state
    std::vector<float> getStateRepresentation(const std::vector<float>& state);
    
    // Get prediction error (novelty)
    float getPredictionError(const std::vector<float>& state,
                            const std::vector<float>& nextState) const;
    
    // Set curiosity sources weights
    void setSourceWeight(const std::string& source, float weight);
    
    // Enable/disable curiosity components
    void enablePredictionBasedCuriosity(bool enabled) { predictionCuriosityEnabled_ = enabled; }
    void enableGoalBasedCuriosity(bool enabled) { goalCuriosityEnabled_ = enabled; }
    void enableNoveltyDetection(bool enabled) { noveltyDetectionEnabled_ = enabled; }
    
    // Get curiosity source types
    enum class CuriositySource {
        PREDICTION_ERROR,
        NOVELTY,
        GOAL_DRIVEN,
        EXPLORATION_DRIVE,
        INTRINSIC_MOTIVATION
    };
    
    // Get statistics
    struct Statistics {
        float totalIntrinsicReward;
        float averageIntrinsicReward;
        float predictionError;
        float noveltyScore;
        size_t updateCount;
        
        Statistics() : totalIntrinsicReward(0.0f), averageIntrinsicReward(0.0f),
                      predictionError(0.0f), noveltyScore(0.0f), updateCount(0) {}
    };
    
    Statistics getStatistics() const { return stats_; }
    
private:
    size_t stateDim_;
    size_t actionDim_;
    float curiosityWeight_;
    float noveltyThreshold_;
    
    // Curiosity components
    std::unique_ptr<LearningNetwork> encoder_;
    std::unique_ptr<LearningNetwork> dynamicsPredictor_;
    std::unique_ptr<LearningNetwork> intrinsicRewardEstimator_;
    
    // State representations
    std::vector<float> currentRepresentation_;
    std::vector<float> nextRepresentation_;
    
    // Prediction errors
    float predictionError_;
    float noveltyScore_;
    
    // Weights for different curiosity sources
    std::unordered_map<std::string, float> sourceWeights_;
    
    // Enable flags
    bool predictionCuriosityEnabled_;
    bool goalCuriosityEnabled_;
    bool noveltyDetectionEnabled_;
    
    // Statistics
    Statistics stats_;
    
    // Compute prediction error
    float computePredictionError(const std::vector<float>& state,
                                const std::vector<float>& nextState);
    
    // Compute novelty using density estimation
    float computeNovelty(const std::vector<float>& state);
    
    // Compute goal-based curiosity
    float computeGoalBasedCuriosity(const std::vector<float>& state,
                                   const std::vector<float>& goal);
};

// Transfer learning and knowledge sharing
class TransferLearning {
public:
    TransferLearning(size_t featureDim = 256, float transferWeight = 0.5f,
                     float adaptationRate = 0.1f);
    ~TransferLearning();
    
    // Store source domain knowledge
    void storeSourceDomain(const std::string& domainId,
                          const std::vector<std::vector<float>>& features,
                          const std::vector<float>& labels,
                          const std::string& taskType);
    
    // Adapt to target domain
    std::vector<float> adaptToTarget(const std::string& targetDomain,
                                     const std::vector<std::vector<float>>& targetFeatures,
                                     const std::vector<float>& targetLabels);
    
    // Transfer knowledge from source to target
    float transferKnowledge(const std::string& sourceDomain,
                           const std::string& targetDomain,
                           const std::string& taskType);
    
    // Get transferred features
    std::vector<float> getTransferredFeatures(const std::vector<float>& sourceFeatures,
                                              const std::string& targetDomain);
    
    // Enable transfer mechanisms
    void enableFeatureExtraction(bool enabled) { featureExtractionEnabled_ = enabled; }
    void enableKnowledge distillation(bool enabled) { knowledgeDistillationEnabled_ = enabled; }
    void enableMeta_learning(bool enabled) { metaLearningEnabled_ = enabled; }
    
    // Get statistics
    struct Statistics {
        float transferAccuracy;
        float adaptationLoss;
        size_t sourceDomains;
        size_t targetDomains;
        size_t transferOperations;
        
        Statistics() : transferAccuracy(0.0f), adaptationLoss(0.0f),
                      sourceDomains(0), targetDomains(0),
                      transferOperations(0) {}
    };
    
    Statistics getStatistics() const { return stats_; }
    
private:
    size_t featureDim_;
    float transferWeight_;
    float adaptationRate_;
    
    // Source domain knowledge
    struct DomainKnowledge {
        std::string domainId;
        std::vector<std::vector<float>> features;
        std::vector<float> labels;
        std::string taskType;
        float accuracy;
        size_t sampleCount;
    };
    
    std::vector<DomainKnowledge> sourceDomains_;
    
    // Target domain adaptation
    std::unordered_map<std::string, std::vector<float>> adaptedParameters_;
    
    // Feature extractor
    std::unique_ptr<LearningNetwork> featureExtractor_;
    
    // Enable flags
    bool featureExtractionEnabled_;
    bool knowledgeDistillationEnabled_;
    bool metaLearningEnabled_;
    
    // Statistics
    Statistics stats_;
    
    // Compute feature similarity
    float computeFeatureSimilarity(const std::vector<float>& features1,
                                  const std::vector<float>& features2);
    
    // Domain adaptation using MMD (Maximum Mean Discrepancy)
    float domainAdaptationLoss(const std::vector<std::vector<float>>& sourceFeatures,
                              const std::vector<std::vector<float>>& targetFeatures);
};