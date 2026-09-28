#include "ConceptFormation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>

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
    , featureVectorSize_(8)
    , temporalPatternHistory_(50)
    , featureSimilarityThreshold_(0.7f)
    , temporalPatternThreshold_(0.6f)
    , conceptFeatureStabilityWeight_(0.6f)
{
}

ConceptFormation::~ConceptFormation() = default;

void ConceptFormation::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("ConceptFormation initialized");
}

// Feature extraction and encoding methods

std::vector<float> ConceptFormation::extractStatisticalFeatures(const std::vector<float>& pattern) const {
    std::vector<float> features(8, 0.0f);
    if (pattern.empty()) return features;
    
    size_t patternSize = pattern.size();
    
    // Mean
    float sum = std::accumulate(pattern.begin(), pattern.end(), 0.0f);
    features[0] = sum / patternSize;
    
    // Variance
    float variance = 0.0f;
    for (float val : pattern) {
        float diff = val - features[0];
        variance += diff * diff;
    }
    features[1] = variance / patternSize;
    
    // Standard deviation
    features[2] = std::sqrt(features[1]);
    
    // Min and max
    features[3] = *std::min_element(pattern.begin(), pattern.end());
    features[4] = *std::max_element(pattern.begin(), pattern.end());
    
    // Peak detection (number of local maxima)
    int peakCount = 0;
    for (size_t i = 1; i < patternSize - 1; ++i) {
        if (pattern[i] > pattern[i - 1] && pattern[i] > pattern[i + 1]) {
            peakCount++;
        }
    }
    features[5] = static_cast<float>(peakCount) / patternSize;
    
    // Skewness
    float m3 = 0.0f;
    for (float val : pattern) {
        float diff = val - features[0];
        m3 += diff * diff * diff;
    }
    if (variance > 0.0001f) {
        features[6] = m3 / (patternSize * variance * variance);
    }
    
    // Kurtosis
    float m4 = 0.0f;
    for (float val : pattern) {
        float diff = val - features[0];
        m4 += diff * diff * diff * diff;
    }
    if (variance > 0.0001f) {
        float stdDev = std::sqrt(variance);
        features[7] = m4 / (patternSize * variance * variance) - 3.0f;
    }
    
    return features;
}

std::vector<float> ConceptFormation::extractPeakFeatures(const std::vector<float>& pattern) const {
    std::vector<float> features;
    if (pattern.size() < 3) return features;
    
    float maxVal = *std::max_element(pattern.begin(), pattern.end());
    float minVal = *std::min_element(pattern.begin(), pattern.end());
    
    std::vector<float> peaks;
    for (size_t i = 1; i < pattern.size() - 1; ++i) {
        if (pattern[i] > pattern[i - 1] && pattern[i] > pattern[i + 1]) {
            peaks.push_back(pattern[i]);
        }
    }
    
    features.push_back(maxVal);
    features.push_back(minVal);
    features.push_back(static_cast<float>(peaks.size()) / pattern.size());
    
    if (!peaks.empty()) {
        float peakSum = std::accumulate(peaks.begin(), peaks.end(), 0.0f);
        features.push_back(peakSum / peaks.size());
    } else {
        features.push_back(0.0f);
    }
    
    float threshold = maxVal * 0.5f;
    int widthPoints = 0;
    for (float val : pattern) {
        if (val >= threshold) widthPoints++;
    }
    features.push_back(static_cast<float>(widthPoints) / pattern.size());
    
    return features;
}

std::vector<float> ConceptFormation::extractSpatialFeatures(const std::vector<float>& pattern) const {
    std::vector<float> features;
    
    if (!pattern.empty()) {
        float sum = std::accumulate(pattern.begin(), pattern.end(), 0.0f);
        float centerOfMass = sum / pattern.size();
        features.push_back(centerOfMass);
    }
    
    float spread = 0.0f;
    for (float val : pattern) {
        spread += val * val;
    }
    features.push_back(spread / pattern.size());
    
    if (pattern.size() >= 3) {
        float edgeSum = pattern.front() + pattern.back();
        float total = std::accumulate(pattern.begin(), pattern.end(), 0.0f);
        features.push_back(edgeSum / total);
    }
    
    return features;
}

std::vector<float> ConceptFormation::extractTemporalFeatures(
    const std::vector<std::vector<float>>& patternHistory,
    size_t lookbackWindow) const {
    std::vector<float> features;
    
    if (patternHistory.size() < 2) return features;
    
    size_t startIdx = std::max<size_t>(0, patternHistory.size() - lookbackWindow);
    std::vector<std::vector<float>> recentPatterns(patternHistory.begin() + startIdx, patternHistory.end());
    
    std::vector<float> temporalMeans;
    std::vector<float> temporalStds;
    
    for (const auto& pattern : recentPatterns) {
        if (!pattern.empty()) {
            float mean = std::accumulate(pattern.begin(), pattern.end(), 0.0f) / pattern.size();
            temporalMeans.push_back(mean);
            
            float variance = 0.0f;
            for (float val : pattern) {
                float diff = val - mean;
                variance += diff * diff;
            }
            temporalStds.push_back(variance / pattern.size());
        }
    }
    
    if (temporalMeans.size() >= 2) {
        float meanChange = 0.0f;
        for (size_t i = 1; i < temporalMeans.size(); ++i) {
            meanChange += temporalMeans[i] - temporalMeans[i - 1];
        }
        features.push_back(meanChange / (temporalMeans.size() - 1));
    }
    
    if (temporalMeans.size() >= 2) {
        float meanOfMeans = std::accumulate(temporalMeans.begin(), temporalMeans.end(), 0.0f) / temporalMeans.size();
        float varianceOfMeans = 0.0f;
        for (float mean : temporalMeans) {
            float diff = mean - meanOfMeans;
            varianceOfMeans += diff * diff;
        }
        features.push_back(varianceOfMeans / temporalMeans.size());
    }
    
    if (temporalStds.size() >= 2) {
        float avgStd = std::accumulate(temporalStds.begin(), temporalStds.end(), 0.0f) / temporalStds.size();
        float stdOfStds = 0.0f;
        for (float std : temporalStds) {
            float diff = std - avgStd;
            stdOfStds += diff * diff;
        }
        features.push_back(stdOfStds / temporalStds.size());
    }
    
    if (recentPatterns.size() >= 3) {
        int repeats = 0;
        for (size_t i = 0; i < recentPatterns.size() - 2; ++i) {
            if (recentPatterns[i] == recentPatterns[i + 2]) {
                repeats++;
            }
        }
        features.push_back(static_cast<float>(repeats) / (recentPatterns.size() - 2));
    }
    
    return features;
}

float ConceptFormation::computeFeatureSimilarity(
    const std::vector<float>& featuresA,
    const std::vector<float>& featuresB) const {
    if (featuresA.empty() || featuresB.empty() || featuresA.size() != featuresB.size()) {
        return 0.0f;
    }
    
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < featuresA.size(); ++i) {
        dot += featuresA[i] * featuresB[i];
        normA += featuresA[i] * featuresA[i];
        normB += featuresB[i] * featuresB[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

// Concept formation with enhanced feature extraction

std::vector<float> ConceptFormation::extractFeatures(const std::vector<float>& pattern) const {
    std::vector<float> features;
    
    std::vector<float> stats = extractStatisticalFeatures(pattern);
    features.insert(features.end(), stats.begin(), stats.end());
    
    std::vector<float> peaks = extractPeakFeatures(pattern);
    features.insert(features.end(), peaks.begin(), peaks.end());
    
    std::vector<float> spatial = extractSpatialFeatures(pattern);
    features.insert(features.end(), spatial.begin(), spatial.end());
    
    if (features.size() > featureVectorSize_) {
        features.resize(featureVectorSize_);
    } else if (features.size() < featureVectorSize_) {
        features.resize(featureVectorSize_, 0.0f);
    }
    
    return features;
}

size_t ConceptFormation::findSimilarConceptByFeatures(
    const std::vector<float>& features,
    float similarityThreshold) const {
    if (features.empty()) return 0;
    
    float bestSimilarity = 0.0f;
    size_t bestConcept = 0;
    
    for (const auto& concept : concepts_) {
        bool hasMatchingInstance = false;
        for (const auto& instance : concept.instances) {
            if (!instance.features.empty()) {
                hasMatchingInstance = true;
                float sim = computeFeatureSimilarity(features, instance.features);
                if (sim > bestSimilarity && sim >= similarityThreshold) {
                    bestSimilarity = sim;
                    bestConcept = concept.id;
                }
                break;
            }
        }
    }
    
    return bestConcept;
}

void ConceptFormation::updateConceptWithFeatures(size_t conceptId,
                                                const std::vector<float>& newPattern,
                                                const std::vector<float>& newFeatures,
                                                float reward,
                                                float predictionError) {
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (!concept) return;
    
    ConceptInstance instance;
    instance.pattern = newPattern;
    instance.features = newFeatures;
    instance.observationCount = concept->instances.size() + 1;
    instance.lastObserved = concept->totalObservations;
    instance.avgReward = reward;
    
    float featureStability = 1.0f;
    if (!concept->instances.empty() && !newFeatures.empty()) {
        const auto& lastInstance = concept->instances.back();
        if (!lastInstance.features.empty()) {
            featureStability = computeFeatureSimilarity(newFeatures, lastInstance.features);
        }
    }
    
    instance.stability = featureStability * conceptFeatureStabilityWeight_;
    
    concept->instances.push_back(instance);
    concept->totalObservations++;
    
    updateConceptFeaturePrototype(conceptId, newFeatures);
    
    updateConceptFeatureStability(conceptId);
    
    if (predictionError > 0.5f) {
        for (auto& inst : concept->instances) {
            inst.stability *= 0.8f;
        }
        concept->avgStability *= 0.9f;
    }
}

void ConceptFormation::updateConceptFeaturePrototype(size_t conceptId,
                                                     const std::vector<float>& newFeatures) {
    for (auto& concept : concepts_) {
        if (concept.id == conceptId) {
            if (concept.instances.empty()) {
                concept.featurePrototype = newFeatures;
                return;
            }
            
            if (concept.featurePrototype.size() == newFeatures.size()) {
                float alpha = 0.3f;
                for (size_t i = 0; i < concept.featurePrototype.size(); ++i) {
                    concept.featurePrototype[i] = 
                        concept.featurePrototype[i] * (1.0f - alpha) + 
                        newFeatures[i] * alpha;
                }
            }
            break;
        }
    }
}

void ConceptFormation::updateConceptFeatureStability(size_t conceptId) {
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (!concept) return;
    
    float totalFeatureStability = 0.0f;
    size_t stableCount = 0;
    
    for (const auto& instance : concept->instances) {
        if (!instance.features.empty() && !concept->featurePrototype.empty()) {
            float instFeatureStability = computeFeatureSimilarity(
                instance.features, concept->featurePrototype);
            totalFeatureStability += instFeatureStability;
            stableCount++;
        }
    }
    
    if (stableCount > 0) {
        float avgFeatureStability = totalFeatureStability / stableCount;
        
        float patternStability = 0.0f;
        size_t patternStableCount = 0;
        
        for (const auto& instance : concept->instances) {
            if (concept->instances.size() > 1) {
                float sim = computeSimilarity(instance.pattern, concept->prototype);
                patternStability += sim;
                patternStableCount++;
            }
        }
        
        if (patternStableCount > 0) {
            patternStability /= patternStableCount;
        }
        
        concept->avgStability = patternStability * (1.0f - conceptFeatureStabilityWeight_) +
                               avgFeatureStability * conceptFeatureStabilityWeight_;
    }
}

float ConceptFormation::getConceptRelevanceToGoals(size_t conceptId) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept) return 0.0f;
    
    float relevance = concept->avgStability;
    
    float freshnessFactor = 1.0f;
    if (concept->totalObservations > 0) {
        float recencyWeight = std::min(1.0f, static_cast<float>(concept->instances.size()) / 5.0f);
        freshnessFactor = 0.7f + 0.3f * recencyWeight;
    }
    
    if (!concept->featurePrototype.empty()) {
        float featureCoherence = 0.0f;
        size_t coherenceCount = 0;
        
        for (const auto& instance : concept->instances) {
            if (!instance.features.empty()) {
                float coherence = computeFeatureSimilarity(instance.features, concept->featurePrototype);
                featureCoherence += coherence;
                coherenceCount++;
            }
        }
        
        if (coherenceCount > 0) {
            featureCoherence /= coherenceCount;
            relevance *= (0.5f + 0.5f * featureCoherence);
        }
    }
    
    return relevance * freshnessFactor;
}

bool ConceptFormation::isConceptFresh(size_t conceptId, SimulationStep currentTime) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept) return false;
    
    if (concept->instances.empty()) return false;
    
    SimulationStep lastObserved = concept->instances.back().lastObserved;
    SimulationStep age = currentTime - lastObserved;
    
    return age <= stabilityWindow_;
}

bool ConceptFormation::meetsGoalRelevanceCriteria(size_t conceptId) const {
    float relevance = getConceptRelevanceToGoals(conceptId);
    
    float stability = getConceptStability(conceptId);
    
    bool fresh = isConceptFresh(conceptId, 0);
    
    return (relevance > 0.5f && stability > stabilityThreshold_ && fresh);
}

// Existing methods implementation

size_t ConceptFormation::presentExperience(const std::vector<float>& pattern,
                                          const std::vector<float>& features,
                                          float reward,
                                          SimulationStep currentTime) {
    if (pattern.empty()) return 0;
    
    std::vector<float> extractedFeatures = features;
    if (extractedFeatures.empty()) {
        extractedFeatures = extractFeatures(pattern);
    }
    
    size_t matchingConcept = findSimilarConceptByFeatures(extractedFeatures, featureSimilarityThreshold_);
    
    if (matchingConcept > 0) {
        updateConceptWithFeatures(matchingConcept, pattern, extractedFeatures, reward, 0.0f);
        return matchingConcept;
    }
    
    if (isNovel(pattern, formationThreshold_)) {
        return createConcept(pattern, extractedFeatures, reward);
    }
    
    return 0;
}

void ConceptFormation::createConcept(const std::vector<float>& pattern,
                                    const std::vector<float>& features,
                                    float reward) {
    DiscoveredConcept concept;
    concept.id = nextConceptId_++;
    concept.prototype = pattern;
    concept.featurePrototype = features;
    concept.totalObservations = 1;
    
    ConceptInstance instance;
    instance.pattern = pattern;
    instance.features = features;
    instance.observationCount = 1;
    instance.lastObserved = 0;
    instance.avgReward = reward;
    instance.stability = 1.0f;
    
    concept.instances.push_back(instance);
    concept.avgStability = 1.0f;
    
    if (reward > 0.5f) {
        concept.categoryHint = "rewarding";
    } else if (reward < -0.3f) {
        concept.categoryHint = "harmful";
    } else {
        concept.categoryHint = "neutral";
    }
    
    concepts_.push_back(concept);
    
    NLM_LOG_INFO("ConceptFormation: Created concept " + std::to_string(concept.id) +
                 " (category: " + concept.categoryHint + ")");
}

void ConceptFormation::updateConcept(size_t conceptId, const std::vector<float>& newPattern,
                                    const std::vector<float>& features, float reward) {
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (!concept) return;
    
    ConceptInstance instance;
    instance.pattern = newPattern;
    instance.features = features;
    instance.observationCount = concept->instances.size() + 1;
    instance.lastObserved = concept->totalObservations;
    instance.avgReward = reward;
    
    float patternStability = 1.0f, featureStability = 1.0f;
    
    if (!concept->instances.empty() && !newPattern.empty()) {
        patternStability = computeSimilarity(newPattern, concept->prototype);
    }
    
    if (!concept->instances.empty() && !features.empty() && !concept->featurePrototype.empty()) {
        featureStability = computeFeatureSimilarity(features, concept->featurePrototype);
    }
    
    instance.stability = (patternStability + featureStability) / 2.0f;
    
    concept->instances.push_back(instance);
    concept->totalObservations++;
    
    updatePrototype(conceptId, newPattern);
    
    updateConceptFeaturePrototype(conceptId, features);
    
    float newStability = 1.0f;
    if (concept->instances.size() > 1) {
        float totalPatternSim = 0.0f, totalFeatureSim = 0.0f;
        size_t recentCount = std::min(concept->instances.size(), stabilityWindow_);
        
        for (size_t i = concept->instances.size() - recentCount; 
             i < concept->instances.size(); ++i) {
            totalPatternSim += computeSimilarity(concept->instances[i].pattern, concept->prototype);
            if (!concept->featurePrototype.empty() && !concept->instances[i].features.empty()) {
                totalFeatureSim += computeFeatureSimilarity(
                    concept->instances[i].features, concept->featurePrototype);
            }
        }
        
        float avgPatternSim = totalPatternSim / recentCount;
        float avgFeatureSim = totalFeatureSim / recentCount;
        
        newStability = (avgPatternSim + avgFeatureSim) / 2.0f;
        concept->avgStability = concept->avgStability * 0.9f + newStability * 0.1f;
    }
    
    concept->instances.back().stability = newStability;
}

void ConceptFormation::updatePrototype(size_t conceptId, const std::vector<float>& newInstance) {
    for (auto& concept : concepts_) {
        if (concept.id == conceptId) {
            float alpha = 0.2f;
            
            if (concept.prototype.size() == newInstance.size()) {
                for (size_t i = 0; i < concept.prototype.size(); ++i) {
                    concept.prototype[i] = concept.prototype[i] * (1.0f - alpha) + 
                                          newInstance[i] * alpha;
                }
            }
            break;
        }
    }
}

float ConceptFormation::computeSimilarity(const std::vector<float>& a,
                                         const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

bool ConceptFormation::isConceptStable(size_t conceptId) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept) return false;
    
    return concept->avgStability >= stabilityThreshold_;
}

float ConceptFormation::getGeneralizationAbility(size_t conceptId) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept) return 0.0f;
    
    if (concept->instances.size() < 2) return 0.0f;
    
    float totalVariance = 0.0f;
    for (const auto& instance : concept->instances) {
        float sim = computeSimilarity(instance.pattern, concept->prototype);
        totalVariance += (1.0f - sim);
    }
    
    return 1.0f - (totalVariance / concept->instances.size());
}

bool ConceptFormation::processExperience(const std::vector<float>& sensoryPattern) {
    if (sensoryPattern.empty() || !brain_) return false;
    
    float reward = 0.0f;
    if (brain_->getPredictionSystem()) {
        float error = brain_->getPredictionSystem()->getPredictionError();
        reward = -std::min(1.0f, error * 2.0f);
    }
    
    size_t conceptId = presentExperience(sensoryPattern, sensoryPattern, reward, 0);
    
    if (conceptId > 0) {
        return true;
    }
    
    return false;
}

void ConceptFormation::getRelevantConceptsForGoals(std::vector<size_t>& conceptIds) const {
    conceptIds.clear();
    
    for (const auto& concept : concepts_) {
        if (concept.avgStability > 0.5f) {
            bool meetsRelevanceCriteria = meetsGoalRelevanceCriteria(concept.id);
            
            if (meetsRelevanceCriteria) {
                conceptIds.push_back(concept.id);
            }
        }
    }
}

void ConceptFormation::clear() {
    concepts_.clear();
    nextConceptId_ = 1;
    temporalPatterns_.clear();
    featureSpace_.clear();
}

void ConceptFormation::mergeConcepts(size_t conceptA, size_t conceptB) {
    DiscoveredConcept* a = nullptr;
    DiscoveredConcept* b = nullptr;
    
    for (auto& c : concepts_) {
        if (c.id == conceptA) a = &c;
        if (c.id == conceptB) b = &c;
    }
    
    if (!a || !b) return;
    
    for (const auto& instance : b->instances) {
        a->instances.push_back(instance);
        a->totalObservations++;
    }
    
    for (size_t i = 0; i < a->prototype.size() && i < b->prototype.size(); ++i) {
        a->prototype[i] = (a->prototype[i] + b->prototype[i]) / 2.0f;
    }
    
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
    
    for (const auto& concept : concepts_) {
        float sim = computeSimilarity(pattern, concept.prototype);
        if (sim > bestSimilarity && sim >= formationThreshold_) {
            bestSimilarity = sim;
            bestConcept = concept.id;
        }
    }
    
    return bestConcept;
}

const DiscoveredConcept* ConceptFormation::getConcept(size_t conceptId) const {
    for (const auto& concept : concepts_) {
        if (concept.id == conceptId) {
            return &concept;
        }
    }
    return nullptr;
}

std::vector<float> ConceptFormation::getConceptPrototype(size_t conceptId) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (concept) {
        return concept->prototype;
    }
    return {};
}

const std::vector<ConceptInstance>& ConceptFormation::getConceptInstances(size_t conceptId) const {
    static std::vector<ConceptInstance> empty;
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (concept) {
        return concept->instances;
    }
    return empty;
}

float ConceptFormation::getConceptStability(size_t conceptId) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (concept) {
        return concept->avgStability;
    }
    return 0.0f;
}

float ConceptFormation::getGeneralizationAbility(size_t conceptId) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept) return 0.0f;
    
    if (concept->instances.size() < 2) return 0.0f;
    
    float totalVariance = 0.0f;
    for (const auto& instance : concept->instances) {
        float sim = computeSimilarity(instance.pattern, concept->prototype);
        totalVariance += (1.0f - sim);
    }
    
    return 1.0f - (totalVariance / concept->instances.size());
}

void ConceptFormation::getRelevantConceptsForGoals(std::vector<size_t>& conceptIds) const {
    conceptIds.clear();
    
    for (const auto& concept : concepts_) {
        if (concept.avgStability > 0.5f) {
            bool meetsRelevanceCriteria = meetsGoalRelevanceCriteria(concept.id);
            
            if (meetsRelevanceCriteria) {
                conceptIds.push_back(concept.id);
            }
        }
    }
}

void ConceptFormation::clear() {
    concepts_.clear();
    nextConceptId_ = 1;
    temporalPatterns_.clear();
    featureSpace_.clear();
}

// SpatialRepresentation Implementation
struct SpatialRepresentation::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

SpatialRepresentation::SpatialRepresentation()
    : pImpl(new Impl)
    , brain_(nullptr)
    , currentX_(0)
    , currentY_(0)
    , gridSpacing_(1.0f)
{
}

SpatialRepresentation::~SpatialRepresentation() = default;

void SpatialRepresentation::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("SpatialRepresentation initialized");
}

void SpatialRepresentation::recordPosition(float x, float y, const std::vector<float>& sensoryCues) {
    currentX_ = x;
    currentY_ = y;
    lastSensoryCue_ = sensoryCues;
    
    bool hasNearbyCell = false;
    for (const auto& pc : placeCells_) {
        float dx = pc.x - x;
        float dy = pc.y - y;
        if (std::sqrt(dx * dx + dy * dy) < pc.fieldRadius) {
            hasNearbyCell = true;
            break;
        }
    }
    
    if (!hasNearbyCell && brain_) {
        PlaceCell newCell;
        newCell.neuron = NeuronId(placeCells_.size() + 40000);
        newCell.x = x;
        newCell.y = y;
        newCell.fieldRadius = 0.5f;
        newCell.activation = 1.0f;
        
        placeCells_.push_back(newCell);
        
        brain_->injectCurrent(newCell.neuron, 5.0f);
    }
}

std::pair<float, float> SpatialRepresentation::getPredictedPosition() const {
    return {currentX_, currentY_};
}

void SpatialRepresentation::integrateMovement(float dx, float dy) {
    currentX_ += dx;
    currentY_ += dy;
}

float SpatialRepresentation::getPositionActivation(float x, float y) const {
    float totalActivation = 0.0f;
    
    for (const auto& pc : placeCells_) {
        float dx = pc.x - x;
        float dy = pc.y - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < pc.fieldRadius * 2.0f) {
            float activation = std::exp(-dist * dist / (2.0f * pc.fieldRadius * pc.fieldRadius));
            totalActivation += activation * pc.activation;
        }
    }
    
    return totalActivation;
}

std::vector<NeuronId> SpatialRepresentation::getPlaceNeuronsNear(float x, float y, float radius) const {
    std::vector<NeuronId> result;
    
    for (const auto& pc : placeCells_) {
        float dx = pc.x - x;
        float dy = pc.y - y;
        if (std::sqrt(dx * dx + dy * dy) < radius) {
            result.push_back(pc.neuron);
        }
    }
    
    return result;
}

void SpatialRepresentation::clear() {
    placeCells_.clear();
    currentX_ = 0;
    currentY_ = 0;
}

// TemporalRelation Implementation
struct TemporalRelation::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

TemporalRelation::TemporalRelation()
    : pImpl(new Impl)
    , brain_(nullptr)
{
}

TemporalRelation::~TemporalRelation() = default;

void TemporalRelation::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
}

void TemporalRelation::recordSequence(const std::vector<float>& eventA,
                                      const std::vector<float>& eventB,
                                      float reward) {
    for (auto& link : sequences_) {
        float simA = 0.0f, simB = 0.0f;
        
        if (link.fromEvent.size() == eventA.size() && link.toEvent.size() == eventB.size()) {
            float dotA = 0.0f, normA = 0.0f, normB = 0.0f;
            for (size_t i = 0; i < eventA.size(); ++i) {
                dotA += link.fromEvent[i] * eventA[i];
                normA += link.fromEvent[i] * link.fromEvent[i];
                normB += eventA[i] * eventA[i];
            }
            if (normA > 0.0001f && normB > 0.0001f) {
                simA = dotA / (std::sqrt(normA) * std::sqrt(normB));
            }
            
            float dotB = 0.0f;
            for (size_t i = 0; i < eventB.size(); ++i) {
                dotB += link.toEvent[i] * eventB[i];
            }
            if (normB > 0.0001f) {
                simB = dotB / normB;
            }
        }
        
        if (simA > 0.8f && simB > 0.8f) {
            link.strength = std::min(1.0f, link.strength + 0.1f);
            link.observationCount++;
            return;
        }
    }
    
    SequenceLink newLink;
    newLink.fromEvent = eventA;
    newLink.toEvent = eventB;
    newLink.strength = 0.5f;
    newLink.observationCount = 1;
    
    sequences_.push_back(newLink);
}

std::vector<float> TemporalRelation::predictNext(const std::vector<float>& eventA) const {
    float bestSim = 0.0f;
    std::vector<float> bestPrediction;
    
    for (const auto& link : sequences_) {
        if (link.fromEvent.size() != eventA.size()) continue;
        
        float dot = 0.0f, normA = 0.0f, normB = 0.0f;
        for (size_t i = 0; i < eventA.size(); ++i) {
            dot += link.fromEvent[i] * eventA[i];
            normA += link.fromEvent[i] * link.fromEvent[i];
            normB += eventA[i] * eventA[i];
        }
        
        if (normA > 0.0001f && normB > 0.0001f) {
            float sim = dot / (std::sqrt(normA) * std::sqrt(normB));
            if (sim > bestSim) {
                bestSim = sim;
                bestPrediction = link.toEvent;
            }
        }
    }
    
    return bestPrediction;
}

std::vector<float> TemporalRelation::predictPrevious(const std::vector<float>& eventB) const {
    float bestSim = 0.0f;
    std::vector<float> bestPrediction;
    
    for (const auto& link : sequences_) {
        if (link.toEvent.size() != eventB.size()) continue;
        
        float dot = 0.0f, normA = 0.0f, normB = 0.0f;
        for (size_t i = 0; i < eventB.size(); ++i) {
            dot += link.toEvent[i] * eventB[i];
            normA += link.toEvent[i] * link.toEvent[i];
            normB += eventB[i] * eventB[i];
        }
        
        if (normA > 0.0001f && normB > 0.0001f) {
            float sim = dot / (std::sqrt(normA) * std::sqrt(normB));
            if (sim > bestSim) {
                bestSim = sim;
                bestPrediction = link.fromEvent;
            }
        }
    }
    
    return bestPrediction;
}

float TemporalRelation::getPredictionConfidence(const std::vector<float>& event) const {
    size_t matchingCount = 0;
    
    for (const auto& link : sequences_) {
        if (link.fromEvent.size() != event.size()) continue;
        
        float dot = 0.0f, normA = 0.0f, normB = 0.0f;
        for (size_t i = 0; i < event.size(); ++i) {
            dot += link.fromEvent[i] * event[i];
            normA += link.fromEvent[i] * link.fromEvent[i];
            normB += event[i] * event[i];
        }
        
        if (normA > 0.0001f && normB > 0.0001f) {
            float sim = dot / (std::sqrt(normA) * std::sqrt(normB));
            if (sim > 0.7f) {
                matchingCount++;
            }
        }
    }
    
    return std::min(1.0f, static_cast<float>(matchingCount) / 3.0f);
}

void TemporalRelation::clear() {
    sequences_.clear();
}

} // namespace nlm
