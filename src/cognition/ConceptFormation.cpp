#include "ConceptFormation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <unordered_map>
#include <limits>
#include <chrono>

namespace nlm {

struct ConceptFormation::Impl {
    Brain* brain;
    // Clustering and pattern storage
    std::vector<PatternCluster> clusters_;
    std::unordered_map<size_t, size_t> conceptToCluster_;
    std::vector<std::vector<float>> patternBuffer_;
    
    // Hebbian learning state
    std::vector<HebbianUpdate> hebbianStates_;
    std::vector<float> eligibilityTraces_;
    
    // Stability tracking
    std::vector<ConceptStabilityMetrics> stabilityMetrics_;
    std::vector<SimulationStep> lastStabilityUpdate_;
    
    // Statistical properties
    std::vector<std::vector<float>> patternMeans_;
    std::vector<std::vector<std::vector<float>>> patternCovariances_;
    
    // Category discovery
    std::vector<std::vector<std::string>> discoveredCategories_;
    
    // Pattern history for decay
    std::vector<std::vector<SimulationStep>> patternAges_;
    
    // Random generator for stochastic operations
    std::mt19937 rng_;
    
    Impl() : brain(nullptr) {
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        rng_.seed(static_cast<unsigned int>(seed));
    }
};

ConceptFormation::ConceptFormation()
    : pImpl(new Impl)
    , brain_(nullptr)
    , nextConceptId_(1)
    , formationThreshold_(0.75f)
    , stabilityThreshold_(0.7f)
    , stabilityWindow_(5)
    , hebbianLearningRate_(0.1f)
    , maxClusters_(3)
    , enableAutomaticCategoryDiscovery_(true)
    , defaultSimilarityMetric_(0) // cosine
    , lastConsolidationStep_(0) {
}

ConceptFormation::~ConceptFormation() = default;

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
    pImpl->patternBuffer_.clear();
    pImpl->hebbianStates_.clear();
    pImpl->eligibilityTraces_.clear();
    pImpl->stabilityMetrics_.clear();
    pImpl->lastStabilityUpdate_.clear();
    pImpl->patternMeans_.clear();
    pImpl->patternCovariances_.clear();
    pImpl->discoveredCategories_.clear();
    pImpl->patternAges_.clear();
    pImpl->clusters_.clear();
    pImpl->conceptToCluster_.clear();
    
    // Reserve space based on brain configuration
    if (brain && brain->getTotalNeuronCount() > 0) {
        size_t estimatedNeurons = std::min(brain->getTotalNeuronCount(), 10000UL);
        pImpl->patternBuffer_.reserve(estimatedNeurons);
        pImpl->hebbianStates_.resize(estimatedNeurons);
        pImpl->eligibilityTraces_.resize(estimatedNeurons, 0.0f);
        pImpl->stabilityMetrics_.resize(estimatedNeurons);
        pImpl->lastStabilityUpdate_.resize(estimatedNeurons, 0);
        pImpl->patternMeans_.resize(estimatedNeurons);
        pImpl->patternCovariances_.resize(estimatedNeurons);
        pImpl->discoveredCategories_.resize(estimatedNeurons);
        pImpl->patternAges_.resize(estimatedNeurons);
    }
    
    NLM_LOG_INFO("Enhanced ConceptFormation initialized with Hebbian learning and automatic category discovery");
}

size_t ConceptFormation::presentExperience(const std::vector<float>& pattern,
                                          const std::vector<float>& features,
                                          float reward,
                                          SimulationStep currentTime) {
    if (pattern.empty()) return 0;
    
    // Store pattern for advanced clustering analysis
    if (pImpl->patternBuffer_.size() < 10000UL) {
        pImpl->patternBuffer_.push_back(pattern);
    }
    
    // Check if this pattern matches any existing concept using real clustering
    size_t matchingConcept = performClustering(pattern);
    
    if (matchingConcept > 0) {
        // Update existing concept with advanced Hebbian learning
        updateConcept(matchingConcept, pattern, features, reward, currentTime);
        
        // Update Hebbian eligibility traces
        if (pImpl->hebbianStates_.size() > 0) {
            pImpl->eligibilityTraces_[matchingConcept % pImpl->eligibilityTraces_.size()] += reward;
        }
        
        return matchingConcept;
    }
    
    // Check if this pattern is similar enough to an existing concept for merging
    for (const auto& concept : concepts_) {
        float similarity = computeSimilarity(pattern, concept.prototype, defaultSimilarityMetric_);
        if (similarity >= formationThreshold_) {
            // High similarity suggests these concepts should be merged
            NLM_LOG_INFO("ConceptFormation: High similarity between patterns detected, merging concepts");
            
            // Create a new concept by merging the closest concept
            DiscoveredConcept* closestConcept = nullptr;
            float maxSimilarity = 0.0f;
            
            for (auto& c : concepts_) {
                float sim = computeSimilarity(pattern, c.prototype, defaultSimilarityMetric_);
                if (sim > maxSimilarity) {
                    maxSimilarity = sim;
                    closestConcept = &c;
                }
            }
            
            if (closestConcept) {
                return closestConcept->id; // Return existing concept ID
            }
            break;
        }
    }
    
    // If we have enough patterns and stability, form a new concept
    if (pImpl->patternBuffer_.size() >= 10) {
        performClusterMaintenance(currentTime);
        
        // Check if pattern is novel using real clustering
        if (!isNovel(pattern, formationThreshold_)) {
            // Still novel - form new concept
            return createConcept(pattern, features, reward, currentTime);
        }
    }
    
    // Early in learning process - store but don't form concept yet
    NLM_LOG_DEBUG("ConceptFormation: Pattern not yet classifiable");
    return 0;  // Not yet classifiable
}

size_t ConceptFormation::presentSensoryExperience(const SensoryInput& sensoryInput,
                                                float reward,
                                                SimulationStep currentTime) {
    // Extract features from sensory input using advanced feature extraction
    std::vector<float> features = extractSensoryFeatures(sensoryInput);
    
    // Convert sensory input to neural pattern
    std::vector<float> pattern = convertToPattern(sensoryInput);
    
    // Present the experience to concept formation
    return presentExperience(pattern, features, reward, currentTime);
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

size_t ConceptFormation::createConcept(const std::vector<float>& pattern,
                                        const std::vector<float>& features,
                                        float reward,
                                        SimulationStep currentTime) {
    DiscoveredConcept concept;
    concept.id = nextConceptId_++;
    concept.prototype = pattern;
    concept.meanPattern = pattern;
    concept.totalObservations = 1;
    concept.creationTime = currentTime;
    concept.lastStabilityUpdate = currentTime;
    
    // Initialize covariance matrix for Mahalanobis distance
    size_t dim = pattern.size();
    concept.covarianceMatrix.resize(dim, std::vector<float>(dim, 0.0f));
    
    // Create first instance with advanced properties
    ConceptInstance instance;
    instance.pattern = pattern;
    instance.features = features;
    instance.observationCount = 1;
    instance.lastObserved = currentTime;
    instance.avgReward = reward;
    instance.stability = 1.0f;
    instance.age = 0;
    
    concept.instances.push_back(instance);
    concept.avgStability = 1.0f;
    
    // Automatically derive category hints
    if (enableAutomaticCategoryDiscovery_) {
        deriveCategoryHint(concept.id);
    }
    
    // Create Hebbian state for this concept
    if (pImpl->hebbianStates_.size() <= concept.id) {
        pImpl->hebbianStates_.resize(concept.id + 1);
    }
    
    concepts_.push_back(concept);
    
    // Update statistical properties
    computePatternVariance(concept.id);
    updatePatternStatistics(concept.id);
    
    // Create cluster for this concept
    PatternCluster cluster;
    cluster.instanceIndices.push_back(0);
    cluster.center = pattern;
    cluster.radius = 0.0f;
    cluster.weight = 1.0f;
    cluster.lastUpdate = currentTime;
    
    pImpl->clusters_.push_back(cluster);
    pImpl->conceptToCluster_[concept.id] = pImpl->clusters_.size() - 1;
    
    NLM_LOG_INFO("Enhanced ConceptFormation: Created concept " + std::to_string(concept.id) +
                 " with prototype size " + std::to_string(pattern.size()) +
                 " and automatic category: " + concept.categoryHint);
    
    return concept.id;
}

void ConceptFormation::performClusterMaintenance(SimulationStep currentTime) {
    // Only run cluster maintenance periodically
    if (currentTime - lastConsolidationStep_ < 100) {
        return;
    }
    
    kmeansLikeClustering(currentTime);
    performHebbianConsolidation(currentTime);
    discoverCategories();
    lastConsolidationStep_ = currentTime;
}

void ConceptFormation::kmeansLikeClustering(SimulationStep currentTime) {
    // Simple iterative clustering algorithm
    size_t maxIterations = 5;
    bool changed = true;
    size_t iteration = 0;
    
    while (changed && iteration < maxIterations) {
        changed = false;
        
        // Clear old clusters
        pImpl->clusters_.clear();
        pImpl->conceptToCluster_.clear();
        
        // For each concept, re-cluster its instances
        for (size_t c = 0; c < concepts_.size(); ++c) {
            auto& concept = concepts_[c];
            
            if (concept.instances.size() > 1) {
                // Create clusters for concept instances
                size_t numClusters = std::min(maxClusters_, concept.instances.size());
                std::vector<PatternCluster> newClusters;
                
                // Initialize cluster centers using k-means++
                std::vector<size_t> clusterAssignments(concept.instances.size(), 0);
                std::vector<size_t> chosenIndices;
                
                if (numClusters > 0) {
                    // Choose first center randomly
                    std::uniform_int_distribution<size_t> dist(0, concept.instances.size() - 1);
                    size_t firstIdx = dist(pImpl->rng_);
                    chosenIndices.push_back(firstIdx);
                    clusterAssignments[firstIdx] = 0;
                    
                    // Choose remaining centers
                    for (size_t i = 1; i < numClusters; ++i) {
                        float totalDistance = 0.0f;
                        std::vector<float> distances;
                        
                        for (size_t j = 0; j < concept.instances.size(); ++j) {
                            float minDist = std::numeric_limits<float>::max();
                            for (size_t k = 0; k < chosenIndices.size(); ++k) {
                                float dist = computePatternDistance(
                                    concept.instances[j].pattern,
                                    concept.instances[chosenIndices[k]].pattern,
                                    concept.id);
                                if (dist < minDist) minDist = dist;
                            }
                            distances.push_back(minDist);
                            totalDistance += minDist;
                        }
                        
                        if (totalDistance > 0.0f) {
                            std::uniform_real_distribution<float> dist(0.0f, totalDistance);
                            float r = dist(pImpl->rng_);
                            float accum = 0.0f;
                            size_t selected = 0;
                            for (size_t j = 0; j < concept.instances.size(); ++j) {
                                accum += distances[j];
                                if (accum >= r) {
                                    selected = j;
                                    break;
                                }
                            }
                            chosenIndices.push_back(selected);
                            clusterAssignments[selected] = i;
                        }
                    }
                }
                
                // Create clusters
                for (size_t i = 0; i < chosenIndices.size(); ++i) {
                    PatternCluster cluster;
                    cluster.center = concept.instances[chosenIndices[i]].pattern;
                    cluster.weight = 1.0f;
                    cluster.lastUpdate = currentTime;
                    cluster.instanceIndices.push_back(chosenIndices[i]);
                    
                    // Calculate radius
                    float maxDist = 0.0f;
                    for (size_t j = 0; j < concept.instances.size(); ++j) {
                        float dist = computePatternDistance(
                            concept.instances[j].pattern,
                            cluster.center,
                            concept.id);
                        if (dist > maxDist) maxDist = dist;
                    }
                    cluster.radius = maxDist;
                    
                    newClusters.push_back(cluster);
                }
                
                // Update concept with new clusters
                pImpl->clusters_.insert(pImpl->clusters_.end(), newClusters.begin(), newClusters.end());
                
                for (size_t i = 0; i < concept.instances.size(); ++i) {
                    pImpl->conceptToCluster_[concept.id * 1000 + i] = clusterAssignments[i];
                }
            }
        }
        
        iteration++;
    }
}

void ConceptFormation::updateClusterCenters(size_t conceptId) {
    auto* concept = getConcept(conceptId);
    if (!concept) return;
    
    size_t clusterIndex = pImpl->conceptToCluster_[conceptId];
    if (clusterIndex >= pImpl->clusters_.size()) return;
    
    auto& cluster = pImpl->clusters_[clusterIndex];
    
    // Recalculate cluster center as weighted average
    std::vector<float> newCenter(concept->prototype.size(), 0.0f);
    float totalWeight = 0.0f;
    
    for (size_t idx : cluster.instanceIndices) {
        if (idx < concept->instances.size()) {
            float weight = concept->instances[idx].observationCount;
            for (size_t i = 0; i < concept->prototype.size(); ++i) {
                newCenter[i] += concept->instances[idx].pattern[i] * weight;
            }
            totalWeight += weight;
        }
    }
    
    if (totalWeight > 0.0f) {
        for (size_t i = 0; i < newCenter.size(); ++i) {
            newCenter[i] /= totalWeight;
        }
        
        // Smooth update with momentum
        float momentum = 0.3f;
        for (size_t i = 0; i < newCenter.size(); ++i) {
            newCenter[i] = concept->prototype[i] * (1.0f - momentum) + newCenter[i] * momentum;
        }
        
        // Update concept prototype
        concept->prototype = newCenter;
        
        // Update covariance matrix
        if (concept->instances.size() > 1) {
            concept->covarianceMatrix = computeCovariance(
                concept->instances,
                concept->meanPattern);
        }
    }
}

float ConceptFormation::computePatternDistance(const std::vector<float>& a,
                                              const std::vector<float>& b,
                                              size_t conceptId) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept) return std::numeric_limits<float>::max();
    
    switch (defaultSimilarityMetric_) {
        case 1: // Euclidean distance
            return euclideanDistance(a, b);
            
        case 2: // Mahalanobis distance
        {
            if (concept->covarianceMatrix.empty()) {
                return euclideanDistance(a, b);
            }
            return mahalanobisDistance(a, b, concept->meanPattern, concept->covarianceMatrix);
        }
            
        case 0: // Cosine similarity (inverted, since we want distance)
        default:
            float sim = cosineSimilarity(a, b);
            return (sim > 0.001f) ? (1.0f - sim) : 1.0f;
    }
}

size_t ConceptFormation::performClustering(const std::vector<float>& pattern) {
    if (pImpl->clusters_.empty()) return 0;
    
    float bestSimilarity = std::numeric_limits<float>::max();
    size_t bestCluster = 0;
    
    // Find closest cluster using advanced distance metrics
    for (size_t c = 0; c < pImpl->clusters_.size(); ++c) {
        const auto& cluster = pImpl->clusters_[c];
        
        // Find closest instance in this cluster
        float minDist = std::numeric_limits<float>::max();
        for (size_t idx : cluster.instanceIndices) {
            for (const auto& concept : concepts_) {
                if (idx < concept.instances.size()) {
                    float dist = computePatternDistance(pattern, concept.instances[idx].pattern, concept.id);
                    if (dist < minDist) minDist = dist;
                }
            }
        }
        
        if (minDist < bestSimilarity) {
            bestSimilarity = minDist;
            bestCluster = c;
        }
    }
    
    // Check if pattern belongs to a cluster
    if (bestSimilarity < clusterRadiusThreshold(bestCluster)) {
        // Find which concept this cluster belongs to
        for (const auto& concept : concepts_) {
            size_t clusterIndex = pImpl->conceptToCluster_[concept.id];
            if (clusterIndex == bestCluster) {
                return concept.id;
            }
        }
    }
    
    return 0; // No matching cluster
}

void ConceptFormation::addInstanceToConcept(size_t conceptId, const std::vector<float>& pattern,
                                           const std::vector<float>& features, float reward,
                                           SimulationStep currentTime) {
    auto* concept = getConcept(conceptId);
    if (!concept) return;
    
    ConceptInstance instance;
    instance.pattern = pattern;
    instance.features = features;
    instance.observationCount = 1;
    instance.lastObserved = currentTime;
    instance.avgReward = reward;
    instance.stability = 1.0f;
    instance.age = concept->totalObservations;
    
    // Add to concept
    concept->instances.push_back(instance);
    concept->totalObservations++;
    
    // Update prototype using Hebbian learning
    updatePrototype(conceptId, pattern, currentTime);
    
    // Update statistical properties
    computePatternVariance(conceptId);
    updateConceptStability(conceptId, currentTime);
    
    // Update associated cluster
    size_t clusterIndex = pImpl->conceptToCluster_[conceptId];
    if (clusterIndex < pImpl->clusters_.size()) {
        pImpl->clusters_[clusterIndex].instanceIndices.push_back(concept->instances.size() - 1);
        pImpl->clusters_[clusterIndex].lastUpdate = currentTime;
    }
}

void ConceptFormation::updateInstanceStability(size_t conceptId, size_t instanceIndex,
                                              SimulationStep currentTime) {
    auto* concept = getConcept(conceptId);
    if (!concept || instanceIndex >= concept->instances.size()) return;
    
    auto& instance = concept->instances[instanceIndex];
    
    // Calculate instance stability based on consistency with prototype
    float stability = 0.0f;
    if (!concept->prototype.empty() && !instance.pattern.empty()) {
        if (defaultSimilarityMetric_ == 1) { // Euclidean
            stability = 1.0f / (1.0f + euclideanDistance(instance.pattern, concept->prototype));
        } else if (defaultSimilarityMetric_ == 2) { // Mahalanobis
            stability = 1.0f / (1.0f + mahalanobisDistance(instance.pattern, concept->prototype,
                                                          concept->meanPattern, concept->covarianceMatrix));
        } else { // Cosine
            stability = cosineSimilarity(instance.pattern, concept->prototype);
        }
    }
    
    // Smooth stability update
    instance.stability = instance.stability * 0.7f + stability * 0.3f;
    
    // Update concept stability
    updateConceptStability(conceptId, currentTime);
}

void ConceptFormation::decayOldInstances(SimulationStep currentTime) {
    for (auto& concept : concepts_) {
        // Remove old instances (older than stabilityWindow_)
        std::vector<ConceptInstance> newInstances;
        for (auto& instance : concept.instances) {
            if (currentTime - instance.lastObserved < stabilityWindow_) {
                // Decay instance stability over time
                instance.stability *= 0.95f;
                newInstances.push_back(instance);
            }
        }
        
        // Update pattern buffer
        if (!newInstances.empty()) {
            concept.instances = newInstances;
            
            // Update prototype after instance removal
            if (!concept.prototype.empty()) {
                std::vector<float> newPrototype(concept.prototype.size(), 0.0f);
                float totalWeight = 0.0f;
                
                for (const auto& instance : concept.instances) {
                    float weight = instance.observationCount;
                    for (size_t i = 0; i < concept.prototype.size(); ++i) {
                        newPrototype[i] += instance.pattern[i] * weight;
                    }
                    totalWeight += weight;
                }
                
                if (totalWeight > 0.0f) {
                    for (size_t i = 0; i < newPrototype.size(); ++i) {
                        newPrototype[i] /= totalWeight;
                    }
                    concept.prototype = newPrototype;
                }
            }
        }
    }
}

void ConceptFormation::consolidateStableConcepts(SimulationStep currentTime) {
    // Merge similar concepts that have become too close
    for (size_t i = 0; i < concepts_.size(); ++i) {
        for (size_t j = i + 1; j < concepts_.size(); ++j) {
            auto& conceptA = concepts_[i];
            auto& conceptB = concepts_[j];
            
            // Check if concepts are too similar
            float similarity = computeSimilarity(conceptA.prototype, conceptB.prototype, defaultSimilarityMetric_);
            
            if (similarity > 0.8f) {
                NLM_LOG_INFO("ConceptFormation: Concepts " + std::to_string(conceptA.id) +
                             " and " + std::to_string(conceptB.id) +
                             " are too similar (similarity: " + std::to_string(similarity) +
                             "), merging them");
                
                // Merge concept B into concept A
                mergeConcepts(conceptA.id, conceptB.id);
                
                // Update indices since concepts_ vector changed
                i = concepts_.size();
                break;
            }
        }
    }
}

void ConceptFormation::hebbianUpdate(size_t conceptId, const std::vector<float>& pattern,
                                    float reward, SimulationStep currentTime) {
    auto* concept = getConcept(conceptId);
    if (!concept) return;
    
    HebbianUpdate& hebbian = pImpl->hebbianStates_[conceptId];
    
    // Update learning rate based on reward
    float rewardFactor = 1.0f;
    if (reward > 0.5f) {
        rewardFactor = 1.5f;
    } else if (reward < -0.5f) {
        rewardFactor = 0.5f;
    }
    
    // Hebbian learning rule: weight changes proportional to pre and post activity
    float alpha = hebbianLearningRate_ * rewardFactor;
    
    // Update prototype using Hebbian averaging
    if (concept->prototype.size() == pattern.size()) {
        for (size_t i = 0; i < concept->prototype.size(); ++i) {
            float preActivity = concept->prototype[i];
            float postActivity = pattern[i];
            
            // Standard Hebbian: Δw = η * x_pre * x_post
            float delta = alpha * preActivity * postActivity;
            
            // Add eligibility trace
            if (pImpl->eligibilityTraces_.size() > conceptId) {
                delta += pImpl->eligibilityTraces_[conceptId] * hebbian.eligibilityTrace;
            }
            
            // Update weight with constraints
            concept->prototype[i] = std::clamp(concept->prototype[i] + delta, -1.0f, 1.0f);
        }
    }
    
    // Update eligibility traces
    applyEligibilityTraces(currentTime);
}

void ConceptFormation::applyEligibilityTraces(SimulationStep currentTime) {
    // Decay eligibility traces over time
    float decayFactor = 0.95f;
    for (auto& trace : pImpl->eligibilityTraces_) {
        trace *= decayFactor;
    }
}

void ConceptFormation::updateConnectionStrength(size_t conceptId, const std::vector<float>& pattern,
                                                float reward, SimulationStep currentTime) {
    // Update synaptic connections based on Hebbian learning
    auto* concept = getConcept(conceptId);
    if (!concept) return;
    
    // Apply weight changes based on reward prediction error
    float predictionError = std::abs(reward - concept->avgStability);
    
    // Stronger connection for consistent rewards
    if (predictionError < 0.1f) {
        // Reinforce connections
        if (pImpl->hebbianStates_.size() > conceptId) {
            pImpl->hebbianStates_[conceptId].weight *= 1.05f;
        }
    } else {
        // Weaken connections for prediction errors
        if (pImpl->hebbianStates_.size() > conceptId) {
            pImpl->hebbianStates_[conceptId].weight *= 0.95f;
        }
    }
}

void ConceptFormation::analyzePatternProperties(size_t conceptId) {
    auto* concept = getConcept(conceptId);
    if (!concept) return;
    
    // Analyze pattern properties for category discovery
    float rewardVariance = 0.0f;
    std::vector<float> rewardSamples;
    
    for (const auto& instance : concept->instances) {
        rewardSamples.push_back(instance.avgReward);
    }
    
    if (rewardSamples.size() > 1) {
        float mean = 0.0f;
        for (float r : rewardSamples) mean += r;
        mean /= rewardSamples.size();
        
        for (float r : rewardSamples) {
            rewardVariance += (r - mean) * (r - mean);
        }
        rewardVariance /= rewardSamples.size();
    }
    
    concept->patternVariance.clear();
    
    // Calculate variance for each dimension
    size_t dim = concept->prototype.size();
    if (dim > 0) {
        concept->patternVariance.resize(dim, 0.0f);
        
        for (const auto& instance : concept->instances) {
            for (size_t i = 0; i < dim && i < instance.pattern.size(); ++i) {
                float diff = instance.pattern[i] - concept->meanPattern[i];
                concept->patternVariance[i] += diff * diff;
            }
        }
        
        for (size_t i = 0; i < concept->patternVariance.size(); ++i) {
            concept->patternVariance[i] /= concept->instances.size();
        }
    }
    
    // Update mean pattern
    if (!concept->instances.empty()) {
        concept->meanPattern.clear();
        concept->meanPattern.resize(dim, 0.0f);
        
        for (const auto& instance : concept->instances) {
            for (size_t i = 0; i < dim && i < instance.pattern.size(); ++i) {
                concept->meanPattern[i] += instance.pattern[i];
            }
        }
        
        for (size_t i = 0; i < concept->meanPattern.size(); ++i) {
            concept->meanPattern[i] /= concept->instances.size();
        }
    }
}

void ConceptFormation::deriveCategoryHint(size_t conceptId) {
    auto* concept = getConcept(conceptId);
    if (!concept) return;
    
    // Clear previous category hint
    concept->categoryHint = "";
    concept->categoryConfidence = 0.0f;
    
    // Analyze instances to determine category
    std::vector<float> rewards;
    for (const auto& instance : concept->instances) {
        rewards.push_back(instance.avgReward);
    }
    
    if (rewards.empty()) return;
    
    // Calculate statistics
    float meanReward = 0.0f;
    float minReward = std::numeric_limits<float>::max();
    float maxReward = std::numeric_limits<float>::lowest();
    
    for (float r : rewards) {
        meanReward += r;
        if (r < minReward) minReward = r;
        if (r > maxReward) maxReward = r;
    }
    meanReward /= rewards.size();
    
    // Determine category based on reward characteristics
    float rewardStd = 0.0f;
    for (float r : rewards) {
        rewardStd += (r - meanReward) * (r - meanReward);
    }
    rewardStd = std::sqrt(rewardStd / rewards.size());
    
    // Check category detectors
    float maxConfidence = 0.0f;
    std::string bestCategory;
    
    for (const auto& cat : automaticCategories_) {
        if (cat.detector) {
            bool matches = cat.detector(*concept);
            if (matches) {
                // Calculate confidence based on reward statistics
                float confidence = 0.0f;
                
                if (cat.name == "rewarding") {
                    confidence = (maxReward - minReward) / 2.0f;  // High range = more rewarding
                } else if (cat.name == "harmful") {
                    confidence = std::abs(meanReward);  // More negative = more harmful
                } else if (cat.name == "familiar") {
                    confidence = 1.0f - (rewardStd / std::max(1.0f, std::abs(meanReward)));
                }
                
                if (confidence > maxConfidence && confidence > cat.confidenceThreshold) {
                    maxConfidence = confidence;
                    bestCategory = cat.name;
                }
            }
        }
    }
    
    // If no automatic category matched, use simple heuristics
    if (bestCategory.empty()) {
        if (meanReward > 0.3f && rewardStd < 0.1f) {
            bestCategory = "reliable_rewarding";
            maxConfidence = 1.0f - (rewardStd / 0.5f);
        } else if (meanReward < -0.3f && rewardStd < 0.1f) {
            bestCategory = "consistent_harmful";
            maxConfidence = 1.0f - (rewardStd / 0.5f);
        } else if (rewardStd < 0.05f) {
            bestCategory = "stable_neutral";
            maxConfidence = 1.0f - (rewardStd / 0.1f);
        } else {
            bestCategory = "variable_" + std::to_string(static_cast<int>(meanReward * 10));
            maxConfidence = 0.5f;
        }
    }
    
    concept->categoryHint = bestCategory;
    concept->categoryConfidence = maxConfidence;
}

bool ConceptFormation::isRewardingPattern(float reward, const DiscoveredConcept& concept) const {
    return reward > 0.5f && concept.avgStability > 0.7f;
}

bool ConceptFormation::isHarmfulPattern(float reward, const DiscoveredConcept& concept) const {
    return reward < -0.3f && concept.avgStability > 0.7f;
}

bool ConceptFormation::isFamiliarPattern(const DiscoveredConcept& concept) const {
    return concept.totalObservations > 10 && concept.avgStability > 0.8f;
}

std::vector<float> ConceptFormation::extractSensoryFeatures(const SensoryInput& sensoryInput) const {
    std::vector<float> features;
    
    // Extract different types of features based on input type
    if (strcmp(sensoryInput.getType(), "Vision") == 0) {
        const auto& vision = dynamic_cast<const Vision&>(sensoryInput);
        const auto& data = vision.getData();
        
        // Extract basic vision features
        features.reserve(10);
        
        // Calculate average brightness
        float brightness = 0.0f;
        for (float val : data) brightness += val;
        brightness /= std::max(1.0f, static_cast<float>(data.size()));
        features.push_back(brightness);
        
        // Calculate contrast
        float mean = brightness;
        float variance = 0.0f;
        for (float val : data) {
            float diff = val - mean;
            variance += diff * diff;
        }
        variance /= std::max(1.0f, static_cast<float>(data.size()));
        features.push_back(std::sqrt(variance));
        
        // Add some texture features
        features.push_back(0.5f); // Placeholder
    } else if (strcmp(sensoryInput.getType(), "Audio") == 0) {
        const auto& audio = dynamic_cast<const Audio&>(sensoryInput);
        const auto& data = audio.getData();
        
        // Extract audio features
        float energy = 0.0f;
        float pitch = 0.0f;
        
        for (float val : data) {
            energy += val * val;
        }
        energy = std::sqrt(energy / std::max(1.0f, static_cast<float>(data.size())));
        
        features.push_back(energy);
        features.push_back(0.5f); // Placeholder pitch
    }
    
    // Ensure we have at least some features
    if (features.empty()) {
        features.push_back(0.0f);
    }
    
    return features;
}

std::vector<float> ConceptFormation::convertToPattern(const SensoryInput& sensoryInput) const {
    std::vector<float> pattern;
    
    // Convert sensory input to pattern vector
    if (strcmp(sensoryInput.getType(), "Vision") == 0) {
        const auto& vision = dynamic_cast<const Vision&>(sensoryInput);
        const auto& data = vision.getData();
        
        // Directly use vision data as pattern
        pattern = data;
    } else if (strcmp(sensoryInput.getType(), "Audio") == 0) {
        const auto& audio = dynamic_cast<const Audio&>(sensoryInput);
        const auto& data = audio.getData();
        
        // Convert audio to pattern using some transformation
        pattern.resize(data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            // Apply simple transformation
            pattern[i] = std::sin(data[i] * 3.14f) * 0.5f + 0.5f;
        }
    } else if (strcmp(sensoryInput.getType(), "InternalSignals") == 0) {
        const auto& signals = dynamic_cast<const InternalSignals&>(sensoryInput);
        const auto& data = signals.getData();
        
        // Internal signals as pattern
        pattern = data;
    }
    
    return pattern;
}

void ConceptFormation::computePatternVariance(size_t conceptId) {
    auto* concept = getConcept(conceptId);
    if (!concept || concept->instances.empty()) return;
    
    size_t dim = concept->prototype.size();
    if (dim == 0) return;
    
    concept->patternVariance.clear();
    concept->patternVariance.resize(dim, 0.0f);
    
    // Calculate variance for each dimension
    for (const auto& instance : concept->instances) {
        for (size_t i = 0; i < dim && i < instance.pattern.size(); ++i) {
            float diff = instance.pattern[i] - concept->meanPattern[i];
            concept->patternVariance[i] += diff * diff;
        }
    }
    
    for (size_t i = 0; i < concept->patternVariance.size(); ++i) {
        concept->patternVariance[i] /= concept->instances.size();
    }
}

void ConceptFormation::updateConceptStability(size_t conceptId, SimulationStep currentTime) {
    auto* concept = getConcept(conceptId);
    if (!concept) return;
    
    ConceptStabilityMetrics& metrics = pImpl->stabilityMetrics_[conceptId];
    
    // Calculate stability metrics
    metrics.patternStability = 0.0f;
    metrics.instanceStability = 0.0f;
    metrics.rewardStability = 0.0f;
    metrics.temporalStability = 0.0f;
    
    if (!concept->instances.empty()) {
        // Pattern stability: average similarity to prototype
        float totalPatternSim = 0.0f;
        size_t validInstances = 0;
        
        for (const auto& instance : concept->instances) {
            float sim = computeSimilarity(instance.pattern, concept->prototype, defaultSimilarityMetric_);
            totalPatternSim += sim;
            validInstances++;
        }
        
        if (validInstances > 0) {
            metrics.patternStability = totalPatternSim / validInstances;
        }
        
        // Instance stability: variance of instance patterns
        metrics.instanceStability = 1.0f - std::sqrt(concept->patternVariance[0]);
        
        // Reward stability: consistency of rewards
        if (concept->instances.size() > 1) {
            float meanReward = 0.0f;
            for (const auto& instance : concept->instances) {
                meanReward += instance.avgReward;
            }
            meanReward /= concept->instances.size();
            
            float rewardVar = 0.0f;
            for (const auto& instance : concept->instances) {
                float diff = instance.avgReward - meanReward;
                rewardVar += diff * diff;
            }
            rewardVar /= concept->instances.size();
            
            metrics.rewardStability = 1.0f / (1.0f + std::sqrt(rewardVar));
        }
        
        // Temporal stability: persistence over time
        if (currentTime > concept->creationTime) {
            float age = static_cast<float>(currentTime - concept->creationTime);
            float decayFactor = std::exp(-age / 100.0f);
            metrics.temporalStability = 1.0f * decayFactor;
        }
        
        // Calculate overall stability
        metrics.overallStability = (metrics.patternStability + metrics.instanceStability +
                                   metrics.rewardStability + metrics.temporalStability) / 4.0f;
    }
    
    // Update concept's average stability
    concept->avgStability = metrics.overallStability;
    concept->lastStabilityUpdate = currentTime;
}

void ConceptFormation::measureTemporalStability(size_t conceptId, SimulationStep currentTime) {
    auto* concept = getConcept(conceptId);
    if (!concept) return;
    
    // Check if concept has been observed consistently over time
    float timeSinceCreation = static_cast<float>(currentTime - concept->creationTime);
    
    // If concept is young, it can't be very stable
    if (timeSinceCreation < 10.0f) {
        return;
    }
    
    // Calculate temporal stability based on observation consistency
    float stability = concept->avgStability;
    
    // Decay stability slowly over time to encourage concept updating
    stability *= std::exp(-timeSinceCreation / 1000.0f);
    
    concept->avgStability = stability;
}

float ConceptFormationUtils::euclideanDistance(const std::vector<float>& a, const std::vector<float>& b) {
    if (a.size() != b.size()) return std::numeric_limits<float>::max();
    
    float sum = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        float diff = a[i] - b[i];
        sum += diff * diff;
    }
    
    return std::sqrt(sum);
}

float ConceptFormationUtils::cosineSimilarity(const std::vector<float>& a, const std::vector<float>& b) {
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

float ConceptFormationUtils::mahalanobisDistance(const std::vector<float>& a, const std::vector<float>& b,
                                               const std::vector<float>& mean,
                                               const std::vector<std::vector<float>>& cov) {
    if (a.size() != b.size() || a.size() != mean.size() || a.empty()) {
        return euclideanDistance(a, b);
    }
    
    // Simple implementation: use regularized inverse covariance
    // In practice, this would use proper matrix inversion
    
    float sum = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        float diff = a[i] - b[i];
        
        // Use diagonal covariance for stability
        float var = (i < cov.size() && i < cov[i].size()) ? cov[i][i] : 1.0f;
        if (var < 0.001f) var = 0.001f;
        
        sum += (diff * diff) / var;
    }
    
    return std::sqrt(sum);
}

std::vector<float> ConceptFormationUtils::computeMean(const std::vector<std::vector<float>>& patterns) {
    if (patterns.empty()) return {};
    
    size_t dim = patterns[0].size();
    std::vector<float> mean(dim, 0.0f);
    
    for (const auto& pattern : patterns) {
        for (size_t i = 0; i < dim && i < pattern.size(); ++i) {
            mean[i] += pattern[i];
        }
    }
    
    for (size_t i = 0; i < mean.size(); ++i) {
        mean[i] /= patterns.size();
    }
    
    return mean;
}

std::vector<std::vector<float>> ConceptFormationUtils::computeCovariance(
    const std::vector<std::vector<float>>& patterns,
    const std::vector<float>& mean) {
    if (patterns.empty() || mean.empty()) return {};
    
    size_t dim = mean.size();
    std::vector<std::vector<float>> cov(dim, std::vector<float>(dim, 0.0f));
    
    for (const auto& pattern : patterns) {
        for (size_t i = 0; i < dim && i < pattern.size(); ++i) {
            float diff = pattern[i] - mean[i];
            for (size_t j = 0; j < dim && j < pattern.size(); ++j) {
                float diff2 = pattern[j] - mean[j];
                cov[i][j] += diff * diff2;
            }
        }
    }
    
    for (size_t i = 0; i < cov.size(); ++i) {
        for (size_t j = 0; j < cov[i].size(); ++j) {
            cov[i][j] /= patterns.size();
        }
    }
    
    return cov;
}

float ConceptFormationUtils::computePatternVariance(const std::vector<float>& pattern, const std::vector<float>& mean) {
    if (pattern.size() != mean.size() || pattern.empty()) return 0.0f;
    
    float variance = 0.0f;
    for (size_t i = 0; i < pattern.size(); ++i) {
        float diff = pattern[i] - mean[i];
        variance += diff * diff;
    }
    
    return variance / pattern.size();
}

bool ConceptFormationUtils::isPatternSimilar(const std::vector<float>& a, const std::vector<float>& b,
                                            float threshold, int metric) {
    if (a.size() != b.size() || a.empty()) return false;
    
    float similarity = 0.0f;
    if (metric == 1) { // Euclidean (inverted)
        float dist = euclideanDistance(a, b);
        similarity = 1.0f / (1.0f + dist);
    } else if (metric == 2) { // Mahalanobis (inverted)
        similarity = 1.0f / (1.0f + mahalanobisDistance(a, b, std::vector<float>(a.size(), 0.0f),
                                                      std::vector<std::vector<float>>(a.size(), std::vector<float>(a.size(), 1.0f)));
    } else { // Cosine
        similarity = cosineSimilarity(a, b);
    }
    
    return similarity >= threshold;
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