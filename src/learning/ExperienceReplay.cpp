#include "ExperienceReplay.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <random>
#include <numeric>
#include <cmath>
#include <unordered_set>

namespace nlm {

ExperienceReplay::ExperienceReplay(size_t capacity, float alpha, float beta, float epsilon)
    : capacity_(capacity), alpha_(alpha), beta_(beta), epsilon_(epsilon),
      episodicMemoryEnabled_(false), hindsightReplayEnabled_(false),
      dimReductionEnabled_(false), reductionRatio_(0.5f) {
    
    maxPriority_ = 1.0f;
    minPriority_ = 1.0f;
    
    NLM_LOG_INFO("ExperienceReplay initialized: capacity=" + std::to_string(capacity_) +
                " alpha=" + std::to_string(alpha_) + " beta=" + std::to_string(beta_));
}

ExperienceReplay::~ExperienceReplay() = default;

void ExperienceReplay::store(const LearningState& state, const Action& action,
                            const LearningState& nextState, float reward, bool done,
                            float priority) {
    // Create transition
    Transition transition(state, action, nextState, reward, 0.99f);
    
    // Store with priority
    experiences_.push_back(transition);
    priorities_.push_back(priority);
    
    // Update statistics
    stats_.totalTransitions++;
    stats_.sampledTransitions++;
    
    // Update priority statistics
    maxPriority_ = std::max(maxPriority_, priority);
    minPriority_ = std::min(minPriority_, priority);
    
    stats_.averagePriority = (stats_.averagePriority * (stats_.sampledTransitions - 1) +
                             priority) / stats_.sampledTransitions;
    stats_.maxPriority = maxPriority_;
    stats_.minPriority = minPriority_;
    
    // Maintain capacity
    if (experiences_.size() > capacity_) {
        // Remove oldest experiences
        experiences_.erase(experiences_.begin());
        priorities_.erase(priorities_.begin());
        
        // Update max/min priorities
        if (!priorities_.empty()) {
            maxPriority_ = *std::max_element(priorities_.begin(), priorities_.end());
            minPriority_ = *std::min_element(priorities_.begin(), priorities_.end());
        }
    }
}

std::vector<Transition> ExperienceReplay::sample(size_t batchSize, bool prioritize) {
    std::vector<Transition> batch;
    
    if (experiences_.empty() || batchSize == 0) {
        return batch;
    }
    
    size_t actualBatchSize = std::min(batchSize, experiences_.size());
    
    if (prioritize && !priorities_.empty() && beta_ > 0.0f) {
        // Prioritized sampling using weighted random choice
        std::vector<float> weights;
        weights.reserve(priorities_.size());
        
        for (float priority : priorities_) {
            float normalizedPriority = normalizePriority(priority);
            float weight = std::pow(normalizedPriority, beta_);
            weights.push_back(weight);
        }
        
        // Sample with replacement using weights
        std::discrete_distribution<size_t> dist(weights.begin(), weights.end());
        std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
        
        batch.reserve(actualBatchSize);
        for (size_t i = 0; i < actualBatchSize; ++i) {
            size_t index = dist(rng);
            batch.push_back(experiences_[index]);
        }
    } else {
        // Uniform random sampling
        std::vector<size_t> indices(experiences_.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::random_shuffle(indices.begin(), indices.end());
        
        batch.reserve(actualBatchSize);
        for (size_t i = 0; i < actualBatchSize; ++i) {
            batch.push_back(experiences_[indices[i]]);
        }
    }
    
    return batch;
}

void ExperienceReplay::clear() {
    experiences_.clear();
    priorities_.clear();
    
    maxPriority_ = 1.0f;
    minPriority_ = 1.0f;
    
    stats_.totalTransitions = 0;
    stats_.sampledTransitions = 0;
    stats_.averagePriority = 0.0f;
    stats_.maxPriority = 0.0f;
    stats_.minPriority = 1.0f;
    
    episodicMemory_.clear();
    
    NLM_LOG_INFO("ExperienceReplay cleared");
}

void ExperienceReplay::updatePriorities(const std::vector<size_t>& indices,
                                       const std::vector<float>& priorities) {
    if (indices.size() != priorities.size() || indices.size() > experiences_.size()) {
        NLM_LOG_ERROR("ExperienceReplay::updatePriorities: Invalid input sizes");
        return;
    }
    
    for (size_t i = 0; i < indices.size(); ++i) {
        size_t index = indices[i];
        if (index < priorities_.size()) {
            priorities_[index] = priorities[i];
        }
    }
    
    // Update priority statistics
    maxPriority_ = *std::max_element(priorities_.begin(), priorities_.end());
    minPriority_ = *std::min_element(priorities_.begin(), priorities_.end());
    
    NLM_LOG_INFO("ExperienceReplay priorities updated");
}

void ExperienceReplay::storeEpisode(const std::vector<Transition>& episode) {
    if (episodicMemoryEnabled_) {
        episodicMemory_.push_back(episode);
        
        // Maintain episodic memory size
        if (episodicMemory_.size() > 1000) {
            episodicMemory_.erase(episodicMemory_.begin());
        }
    }
}

std::vector<Transition> ExperienceReplay::sampleFromEpisodicMemory(size_t maxEpisodes) {
    std::vector<Transition> sampled;
    
    if (!episodicMemoryEnabled_ || episodicMemory_.empty()) {
        return sampled;
    }
    
    size_t episodeCount = std::min(maxEpisodes, episodicMemory_.size());
    
    // Randomly select episodes
    std::vector<size_t> indices(episodicMemory_.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::random_shuffle(indices.begin(), indices.end());
    
    for (size_t i = 0; i < episodeCount; ++i) {
        size_t episodeIndex = indices[i];
        if (episodeIndex < episodicMemory_.size()) {
            const auto& episode = episodicMemory_[episodeIndex];
            sampled.insert(sampled.end(), episode.begin(), episode.end());
        }
    }
    
    return sampled;
}

// CuriosityDrivenLearning implementation
CuriosityDrivenLearning::CuriosityDrivenLearning(size_t stateDim, size_t actionDim,
                                               float curiosityWeight, float noveltyThreshold)
    : stateDim_(stateDim), actionDim_(actionDim),
      curiosityWeight_(curiosityWeight), noveltyThreshold_(noveltyThreshold),
      predictionCuriosityEnabled_(true), goalCuriosityEnabled_(true),
      noveltyDetectionEnabled_(true) {
    
    NLM_LOG_INFO("CuriosityDrivenLearning initialized: state_dim=" + std::to_string(stateDim_) +
                " action_dim=" + std::to_string(actionDim_));
}

CuriosityDrivenLearning::~CuriosityDrivenLearning() = default;

void CuriosityDrivenLearning::initialize(std::unique_ptr<LearningNetwork> encoder,
                                        std::unique_ptr<LearningNetwork> dynamicsPredictor,
                                        std::unique_ptr<LearningNetwork> intrinsicRewardEstimator) {
    encoder_ = std::move(encoder);
    dynamicsPredictor_ = std::move(dynamicsPredictor);
    intrinsicRewardEstimator_ = std::move(intrinsicRewardEstimator);
    
    // Initialize source weights
    sourceWeights_["prediction_error"] = 1.0f;
    sourceWeights_["novelty"] = 0.5f;
    sourceWeights_["goal_driven"] = 0.3f;
    
    NLM_LOG_INFO("CuriosityDrivenLearning networks initialized");
}

float CuriosityDrivenLearning::computeIntrinsicReward(const std::vector<float>& state,
                                                     const std::vector<float>& nextState,
                                                     const std::vector<float>& action) {
    float intrinsicReward = 0.0f;
    
    if (predictionCuriosityEnabled_) {
        float predError = computePredictionError(state, nextState);
        intrinsicReward += sourceWeights_["prediction_error"] * predError;
    }
    
    if (noveltyDetectionEnabled_) {
        float novelty = computeNovelty(nextState);
        intrinsicReward += sourceWeights_["novelty"] * novelty;
    }
    
    if (goalCuriosityEnabled_) {
        // Simplified goal-based curiosity
        float goalCuriosity = 1.0f - noveltyThreshold_;
        intrinsicReward += sourceWeights_["goal_driven"] * goalCuriosity;
    }
    
    stats_.totalIntrinsicReward += intrinsicReward;
    stats_.updateCount++;
    stats_.averageIntrinsicReward = stats_.totalIntrinsicReward / stats_.updateCount;
    
    return curiosityWeight_ * intrinsicReward;
}

float CuriosityDrivenLearning::getCuriosityLevel(const std::vector<float>& state) const {
    float curiosity = 0.0f;
    
    if (predictionCuriosityEnabled_) {
        curiosity += sourceWeights_["prediction_error"] * std::abs(predictionError_);
    }
    
    if (noveltyDetectionEnabled_) {
        curiosity += sourceWeights_["novelty"] * noveltyScore_;
    }
    
    return std::min(curiosity, 1.0f);
}

void CuriosityDrivenLearning::update(const std::vector<float>& state,
                                    const std::vector<float>& nextState,
                                    const std::vector<float>& action,
                                    float extrinsicReward) {
    // Update curiosity model
    predictionError_ = computePredictionError(state, nextState);
    noveltyScore_ = computeNovelty(nextState);
    
    // Update statistics
    stats_.predictionError = predictionError_;
    stats_.noveltyScore = noveltyScore_;
}

std::vector<float> CuriosityDrivenLearning::getStateRepresentation(const std::vector<float>& state) {
    if (encoder_) {
        return encoder_->forward(state);
    }
    return std::vector<float>();
}

float CuriosityDrivenLearning::getPredictionError(const std::vector<float>& state,
                                                 const std::vector<float>& nextState) const {
    return computePredictionError(state, nextState);
}

void CuriosityDrivenLearning::setSourceWeight(const std::string& source, float weight) {
    sourceWeights_[source] = weight;
    NLM_LOG_INFO("Curiosity source " + source + " weight set to " + std::to_string(weight));
}

float CuriosityDrivenLearning::computePredictionError(const std::vector<float>& state,
                                                     const std::vector<float>& nextState) {
    if (!dynamicsPredictor_) {
        return 0.0f;
    }
    
    std::vector<float> stateRepr = getStateRepresentation(state);
    std::vector<float> predictedNext = dynamicsPredictor_->forward(stateRepr);
    
    // Compute prediction error
    float error = 0.0f;
    size_t minSize = std::min(predictedNext.size(), nextState.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        error += std::abs(predictedNext[i] - nextState[i]);
    }
    
    return error / minSize;
}

float CuriosityDrivenLearning::computeNovelty(const std::vector<float>& state) {
    if (!encoder_) {
        return 0.0f;
    }
    
    std::vector<float> stateRepr = encoder_->forward(state);
    
    // Simplified novelty based on representation variance
    float mean = std::accumulate(stateRepr.begin(), stateRepr.end(), 0.0f) / stateRepr.size();
    float variance = 0.0f;
    
    for (float val : stateRepr) {
        variance += std::pow(val - mean, 2);
    }
    variance /= stateRepr.size();
    
    return std::sqrt(variance);
}

float CuriosityDrivenLearning::computeGoalBasedCuriosity(const std::vector<float>& state,
                                                        const std::vector<float>& goal) {
    // Simplified goal-based curiosity
    float distance = 0.0f;
    size_t minSize = std::min(state.size(), goal.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        distance += std::abs(state[i] - goal[i]);
    }
    
    return 1.0f - std::min(1.0f, distance / minSize);
}

// TransferLearning implementation
TransferLearning::TransferLearning(size_t featureDim, float transferWeight,
                                   float adaptationRate)
    : featureDim_(featureDim), transferWeight_(transferWeight),
      adaptationRate_(adaptationRate),
      featureExtractionEnabled_(true), knowledgeDistillationEnabled_(true),
      metaLearningEnabled_(false) {
    
    NLM_LOG_INFO("TransferLearning initialized: feature_dim=" + std::to_string(featureDim_));
}

TransferLearning::~TransferLearning() = default;

void TransferLearning::storeSourceDomain(const std::string& domainId,
                                        const std::vector<std::vector<float>>& features,
                                        const std::vector<float>& labels,
                                        const std::string& taskType) {
    DomainKnowledge knowledge;
    knowledge.domainId = domainId;
    knowledge.features = features;
    knowledge.labels = labels;
    knowledge.taskType = taskType;
    knowledge.accuracy = 0.0f; // Would be computed during training
    knowledge.sampleCount = features.size();
    
    sourceDomains_.push_back(knowledge);
    stats_.sourceDomains++;
    
    NLM_LOG_INFO("Stored source domain " + domainId + " with " + std::to_string(features.size()) + " samples");
}

std::vector<float> TransferLearning::adaptToTarget(const std::string& targetDomain,
                                                   const std::vector<std::vector<float>>& targetFeatures,
                                                   const std::vector<float>& targetLabels) {
    std::vector<float> adaptedParams(featureDim_, 0.0f);
    
    // Find source domains for transfer
    for (const auto& source : sourceDomains_) {
        float similarity = computeFeatureSimilarity(targetFeatures[0], source.features[0]);
        
        if (similarity > 0.5f) {  // Threshold for transferability
            float transferLoss = domainAdaptationLoss(source.features, targetFeatures);
            
            // Adapt parameters based on transfer loss
            for (size_t i = 0; i < featureDim_ && i < source.features[0].size(); ++i) {
                adaptedParams[i] = adaptationRate_ * (1.0f - transferLoss);
            }
        }
    }
    
    return adaptedParams;
}

float TransferLearning::transferKnowledge(const std::string& sourceDomain,
                                         const std::string& targetDomain,
                                         const std::string& taskType) {
    // Find source and target domains
    DomainKnowledge* source = nullptr;
    DomainKnowledge* target = nullptr;
    
    for (auto& domain : sourceDomains_) {
        if (domain.domainId == sourceDomain) {
            source = &domain;
        }
        if (domain.domainId == targetDomain) {
            target = &domain;
        }
    }
    
    if (!source || !target) {
        return 0.0f;
    }
    
    // Compute transfer success
    float transferSuccess = 0.0f;
    
    if (featureExtractionEnabled_ && knowledgeDistillationEnabled_) {
        // Simplified transfer success based on feature similarity
        float featureSimilarity = computeFeatureSimilarity(source->features[0], target->features[0]);
        transferSuccess = source->accuracy * featureSimilarity * transferWeight_;
    }
    
    // Update statistics
    stats_.transferOperations++;
    stats_.transferAccuracy = (stats_.transferAccuracy * (stats_.transferOperations - 1) +
                              transferSuccess) / stats_.transferOperations;
    
    NLM_LOG_INFO("Transfer from " + sourceDomain + " to " + targetDomain +
                ": success=" + std::to_string(transferSuccess));
    
    return transferSuccess;
}

std::vector<float> TransferLearning::getTransferredFeatures(const std::vector<float>& sourceFeatures,
                                                            const std::string& targetDomain) {
    std::vector<float> transferredFeatures(sourceFeatures.size(), 0.0f);
    
    for (const auto& domain : sourceDomains_) {
        if (domain.domainId == targetDomain) {
            // Simple feature transfer (in practice would use sophisticated methods)
            float similarity = computeFeatureSimilarity(sourceFeatures, domain.features[0]);
            
            for (size_t i = 0; i < transferredFeatures.size() && i < domain.features[0].size(); ++i) {
                transferredFeatures[i] = sourceFeatures[i] * similarity +
                                       domain.features[0][i] * (1.0f - similarity);
            }
            
            break;
        }
    }
    
    return transferredFeatures;
}

float TransferLearning::computeFeatureSimilarity(const std::vector<float>& features1,
                                                const std::vector<float>& features2) {
    if (features1.size() != features2.size() || features1.empty()) {
        return 0.0f;
    }
    
    float dotProduct = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;
    
    for (size_t i = 0; i < features1.size(); ++i) {
        dotProduct += features1[i] * features2[i];
        norm1 += features1[i] * features1[i];
        norm2 += features2[i] * features2[i];
    }
    
    norm1 = std::sqrt(norm1);
    norm2 = std::sqrt(norm2);
    
    if (norm1 * norm2 == 0.0f) {
        return 0.0f;
    }
    
    return dotProduct / (norm1 * norm2);
}

float TransferLearning::domainAdaptationLoss(const std::vector<std::vector<float>>& sourceFeatures,
                                            const std::vector<std::vector<float>>& targetFeatures) {
    if (sourceFeatures.empty() || targetFeatures.empty()) {
        return 1.0f;
    }
    
    float mmd = 0.0f;
    size_t sourceSamples = sourceFeatures.size();
    size_t targetSamples = targetFeatures.size();
    
    // Simplified MMD computation
    for (size_t i = 0; i < sourceSamples && i < 10; ++i) {
        for (size_t j = 0; j < targetSamples && j < 10; ++j) {
            float sim = computeFeatureSimilarity(sourceFeatures[i], targetFeatures[j]);
            mmd += (1.0f - sim);
        }
    }
    
    mmd /= std::min<size_t>(sourceSamples * targetSamples, 100);
    
    return std::min(1.0f, mmd);
}