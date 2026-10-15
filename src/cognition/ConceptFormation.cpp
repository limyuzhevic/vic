#include "ConceptFormation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>
#include <limits>
#include <memory>
#include <deque>

namespace nlm {

// Helper function for combined distance and correlation metrics
static float combinedPatternDistance(const std::vector<float>& a,
                                     const std::vector<float>& b) {
    if (a.size() != b.size() || a.empty()) return 1.0f;
    
    // Euclidean distance normalized by vector length
    float euclideanDist = 0.0f;
    float normA = 0.0f, normB = 0.0f;
    
    for (size_t i = 0; i < a.size(); ++i) {
        float diff = a[i] - b[i];
        euclideanDist += diff * diff;
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 1.0f;
    
    float normalizedEuclidean = euclideanDist / (normA + normB);
    
    // Cosine similarity
    float dot = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
    }
    float cosineSim = dot / (std::sqrt(normA) * std::sqrt(normB));
    
    // Combine: distance (lower is better) + correlation (higher is better)
    // Normalize both to [0,1] range and combine
    float correlationDistance = 1.0f - cosineSim;
    return (normalizedEuclidean * 0.6f + correlationDistance * 0.4f);
}

// Helper for dynamic threshold calculation
static float calculateAdaptiveThreshold(const std::vector<nlm::DiscoveredConcept>& concepts,
                                        float baseThreshold,
                                        float memoryDensity) {
    if (concepts.empty()) return baseThreshold;
    
    // Consider concept stability distribution
    float stabilitySum = 0.0f;
    float stabilityCount = 0.0f;
    
    for (const auto& concept : concepts) {
        stabilitySum += concept.avgStability;
        stabilityCount += 1.0f;
    }
    
    float avgStability = stabilitySum / stabilityCount;
    
    // Consider memory density (normalized by max reasonable size)
    float densityFactor = std::min(1.0f, memoryDensity / 1000.0f);
    
    // Adjust threshold based on stability distribution
    float stabilityFactor = avgStability * 0.5f;
    
    // Adjust based on memory density
    float densityFactor2 = densityFactor * 0.3f;
    
    // Adaptive threshold: more stable concepts = lower threshold
    // More memory density = higher threshold (more selective)
    float adaptive = baseThreshold * (1.0f - stabilityFactor + densityFactor2);
    
    // Clamp to reasonable range
    return std::max(0.1f, std::min(0.95f, adaptive));
}

struct ConceptFormation::Impl {
    Brain* brain;
    
    // Multi-resolution cache for approximate matching
    std::unordered_map<size_t, std::vector<float>> resolutionCache;
    
    // Temporal novelty tracking
    std::unordered_map<size_t, float> temporalNovelty;
    std::unordered_map<size_t, SimulationStep> lastObservationTime;
    
    // Brain state for contextual novelty detection
    std::vector<float> currentBrainState;
    
    // Statistical novelty tracking
    struct NoveltyStats {
        float meanPatternEnergy;
        float stddevPatternEnergy;
        float meanTemporalGaps;
        float stddevTemporalGaps;
        size_t observationCount;
    } noveltyStats;
    
    // Instance lifecycle management
    std::unordered_map<size_t, std::deque<ConceptInstance>> instanceHistory;
    size_t maxInstancesPerConcept;
    float instanceAgingDecay;
    
    Impl() : brain(nullptr), maxInstancesPerConcept(100), instanceAgingDecay(0.95f) {
        noveltyStats.meanPatternEnergy = 0.0f;
        noveltyStats.stddevPatternEnergy = 1.0f;
        noveltyStats.meanTemporalGaps = 1.0f;
        noveltyStats.stddevTemporalGaps = 0.5f;
        noveltyStats.observationCount = 0;
    }
};

ConceptFormation::ConceptFormation()
    : pImpl(new Impl)
    , brain_(nullptr)
    , nextConceptId_(1)
    , formationThreshold_(0.75f)
    , stabilityThreshold_(0.7f)
    , stabilityWindow_(5)
    , memoryCapacity_(1000)
    , minObservationsForStability_(3)
    , conceptPruningThreshold_(0.95f)
{
}

ConceptFormation::~ConceptFormation() = default;

void ConceptFormation::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    pImpl->currentBrainState.resize(64, 0.0f);  // Approximate brain state dimension
    NLM_LOG_INFO("ConceptFormation initialized with advanced metrics");
}

size_t ConceptFormation::presentExperience(const std::vector<float>& pattern,
                                          const std::vector<float>& features,
                                          float reward,
                                          SimulationStep currentTime) {
    if (pattern.empty()) return 0;
    
    // Calculate temporal novelty (time since last observation of similar patterns)
    float temporalNovelty = calculateTemporalNovelty(pattern, currentTime);
    
    // Multi-resolution matching: first coarse, then precise
    size_t matchingConcept = findMatchingConceptMultiResolution(pattern, temporalNovelty);
    
    if (matchingConcept > 0) {
        // Update existing concept with novel-aware updates
        updateConceptEnhanced(matchingConcept, pattern, features, reward, 
                             temporalNovelty, currentTime);
        return matchingConcept;
    }
    
    // Check novelty using comprehensive metrics
    if (isNovelComprehensive(pattern, features, reward, currentTime)) {
        return createConceptEnhanced(pattern, features, reward, currentTime);
    }
    
    return 0;  // Not yet classifiable
}

size_t ConceptFormation::createConceptEnhanced(const std::vector<float>& pattern,
                                              const std::vector<float>& features,
                                              float reward,
                                              SimulationStep currentTime) {
    DiscoveredConcept concept;
    concept.id = nextConceptId_++;
    concept.prototype = pattern;
    concept.totalObservations = 1;
    
    // Enhanced category hint: multi-dimensional representation
    concept.categoryHint = generateMultiDimensionalCategory(pattern, features, reward);
    concept.categoryVector = extractFeatureVector(pattern, features);
    concept.categoryEntropy = calculateCategoryEntropy(pattern, features);
    
    // Enhanced instance with lifecycle management
    ConceptInstance instance;
    instance.pattern = pattern;
    instance.features = features;
    instance.observationCount = 1;
    instance.lastObserved = currentTime;
    instance.avgReward = reward;
    instance.stability = 1.0f;
    instance.age = 0;
    instance.priority = calculateInstancePriority(concept.categoryVector, reward);
    
    concept.instances.push_back(instance);
    
    // Store in history for lifecycle management
    pImpl->instanceHistory[concept.id].push_back(instance);
    if (pImpl->instanceHistory[concept.id].size() > pImpl->maxInstancesPerConcept) {
        pImpl->instanceHistory[concept.id].pop_front();
    }
    
    concept.avgStability = 1.0f;
    concept.associatedConceptIds = {};  // Initially empty
    concepts_.push_back(concept);
    
    // Update novelty statistics
    updateNoveltyStatistics(pattern, currentTime);
    
    NLM_LOG_INFO("ConceptFormation: Created enhanced concept " + std::to_string(concept.id) +
                 " (category: " + concept.categoryHint + ")");
    
    return concept.id;
}

void ConceptFormation::updateConceptEnhanced(size_t conceptId,
                                            const std::vector<float>& newPattern,
                                            const std::vector<float>& features,
                                            float reward,
                                            float temporalNovelty,
                                            SimulationStep currentTime) {
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (!concept) return;
    
    // Add new instance with enhanced tracking
    ConceptInstance instance;
    instance.pattern = newPattern;
    instance.features = features;
    instance.observationCount = 1;
    instance.lastObserved = concept->totalObservations;
    instance.avgReward = reward;
    
    // Enhanced stability calculation using combined metrics
    instance.stability = calculateEnhancedStability(*concept, newPattern, temporalNovelty);
    instance.age = 0;
    instance.priority = calculateInstancePriority(
        extractFeatureVector(newPattern, features), reward);
    
    concept->instances.push_back(instance);
    concept->totalObservations++;
    
    // Update instance history
    pImpl->instanceHistory[conceptId].push_back(instance);
    if (pImpl->instanceHistory[conceptId].size() > pImpl->maxInstancesPerConcept) {
        pImpl->instanceHistory[conceptId].pop_front();
    }
    
    // Update prototype with adaptive learning rate
    updatePrototypeAdaptive(conceptId, newPattern, features, temporalNovelty);
    
    // Update concept stability
    float newStability = calculateConceptStability(*concept);
    concept->avgStability = concept->avgStability * 0.9f + newStability * 0.1f;
    concept->instances.back().stability = newStability;
    
    // Update category hint evolution
    updateCategoryEvolution(conceptId, newPattern, features, reward);
}

void ConceptFormation::updatePrototypeAdaptive(size_t conceptId,
                                              const std::vector<float>& newInstance,
                                              const std::vector<float>& features,
                                              float temporalNovelty) {
    for (auto& concept : concepts_) {
        if (concept.id == conceptId) {
            // Adaptive learning rate based on stability and novelty
            float stabilityFactor = concept.avgStability;
            float noveltyFactor = 1.0f - std::min(1.0f, temporalNovelty);
            
            // Base learning rate with biological plausibility (closer to 0.1 for stability)
            float baseAlpha = 0.15f;
            
            // Adjust for stability: slower updates for stable concepts
            float stabilityAdjustedAlpha = baseAlpha * stabilityFactor * 0.5f;
            
            // Adjust for novelty: faster updates for novel instances
            float noveltyAdjustedAlpha = stabilityAdjustedAlpha * (0.5f + noveltyFactor * 0.5f);
            
            // Multi-modal update: pattern and feature integration
            if (concept.prototype.size() == newInstance.size() && 
                !features.empty() && concept.categoryVector.size() == features.size()) {
                for (size_t i = 0; i < concept.prototype.size(); ++i) {
                    // Pattern update
                    float patternUpdate = concept.prototype[i] * (1.0f - noveltyAdjustedAlpha) + 
                                        newInstance[i] * noveltyAdjustedAlpha;
                    
                    // Feature-based update
                    if (i < features.size() && i < concept.categoryVector.size()) {
                        float featureInfluence = calculateFeatureInfluence(features[i], concept.categoryVector[i]);
                        patternUpdate = patternUpdate * (1.0f - featureInfluence * 0.1f) + 
                                     newInstance[i] * featureInfluence * 0.1f;
                    }
                    
                    concept.prototype[i] = patternUpdate;
                }
            }
            
            // Also update category vector
            if (!features.empty() && concept.categoryVector.size() == features.size()) {
                float alpha = noveltyAdjustedAlpha * 0.3f;
                for (size_t i = 0; i < features.size(); ++i) {
                    concept.categoryVector[i] = concept.categoryVector[i] * (1.0f - alpha) + 
                                               features[i] * alpha;
                }
            }
            
            break;
        }
    }
}

float ConceptFormation::calculateEnhancedStability(const DiscoveredConcept& concept,
                                                  const std::vector<float>& newPattern,
                                                  float temporalNovelty) const {
    if (concept.instances.size() < 2) return 1.0f;
    
    // Multi-factor stability calculation
    float stability = 0.0f;
    
    // 1. Pattern consistency (normalized distance to prototype)
    float patternConsistency = 0.0f;
    size_t recentCount = std::min(concept.instances.size(), stabilityWindow_);
    
    for (size_t i = concept.instances.size() - recentCount; 
         i < concept.instances.size(); ++i) {
        float distance = combinedPatternDistance(
            concept.instances[i].pattern, concept.prototype);
        patternConsistency += (1.0f - distance);
    }
    patternConsistency /= recentCount;
    
    // 2. Temporal consistency (based on observation intervals)
    float temporalConsistency = 1.0f - std::min(1.0f, temporalNovelty);
    
    // 3. Reward consistency
    float rewardConsistency = 0.0f;
    float totalReward = 0.0f;
    for (const auto& instance : concept.instances) {
        totalReward += instance.avgReward;
    }
    float avgReward = totalReward / concept.instances.size();
    
    for (const auto& instance : concept.instances) {
        rewardConsistency += std::abs(instance.avgReward - avgReward);
    }
    rewardConsistency = 1.0f - std::min(1.0f, rewardConsistency / concept.instances.size());
    
    // 4. Novelty penalty (recent novel instances reduce stability)
    float noveltyPenalty = std::min(1.0f, temporalNovelty * 0.5f);
    
    // Weighted combination (pattern consistency most important)
    stability = patternConsistency * 0.4f + 
                temporalConsistency * 0.3f + 
                rewardConsistency * 0.2f;
    
    stability = stability * (1.0f - noveltyPenalty * 0.1f);
    
    return std::max(0.0f, std::min(1.0f, stability));
}

float ConceptFormation::calculateConceptStability(const DiscoveredConcept& concept) const {
    if (concept.instances.size() < 2) return 1.0f;
    
    // Generalized stability calculation using combined metrics
    float totalSimilarity = 0.0f;
    size_t count = 0;
    
    for (const auto& instance : concept.instances) {
        float similarity = 1.0f - combinedPatternDistance(instance.pattern, concept.prototype);
        totalSimilarity += similarity;
        count++;
    }
    
    float avgSimilarity = totalSimilarity / count;
    
    // Decay factor for older instances
    float decayFactor = 1.0f;
    if (concept.instances.size() > stabilityWindow_) {
        decayFactor = 0.8f;
    }
    
    return avgSimilarity * decayFactor;
}

float ConceptFormation::calculateTemporalNovelty(const std::vector<float>& pattern,
                                               SimulationStep currentTime) const {
    // Calculate based on time since last similar observation
    float bestSimilarity = 1.0f;
    
    for (const auto& concept : concepts_) {
        for (const auto& instance : concept.instances) {
            float similarity = 1.0f - combinedPatternDistance(pattern, instance.pattern);
            if (similarity > bestSimilarity) {
                bestSimilarity = similarity;
            }
        }
    }
    
    if (bestSimilarity > 0.9f) return 0.0f;  // Very similar to something seen recently
    
    // Temporal gap calculation (simplified)
    float gap = 1.0f;  // Default gap when no previous observations
    
    if (!pImpl->lastObservationTime.empty()) {
        auto minTimeIter = pImpl->lastObservationTime.begin();
        for (const auto& pair : pImpl->lastObservationTime) {
            if (pair.second < minTimeIter->second) {
                minTimeIter = pair.begin();
            }
        }
        gap = static_cast<float>(currentTime - minTimeIter->second);
    }
    
    // Novelty based on temporal gap (logarithmic scaling)
    float novelty = std::min(1.0f, std::log10(gap + 1.0f) / 3.0f);
    
    return novelty;
}

size_t ConceptFormation::findMatchingConceptMultiResolution(const std::vector<float>& pattern,
                                                          float temporalNovelty) {
    if (concepts_.empty()) return 0;
    
    size_t bestConcept = 0;
    float bestScore = formationThreshold_;
    
    // Multi-resolution approach
    std::vector<size_t> candidateConcepts;
    
    // Level 1: Coarse matching (downsampled pattern)
    std::vector<float> coarsePattern = downsamplePattern(pattern, 4);
    for (const auto& concept : concepts_) {
        std::vector<float> coarsePrototype = downsamplePattern(concept.prototype, 4);
        float sim = 1.0f - combinedPatternDistance(coarsePattern, coarsePrototype);
        if (sim > 0.6f) {  // Lower threshold for coarse matching
            candidateConcepts.push_back(concept.id);
        }
    }
    
    // Level 2: Precise matching only on coarse candidates
    for (size_t conceptId : candidateConcepts) {
        const DiscoveredConcept* concept = getConcept(conceptId);
        if (!concept) continue;
        
        float similarity = 1.0f - combinedPatternDistance(pattern, concept->prototype);
        
        // Adjust score based on temporal novelty
        float adjustedScore = similarity * (1.0f - temporalNovelty * 0.3f);
        
        if (adjustedScore > bestScore) {
            bestScore = adjustedScore;
            bestConcept = conceptId;
        }
    }
    
    return bestConcept;
}

bool ConceptFormation::isNovelComprehensive(const std::vector<float>& pattern,
                                          const std::vector<float>& features,
                                          float reward,
                                          SimulationStep currentTime) const {
    // Statistical novelty detection
    float patternEnergy = calculatePatternEnergy(pattern);
    bool isOutlier = isPatternOutlier(patternEnergy);
    
    // Contextual novelty (considering brain state)
    float contextualNovelty = calculateContextualNovelty(pattern);
    
    // Temporal novelty (already calculated in presentExperience)
    // For simplicity, re-calculate
    float temporalNovelty = calculateTemporalNovelty(pattern, currentTime);
    
    // Prediction error based novelty
    float predictionError = calculatePredictionError(pattern);
    
    // Combined novelty score
    float noveltyScore = (isOutlier ? 0.4f : 0.0f) +
                        (contextualNovelty * 0.3f) +
                        (temporalNovelty * 0.2f) +
                        (predictionError * 0.1f);
    
    // Adaptive threshold based on concept stability and memory density
    float memoryDensity = static_cast<float>(concepts_.size()) / memoryCapacity_;
    float adaptiveThreshold = calculateAdaptiveThreshold(concepts_, formationThreshold_, memoryDensity);
    
    return noveltyScore > adaptiveThreshold;
}

void ConceptFormation::consolidateInstances(size_t conceptId) {
    auto it = pImpl->instanceHistory.find(conceptId);
    if (it == pImpl->instanceHistory.end()) return;
    
    auto& history = it->second;
    
    // Sort instances by priority (higher first)
    std::sort(history.begin(), history.end(),
              [](const ConceptInstance& a, const ConceptInstance& b) {
                  return a.priority > b.priority;
              });
    
    // Keep only top instances up to max size
    if (history.size() > pImpl->maxInstancesPerConcept) {
        history.resize(pImpl->maxInstancesPerConcept);
    }
    
    // Consolidate similar instances
    std::vector<ConceptInstance> consolidated;
    float consolidationThreshold = 0.85f;
    
    for (const auto& instance : history) {
        bool merged = false;
        for (auto& consolidatedInst : consolidated) {
            float similarity = 1.0f - combinedPatternDistance(
                instance.pattern, consolidatedInst.pattern);
            
            if (similarity > consolidationThreshold) {
                // Merge instances
                consolidatedInst.observationCount += instance.observationCount;
                consolidatedInst.avgReward = (consolidatedInst.avgReward + instance.avgReward) * 0.5f;
                consolidatedInst.stability = (consolidatedInst.stability + instance.stability) * 0.5f;
                consolidatedInst.age = std::max(consolidatedInst.age, instance.age);
                consolidatedInst.priority = (consolidatedInst.priority + instance.priority) * 0.5f;
                merged = true;
                break;
            }
        }
        
        if (!merged) {
            consolidated.push_back(instance);
        }
    }
    
    // Update concept instances
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (concept) {
        concept->instances = consolidated;
        
        // Recompute prototype
        if (!concept->instances.empty()) {
            std::vector<float> newPrototype(concept->instances[0].pattern.size(), 0.0f);
            float totalWeight = 0.0f;
            
            for (const auto& instance : concept->instances) {
                float weight = instance.priority * instance.stability;
                for (size_t i = 0; i < instance.pattern.size() && i < newPrototype.size(); ++i) {
                    newPrototype[i] += instance.pattern[i] * weight;
                }
                totalWeight += weight;
            }
            
            if (totalWeight > 0.0f) {
                for (size_t i = 0; i < newPrototype.size(); ++i) {
                    newPrototype[i] /= totalWeight;
                }
            }
            
            concept->prototype = newPrototype;
        }
    }
}

void ConceptFormation::pruneOutdatedConcepts() {
    std::vector<size_t> conceptsToRemove;
    
    for (auto& concept : concepts_) {
        // Check if concept has become too stable or unstable
        if (concept.avgStability < 0.2f || concept.avgStability > 0.98f) {
            conceptsToRemove.push_back(concept.id);
            
            // Remove from history
            pImpl->instanceHistory.erase(concept.id);
            
            NLM_LOG_INFO("ConceptFormation: Pruning concept " + std::to_string(concept.id) +
                         " (stability: " + std::to_string(concept.avgStability) + ")");
        }
    }
    
    // Remove outdated concepts
    for (size_t id : conceptsToRemove) {
        concepts_.erase(
            std::remove_if(concepts_.begin(), concepts_.end(),
                          [id](const DiscoveredConcept& c) { return c.id == id; }),
            concepts_.end()
        );
    }
}

void ConceptFormation::ageInstances() {
    for (auto& concept : concepts_) {
        for (auto& instance : concept.instances) {
            instance.age++;
            
            // Decay stability over time
            instance.stability *= pImpl->instanceAgingDecay;
            
            // Update priority based on recency
            instance.priority *= std::pow(0.95f, static_cast<float>(instance.age) / 100.0f);
        }
    }
}

void ConceptFormation::updateCategoryEvolution(size_t conceptId,
                                             const std::vector<float>& newPattern,
                                             const std::vector<float>& features,
                                             float reward) {
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (!concept) return;
    
    // Multi-dimensional category evolution
    std::vector<float> newCategoryVector = extractFeatureVector(newPattern, features);
    
    // Smooth category evolution
    if (concept->categoryVector.size() == newCategoryVector.size() && 
        !concept->categoryVector.empty()) {
        float alpha = 0.1f;  // Learning rate for category evolution
        for (size_t i = 0; i < newCategoryVector.size(); ++i) {
            concept->categoryVector[i] = concept->categoryVector[i] * (1.0f - alpha) + 
                                        newCategoryVector[i] * alpha;
        }
    }
    
    // Dynamic category merging/splitting based on similarity
    mergeSimilarCategories(conceptId);
    splitCategoryIfNeeded(conceptId);
}

void ConceptFormation::mergeSimilarCategories(size_t conceptId) {
    DiscoveredConcept* target = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            target = &c;
            break;
        }
    }
    
    if (!target) return;
    
    float minSimilarity = 0.8f;
    
    for (auto& other : concepts_) {
        if (other.id == conceptId) continue;
        
        float similarity = calculateCategorySimilarity(target->categoryVector, other.categoryVector);
        if (similarity > minSimilarity) {
            // Merge categories
            for (size_t id : other.associatedConceptIds) {
                target->associatedConceptIds.push_back(id);
            }
            
            NLM_LOG_INFO("ConceptFormation: Merged category " + std::to_string(other.id) +
                         " into concept " + std::to_string(conceptId));
        }
    }
}

void ConceptFormation::splitCategoryIfNeeded(size_t conceptId) {
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (!concept) return;
    
    // Split if category entropy is high (diverse)
    if (concept->categoryEntropy > 0.7f && concept->instances.size() > 10) {
        // Find clusters within the concept
        std::vector<std::vector<float>> clusters;
        performConceptClustering(*concept, clusters);
        
        if (clusters.size() > 1) {
            // Create new concepts for clusters
            for (size_t i = 1; i < clusters.size(); ++i) {
                DiscoveredConcept newConcept;
                newConcept.id = nextConceptId_++;
                newConcept.prototype = clusters[i];
                newConcept.totalObservations = 0;
                newConcept.categoryHint = "split_from_" + std::to_string(conceptId);
                newConcept.categoryVector = clusters[i];
                newConcept.categoryEntropy = calculateCategoryEntropy(clusters[i], {});
                
                // Copy instances belonging to this cluster
                // (This is a simplified version)
                
                concepts_.push_back(newConcept);
                pImpl->instanceHistory[newConcept.id] = {};
                
                NLM_LOG_INFO("ConceptFormation: Split concept " + std::to_string(conceptId) +
                             " into new concept " + std::to_string(newConcept.id));
            }
        }
    }
}

void ConceptFormation::performConceptClustering(const DiscoveredConcept& concept,
                                              std::vector<std::vector<float>>& clusters) const {
    // Simple K-means like clustering of concept instances
    if (concept.instances.empty()) return;
    
    // Initialize cluster centers with prototype and a few instances
    std::vector<std::vector<float>> centers;
    centers.push_back(concept.prototype);
    
    if (concept.instances.size() >= 3) {
        auto it = concept.instances.begin();
        std::advance(it, 1);
        centers.push_back(it->pattern);
        
        if (concept.instances.size() >= 5) {
            std::advance(it, 2);
            centers.push_back(it->pattern);
        }
    }
    
    // Simple iteration to assign instances to nearest center
    std::vector<size_t> assignments(concept.instances.size());
    bool changed = true;
    int maxIterations = 10;
    int iter = 0;
    
    while (changed && iter < maxIterations) {
        changed = false;
        
        for (size_t i = 0; i < concept.instances.size(); ++i) {
            size_t bestCluster = 0;
            float bestDist = std::numeric_limits<float>::max();
            
            for (size_t j = 0; j < centers.size(); ++j) {
                float dist = combinedPatternDistance(
                    concept.instances[i].pattern, centers[j]);
                if (dist < bestDist) {
                    bestDist = dist;
                    bestCluster = j;
                }
            }
            
            if (assignments[i] != bestCluster) {
                assignments[i] = bestCluster;
                changed = true;
            }
        }
        
        // Update centers
        std::vector<std::vector<float>> newCenters(centers.size(), 
                                                  std::vector<float>(concept.prototype.size(), 0.0f));
        std::vector<size_t> clusterCounts(centers.size(), 0);
        
        for (size_t i = 0; i < concept.instances.size(); ++i) {
            size_t cluster = assignments[i];
            clusterCounts[cluster]++;
            
            for (size_t j = 0; j < concept.instances[i].pattern.size() && 
                                 j < newCenters[cluster].size(); ++j) {
                newCenters[cluster][j] += concept.instances[i].pattern[j];
            }
        }
        
        for (size_t j = 0; j < centers.size(); ++j) {
            if (clusterCounts[j] > 0) {
                for (size_t k = 0; k < newCenters[j].size(); ++k) {
                    newCenters[j][k] /= static_cast<float>(clusterCounts[j]);
                }
            }
        }
        
        centers = newCenters;
        iter++;
    }
    
    // Convert centers to vectors for clusters
    for (const auto& center : centers) {
        clusters.push_back(center);
    }
}

float ConceptFormation::calculateCategorySimilarity(const std::vector<float>& a,
                                                 const std::vector<float>& b) const {
    if (a.empty() || b.empty() || a.size() != b.size()) return 0.0f;
    
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

std::string ConceptFormation::generateMultiDimensionalCategory(const std::vector<float>& pattern,
                                                              const std::vector<float>& features,
                                                              float reward) const {
    std::string category = "multi_dim_";
    
    // Extract multiple category dimensions
    std::vector<std::string> dimensions;
    
    // Reward-based dimension
    if (reward > 0.5f) dimensions.push_back("rewarding");
    else if (reward < -0.3f) dimensions.push_back("punishing");
    else dimensions.push_back("neutral");
    
    // Pattern complexity dimension
    float complexity = calculatePatternComplexity(pattern);
    if (complexity < 0.3f) dimensions.push_back("simple");
    else if (complexity < 0.7f) dimensions.push_back("moderate");
    else dimensions.push_back("complex");
    
    // Feature diversity dimension
    float featureDiversity = calculateFeatureDiversity(features);
    if (featureDiversity < 0.3f) dimensions.push_back("coherent");
    else if (featureDiversity < 0.7f) dimensions.push_back("mixed");
    else dimensions.push_back("diverse");
    
    // Combine dimensions
    for (size_t i = 0; i < dimensions.size(); ++i) {
        if (i > 0) category += "_";
        category += dimensions[i];
    }
    
    return category;
}

std::vector<float> ConceptFormation::extractFeatureVector(const std::vector<float>& pattern,
                                                          const std::vector<float>& features) const {
    std::vector<float> featureVector;
    
    // Extract multiple feature representations
    featureVector.push_back(calculatePatternEnergy(pattern));
    featureVector.push_back(calculatePatternComplexity(pattern));
    featureVector.push_back(calculateFeatureDiversity(features));
    
    // Normalize reward range to [0,1]
    float normalizedReward = std::max(0.0f, std::min(1.0f, (pattern[0] + 1.0f) * 0.5f));
    featureVector.push_back(normalizedReward);
    
    // Add pattern statistics
    if (!pattern.empty()) {
        float min = pattern[0], max = pattern[0], sum = 0.0f;
        for (float val : pattern) {
            min = std::min(min, val);
            max = std::max(max, val);
            sum += val;
        }
        
        featureVector.push_back(min);  // Minimum value
        featureVector.push_back(max);  // Maximum value
        featureVector.push_back(sum / pattern.size());  // Mean value
        
        if (max > min) {
            featureVector.push_back((max - min) / 2.0f);  // Range midpoint
            featureVector.push_back((max - min));  // Range
        } else {
            featureVector.push_back(0.0f);  // Range midpoint
            featureVector.push_back(0.0f);  // Range
        }
    }
    
    return featureVector;
}

float ConceptFormation::calculatePatternComplexity(const std::vector<float>& pattern) const {
    if (pattern.empty()) return 0.0f;
    
    // Calculate complexity using entropy-like measure
    float mean = std::accumulate(pattern.begin(), pattern.end(), 0.0f) / pattern.size();
    float variance = 0.0f;
    
    for (float val : pattern) {
        variance += (val - mean) * (val - mean);
    }
    variance /= pattern.size();
    
    float stddev = std::sqrt(variance);
    
    // Normalize complexity to [0,1] range
    return std::min(1.0f, stddev * 2.0f);  // Assuming values in [-1,1] range
}

float ConceptFormation::calculatePatternEnergy(const std::vector<float>& pattern) const {
    float energy = 0.0f;
    for (float val : pattern) {
        energy += val * val;
    }
    return std::sqrt(energy);
}

float ConceptFormation::calculateFeatureDiversity(const std::vector<float>& features) const {
    if (features.empty()) return 0.0f;
    
    float mean = std::accumulate(features.begin(), features.end(), 0.0f) / features.size();
    float variance = 0.0f;
    
    for (float val : features) {
        variance += (val - mean) * (val - mean);
    }
    variance /= features.size();
    
    float stddev = std::sqrt(variance);
    return std::min(1.0f, stddev * 2.0f);  // Assuming values in [-1,1] range
}

float ConceptFormation::calculateCategoryEntropy(const std::vector<float>& pattern,
                                                const std::vector<float>& features) const {
    std::vector<float> featureVector = extractFeatureVector(pattern, features);
    if (featureVector.empty()) return 0.0f;
    
    // Calculate entropy-like measure of diversity
    float mean = std::accumulate(featureVector.begin(), featureVector.end(), 0.0f) / featureVector.size();
    float variance = 0.0f;
    
    for (float val : featureVector) {
        variance += (val - mean) * (val - mean);
    }
    variance /= featureVector.size();
    
    float stddev = std::sqrt(variance);
    
    // Convert variance to entropy-like measure [0,1]
    return std::min(1.0f, variance * 10.0f);  // Scaling factor
}

float ConceptFormation::calculateFeatureInfluence(float featureValue, float categoryValue) const {
    // Calculate how much a feature influences the category
    float diff = std::abs(featureValue - categoryValue);
    return std::max(0.0f, 1.0f - diff * 2.0f);  // Higher influence when closer
}

float ConceptFormation::calculatePatternComplexity(const std::vector<float>& pattern) const {
    // Duplicate implementation for consistency
    if (pattern.empty()) return 0.0f;
    
    float mean = std::accumulate(pattern.begin(), pattern.end(), 0.0f) / pattern.size();
    float variance = 0.0f;
    
    for (float val : pattern) {
        variance += (val - mean) * (val - mean);
    }
    variance /= pattern.size();
    
    float stddev = std::sqrt(variance);
    return std::min(1.0f, stddev * 2.0f);
}

float ConceptFormation::calculatePatternEnergy(const std::vector<float>& pattern) const {
    // Duplicate implementation for consistency
    float energy = 0.0f;
    for (float val : pattern) {
        energy += val * val;
    }
    return std::sqrt(energy);
}

float ConceptFormation::calculateFeatureDiversity(const std::vector<float>& features) const {
    // Duplicate implementation for consistency
    if (features.empty()) return 0.0f;
    
    float mean = std::accumulate(features.begin(), features.end(), 0.0f) / features.size();
    float variance = 0.0f;
    
    for (float val : features) {
        variance += (val - mean) * (val - mean);
    }
    variance /= features.size();
    
    float stddev = std::sqrt(variance);
    return std::min(1.0f, stddev * 2.0f);
}

float ConceptFormation::calculateCategoryEntropy(const std::vector<float>& pattern,
                                                const std::vector<float>& features) const {
    // Duplicate implementation for consistency
    std::vector<float> featureVector = extractFeatureVector(pattern, features);
    if (featureVector.empty()) return 0.0f;
    
    float mean = std::accumulate(featureVector.begin(), featureVector.end(), 0.0f) / featureVector.size();
    float variance = 0.0f;
    
    for (float val : featureVector) {
        variance += (val - mean) * (val - mean);
    }
    variance /= featureVector.size();
    
    float stddev = std::sqrt(variance);
    return std::min(1.0f, variance * 10.0f);
}

float ConceptFormation::calculateInstancePriority(const std::vector<float>& categoryVector,
                                               float reward) const {
    // Calculate instance priority based on category similarity to concept
    float rewardPriority = (reward + 1.0f) * 0.5f;  // Normalize reward to [0,1]
    
    float categoryScore = 0.0f;
    for (float val : categoryVector) {
        categoryScore += std::abs(val);
    }
    categoryScore /= categoryVector.size();
    
    return (rewardPriority * 0.6f) + (categoryScore * 0.4f);
}

std::vector<float> ConceptFormation::downsamplePattern(const std::vector<float>& pattern,
                                                       size_t factor) const {
    if (pattern.empty()) return {};
    
    std::vector<float> downsampled;
    size_t step = std::max(1U, static_cast<size_t>(pattern.size()) / factor);
    
    for (size_t i = 0; i < pattern.size(); i += step) {
        downsampled.push_back(pattern[i]);
    }
    
    return downsampled;
}

float ConceptFormation::calculateContextualNovelty(const std::vector<float>& pattern) const {
    // Calculate novelty based on brain state context
    if (pImpl->currentBrainState.empty()) return 0.5f;  // Default medium novelty
    
    // Compare pattern to brain state
    float similarity = 0.0f;
    size_t minSize = std::min(pattern.size(), pImpl->currentBrainState.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        similarity += std::abs(pattern[i] - pImpl->currentBrainState[i]);
    }
    
    // Normalize to [0,1]
    float maxDiff = 2.0f * static_cast<float>(minSize);  // Values in [-1,1]
    float novelty = 1.0f - (similarity / maxDiff);
    
    return std::max(0.0f, std::min(1.0f, novelty));
}

float ConceptFormation::calculatePredictionError(const std::vector<float>& pattern) const {
    // Calculate prediction error based on temporal relations
    if (!pImpl->brain) return 0.0f;
    
    // Get predicted pattern from temporal relations (if available)
    std::vector<float> predicted = pImpl->brain->getTemporalPrediction(pattern);
    
    if (predicted.empty()) return 0.5f;  // Default medium error
    
    return combinedPatternDistance(pattern, predicted);
}

void ConceptFormation::updateNoveltyStatistics(const std::vector<float>& pattern,
                                             SimulationStep currentTime) {
    pImpl->noveltyStats.observationCount++;
    
    float patternEnergy = calculatePatternEnergy(pattern);
    
    // Update moving averages
    pImpl->noveltyStats.meanPatternEnergy = 
        pImpl->noveltyStats.meanPatternEnergy * 0.9f + patternEnergy * 0.1f;
    
    // Update standard deviation
    float energyDiff = patternEnergy - pImpl->noveltyStats.meanPatternEnergy;
    pImpl->noveltyStats.stddevPatternEnergy = 
        std::sqrt(pImpl->noveltyStats.stddevPatternEnergy * pImpl->noveltyStats.stddevPatternEnergy * 0.9f +
                  energyDiff * energyDiff * 0.1f);
    
    // Update temporal gaps
    static SimulationStep lastTime = 0;
    if (lastTime > 0) {
        float temporalGap = static_cast<float>(currentTime - lastTime);
        pImpl->noveltyStats.meanTemporalGaps = 
            pImpl->noveltyStats.meanTemporalGaps * 0.9f + temporalGap * 0.1f;
        
        float gapDiff = temporalGap - pImpl->noveltyStats.meanTemporalGaps;
        pImpl->noveltyStats.stddevTemporalGaps = 
            std::sqrt(pImpl->noveltyStats.stddevTemporalGaps * pImpl->noveltyStats.stddevTemporalGaps * 0.9f +
                      gapDiff * gapDiff * 0.1f);
    }
    
    lastTime = currentTime;
}

bool ConceptFormation::isPatternOutlier(float patternEnergy) const {
    // Statistical outlier detection based on pattern energy
    if (pImpl->noveltyStats.observationCount < 5) return false;
    
    float zScore = (patternEnergy - pImpl->noveltyStats.meanPatternEnergy) / 
                   (pImpl->noveltyStats.stddevPatternEnergy + 0.0001f);
    
    // Consider outlier if > 2 standard deviations from mean
    return std::abs(zScore) > 2.0f;
}

void ConceptFormation::updateBrainState(const std::vector<float>& newState) {
    pImpl->currentBrainState = newState;
}

void ConceptFormation::forgetOutdatedConcepts() {
    // Forgetting mechanism for outdated concepts
    float memoryPressure = static_cast<float>(concepts_.size()) / memoryCapacity_;
    
    if (memoryPressure > conceptPruningThreshold_) {
        // Sort concepts by stability (lower stability first)
        std::sort(concepts_.begin(), concepts_.end(),
                  [](const DiscoveredConcept& a, const DiscoveredConcept& b) {
                      return a.avgStability < b.avgStability;
                  });
        
        // Remove least stable concepts
        size_t targetSize = static_cast<size_t>(memoryCapacity_ * 0.9f);
        if (concepts_.size() > targetSize) {
            concepts_.resize(targetSize);
            
            // Also clean up instance history
            std::vector<size_t> conceptIds;
            for (const auto& concept : concepts_) {
                conceptIds.push_back(concept.id);
            }
            
            std::vector<size_t> idsToRemove;
            for (const auto& pair : pImpl->instanceHistory) {
                if (std::find(conceptIds.begin(), conceptIds.end(), pair.first) == conceptIds.end()) {
                    idsToRemove.push_back(pair.first);
                }
            }
            
            for (size_t id : idsToRemove) {
                pImpl->instanceHistory.erase(id);
            }
        }
    }
}

// Global state update function for concept formation system
void ConceptFormation::updateGlobalState(const std::vector<float>& brainState) {
    updateBrainState(brainState);
    
    // Periodic cleanup and consolidation
    if (concepts_.size() > 10) {
        ageInstances();
        forgetOutdatedConcepts();
        
        // Consolidate instances periodically
        for (auto& concept : concepts_) {
            if (concept.instances.size() > 20) {
                consolidateInstances(concept.id);
            }
        }
    }
}

} // namespace nlm