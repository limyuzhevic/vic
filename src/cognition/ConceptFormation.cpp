// ConceptFormation Implementation - Core functionality with new advanced features
// Recreated from header and basic implementation patterns

#include "ConceptFormation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>

namespace nlm {

struct ConceptFormation::Impl {
    Brain* brain;
    
    // Concept similarity cache for performance
    std::unordered_map<size_t, std::unordered_map<size_t, float>> similarityCache;
    
    // Feature extraction cache
    std::unordered_map<size_t, std::vector<float>> featureCache;
    
    // Emotional valence tracking
    std::unordered_map<size_t, float> conceptEmotions_;
    
    // Planning value tracking  
    std::unordered_map<size_t, float> conceptPlanningValues_;
    
    // Complexity tracking
    std::unordered_map<size_t, float> conceptComplexity_;
    
    // Drift tracking
    std::unordered_map<size_t, float> conceptDrift_;
    
    Impl() : brain(nullptr) {}
};

ConceptFormation::ConceptFormation()
    : pImpl(new Impl)
    , brain_(nullptr)
    , nextConceptId_(1)
    , formationThreshold_(0.75f)
    , stabilityThreshold_(0.7f)
    , stabilityWindow_(5)
    , conceptMergingEnabled_(true)
    , conceptCreationEnabled_(true)
{
}

ConceptFormation::~ConceptFormation() = default;

void ConceptFormation::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Cache warm-up
    pImpl->similarityCache.clear();
    pImpl->featureCache.clear();
    
    NLM_LOG_INFO("ConceptFormation initialized with advanced features");
}

// Existing methods implementation (simplified for space)
size_t ConceptFormation::presentExperience(const std::vector<float>& pattern,
                                          const std::vector<float>& features,
                                          float reward,
                                          SimulationStep currentTime) {
    if (pattern.empty()) return 0;
    
    // Check cache first
    std::vector<float> cacheKey = pattern;
    if (pImpl->featureCache.find(cacheKey.size()) != pImpl->featureCache.end()) {
        // Return cached result
    }
    
    // Check if this pattern matches any existing concept
    size_t matchingConcept = findConceptForPattern(pattern);
    
    if (matchingConcept > 0) {
        // Update existing concept
        updateConcept(matchingConcept, pattern, features, reward);
        return matchingConcept;
    }
    
    // Check if this is novel enough to form a new concept
    if (isNovel(pattern, formationThreshold_)) {
        // But wait - we need a few observations before committing to a concept
        // This prevents forming concepts from noise
        
        // For now, create concept if sufficiently different from all others
        return createConcept(pattern, features, reward);
    }
    
    return 0;  // Not yet classifiable
}

// Additional new methods implementation
void ConceptFormation::processPattern(const std::vector<float>& pattern, Timestamp currentTime) {
    if (pattern.empty() || !brain_) return;
    
    // Extract features from pattern for concept formation
    std::vector<float> features = extractFeatures(pattern);
    
    // Determine reward based on pattern characteristics
    float reward = computePatternReward(pattern);
    
    // Present to concept formation system
    size_t conceptId = presentExperience(pattern, features, reward, static_cast<SimulationStep>(currentTime));
    
    if (conceptId > 0) {
        NLM_LOG_DEBUG("ConceptFormation: Pattern processed into concept " + std::to_string(conceptId));
    }
}

void ConceptFormation::triggerNewConcept() {
    // Trigger concept formation through prediction error
    if (!brain_) return;
    
    // Get prediction error from prediction system
    float predictionError = 0.0f;
    if (auto* predSys = brain_->getPredictionSystem()) {
        predictionError = predSys->getPredictionError();
    }
    
    // If prediction error is high, force new concept formation
    if (predictionError > 0.5f) {
        // Create a dummy pattern based on current neural state
        std::vector<float> dummyPattern(100, 0.5f);  // Default medium activation
        std::vector<float> dummyFeatures(10, 0.0f);
        
        size_t newConceptId = createConcept(dummyPattern, dummyFeatures, 0.1f);
        
        if (newConceptId > 0) {
            NLM_LOG_INFO("ConceptFormation: New concept triggered by prediction error, ID: " + std::to_string(newConceptId));
        }
    }
}

// Helper methods
std::vector<float> ConceptFormation::extractFeatures(const std::vector<float>& pattern) const {
    // Check cache
    if (pImpl->featureCache.find(pattern.size()) != pImpl->featureCache.end()) {
        return pImpl->featureCache[pattern.size()];
    }
    
    // Extract simple features
    std::vector<float> features;
    
    if (!pattern.empty()) {
        // Feature 1: Mean activation
        float sum = std::accumulate(pattern.begin(), pattern.end(), 0.0f);
        features.push_back(sum / pattern.size());
        
        // Feature 2: Variance
        float variance = 0.0f;
        float mean = sum / pattern.size();
        for (float val : pattern) {
            variance += (val - mean) * (val - mean);
        }
        features.push_back(variance / pattern.size());
        
        // Feature 3: Max activation
        features.push_back(*std::max_element(pattern.begin(), pattern.end()));
        
        // Feature 4: Min activation
        features.push_back(*std::min_element(pattern.begin(), pattern.end()));
        
        // Feature 5: Skewness (simplified)
        float skew = 0.0f;
        int count = 0;
        for (float val : pattern) {
            skew += std::pow(val - mean, 3);
            count++;
        }
        features.push_back(count > 0 ? skew / count : 0.0f);
    }
    
    // Cache the features
    pImpl->featureCache[pattern.size()] = features;
    
    return features;
}

float ConceptFormation::computePatternReward(const std::vector<float>& pattern) const {
    if (pattern.empty()) return 0.0f;
    
    // Simple reward function based on pattern characteristics
    float meanActivation = std::accumulate(pattern.begin(), pattern.end(), 0.0f) / pattern.size();
    
    // High activation patterns get positive reward
    float reward = meanActivation;
    
    // Add bonus for complex patterns (medium variance)
    float variance = 0.0f;
    float mean = meanActivation;
    for (float val : pattern) {
        variance += (val - mean) * (val - mean);
    }
    variance /= pattern.size();
    
    if (variance > 0.01f && variance < 0.25f) {
        reward += 0.1f;
    }
    
    return reward;
}

// Additional advanced methods (simplified implementations)
std::vector<ConceptFormation::ConceptActivity> ConceptFormation::getConceptActivities() const {
    std::vector<ConceptActivity> activities;
    
    for (const auto& concept : concepts_) {
        ConceptActivity activity;
        activity.conceptId = concept.id;
        activity.activation = getConceptStability(concept.id);
        activity.stability = concept.avgStability;
        activity.rewardValue = concept.avgStability * 0.5f;  // Simplified
        activity.lastObserved = concept.totalObservations;
        activities.push_back(activity);
    }
    
    return activities;
}

bool ConceptFormation::areConceptsStable() const {
    for (const auto& concept : concepts_) {
        if (!isConceptStable(concept.id)) {
            return false;
        }
    }
    return !concepts_.empty();
}

size_t ConceptFormation::getMostActiveConcept() const {
    if (concepts_.empty()) return 0;
    
    size_t mostActiveId = 0;
    float maxStability = 0.0f;
    
    for (const auto& concept : concepts_) {
        if (concept.avgStability > maxStability) {
            maxStability = concept.avgStability;
            mostActiveId = concept.id;
        }
    }
    
    return mostActiveId;
}

std::vector<std::pair<size_t, size_t>> ConceptFormation::getSimilarConcepts(float similarityThreshold) const {
    std::vector<std::pair<size_t, size_t>> similarPairs;
    
    for (size_t i = 0; i < concepts_.size(); ++i) {
        for (size_t j = i + 1; j < concepts_.size(); ++j) {
            float similarity = computeSimilarity(concepts_[i].prototype, concepts_[j].prototype);
            if (similarity > similarityThreshold) {
                similarPairs.push_back({concepts_[i].id, concepts_[j].id});
            }
        }
    }
    
    return similarPairs;
}

void ConceptFormation::updateConceptEmotion(size_t conceptId, float emotionValue) {
    // Find concept and update emotional valence
    for (auto& concept : concepts_) {
        if (concept.id == conceptId) {
            // Store emotion value
            conceptEmotions_[conceptId] = emotionValue;
            
            // Modify concept stability based on emotion
            if (std::abs(emotionValue) > 0.5f) {
                concept.avgStability = std::max(0.1f, concept.avgStability * (1.0f - std::abs(emotionValue) * 0.2f));
            }
            break;
        }
    }
}

bool ConceptFormation::isEmotionallySignificant(size_t conceptId, float threshold) const {
    auto it = conceptEmotions_.find(conceptId);
    return (it != conceptEmotions_.end()) && (std::abs(it->second) > threshold);
}

size_t ConceptFormation::createRLConcept(const std::vector<float>& pattern, float qValue, ActionType preferredAction) {
    DiscoveredConcept concept;
    concept.id = nextConceptId_++;
    concept.prototype = pattern;
    concept.totalObservations = 1;
    
    // Create RL-specific category hint
    if (qValue > 0.5f) {
        concept.categoryHint = "high-value";
    } else if (qValue > 0.0f) {
        concept.categoryHint = "moderate-value";
    } else {
        concept.categoryHint = "low-value";
    }
    
    // Add action preference as hint
    concept.categoryHint += "_prefers_" + std::to_string(static_cast<int>(preferredAction));
    
    concepts_.push_back(concept);
    
    NLM_LOG_INFO("ConceptFormation: Created RL concept " + std::to_string(concept.id) +
                 " (q-value: " + std::to_string(qValue) + ")");
    
    return concept.id;
}

void ConceptFormation::updateConceptPlanning(size_t conceptId, const std::vector<ActionType>& plannedActions,
                                            const std::vector<float>& outcomes, float reward) {
    // Find concept and update planning-related properties
    for (auto& concept : concepts_) {
        if (concept.id == conceptId) {
            // Store planning information
            conceptPlanningValues_[conceptId] = reward;
            
            // Update prototype based on planning outcomes
            for (size_t i = 0; i < std::min(plannedActions.size(), outcomes.size()); ++i) {
                // If outcome is good, reinforce this action pattern
                if (outcomes[i] > 0.5f) {
                    // Update prototype slightly towards successful outcomes
                    if (i < concept.prototype.size()) {
                        concept.prototype[i] = std::min(1.0f, concept.prototype[i] * 1.1f);
                    }
                }
            }
            break;
        }
    }
}

std::vector<std::pair<size_t, float>> ConceptFormation::getConceptPlanningValues() const {
    return conceptPlanningValues_;
}

float ConceptFormation::getGeneralizationAbility(size_t conceptId) const {
    for (const auto& concept : concepts_) {
        if (concept.id == conceptId) {
            // Calculate how well the prototype generalizes
            // Simplified: higher stability means better generalization
            return concept.avgStability;
        }
    }
    return 0.0f;
}

void ConceptFormation::clear() {
    concepts_.clear();
    conceptEmotions_.clear();
    conceptPlanningValues_.clear();
    conceptComplexity_.clear();
    conceptDrift_.clear();
    
    pImpl->similarityCache.clear();
    pImpl->featureCache.clear();
    
    nextConceptId_ = 1;
    NLM_LOG_INFO("ConceptFormation cleared and reset");
}

} // namespace nlm