#include "ConceptFormation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <set>
#include <unordered_map>
#include <queue>
#include <limits>

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
    , maxClusters_(10)
{
    // Initialize concept clustering structures
    clusterAssignments_.clear();
    conceptCenters_.clear();
}

ConceptFormation::~ConceptFormation() = default;

void ConceptFormation::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Initialize concept activation levels
    for (auto& concept : concepts_) {
        concept.activationLevel = 0.0f;
        concept.lastActivationTime = 0;
        concept.recentActivationCount = 0;
    }
    
    // Initialize concept clustering structures
    clusterAssignments_.clear();
    conceptCenters_.clear();
    
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
        updateConceptActivation(matchingConcept, currentTime);
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
    concept.activationLevel = 1.0f;
    concept.lastActivationTime = 0;
    concept.recentActivationCount = 1;
    
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
    
    // Add new instance
    ConceptInstance instance;
    instance.pattern = newPattern;
    instance.features = features;
    instance.observationCount = 1;
    instance.lastObserved = concept->totalObservations;
    instance.avgReward = reward;
    
    concept->instances.push_back(instance);
    concept->totalObservations++;
    
    // Update prototype using exponential moving average (Hebbian-like)
    updatePrototype(conceptId, newPattern);
    
    // Update stability
    float newStability = 1.0f;
    if (concept->instances.size() > 1) {
        // Compute stability as average similarity to prototype
        float totalSim = 0.0f;
        size_t recentCount = std::min(concept->instances.size(), stabilityWindow_);
        
        for (size_t i = concept->instances.size() - recentCount; 
             i < concept->instances.size(); ++i) {
            totalSim += computeSimilarity(concept->instances[i].pattern, concept->prototype);
        }
        
        newStability = totalSim / recentCount;
        concept->avgStability = concept->avgStability * 0.9f + newStability * 0.1f;
    }
    
    // Update instance stability
    concept->instances.back().stability = newStability;
}

void ConceptFormation::updatePrototype(size_t conceptId, const std::vector<float>& newInstance) {
    for (auto& concept : concepts_) {
        if (concept.id == conceptId) {
            // Hebbian update: weight prototype towards new instance
            float alpha = 0.2f;  // Learning rate
            
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

void ConceptFormation::updateConceptActivation(size_t conceptId, SimulationStep currentTime) {
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (!concept) return;
    
    // Increase activation
    concept->activationLevel = std::min(1.0f, concept->activationLevel + 0.1f);
    concept->lastActivationTime = currentTime;
    concept->recentActivationCount++;
    
    // Apply decay if concept not recently activated
    if (currentTime - concept->lastActivationTime > stabilityWindow_) {
        concept->activationLevel *= 0.95f;
    }
}

void ConceptFormation::clusterConcepts() {
    clusters_.clear();
    clusterAssignments_.assign(concepts_.size(), 0);
    conceptCenters_.clear();
    
    if (concepts_.size() < 2 || maxClusters_ <= 1) {
        // No clustering needed
        for (size_t i = 0; i < concepts_.size(); ++i) {
            clusterAssignments_[i] = 0;
            conceptCenters_.push_back(concepts_[i].prototype);
        }
        return;
    }
    
    // Simple k-means clustering based on concept similarity
    size_t numClusters = std::min(maxClusters_, concepts_.size());
    
    // Initialize cluster centers randomly
    std::vector<size_t> centerIndices;
    std::vector<size_t> usedIndices;
    
    for (size_t i = 0; i < numClusters && i < concepts_.size(); ++i) {
        size_t idx;
        do {
            idx = getRandomGenerator() ? getRandomGenerator()->uniformInt(0, concepts_.size() - 1) : 0;
        } while (std::find(usedIndices.begin(), usedIndices.end(), idx) != usedIndices.end());
        
        usedIndices.push_back(idx);
        centerIndices.push_back(idx);
        
        Cluster cluster;
        cluster.centerConceptId = concepts_[idx].id;
        cluster.centerPrototype = concepts_[idx].prototype;
        cluster.radius = 0.0f;
        
        clusters_.push_back(cluster);
    }
    
    // Assign each concept to nearest cluster
    for (size_t i = 0; i < concepts_.size(); ++i) {
        float minDistance = std::numeric_limits<float>::max();
        size_t bestCluster = 0;
        
        for (size_t j = 0; j < clusters_.size(); ++j) {
            float distance = computeSimilarity(concepts_[i].prototype, clusters_[j].centerPrototype);
            if (distance < minDistance) {
                minDistance = distance;
                bestCluster = j;
            }
        }
        
        clusterAssignments_[i] = bestCluster;
        clusters_[bestCluster].conceptIds.push_back(concepts_[i].id);
        clusters_[bestCluster].radius = std::max(clusters_[bestCluster].radius, 1.0f - minDistance);
    }
    
    // Update cluster centers as mean of assigned concepts
    for (size_t i = 0; i < clusters_.size(); ++i) {
        if (!clusters_[i].conceptIds.empty()) {
            std::vector<float> newCenter(concepts_[0].prototype.size(), 0.0f);
            size_t count = 0;
            
            for (size_t conceptId : clusters_[i].conceptIds) {
                const DiscoveredConcept* concept = getConcept(conceptId);
                if (concept) {
                    for (size_t j = 0; j < concept->prototype.size() && j < newCenter.size(); ++j) {
                        newCenter[j] += concept->prototype[j];
                    }
                    count++;
                }
            }
            
            if (count > 0) {
                for (size_t j = 0; j < newCenter.size(); ++j) {
                    newCenter[j] /= count;
                }
                clusters_[i].centerPrototype = newCenter;
            }
        }
    }
}

size_t ConceptFormation::getMatchingConcept(const std::vector<float>& pattern,
                                          float similarityThreshold) const {
    return findConceptForPattern(pattern);
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
    
    // Update associated concepts
    for (size_t associatedId : b->associatedConceptIds) {
        if (std::find(a->associatedConceptIds.begin(), a->associatedConceptIds.end(), associatedId) == a->associatedConceptIds.end()) {
            a->associatedConceptIds.push_back(associatedId);
        }
    }
    
    // Update category hint based on merged properties
    if ((a->categoryHint == "rewarding" && b->categoryHint == "rewarding") ||
        (a->categoryHint == "harmful" && b->categoryHint == "harmful")) {
        a->categoryHint = a->categoryHint;
    } else {
        a->categoryHint = "mixed";
    }
    
    // Remove B
    concepts_.erase(
        std::remove_if(concepts_.begin(), concepts_.end(),
            [conceptB](const DiscoveredConcept& c) { return c.id == conceptB; }),
        concepts_.end()
    );
    
    // Update cluster assignments
    for (size_t& assignment : clusterAssignments_) {
        if (assignment == conceptB) {
            assignment = conceptA;
        }
    }
    
    NLM_LOG_INFO("ConceptFormation: Merged concepts " + std::to_string(conceptA) +
                 " and " + std::to_string(conceptB));
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
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
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

float ConceptFormation::getConceptActivation(size_t conceptId) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (concept) {
        return concept->activationLevel;
    }
    return 0.0f;
}

size_t ConceptFormation::getConceptCluster(size_t conceptId) const {
    for (size_t i = 0; i < concepts_.size(); ++i) {
        if (concepts_[i].id == conceptId) {
            return clusterAssignments_[i];
        }
    }
    return 0;
}

float ConceptFormation::getGeneralizationAbility(size_t conceptId) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept) return 0.0f;
    
    // Generalization ability = how varied are the instances
    if (concept->instances.size() < 2) return 0.0f;
    
    float totalVariance = 0.0f;
    for (const auto& instance : concept->instances) {
        float sim = computeSimilarity(instance.pattern, concept->prototype);
        totalVariance += (1.0f - sim);
    }
    
    return 1.0f - (totalVariance / concept->instances.size());
}

void ConceptFormation::clear() {
    concepts_.clear();
    nextConceptId_ = 1;
    clusterAssignments_.clear();
    conceptCenters_.clear();
    clusters_.clear();
}

void ConceptFormation::integrateWithNeuralPlanner(size_t conceptId) {
    if (!brain_ || !brain_->getPlanner()) return;
    
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept) return;
    
    // Use concept to influence planning
    // For example, if concept is "dangerous", reduce planning depth
    // if concept is "rewarding", increase planning depth
    
    NeuralPlanner* planner = brain_->getPlanner();
    
    if (concept->categoryHint == "rewarding") {
        // Increase planning depth for rewarding concepts
        size_t currentDepth = planner->getPlanningDepth();
        planner->setPlanningDepth(std::min(currentDepth + 1, 5));
    } else if (concept->categoryHint == "harmful") {
        // Decrease planning depth for harmful concepts
        size_t currentDepth = planner->getPlanningDepth();
        planner->setPlanningDepth(std::max(currentDepth - 1, 1));
    }
}

void ConceptFormation::linkToPredictionSystem(size_t conceptId) {
    if (!brain_ || !brain_->getPredictionSystem()) return;
    
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept) return;
    
    // Use concept to improve predictions
    // For example, if concept is "predator", predict predator behavior
    
    PredictionSystem* predictionSystem = brain_->getPredictionSystem();
    
    // Update prediction system with concept-based knowledge
    // This would be implemented in a full version
}

void ConceptFormation::connectToWorkingMemory(size_t conceptId) {
    if (!brain_ || !brain_->getWorkingMemory()) return;
    
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept) return;
    
    NeuralWorkingMemory* workingMemory = brain_->getWorkingMemory();
    
    // Connect concept to working memory for active maintenance
    // If concept is highly activated, maintain it in working memory
    
    if (concept->activationLevel > 0.7f) {
        std::vector<float> conceptPattern = getConceptPrototype(conceptId);
        if (!conceptPattern.empty()) {
            workingMemory->store(conceptPattern, concept->activationLevel);
        }
    }
}

RandomGenerator* ConceptFormation::getRandomGenerator() const {
    return brain_ ? brain_->getRandomGenerator() : nullptr;
}

} // namespace nlm
