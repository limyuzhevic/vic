#include "ConceptFormation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <map>

namespace nlm {

struct ConceptFormation::Impl {
    Brain* brain;
    
    // Random generator for stochastic concept discovery
    std::mt19937 rng;
    
    // Neural similarity function (cosine similarity)
    float computeCosineSimilarity(const std::vector<float>& a, const std::vector<float>& b) const {
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
    
    // Current simulation step for temporal tracking
    SimulationStep currentStep;
    
    Impl() : brain(nullptr), rng(std::random_device{}()), currentStep(0) {}
};

ConceptFormation::ConceptFormation()
    : pImpl(new Impl)
    , brain_(nullptr)
    , nextConceptId_(0)
    , formationThreshold_(0.7f)
    , stabilityThreshold_(0.6f)
    , stabilityWindow_(10)
{
    std::random_device rd;
    pImpl->rng.seed(rd());
}

ConceptFormation::~ConceptFormation() = default;

void ConceptFormation::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    clear();
    
    NLM_LOG_INFO("ConceptFormation initialized");
}

size_t ConceptFormation::presentExperience(const std::vector<float>& pattern,
                             const std::vector<float>& features,
                             float reward,
                             SimulationStep currentTime) {
    if (!brain_ || pattern.empty()) {
        return 0;
    }
    
    // First check if pattern belongs to existing concept
    size_t matchingConceptId = getMatchingConcept(pattern);
    
    if (matchingConceptId > 0 && features.size() > 0) {
        // Update existing concept
        updateConcept(matchingConceptId, pattern, features, reward);
        return matchingConceptId;
    }
    
    // Check if it's novel enough to form a new concept
    if (isNovel(pattern)) {
        size_t newConceptId = createConcept(pattern, features, reward);
        
        // Update concept with this first instance
        updateConcept(newConceptId, pattern, features, reward);
        
        return newConceptId;
    }
    
    // Too similar to existing concepts - ignore for now
    return 0;
}

size_t ConceptFormation::getMatchingConcept(const std::vector<float>& pattern,
                                          float similarityThreshold) const {
    if (pattern.empty()) return 0;
    
    size_t bestMatch = 0;
    float bestSimilarity = 0.0f;
    
    for (const auto& concept : concepts_) {
        // Compare with concept prototype
        float similarity = pImpl->computeCosineSimilarity(pattern, concept.prototype);
        
        if (similarity > bestSimilarity && similarity >= similarityThreshold) {
            bestSimilarity = similarity;
            bestMatch = concept.id;
        }
    }
    
    return bestMatch;
}

void ConceptFormation::updateConcept(size_t conceptId, const std::vector<float>& newPattern,
                                   const std::vector<float>& features, float reward) {
    for (auto& concept : concepts_) {
        if (concept.id == conceptId) {
            // Find matching instance to update
            bool foundInstance = false;
            
            for (auto& instance : concept.instances) {
                float similarity = pImpl->computeCosineSimilarity(newPattern, instance.pattern);
                if (similarity > 0.8f) {
                    // Update existing instance
                    instance.pattern = newPattern;
                    instance.features = features;
                    instance.observationCount++;
                    instance.avgReward = (instance.avgReward * (instance.observationCount - 1) + reward) / instance.observationCount;
                    foundInstance = true;
                    break;
                }
            }
            
            if (!foundInstance && concept.instances.size() < 5) {
                // Add new instance
                ConceptInstance newInstance;
                newInstance.pattern = newPattern;
                newInstance.features = features;
                newInstance.observationCount = 1;
                newInstance.lastObserved = currentStep_;
                newInstance.avgReward = reward;
                newInstance.stability = 0.5f;
                
                concept.instances.push_back(newInstance);
            }
            
            // Update prototype using Hebbian averaging
            updatePrototype(conceptId, newPattern);
            
            // Update concept stability
            if (isConceptStable(conceptId)) {
                concept.avgStability = 1.0f;
            }
            
            NLM_LOG_INFO("Updated concept " + std::to_string(conceptId) + 
                        ", similarity = " + std::to_string(pImpl->computeCosineSimilarity(newPattern, concept.prototype)));
            return;
        }
    }
}

void ConceptFormation::updatePrototype(size_t conceptId, const std::vector<float>& newInstance) {
    for (auto& concept : concepts_) {
        if (concept.id == conceptId) {
            if (concept.instances.empty()) return;
            
            // New prototype is weighted average of existing instances and new instance
            size_t totalInstances = concept.instances.size() + 1;
            size_t oldCount = concept.instances.size();
            
            // Create new prototype
            std::vector<float> newPrototype;
            if (!concept.prototype.empty()) {
                newPrototype.resize(concept.prototype.size());
                
                // Copy old prototype (weighted less)
                for (size_t i = 0; i < concept.prototype.size(); ++i) {
                    newPrototype[i] = concept.prototype[i] * (oldCount * 0.5f);
                }
                
                // Add new instance contribution
                for (size_t i = 0; i < newInstance.size() && i < newPrototype.size(); ++i) {
                    newPrototype[i] += newInstance[i] * 0.5f;
                }
                
                // Normalize
                float norm = 0.0f;
                for (float val : newPrototype) norm += val * val;
                if (norm > 0.0f) {
                    float invNorm = 1.0f / std::sqrt(norm);
                    for (float& val : newPrototype) val *= invNorm;
                }
            } else {
                newPrototype = newInstance;
            }
            
            concept.prototype = newPrototype;
            
            return;
        }
    }
}

bool ConceptFormation::isConceptStable(size_t conceptId) const {
    for (const auto& concept : concepts_) {
        if (concept.id == conceptId) {
            if (concept.instances.size() < stabilityWindow_) {
                return false;
            }
            
            // Calculate average stability of instances
            float totalStability = 0.0f;
            for (const auto& instance : concept.instances) {
                totalStability += instance.stability;
            }
            
            float avgStability = totalStability / concept.instances.size();
            return avgStability >= stabilityThreshold_;
        }
    }
    
    return false;
}

size_t ConceptFormation::createConcept(const std::vector<float>& pattern,
                               const std::vector<float>& features,
                               float reward) {
    DiscoveredConcept newConcept;
    newConcept.id = nextConceptId_++;
    newConcept.prototype = pattern;
    newConcept.categoryHint = "novel";  // Could be derived from features
    newConcept.totalObservations = 1;
    newConcept.avgStability = 0.5f;
    
    concepts_.push_back(newConcept);
    
    NLM_LOG_INFO("Created new concept " + std::to_string(newConcept.id) + 
                " with similarity threshold " + std::to_string(formationThreshold_));
    
    return newConcept.id;
}

void ConceptFormation::mergeConcepts(size_t conceptA, size_t conceptB) {
    if (conceptA == conceptB) return;
    
    // Find the two concepts
    auto itA = std::find_if(concepts_.begin(), concepts_.end(),
                          [conceptA](const DiscoveredConcept& c) { return c.id == conceptA; });
    auto itB = std::find_if(concepts_.begin(), concepts_.end(),
                          [conceptB](const DiscoveredConcept& c) { return c.id == conceptB; });
    
    if (itA == concepts_.end() || itB == concepts_.end()) return;
    
    // Merge concept B into concept A
    itA->instances.insert(itA->instances.end(), itB->instances.begin(), itB->instances.end());
    itA->associatedConceptIds.insert(itA->associatedConceptIds.end(), 
                                    itB->associatedConceptIds.begin(), itB->associatedConceptIds.end());
    
    // Update prototype
    updatePrototype(conceptA, itB->prototype);
    
    // Update total observations
    itA->totalObservations += itB->totalObservations;
    
    // Calculate new average stability
    float totalStability = 0.0f;
    size_t totalInstances = 0;
    for (const auto& instance : itA->instances) {
        totalStability += instance.stability;
        totalInstances++;
    }
    
    if (totalInstances > 0) {
        itA->avgStability = totalStability / totalInstances;
    }
    
    // Remove concept B
    concepts_.erase(itB);
    
    NLM_LOG_INFO("Merged concept " + std::to_string(conceptB) + " into " + std::to_string(conceptA));
}

void ConceptFormation::clear() {
    concepts_.clear();
    nextConceptId_ = 0;
}

float ConceptFormation::computeSimilarity(const std::vector<float>& a,
                                        const std::vector<float>& b) const {
    return pImpl->computeCosineSimilarity(a, b);
}

bool ConceptFormation::isNovel(const std::vector<float>& pattern,
                             float similarityThreshold) const {
    return getMatchingConcept(pattern, similarityThreshold) == 0;
}

size_t ConceptFormation::findConceptForPattern(const std::vector<float>& pattern) const {
    return getMatchingConcept(pattern);
}

float ConceptFormation::getConceptStability(size_t conceptId) const {
    for (const auto& concept : concepts_) {
        if (concept.id == conceptId) {
            return concept.avgStability;
        }
    }
    
    return 0.0f;
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
    const auto* concept = getConcept(conceptId);
    if (concept) {
        return concept->prototype;
    }
    return std::vector<float>();
}

const std::vector<ConceptInstance>& ConceptFormation::getConceptInstances(size_t conceptId) const {
    static const std::vector<ConceptInstance> empty;
    
    for (const auto& concept : concepts_) {
        if (concept.id == conceptId) {
            return concept.instances;
        }
    }
    
    return empty;
}

float ConceptFormation::getGeneralizationAbility(size_t conceptId) const {
    const auto* concept = getConcept(conceptId);
    if (!concept || concept->instances.empty()) return 0.0f;
    
    // Calculate generalization ability as average similarity between instances
    float totalSimilarity = 0.0f;
    size_t similarityCount = 0;
    
    for (size_t i = 0; i < concept->instances.size(); ++i) {
        for (size_t j = i + 1; j < concept->instances.size(); ++j) {
            float similarity = pImpl->computeCosineSimilarity(concept->instances[i].pattern,
                                                            concept->instances[j].pattern);
            totalSimilarity += similarity;
            similarityCount++;
        }
    }
    
    if (similarityCount == 0) return 0.0f;
    
    return totalSimilarity / similarityCount;
}

} // namespace nlm