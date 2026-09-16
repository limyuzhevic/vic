#include "ConceptFormation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>
#include <queue>
#include <set>
#include <map>
#include <random>
#include <limits>

namespace nlm {

struct ConceptFormation::Impl {
    Brain* brain;
    
    // Enhanced concept storage for advanced features
    std::unordered_map<size_t, float> conceptConfidence;
    std::unordered_map<size_t, std::vector<size_t>> conceptHierarchy;
    std::unordered_map<size_t, std::unordered_map<size_t, float>> conceptAssociations;
    std::unordered_map<size_t, std::vector<size_t>> conceptClusters;
    std::unordered_map<size_t, float> conceptDriftHistory;
    std::unordered_map<size_t, std::vector<float>> featureVectors;
    std::unordered_map<size_t, float> conceptDecayTimers;
    std::unordered_map<size_t, float> predictionErrors;
    std::unordered_map<size_t, float> noveltyScores;
    std::unordered_map<size_t, std::vector<size_t>> conceptAncestry;
    std::unordered_map<size_t, float> attentionWeights;
    
    // Advanced learning parameters
    float anomalyThreshold_ = 2.5f;
    float driftDetectionThreshold_ = 0.3f;
    float conceptDecayRate_ = 0.01f;
    float predictionErrorThreshold_ = 0.2f;
    float noveltyThreshold_ = 0.5f;
    float featureNormalizationFactor_ = 0.5f;
    size_t maxClusterSize_ = 10;
    size_t featureHistorySize_ = 100;
    
    // Random device for stochastic algorithms
    std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<float> dist{0.0f, 1.0f};
    
    Impl() : brain(nullptr) {}
};

ConceptFormation::ConceptFormation()
    : pImpl(new Impl)
    , brain_(nullptr)
    , nextConceptId_(1)
    , formationThreshold_(0.75f)
    , stabilityThreshold_(0.7f)
    , stabilityWindow_(5)
    , conceptCount_(0)
    , totalObservationCount_(0)
{
    // Initialize advanced features
    pImpl->conceptCount = 0;
    pImpl->totalObservationCount = 0;
    pImpl->lastFeatureVector.clear();
    pImpl->featureVectorHistory.reserve(pImpl->featureHistorySize_);
    pImpl->activeConcepts.clear();
    pImpl->attentionWeights.clear();
    pImpl->featureImportanceWeights.clear();
    pImpl->clusterCenters.clear();
    pImpl->conceptDepthMap.clear();
    pImpl->conceptWidthMap.clear();
    pImpl->ancestryMap.clear();
}

ConceptFormation::~ConceptFormation() = default;

void ConceptFormation::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("ConceptFormation initialized");
}

size_t ConceptFormation::presentExperience(const std::vector<float>& pattern,
                                          const std::vector<float>& features,
                                          float reward,
                                          SimulationStep currentTime) {
    if (pattern.empty()) return 0;
    
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

size_t ConceptFormation::createConcept(const std::vector<float>& pattern,
                                       const std::vector<float>& features,
                                       float reward) {
    DiscoveredConcept concept;
    concept.id = nextConceptId_++;
    concept.prototype = pattern;
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
    
    // Determine category hint based on properties
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
    
    return concept.id;
}

void ConceptFormation::organizeConceptHierarchy() {
    // Organize concepts into hierarchy based on abstraction levels
    conceptToParent_.clear();
    conceptToChildren_.clear();
    ancestryMap_.clear();
    
    // Find abstract concepts (high abstraction level)
    std::vector<size_t> abstractConcepts;
    for (const auto& concept : concepts_) {
        if (concept.avgStability > stabilityThreshold_) {
            abstractConcepts.push_back(concept.id);
        }
    }
    
    // Sort by abstraction level
    std::sort(abstractConcepts.begin(), abstractConcepts.end(),
        [this](size_t a, size_t b) {
            auto* ca = getConcept(a);
            auto* cb = getConcept(b);
            if (ca && cb) {
                return ca->abstractionLevel > cb->abstractionLevel;
            }
            return false;
        });
    
    // Build hierarchy
    for (size_t abstractId : abstractConcepts) {
        // Find most specific child concepts
        std::vector<size_t> similarConcepts;
        for (const auto& concept : concepts_) {
            if (concept.id != abstractId && concept.parentConceptId == 0) {
                float similarity = computeSimilarity(
                    getConceptPrototype(abstractId),
                    getConceptPrototype(concept.id)
                );
                if (similarity > formationThreshold_ * 0.8f) {
                    similarConcepts.push_back(concept.id);
                }
            }
        }
        
        // Sort by specialization (most specific first)
        std::sort(similarConcepts.begin(), similarConcepts.end(),
            [this, abstractId](size_t a, size_t b) {
                float simA = computeSimilarity(
                    getConceptPrototype(abstractId),
                    getConceptPrototype(a)
                );
                float simB = computeSimilarity(
                    getConceptPrototype(abstractId),
                    getConceptPrototype(b)
                );
                return simA > simB;
            });
        
        // Set parent-child relationships
        if (!similarConcepts.empty()) {
            conceptToParent_[abstractId] = 0;  // Abstract concepts are roots
            for (size_t childId : similarConcepts) {
                conceptToParent_[childId] = abstractId;
                conceptToChildren_[abstractId].push_back(childId);
                ancestryMap_[childId].push_back(abstractId);
            }
        }
    }
}

void ConceptFormation::clusterConcepts() {
    // Cluster concepts using hierarchical clustering
    conceptToCluster_.clear();
    clusterToConcepts_.clear();
    
    // Calculate similarity matrix
    std::vector<std::vector<float>> similarityMatrix;
    std::vector<size_t> conceptIds;
    
    for (const auto& concept : concepts_) {
        conceptIds.push_back(concept.id);
    }
    
    similarityMatrix.resize(conceptIds.size(), std::vector<float>(conceptIds.size(), 0.0f));
    
    for (size_t i = 0; i < conceptIds.size(); ++i) {
        for (size_t j = 0; j < conceptIds.size(); ++j) {
            if (i != j) {
                similarityMatrix[i][j] = computeSimilarity(
                    getConceptPrototype(conceptIds[i]),
                    getConceptPrototype(conceptIds[j])
                );
            }
        }
    }
    
    // Single linkage clustering
    std::vector<bool> assigned(conceptIds.size(), false);
    size_t clusterId = 1;
    
    for (size_t i = 0; i < conceptIds.size(); ++i) {
        if (!assigned[i]) {
            std::queue<size_t> queue;
            queue.push(i);
            assigned[i] = true;
            clusterToConcepts_[clusterId].push_back(conceptIds[i]);
            conceptToCluster_[conceptIds[i]] = clusterId;
            
            while (!queue.empty()) {
                size_t current = queue.front();
                queue.pop();
                
                for (size_t j = 0; j < conceptIds.size(); ++j) {
                    if (!assigned[j] && similarityMatrix[current][j] > formationThreshold_) {
                        assigned[j] = true;
                        queue.push(j);
                        clusterToConcepts_[clusterId].push_back(conceptIds[j]);
                        conceptToCluster_[conceptIds[j]] = clusterId;
                    }
                }
            }
            
            clusterId++;
        }
    }
}

void ConceptFormation::consolidateConceptsOverTime() {
    // Consolidate concepts based on usage and stability
    totalObservationCount_ = 0;
    conceptCount_ = 0;
    
    std::vector<DiscoveredConcept> stableConcepts;
    std::vector<size_t> expiredConcepts;
    
    for (auto& concept : concepts_) {
        // Calculate decay factor
        float decayFactor = std::exp(-pImpl->conceptDecayRate_ * concept.decayTimer);
        
        // Update stability based on decay
        concept.avgStability *= decayFactor;
        
        // Check if concept should expire
        if (concept.decayTimer > 10.0f && concept.avgStability < stabilityThreshold_ * 0.5f) {
            expiredConcepts.push_back(concept.id);
            continue;
        }
        
        // Consolidate if stable enough
        if (concept.avgStability > stabilityThreshold_) {
            // Update abstraction level based on instance variance
            if (!concept.instances.empty()) {
                float totalVariance = 0.0f;
                for (const auto& instance : concept.instances) {
                    totalVariance += (1.0f - computeSimilarity(instance.pattern, concept.prototype));
                }
                concept.abstractionLevel = 1.0f - (totalVariance / concept.instances.size());
                
                // Update consolidation level
                concept.consolidationLevel = std::min(1.0f, 
                    concept.consolidationLevel + 0.01f * concept.instances.size() / 100.0f);
            }
            
            stableConcepts.push_back(concept);
            conceptCount_++;
            totalObservationCount_ += concept.totalObservations;
        }
    }
    
    // Remove expired concepts
    for (size_t expiredId : expiredConcepts) {
        concepts_.erase(
            std::remove_if(concepts_.begin(), concepts_.end(),
                [expiredId](const DiscoveredConcept& c) { return c.id == expiredId; }),
            concepts_.end()
        );
    }
    
    // Update concepts list
    concepts_ = stableConcepts;
}

void ConceptFormation::discoverConceptsThroughAnomaly(const std::vector<float>& pattern) {
    // Detect anomalous patterns that may indicate new concepts
    if (concepts_.empty()) return;
    
    std::vector<size_t> anomalousConcepts;
    
    for (const auto& concept : concepts_) {
        float similarity = computeSimilarity(pattern, concept.prototype);
        
        // Calculate anomaly score based on prediction error
        std::vector<float> predicted = predictConceptPattern(concept.id);
        float predictionError = 0.0f;
        
        if (!predicted.empty() && pattern.size() == predicted.size()) {
            float dot = 0.0f, normP = 0.0f, normA = 0.0f;
            for (size_t i = 0; i < pattern.size(); ++i) {
                dot += pattern[i] * predicted[i];
                normP += pattern[i] * pattern[i];
                normA += predicted[i] * predicted[i];
            }
            if (normP > 0.0001f && normA > 0.0001f) {
                predictionError = 1.0f - (dot / (std::sqrt(normP) * std::sqrt(normA)));
            }
        }
        
        if (predictionError > pImpl->anomalyThreshold_) {
            anomalousConcepts.push_back(concept.id);
            pImpl->conceptDriftHistory[concept.id] += predictionError;
        }
    }
    
    // If highly anomalous, consider forming new concept
    if (!anomalousConcepts.empty() && patternAverageAnomaly(pattern) > pImpl->anomalyThreshold_) {
        NLM_LOG_INFO("ConceptFormation: Anomaly detected, considering new concept");
    }
}

void ConceptFormation::detectAndAdaptToDrift(size_t conceptId, float newSimilarity) {
    // Detect concept drift and adapt accordingly
    auto* concept = getConcept(conceptId);
    if (!concept) return;
    
    if (pImpl->conceptDriftHistory.find(conceptId) == pImpl->conceptDriftHistory.end()) {
        pImpl->conceptDriftHistory[conceptId] = 0.0f;
    }
    
    // Update drift history with exponential moving average
    pImpl->conceptDriftHistory[conceptId] = 
        pImpl->conceptDriftHistory[conceptId] * 0.9f + 
        (1.0f - newSimilarity) * 0.1f;
    
    concept->driftRate = pImpl->conceptDriftHistory[conceptId];
    
    // Adapt if drift exceeds threshold
    if (concept->driftRate > pImpl->driftDetectionThreshold_) {
        // Increase learning rate for adaptation
        float alpha = 0.3f;  // Higher learning rate for adaptation
        
        // Update prototype gradually
        std::vector<float> targetPattern = predictConceptPattern(conceptId);
        if (!targetPattern.empty() && concept->prototype.size() == targetPattern.size()) {
            for (size_t i = 0; i < concept->prototype.size(); ++i) {
                concept->prototype[i] = concept->prototype[i] * (1.0f - alpha) + 
                                      targetPattern[i] * alpha;
            }
        }
        
        NLM_LOG_INFO("ConceptFormation: Concept " + std::to_string(conceptId) +
                     " adapted due to drift (rate: " + std::to_string(concept->driftRate) + ")");
    }
}

void ConceptFormation::learnConceptCoOccurrences(const std::vector<float>& pattern1,
                                                 const std::vector<float>& pattern2) {
    // Learn co-occurrence relationships between patterns
    std::vector<size_t> concepts1 = findSimilarConcepts(pattern1);
    std::vector<size_t> concepts2 = findSimilarConcepts(pattern2);
    
    // Create associations between concepts
    for (size_t id1 : concepts1) {
        for (size_t id2 : concepts2) {
            if (id1 != id2) {
                pImpl->conceptAssociations[id1][id2] += 0.1f;
                pImpl->conceptAssociations[id2][id1] += 0.1f;
            }
        }
    }
    
    // Update concept associations in discovered concepts
    for (auto& concept : concepts_) {
        for (auto& assoc : pImpl->conceptAssociations[concept.id]) {
            concept.associatedConceptIds.push_back(assoc.first);
        }
    }
}

void ConceptFormation::reinforceConcepts(const std::vector<float>& pattern, float reward) {
    // Reinforce concepts based on received reward
    size_t matchingConcept = findConceptForPattern(pattern);
    
    if (matchingConcept > 0) {
        auto* concept = getConcept(matchingConcept);
        if (concept) {
            // Update confidence based on reward
            if (reward > 0.0f) {
                concept->confidence = std::min(1.0f, concept->confidence + 0.1f);
                concept->rewardHistory += reward;
                
                // Increase importance weight
                for (auto& instance : concept->instances) {
                    if (&instance == &concept->instances.back()) {
                        instance.weight = std::min(2.0f, instance.weight * 1.1f);
                    }
                }
            } else if (reward < 0.0f) {
                concept->confidence = std::max(0.0f, concept->confidence - 0.1f);
                
                // Decrease importance weight
                for (auto& instance : concept->instances) {
                    if (&instance == &concept->instances.back()) {
                        instance.weight = std::max(0.1f, instance.weight * 0.9f);
                    }
                }
            }
        }
    }
}

void ConceptFormation::updateFeatureImportance(const std::vector<float>& features) {
    // Update feature importance weights based on usage
    updateFeatureImportanceWeights(features);
    
    // Store in history
    if (features.size() == lastFeatureVector_.size()) {
        for (size_t i = 0; i < features.size(); ++i) {
            lastFeatureVector_[i] = features[i];
        }
    } else {
        lastFeatureVector_ = features;
        featureVectorHistory_.push_back(lastFeatureVector_);
        
        // Keep history manageable
        if (featureVectorHistory_.size() > pImpl->featureHistorySize_) {
            featureVectorHistory_.erase(featureVectorHistory_.begin());
        }
    }
}

void ConceptFormation::bindFeaturesToConcepts(const std::vector<float>& pattern,
                                              const std::vector<size_t>& featureIndices) {
    // Bind features to concepts based on neural pattern matching
    size_t matchingConcept = findConceptForPattern(pattern);
    
    if (matchingConcept > 0) {
        auto* concept = getConcept(matchingConcept);
        if (concept) {
            // Clear existing bindings
            concept->boundFeatures.clear();
            
            // Bind matching features
            for (size_t idx : featureIndices) {
                if (idx < pattern.size()) {
                    float featureValue = pattern[idx];
                    
                    // Check if feature is significant
                    if (std::abs(featureValue) > 0.1f) {
                        concept->boundFeatures.push_back(idx);
                    }
                }
            }
            
            // Update neural pattern based on bound features
            concept->neuralPattern = normalizeAndCompressFeatures(pattern);
        }
    }
}

void ConceptFormation::updateConceptConfidence(size_t conceptId, float error) {
    // Update concept confidence based on prediction error
    auto* concept = getConcept(conceptId);
    if (!concept) return;
    
    pImpl->conceptConfidence[conceptId] = 1.0f - std::min(1.0f, error * 2.0f);
    
    // Update concept-level metrics
    concept->predictionError = error;
    concept->confidence = pImpl->conceptConfidence[conceptId];
    
    // Decay if error is high
    if (error > pImpl->predictionErrorThreshold_) {
        concept->decayTimer += 1.0f;
    }
}

void ConceptFormation::updateConceptUncertainty(size_t conceptId, float novelty) {
    // Update concept uncertainty based on novelty
    auto* concept = getConcept(conceptId);
    if (!concept) return;
    
    concept->uncertainty = std::min(1.0f, novelty * 2.0f);
    concept->noveltySum += novelty;
}

void ConceptFormation::decayConcepts() {
    // Decay concept activation and stability over time
    for (auto& concept : concepts_) {
        // Decay activation
        concept.decayTimer += 1.0f;
        
        // Apply decay factor
        float decayFactor = std::exp(-pImpl->conceptDecayRate_ * concept.decayTimer);
        concept.avgStability *= decayFactor;
        
        // Decay confidence
        concept.confidence *= decayFactor;
        
        // Remove instances with low stability
        std::vector<ConceptInstance> stableInstances;
        for (const auto& instance : concept.instances) {
            if (instance.stability > 0.3f) {
                stableInstances.push_back(instance);
            }
        }
        
        concept.instances = stableInstances;
        concept.totalObservations = concept.instances.size();
    }
}

void ConceptFormation::consolidateConcepts() {
    // Consolidate similar concepts
    consolidateConceptsOverTime();
    
    // Merge concepts with high similarity
    for (size_t i = 0; i < concepts_.size(); ++i) {
        for (size_t j = i + 1; j < concepts_.size(); ++j) {
            float similarity = computeSimilarity(
                concepts_[i].prototype,
                concepts_[j].prototype
            );
            
            if (similarity > formationThreshold_ * 1.2f) {
                mergeConcepts(concepts_[i].id, concepts_[j].id);
                
                // Update indices after merge
                if (j < concepts_.size()) {
                    std::swap(concepts_[i], concepts_[j]);
                }
                break;
            }
        }
    }
}

void ConceptFormation::integrateWithEpisodicMemory(size_t conceptId, const std::vector<float>& experience) {
    // Integrate with episodic memory
    auto* concept = getConcept(conceptId);
    if (!concept) return;
    
    // Update concept based on episodic experience
    float similarity = computeSimilarity(experience, concept->prototype);
    
    if (similarity > formationThreshold_) {
        // Enhance concept with episodic details
        for (size_t i = 0; i < experience.size() && i < concept->prototype.size(); ++i) {
            concept->prototype[i] = concept->prototype[i] * 0.7f + experience[i] * 0.3f;
        }
        
        // Add new instance
        ConceptInstance instance;
        instance.pattern = experience;
        instance.features = extractMultiScaleFeatures(experience).back();
        instance.observationCount = 1;
        instance.lastObserved = concept->totalObservations;
        instance.avgReward = 0.5f; // Default reward from episodic memory
        instance.stability = similarity;
        
        concept->instances.push_back(instance);
        concept->totalObservations++;
    }
}

void ConceptFormation::integrateWithWorkingMemory(size_t conceptId, float attention) {
    // Integrate with working memory
    auto* concept = getConcept(conceptId);
    if (!concept) return;
    
    pImpl->activeConcepts_.insert(conceptId);
    pImpl->attentionWeights_[conceptId] = attention;
    
    // Update concept metrics based on attention
    concept->confidence = std::min(1.0f, concept->confidence + attention * 0.1f);
    concept->abstractionLevel = std::min(1.0f, concept->abstractionLevel + attention * 0.05f);
}

void ConceptFormation::integrateWithNeuromodulation(size_t conceptId, float reward) {
    // Integrate with neuromodulation
    auto* concept = getConcept(conceptId);
    if (!concept) return;
    
    // Update concept based on neuromodulatory signals
    if (reward > 0.0f) {
        concept->confidence = std::min(1.0f, concept->confidence + 0.2f);
        concept->rewardHistory += reward;
        
        // Strengthen connections
        for (auto& instance : concept->instances) {
            instance.weight = std::min(2.0f, instance.weight * 1.2f);
        }
    } else {
        concept->confidence = std::max(0.0f, concept->confidence - 0.1f);
        
        // Weaken connections
        for (auto& instance : concept->instances) {
            instance.weight = std::max(0.1f, instance.weight * 0.8f);
        }
    }
}

void ConceptFormation::integrateWithAttention(size_t conceptId, float attentionWeight) {
    // Integrate with attention system
    pImpl->attentionWeights_[conceptId] = attentionWeight;
    
    // Boost concept that receives attention
    auto* concept = getConcept(conceptId);
    if (concept) {
        concept->confidence = std::min(1.0f, concept->confidence + attentionWeight * 0.15f);
        concept->abstractionLevel = std::min(1.0f, concept->abstractionLevel + attentionWeight * 0.08f);
    }
}

size_t ConceptFormation::composeConcepts(size_t conceptA, size_t conceptB) {
    // Compose two concepts into a new composite concept
    auto* a = getConcept(conceptA);
    auto* b = getConcept(conceptB);
    
    if (!a || !b) return 0;
    
    DiscoveredConcept composite;
    composite.id = nextConceptId_++;
    composite.conceptType = "composite";
    composite.parentConceptId = 0;  // Will be set during hierarchy organization
    
    // Compose prototypes
    if (a->prototype.size() == b->prototype.size()) {
        composite.prototype.resize(a->prototype.size());
        for (size_t i = 0; i < a->prototype.size(); ++i) {
            composite.prototype[i] = (a->prototype[i] + b->prototype[i]) * 0.5f;
        }
    } else {
        composite.prototype = a->prototype;  // Use larger one
    }
    
    // Add instances from both concepts
    for (const auto& instance : a->instances) {
        composite.instances.push_back(instance);
    }
    for (const auto& instance : b->instances) {
        composite.instances.push_back(instance);
    }
    
    composite.totalObservations = composite.instances.size();
    composite.avgStability = (a->avgStability + b->avgStability) * 0.5f;
    composite.associatedConceptIds = {conceptA, conceptB};
    
    // Determine category
    if (a->categoryHint == b->categoryHint) {
        composite.categoryHint = a->categoryHint;
    } else {
        composite.categoryHint = "mixed";
    }
    
    concepts_.push_back(composite);
    
    NLM_LOG_INFO("ConceptFormation: Composed concepts " + std::to_string(conceptA) +
                 " and " + std::to_string(conceptB) +
                 " into new concept " + std::to_string(composite.id));
    
    return composite.id;
}

size_t ConceptFormation::generalizeConcept(size_t conceptId, const std::vector<float>& novelPattern) {
    // Generalize a concept to handle novel patterns
    auto* concept = getConcept(conceptId);
    if (!concept) return 0;
    
    // Create generalized version
    DiscoveredConcept generalized = *concept;
    generalized.id = nextConceptId_++;
    generalized.conceptType = "generalized";
    generalized.parentConceptId = conceptId;
    
    // Increase abstraction level
    generalized.abstractionLevel = std::min(1.0f, concept->abstractionLevel + 0.2f);
    
    // Update prototype to be more general
    std::vector<float> generalizedProto = concept->prototype;
    for (size_t i = 0; i < generalizedProto.size(); ++i) {
        // Add noise to make it more general
        generalizedProto[i] += (pImpl->dist(pImpl->rng) - 0.5f) * 0.1f;
    }
    generalized.prototype = generalizedProto;
    
    // Add the novel pattern as instance
    ConceptInstance instance;
    instance.pattern = novelPattern;
    instance.features = extractMultiScaleFeatures(novelPattern).back();
    instance.observationCount = 1;
    instance.lastObserved = concept->totalObservations;
    instance.avgReward = 0.5f;
    instance.stability = computeSimilarity(novelPattern, generalizedProto);
    
    generalized.instances.push_back(instance);
    generalized.totalObservations++;
    
    concepts_.push_back(generalized);
    
    NLM_LOG_INFO("ConceptFormation: Generalized concept " + std::to_string(conceptId) +
                 " into new concept " + std::to_string(generalized.id));
    
    return generalized.id;
}

std::vector<size_t> ConceptFormation::findSimilarConcepts(const std::vector<float>& pattern,
                                                        float similarityThreshold) const {
    // Find all concepts similar to pattern
    std::vector<size_t> similarConcepts;
    
    for (const auto& concept : concepts_) {
        float similarity = computeSimilarity(pattern, concept.prototype);
        if (similarity >= similarityThreshold) {
            similarConcepts.push_back(concept.id);
        }
    }
    
    return similarConcepts;
}

void ConceptFormation::updateFeatureImportanceWeights(const std::vector<float>& features) {
    // Update feature importance weights based on feature values
    pImpl->featureImportanceWeights_[++pImpl->featureVectorHistory_.size()] = features;
    
    // Calculate importance based on magnitude and frequency
    if (pImpl->featureImportanceWeights_.size() > 1) {
        auto it = pImpl->featureImportanceWeights_.end();
        --it;
        auto prevIt = std::prev(it);
        
        std::vector<float> importance(features.size(), 0.0f);
        for (size_t i = 0; i < features.size(); ++i) {
            float magnitude = std::abs(features[i]);
            importance[i] = magnitude * pImpl->featureNormalizationFactor_;
            
            if (prevIt->second.size() > i) {
                float prevMagnitude = std::abs(prevIt->second[i]);
                importance[i] += prevMagnitude * 0.5f;
            }
        }
        
        // Store normalized importance
        for (size_t i = 0; i < features.size(); ++i) {
            if (!features.empty()) {
                float maxVal = *std::max_element(features.begin(), features.end());
                if (maxVal > 0.0f) {
                    pImpl->featureImportanceWeights_[it->first][i] = importance[i] / maxVal;
                }
            }
        }
    }
}

std::vector<size_t> ConceptFormation::bindFeaturesWithNeuralPattern(const std::vector<float>& pattern,
                                                                    const std::vector<float>& prototype) const {
    // Bind features with neural pattern
    std::vector<size_t> boundFeatures;
    
    if (pattern.size() != prototype.size()) return boundFeatures;
    
    for (size_t i = 0; i < pattern.size(); ++i) {
        // Calculate feature binding strength
        float bindingStrength = computeSimilarity(std::vector<float>{pattern[i]},
                                                std::vector<float>{prototype[i]});
        
        if (bindingStrength > 0.5f) {
            boundFeatures.push_back(i);
        }
    }
    
    return boundFeatures;
}

void ConceptFormation::updateMetaCognitiveMetrics(size_t conceptId, const std::vector<float>& pattern) {
    // Update meta-cognitive metrics for a concept
    auto* concept = getConcept(conceptId);
    if (!concept) return;
    
    // Update prediction error
    updateConceptPredictionError(conceptId, pattern);
    
    // Update novelty score
    float novelty = computeNovelty(pattern, concept->prototype);
    concept->noveltyScore = novelty;
    
    // Update confidence
    float error = concept->predictionError;
    updateConceptConfidence(conceptId, error);
    
    // Update uncertainty
    updateConceptUncertainty(conceptId, novelty);
}

void ConceptFormation::updateConceptPredictionError(size_t conceptId, const std::vector<float>& pattern) {
    // Update concept prediction error
    auto* concept = getConcept(conceptId);
    if (!concept) return;
    
    std::vector<float> predicted = predictConceptPattern(conceptId);
    if (predicted.empty()) return;
    
    float error = 0.0f;
    for (size_t i = 0; i < pattern.size() && i < predicted.size(); ++i) {
        error += std::abs(pattern[i] - predicted[i]);
    }
    
    if (!pattern.empty()) {
        error /= pattern.size();
    }
    
    pImpl->predictionErrors[conceptId] = error;
    concept->predictionError = error;
}

void ConceptFormation::updateEpisodicIntegration(size_t conceptId, const std::vector<float>& experience) {
    // Update episodic memory integration
    integrateWithEpisodicMemory(conceptId, experience);
}

void ConceptFormation::updateWorkingMemoryIntegration(size_t conceptId, float attention) {
    // Update working memory integration
    integrateWithWorkingMemory(conceptId, attention);
}

void ConceptFormation::updateNeuromodulationIntegration(size_t conceptId, float reward) {
    // Update neuromodulation integration
    integrateWithNeuromodulation(conceptId, reward);
}

float ConceptFormation::computeAnomalousSimilarity(const std::vector<float>& pattern,
                                                 size_t conceptId) const {
    // Compute similarity with anomaly detection
    auto* concept = getConcept(conceptId);
    if (!concept) return 0.0f;
    
    float similarity = computeSimilarity(pattern, concept->prototype);
    
    // Adjust for concept age and stability
    float ageFactor = std::min(1.0f, concept->totalObservations / 100.0f);
    float stabilityFactor = concept->avgStability;
    
    return similarity * ageFactor * stabilityFactor;
}

std::vector<float> ConceptFormation::predictConceptPattern(size_t conceptId) const {
    // Predict concept pattern based on instances
    auto* concept = getConcept(conceptId);
    if (!concept || concept->instances.empty()) return {};
    
    // Use weighted average of instances
    std::vector<float> predicted(concept->prototype.size(), 0.0f);
    float totalWeight = 0.0f;
    
    for (const auto& instance : concept->instances) {
        float weight = instance.weight * instance.stability;
        totalWeight += weight;
        
        for (size_t i = 0; i < instance.pattern.size() && i < predicted.size(); ++i) {
            predicted[i] += instance.pattern[i] * weight;
        }
    }
    
    if (totalWeight > 0.0f) {
        for (size_t i = 0; i < predicted.size(); ++i) {
            predicted[i] /= totalWeight;
        }
    }
    
    return predicted;
}

std::vector<size_t> ConceptFormation::getConceptAssociations(size_t conceptId) const {
    // Get concept associations
    std::vector<size_t> associations;
    
    if (pImpl->conceptAssociations.find(conceptId) != pImpl->conceptAssociations.end()) {
        associations.reserve(pImpl->conceptAssociations.at(conceptId).size());
        for (const auto& assoc : pImpl->conceptAssociations.at(conceptId)) {
            associations.push_back(assoc.first);
        }
    }
    
    return associations;
}

float ConceptFormation::patternAverageAnomaly(const std::vector<float>& pattern) const {
    // Calculate average anomaly score across all concepts
    if (concepts_.empty()) return 0.0f;
    
    float totalAnomaly = 0.0f;
    
    for (const auto& concept : concepts_) {
        std::vector<float> predicted = predictConceptPattern(concept.id);
        if (predicted.empty()) continue;
        
        float dot = 0.0f, normP = 0.0f, normA = 0.0f;
        for (size_t i = 0; i < pattern.size() && i < predicted.size(); ++i) {
            dot += pattern[i] * predicted[i];
            normP += pattern[i] * pattern[i];
            normA += predicted[i] * predicted[i];
        }
        
        if (normP > 0.0001f && normA > 0.0001f) {
            float similarity = dot / (std::sqrt(normP) * std::sqrt(normA));
            totalAnomaly += 1.0f - similarity;
        }
    }
    
    return totalAnomaly / concepts_.size();
}

float ConceptFormation::computeNovelty(const std::vector<float>& pattern,
                                     const std::vector<float>& prototype) const {
    // Compute novelty score
    float similarity = computeSimilarity(pattern, prototype);
    return std::max(0.0f, 1.0f - similarity);
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
    
    // Check if we have a place cell near this position
    bool hasNearbyCell = false;
    for (const auto& pc : placeCells_) {
        float dx = pc.x - x;
        float dy = pc.y - y;
        if (std::sqrt(dx * dx + dy * dy) < pc.fieldRadius) {
            hasNearbyCell = true;
            break;
        }
    }
    
    // Create new place cell if needed
    if (!hasNearbyCell && brain_) {
        PlaceCell newCell;
        newCell.neuron = NeuronId(placeCells_.size() + 40000);
        newCell.x = x;
        newCell.y = y;
        newCell.fieldRadius = 0.5f;  // Place field size
        newCell.activation = 1.0f;
        
        placeCells_.push_back(newCell);
        
        // Inject current to activate this neuron
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
        
        // Gaussian place field
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
    // Find existing link or create new
    for (auto& link : sequences_) {
        float simA = 0.0f, simB = 0.0f;
        
        // Compute similarity
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
            // Update existing link
            link.strength = std::min(1.0f, link.strength + 0.1f);
            link.observationCount++;
            return;
        }
    }
    
    // Create new link
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
