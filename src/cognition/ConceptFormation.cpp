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

size_t ConceptFormation::presentExperience(const std::vector<float>& pattern,
                                           const std::vector<float>& features,
                                           float reward,
                                           SimulationStep currentTime) {
    if (pattern.empty()) return 0;
    
    // Check if this pattern matches any existing concept
    size_t matchingConcept = findConceptForPattern(pattern);
    
    if (matchingConcept > 0) {
        // This pattern belongs to an existing concept - reinforce it
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
    
    // Update prototype using Hebbian averaging
    updatePrototype(conceptId, newPattern);
    
    // Create new instance
    ConceptInstance instance;
    instance.pattern = newPattern;
    instance.features = features;
    instance.observationCount = 1;
    instance.lastObserved = 0;
    instance.avgReward = reward;
    instance.stability = 1.0f;
    
    concept->instances.push_back(instance);
    concept->totalObservations++;
    
    // Update average stability
    float totalStability = 0.0f;
    for (const auto& inst : concept->instances) {
        totalStability += inst.stability;
    }
    concept->avgStability = totalStability / concept->instances.size();
}

size_t ConceptFormation::findConceptForPattern(const std::vector<float>& pattern) const {
    // Find the concept that best matches this pattern
    size_t bestMatch = 0;
    float bestSimilarity = 0.0f;
    
    for (const auto& concept : concepts_) {
        float similarity = computeSimilarity(pattern, concept.prototype);
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestMatch = concept.id;
        }
    }
    
    return (bestSimilarity > 0.7f) ? bestMatch : 0;
}

bool ConceptFormation::isNovel(const std::vector<float>& pattern,
                              float similarityThreshold) const {
    for (const auto& concept : concepts_) {
        float similarity = computeSimilarity(pattern, concept.prototype);
        if (similarity > similarityThreshold) {
            return false;
        }
    }
    return true;
}

size_t ConceptFormation::getMatchingConcept(const std::vector<float>& pattern,
                                           float similarityThreshold) const {
    for (const auto& concept : concepts_) {
        float similarity = computeSimilarity(pattern, concept.prototype);
        if (similarity > similarityThreshold) {
            return concept.id;
        }
    }
    return 0;
}

void ConceptFormation::mergeConcepts(size_t conceptA, size_t conceptB) {
    // Merge concept B into concept A
    auto itB = std::find_if(concepts_.begin(), concepts_.end(), [conceptB](const DiscoveredConcept& c) {
        return c.id == conceptB;
    });
    
    if (itB == concepts_.end()) return;
    
    auto itA = std::find_if(concepts_.begin(), concepts_.end(), [conceptA](const DiscoveredConcept& c) {
        return c.id == conceptA;
    });
    
    if (itA == concepts_.end()) return;
    
    // Merge instances
    for (const auto& instance : itB->instances) {
        itA->instances.push_back(instance);
    }
    
    // Update prototype
    updatePrototype(conceptA, itB->prototype);
    
    // Update total observations
    itA->totalObservations += itB->totalObservations;
    
    // Update average stability
    float totalStability = 0.0f;
    for (const auto& inst : itA->instances) {
        totalStability += inst.stability;
    }
    itA->avgStability = totalStability / itA->instances.size();
    
    // Remove concept B
    concepts_.erase(itB);
    
    NLM_LOG_INFO("ConceptFormation: Merged concept " + std::to_string(conceptB) + " into concept " + std::to_string(conceptA));
}

void ConceptFormation::updatePrototype(size_t conceptId, const std::vector<float>& newInstance) {
    for (auto& concept : concepts_) {
        if (concept.id == conceptId) {
            // Hebbian update of prototype
            for (size_t i = 0; i < concept.prototype.size() && i < newInstance.size(); ++i) {
                concept.prototype[i] = 0.9f * concept.prototype[i] + 0.1f * newInstance[i];
            }
            break;
        }
    }
}

bool ConceptFormation::isConceptStable(size_t conceptId) const {
    for (const auto& concept : concepts_) {
        if (concept.id == conceptId) {
            // Check if concept has enough instances and stability
            if (concept.instances.size() < stabilityWindow_) {
                return false;
            }
            
            // Calculate average stability of instances
            float totalStability = 0.0f;
            for (const auto& instance : concept.instances) {
                totalStability += instance.stability;
            }
            
            float avgStability = totalStability / concept.instances.size();
            return avgStability > stabilityThreshold_;
        }
    }
    return false;
}

void ConceptFormation::clear() {
    concepts_.clear();
    NLM_LOG_INFO("ConceptFormation: Cleared all concepts");
}

float ConceptFormation::getConceptStability(size_t conceptId) const {
    for (const auto& concept : concepts_) {
        if (concept.id == conceptId) {
            // Calculate average stability of instances
            if (concept.instances.empty()) return 0.0f;
            
            float totalStability = 0.0f;
            for (const auto& instance : concept.instances) {
                totalStability += instance.stability;
            }
            
            return totalStability / concept.instances.size();
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
    for (const auto& concept : concepts_) {
        if (concept.id == conceptId) {
            return concept.prototype;
        }
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

float ConceptFormation::getGeneralizationAbility(size_t conceptId) const {
    for (const auto& concept : concepts_) {
        if (concept.id == conceptId) {
            // Generalization ability is based on prototype stability
            // and diversity of instances
            if (concept.instances.empty()) return 0.0f;
            
            // Calculate average distance of instances from prototype
            float totalDistance = 0.0f;
            size_t validInstances = 0;
            
            for (const auto& instance : concept.instances) {
                float dist = 1.0f - computeSimilarity(instance.pattern, concept.prototype);
                totalDistance += dist;
                validInstances++;
            }
            
            if (validInstances == 0) return 0.0f;
            
            float avgDistance = totalDistance / validInstances;
            
            // Lower average distance means better generalization (more similar instances)
            return 1.0f - avgDistance;
        }
    }
    return 0.0f;
}

void ConceptFormation::setNewConcepts(const std::vector<size_t>& concepts) {
    newConcepts_ = concepts;
}

void ConceptFormation::processExperience(const std::vector<float>& pattern,
                                        const std::vector<NeuronId>& attentionWinners,
                                        SimulationStep currentTime,
                                        float neuromodulationLevel) {
    if (pattern.empty() || !brain_) return;
    
    // Determine if this is a novel pattern or belongs to existing concepts
    size_t conceptId = findConceptForPattern(pattern);
    
    if (conceptId > 0) {
        // This pattern belongs to an existing concept - reinforce it
        updateConcept(conceptId, pattern, std::vector<float>(pattern.size(), 0), neuromodulationLevel);
        
        // Record observation
        for (auto& concept : concepts_) {
            if (concept.id == conceptId) {
                for (auto& instance : concept.instances) {
                    instance.observationCount++;
                    instance.lastObserved = currentTime;
                    instance.stability = std::min(1.0f, instance.stability * 0.95f);  // Decrease with each observation
                }
                break;
            }
        }
    } else {
        // This is a novel pattern - create new concept if significantly different
        if (isNovel(pattern, formationThreshold_)) {
            size_t newId = createConcept(pattern, std::vector<float>(pattern.size(), 0), neuromodulationLevel);
            newConcepts_.push_back(newId);
        }
    }
}

std::vector<size_t> ConceptFormation::getNewConcepts() const {
    return newConcepts_;
}

} // namespace nlm
