#include "ConceptFormation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct ConceptFormation::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

ConceptFormation::ConceptFormation()
    : pImpl(new Impl)
    , brain_(nullptr)
    , nextConceptId_(1)
    , formationThreshold_(0.75f)
    , stabilityThreshold_(0.7f)
    , stabilityWindow_(5)
{
}

ConceptFormation::~ConceptFormation() = default;

void ConceptFormation::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("ConceptFormation initialized");
}

void ConceptFormation::presentExperience(const std::vector<float>& pattern,
                                           const std::vector<float>& features,
                                           float reward,
                                           SimulationStep currentTime) {
    if (pattern.empty()) return;
    
    // Extract advanced features from the pattern
    std::vector<float> advancedFeatures = extractAdvancedFeatures(pattern);
    
    // Check if this pattern matches any existing concept using advanced similarity
    size_t matchingConcept = findConceptForPattern(advancedFeatures);
    
    if (matchingConcept > 0) {
        // Update existing concept with advanced learning
        updateConcept(matchingConcept, advancedFeatures, features, reward, currentTime);
        
        // Apply predictive coding: what should the concept expect?
        std::vector<float> predictedPattern = predictConceptOutcome(matchingConcept);
        
        // Calculate prediction error for learning
        float predictionError = computePredictionError(pattern, predictedPattern);
        
        // Use prediction error to refine concept
        applyPredictionErrorLearning(matchingConcept, predictionError, reward);
        
        return matchingConcept;
    }
    
    // Advanced novelty detection with multiple criteria
    if (isNovel(advancedFeatures, formationThreshold_, advancedFeatures)) {
        // But wait - we need multiple observations before committing to a concept
        // This prevents forming concepts from noise and ensures selectivity
        
        // Check concept stability from similar patterns
        float similarityToNearest = findSimilarityToNearestConcept(advancedFeatures);
        
        // Form new concept only if sufficiently different and with evidence
        if (similarityToNearest < formationThreshold_ * 0.8f) {
            // Accumulate evidence for concept formation
            return createConceptWithEvidence(advancedFeatures, features, reward, currentTime, similarityToNearest);
        }
    }
    
    return 0;  // Not yet classifiable
}

size_t ConceptFormation::createConcept(const std::vector<float>& pattern,
                                       const std::vector<float>& features,
                                       float reward) {
    DiscoveredConcept conceptObj;
    conceptObj.id = nextConceptId_++;
    conceptObj.prototype = pattern;
    conceptObj.totalObservations = 1;
    
    ConceptInstance instance;
    instance.pattern = pattern;
    instance.features = features;
    instance.observationCount = 1;
    instance.lastObserved = 0;
    instance.avgReward = reward;
    instance.stability = 1.0f;
    
    conceptObj.instances.push_back(instance);
    conceptObj.avgStability = 1.0f;
    
    // Determine category hint based on properties
    if (reward > 0.5f) {
        conceptObj.categoryHint = "rewarding";
    } else if (reward < -0.3f) {
        conceptObj.categoryHint = "harmful";
    } else {
        conceptObj.categoryHint = "neutral";
    }
    
    concepts_.push_back(conceptObj);
    
    NLM_LOG_INFO("ConceptFormation: Created concept " + std::to_string(conceptObj.id) +
                 " (category: " + conceptObj.categoryHint + ")");
    
    return conceptObj.id;
}

void ConceptFormation::updateConcept(size_t conceptId, const std::vector<float>& newPattern,
                                    const std::vector<float>& features, float reward) {
    DiscoveredConcept* conceptObj = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            conceptObj = &c;
            break;
        }
    }
    
    if (!conceptObj) return;
    
    // Add new instance
    ConceptInstance instance;
    instance.pattern = newPattern;
    instance.features = features;
    instance.observationCount = 1;
    instance.lastObserved = conceptObj->totalObservations;
    instance.avgReward = reward;
    
    conceptObj->instances.push_back(instance);
    conceptObj->totalObservations++;
    
    // Update prototype using exponential moving average (Hebbian-like)
    updatePrototype(conceptId, newPattern);
    
    // Update stability
    float newStability = 1.0f;
    if (conceptObj->instances.size() > 1) {
        // Compute stability as average similarity to prototype
        float totalSim = 0.0f;
        size_t recentCount = std::min(conceptObj->instances.size(), stabilityWindow_);
        
        for (size_t i = conceptObj->instances.size() - recentCount; 
             i < conceptObj->instances.size(); ++i) {
            totalSim += computeSimilarity(conceptObj->instances[i].pattern, conceptObj->prototype);
        }
        
        newStability = totalSim / recentCount;
        conceptObj->avgStability = conceptObj->avgStability * 0.9f + newStability * 0.1f;
    }
    
    // Update instance stability
    conceptObj->instances.back().stability = newStability;
}

void ConceptFormation::updatePrototype(size_t conceptId, const std::vector<float>& newInstance) {
    for (auto& conceptObj : concepts_) {
        if (conceptObj.id == conceptId) {
            // Hebbian update: weight prototype towards new instance
            float alpha = 0.2f;  // Learning rate
            
            if (conceptObj.prototype.size() == newInstance.size()) {
                for (size_t i = 0; i < conceptObj.prototype.size(); ++i) {
                    conceptObj.prototype[i] = conceptObj.prototype[i] * (1.0f - alpha) + 
                                          newInstance[i] * alpha;
                }
            }
            break;
        }
    }
}

size_t ConceptFormation::getMatchingConcept(const std::vector<float>& pattern,
                                           float similarityThreshold) const {
    return findConceptForPattern(pattern);
}

const DiscoveredConcept* ConceptFormation::getConcept(size_t conceptId) const {
    for (const auto& conceptObj : concepts_) {
        if (conceptObj.id == conceptId) {
            return &conceptObj;
        }
    }
    return nullptr;
}

std::vector<float> ConceptFormation::getConceptPrototype(size_t conceptId) const {
    const DiscoveredConcept* conceptObj = getConcept(conceptId);
    if (conceptObj) {
        return conceptObj->prototype;
    }
    return {};
}

const std::vector<ConceptInstance>& ConceptFormation::getConceptInstances(
    size_t conceptId) const {
    static std::vector<ConceptInstance> empty;
    const DiscoveredConcept* conceptObj = getConcept(conceptId);
    if (conceptObj) {
        return conceptObj->instances;
    }
    return empty;
}

float ConceptFormation::getConceptStability(size_t conceptId) const {
    const DiscoveredConcept* conceptObj = getConcept(conceptId);
    if (conceptObj) {
        return conceptObj->avgStability;
    }
    return 0.0f;
}

void ConceptFormation::mergeConcepts(size_t conceptA, size_t conceptB) {
    DiscoveredConcept* a = nullptr;
    DiscoveredConcept* b = nullptr;
    
    for (auto& c : concepts_) {
        if (c.id == conceptA) a = &c;
        if (c.id == conceptB) b = &c;
    }
    
    if (!a || !b) return;
    
    // Merge B into A
    for (const auto& instance : b->instances) {
        a->instances.push_back(instance);
        a->totalObservations++;
    }
    
    // Update prototype as average of both
    for (size_t i = 0; i < a->prototype.size() && i < b->prototype.size(); ++i) {
        a->prototype[i] = (a->prototype[i] + b->prototype[i]) / 2.0f;
    }
    
    // Remove B
    concepts_.erase(
        std::remove_if(concepts_.begin(), concepts_.end(),
            [conceptB](const DiscoveredConcept& c) { return c.id == conceptB; }),
        concepts_.end()
    );
    
    NLM_LOG_INFO("ConceptFormation: Merged concepts " + std::to_string(conceptA) +
                 " and " + std::to_string(conceptB));
}

bool ConceptFormation::isNovel(const std::vector<float>& pattern,
                               float similarityThreshold) const {
    return findConceptForPattern(pattern) == 0;
}

size_t ConceptFormation::findConceptForPattern(const std::vector<float>& pattern) const {
    if (concepts_.empty()) return 0;
    
    float bestSimilarity = 0.0f;
    size_t bestConcept = 0;
    
    for (const auto& conceptObj : concepts_) {
        float sim = computeSimilarity(pattern, conceptObj.prototype);
        if (sim > bestSimilarity && sim >= formationThreshold_) {
            bestSimilarity = sim;
            bestConcept = conceptObj.id;
        }
    }
    
    return bestConcept;
}

float ConceptFormation::computeSimilarity(const std::vector<float>& a,
                                         const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    // Cosine similarity
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;

    // Phase 6: cosine alone cannot tell a bright scene from a dark
    // one — closeness patterns for a resource (centre ~0.3) and a
    // hazard (centre ~0.75) point the same way and score cosine
    // 1.0, so every scene collapsed into one concept. Multiply by a
    // magnitude factor: patterns of similar strength stay similar,
    // patterns whose strength differs a lot do not. Same object at
    // a somewhat different distance (norms 0.74 vs 1.2) still scores
    // ~0.76 and matches; resource vs hazard (0.74 vs 1.84) scores
    // ~0.57 and forms a separate concept. Direction and magnitude
    // are both properties of the experience, not labels.
    float cosine = dot / (std::sqrt(normA) * std::sqrt(normB));
    float magA = std::sqrt(normA), magB = std::sqrt(normB);
    float magFactor = 1.0f - std::abs(magA - magB) / (magA + magB + 0.0001f);
    return cosine * magFactor;
}

bool ConceptFormation::isConceptStable(size_t conceptId) const {
    const DiscoveredConcept* conceptObj = getConcept(conceptId);
    if (!conceptObj) return false;
    
    return conceptObj->avgStability >= stabilityThreshold_;
}

float ConceptFormation::getGeneralizationAbility(size_t conceptId) const {
    const DiscoveredConcept* conceptObj = getConcept(conceptId);
    if (!conceptObj) return 0.0f;
    
    // Generalization ability = how varied are the instances
    if (conceptObj->instances.size() < 2) return 0.0f;
    
    float totalVariance = 0.0f;
    for (const auto& instance : conceptObj->instances) {
        float sim = computeSimilarity(instance.pattern, conceptObj->prototype);
        totalVariance += (1.0f - sim);
    }
    
    return 1.0f - (totalVariance / conceptObj->instances.size());
}

// Extract advanced features from pattern for better concept formation
std::vector<float> ConceptFormation::extractAdvancedFeatures(const std::vector<float>& pattern) const {
    std::vector<float> features;
    
    if (pattern.empty()) return features;
    
    // Statistical features
    float mean = 0.0f, variance = 0.0f;
    for (float val : pattern) {
        mean += val;
    }
    mean /= static_cast<float>(pattern.size());
    
    for (float val : pattern) {
        float diff = val - mean;
        variance += diff * diff;
    }
    variance /= static_cast<float>(pattern.size());
    float stdDev = std::sqrt(variance);
    
    // Normalize and add features
    if (stdDev > 0.0001f) {
        for (float val : pattern) {
            features.push_back((val - mean) / stdDev);  // Z-normalized
        }
    }
    
    // Add pattern-specific features
    float minVal = pattern[0], maxVal = pattern[0];
    for (float val : pattern) {
        minVal = std::min(minVal, val);
        maxVal = std::max(maxVal, val);
    }
    
    // Add range and distribution characteristics
    features.push_back(maxVal - minVal);  // Range
    features.push_back(minVal);           // Minimum
    features.push_back(maxVal);           // Maximum
    features.push_back(mean);             // Mean
    
    // Add higher-order statistics
    float skew = 0.0f, kurtosis = 0.0f;
    for (float val : pattern) {
        float z = (val - mean) / (stdDev + 0.0001f);
        skew += z * z * z;
        kurtosis += z * z * z * z;
    }
    skew /= static_cast<float>(pattern.size());
    kurtosis /= static_cast<float>(pattern.size());
    features.push_back(skew);
    features.push_back(kurtosis);
    
    return features;
}

// Find concept for pattern using advanced similarity metrics
size_t ConceptFormation::findConceptForPattern(const std::vector<float>& features) const {
    if (features.empty()) return 0;
    
    // Simple version: find closest concept using Euclidean distance
    float bestSimilarity = 0.0f;
    size_t bestConcept = 0;
    
    for (const auto& conceptObj : concepts_) {
        // Use advanced similarity calculation
        float similarity = computeAdvancedSimilarity(features, conceptObj.prototype);
        
        if (similarity > bestSimilarity && similarity >= formationThreshold_) {
            bestSimilarity = similarity;
            bestConcept = conceptObj.id;
        }
    }
    
    return bestConcept;
}

// Advanced similarity calculation using multiple metrics
float ConceptFormation::computeAdvancedSimilarity(const std::vector<float>& pattern,
                                               const std::vector<float>& prototype) const {
    if (pattern.size() != prototype.size() || pattern.empty()) return 0.0f;
    
    // Multiple similarity metrics for robust concept matching
    
    // 1. Cosine similarity
    float cosine = 0.0f, normP = 0.0f, normPr = 0.0f;
    for (size_t i = 0; i < pattern.size(); ++i) {
        cosine += pattern[i] * prototype[i];
        normP += pattern[i] * pattern[i];
        normPr += prototype[i] * prototype[i];
    }
    float cosineSim = 0.0f;
    if (normP > 0.0001f && normPr > 0.0001f) {
        cosineSim = cosine / (std::sqrt(normP) * std::sqrt(normPr));
    }
    
    // 2. Pearson correlation
    float meanP = 0.0f, meanPr = 0.0f;
    for (size_t i = 0; i < pattern.size(); ++i) {
        meanP += pattern[i];
        meanPr += prototype[i];
    }
    meanP /= static_cast<float>(pattern.size());
    meanPr /= static_cast<float>(pattern.size());
    
    float corr = 0.0f, stdP = 0.0f, stdPr = 0.0f;
    for (size_t i = 0; i < pattern.size(); ++i) {
        float diffP = pattern[i] - meanP;
        float diffPr = prototype[i] - meanPr;
        corr += diffP * diffPr;
        stdP += diffP * diffP;
        stdPr += diffPr * diffPr;
    }
    float corrSim = 0.0f;
    if (stdP > 0.0001f && stdPr > 0.0001f) {
        corrSim = corr / (std::sqrt(stdP) * std::sqrt(stdPr));
    }
    
    // 3. Magnitude similarity (pattern strength comparison)
    float magP = std::sqrt(normP);
    float magPr = std::sqrt(normPr);
    float magSim = 1.0f - std::abs(magP - magPr) / (magP + magPr + 0.0001f);
    
    // 4. Phase alignment (for oscillatory patterns)
    float phaseSim = 0.0f;
    if (pattern.size() > 2) {
        // Check if patterns have similar phase relationships
        for (size_t i = 1; i < pattern.size(); ++i) {
            float phaseP = std::atan2(pattern[i] - pattern[i-1], pattern[i] + pattern[i-1]);
            float phasePr = std::atan2(prototype[i] - prototype[i-1], prototype[i] + prototype[i-1]);
            float phaseDiff = std::abs(phaseP - phasePr);
            if (phaseDiff > 3.14159f) phaseDiff = 6.28318f - phaseDiff;
            phaseSim += 1.0f - phaseDiff / 3.14159f;
        }
        phaseSim /= static_cast<float>(pattern.size() - 1);
    }
    
    // Weighted combination of similarity metrics
    float finalSimilarity = (
        0.4f * cosineSim +      // Primary pattern matching
        0.3f * corrSim +         // Correlation/linear relationship
        0.2f * magSim +          // Pattern strength consistency
        0.1f * phaseSim         // Phase relationships
    );
    
    return std::max(0.0f, std::min(1.0f, finalSimilarity));
}

// Create concept with evidence-based approach
size_t ConceptFormation::createConceptWithEvidence(const std::vector<float>& advancedFeatures,
                                                  const std::vector<float>& features,
                                                  float reward,
                                                  SimulationStep currentTime,
                                                  float initialSimilarity) {
    // Create concept with weighted evidence
    DiscoveredConcept conceptObj;
    conceptObj.id = nextConceptId_++;
    conceptObj.prototype = advancedFeatures;
    conceptObj.totalObservations = 1;
    conceptObj.initialSimilarity = initialSimilarity;
    
    ConceptInstance instance;
    instance.pattern = advancedFeatures;
    instance.features = features;
    instance.observationCount = 1;
    instance.lastObserved = 0;
    instance.avgReward = reward;
    instance.stability = 1.0f - initialSimilarity;  // Novelty-based stability
    
    conceptObj.instances.push_back(instance);
    conceptObj.avgStability = 1.0f;
    
    // Determine category hint based on properties
    if (reward > 0.5f) {
        conceptObj.categoryHint = "rewarding";
    } else if (reward < -0.3f) {
        conceptObj.categoryHint = "harmful";
    } else {
        conceptObj.categoryHint = "neutral";
    }
    
    concepts_.push_back(conceptObj);
    
    NLM_LOG_INFO("ConceptFormation: Created evidence-based concept " + std::to_string(conceptObj.id) +
                 " (similarity: " + std::to_string(initialSimilarity) + ", category: " + conceptObj.categoryHint + ")");
    
    return conceptObj.id;
}

// Find similarity to nearest concept
float ConceptFormation::findSimilarityToNearestConcept(const std::vector<float>& pattern) const {
    if (concepts_.empty()) return 1.0f;  // No concepts, so pattern is fully novel
    
    float minSimilarity = 1.0f;
    
    for (const auto& conceptObj : concepts_) {
        float similarity = computeAdvancedSimilarity(pattern, conceptObj.prototype);
        minSimilarity = std::min(minSimilarity, similarity);
    }
    
    return minSimilarity;
}

// Update concept with advanced learning
void ConceptFormation::updateConcept(size_t conceptId, const std::vector<float>& advancedFeatures,
                                   const std::vector<float>& features, float reward,
                                   SimulationStep currentTime) {
    DiscoveredConcept* conceptObj = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            conceptObj = &c;
            break;
        }
    }
    
    if (!conceptObj) return;
    
    // Apply learning rate that adapts based on prediction error
    float learningRate = calculateAdaptiveLearningRate(conceptObj, reward, currentTime);
    
    // Add new instance with evidence tracking
    ConceptInstance instance;
    instance.pattern = advancedFeatures;
    instance.features = features;
    instance.observationCount = 1;
    instance.lastObserved = conceptObj->totalObservations;
    instance.avgReward = reward;
    
    // Calculate instance stability based on integration with prototype
    float similarity = computeAdvancedSimilarity(advancedFeatures, conceptObj->prototype);
    instance.stability = 1.0f - similarity;
    
    conceptObj->instances.push_back(instance);
    conceptObj->totalObservations++;
    
    // Update prototype using adaptive learning
    updatePrototype(conceptId, advancedFeatures, learningRate);
    
    // Update stability using exponential moving average
    float recentStability = instance.stability;
    conceptObj->avgStability = conceptObj->avgStability * 0.9f + recentStability * 0.1f;
    
    // Update instance stability
    conceptObj->instances.back().stability = recentStability;
    
    // Apply memory consolidation if enough instances
    if (conceptObj->instances.size() >= 3) {
        consolidateConceptMemory(conceptId);
    }
}

// Calculate adaptive learning rate based on prediction error and reward
float ConceptFormation::calculateAdaptiveLearningRate(const DiscoveredConcept* conceptObj,
                                                     float reward, SimulationStep currentTime) const {
    // Base learning rate
    float learningRate = 0.2f;
    
    // Reduce learning rate for stable concepts
    learningRate *= (1.0f - conceptObj->avgStability);
    
    // Increase learning rate for novel experiences
    if (reward > 0.1f || reward < -0.1f) {
        learningRate *= 1.5f;
    }
    
    // Apply temporal decay to learning rate
    if (currentTime > 0) {
        float temporalFactor = 1.0f - std::exp(-0.01f * currentTime);
        learningRate *= temporalFactor;
    }
    
    return std::clamp(learningRate, 0.05f, 0.5f);
}

// Consolidate concept memory for better pattern separation
void ConceptFormation::consolidateConceptMemory(size_t conceptId) {
    DiscoveredConcept* conceptObj = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            conceptObj = &c;
            break;
        }
    }
    
    if (!conceptObj || conceptObj->instances.size() < 2) return;
    
    // Analyze pattern variance for consolidation
    float totalVariance = 0.0f;
    std::vector<float> variancePerInstance;
    
    for (const auto& instance : conceptObj->instances) {
        float variance = computePatternVariance(instance.pattern, conceptObj->prototype);
        variancePerInstance.push_back(variance);
        totalVariance += variance;
    }
    
    // Consolidate high-variance instances (more novel patterns)
    float avgVariance = totalVariance / conceptObj->instances.size();
    
    for (size_t i = 0; i < conceptObj->instances.size(); ++i) {
        if (variancePerInstance[i] > avgVariance * 1.5f) {
            // Promote this instance to primary status
            std::swap(conceptObj->instances[0], conceptObj->instances[i]);
            break;
        }
    }
    
    // Update prototype with consolidated knowledge
    std::vector<float> consolidatedPrototype;
    float totalWeight = 0.0f;
    
    for (size_t i = 0; i < conceptObj->instances.size(); ++i) {
        float weight = 1.0f / (1.0f + variancePerInstance[i]);  // Low variance = higher weight
        totalWeight += weight;
        
        if (i == 0) {  // Skip primary instance to avoid bias
            continue;
        }
        
        for (size_t j = 0; j < conceptObj->instances[i].pattern.size(); ++j) {
            if (j < consolidatedPrototype.size()) {
                consolidatedPrototype[j] += conceptObj->instances[i].pattern[j] * weight;
            } else {
                consolidatedPrototype.push_back(conceptObj->instances[i].pattern[j] * weight);
            }
        }
    }
    
    if (totalWeight > 0.0f) {
        for (size_t j = 0; j < consolidatedPrototype.size(); ++j) {
            consolidatedPrototype[j] /= totalWeight;
        }
        
        // Blend with existing prototype
        for (size_t j = 0; j < conceptObj->prototype.size() && j < consolidatedPrototype.size(); ++j) {
            conceptObj->prototype[j] = conceptObj->prototype[j] * 0.7f + consolidatedPrototype[j] * 0.3f;
        }
    }
}

// Compute pattern variance relative to prototype
float ConceptFormation::computePatternVariance(const std::vector<float>& pattern,
                                            const std::vector<float>& prototype) const {
    if (pattern.size() != prototype.size() || pattern.empty()) return 0.0f;
    
    float variance = 0.0f;
    for (size_t i = 0; i < pattern.size(); ++i) {
        float diff = pattern[i] - prototype[i];
        variance += diff * diff;
    }
    
    return variance / static_cast<float>(pattern.size());
}

// Apply prediction error learning
void ConceptFormation::applyPredictionErrorLearning(size_t conceptId, float predictionError, float reward) {
    DiscoveredConcept* conceptObj = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            conceptObj = &c;
            break;
        }
    }
    
    if (!conceptObj) return;
    
    // Use prediction error to refine prototype
    float errorLearningRate = std::min(0.5f, predictionError * 0.2f);
    if (std::abs(errorLearningRate) > 0.01f) {
        for (size_t i = 0; i < conceptObj->prototype.size(); ++i) {
            conceptObj->prototype[i] += errorLearningRate * 0.1f;
        }
    }
    
    // Reward prediction bonus
    if (reward > 0.0f) {
        float rewardBonus = reward * 0.05f;
        for (size_t i = 0; i < conceptObj->prototype.size(); ++i) {
            conceptObj->prototype[i] += rewardBonus * 0.05f;
        }
    }
}

// Predict concept outcome using current prototype
std::vector<float> ConceptFormation::predictConceptOutcome(size_t conceptId) const {
    const DiscoveredConcept* conceptObj = getConcept(conceptId);
    if (!conceptObj) {
        return {};
    }
    
    return conceptObj->prototype;
}

// Compute prediction error
float ConceptFormation::computePredictionError(const std::vector<float>& actual,
                                            const std::vector<float>& predicted) const {
    if (actual.size() != predicted.size() || actual.empty()) return 1.0f;
    
    float error = 0.0f;
    for (size_t i = 0; i < actual.size(); ++i) {
        float diff = actual[i] - predicted[i];
        error += diff * diff;
    }
    
    return std::sqrt(error / static_cast<float>(actual.size()));
}

// Enhanced novelty detection with multiple criteria
bool ConceptFormation::isNovel(const std::vector<float>& pattern,
                             float similarityThreshold,
                             const std::vector<float>& features) const {
    // Use advanced features for novelty detection
    std::vector<float> advancedFeatures = features.empty() ? extractAdvancedFeatures(pattern) : features;
    
    // Check if pattern matches existing concepts
    size_t matchingConcept = findConceptForPattern(advancedFeatures);
    
    if (matchingConcept == 0) {
        // No existing concept, so it's novel
        return true;
    }
    
    // Check if similarity is below threshold
    const DiscoveredConcept* conceptObj = getConcept(matchingConcept);
    if (!conceptObj) return true;
    
    float similarity = computeAdvancedSimilarity(advancedFeatures, conceptObj->prototype);
    return similarity < similarityThreshold;
}

// Update prototype with adaptive learning rate
void ConceptFormation::updatePrototype(size_t conceptId, const std::vector<float>& newInstance,
                                    float learningRate) {
    for (auto& conceptObj : concepts_) {
        if (conceptObj.id == conceptId) {
            // Adaptive Hebbian update
            if (conceptObj.prototype.size() == newInstance.size()) {
                for (size_t i = 0; i < conceptObj.prototype.size(); ++i) {
                    float adaptation = learningRate * (newInstance[i] - conceptObj.prototype[i]);
                    conceptObj.prototype[i] += adaptation;
                    
                    // Apply normalization to keep within reasonable bounds
                    conceptObj.prototype[i] = std::clamp(conceptObj.prototype[i], 0.0f, 1.0f);
                }
            }
            break;
        }
    }
}

void ConceptFormation::clear() {
    concepts_.clear();
    nextConceptId_ = 1;
}