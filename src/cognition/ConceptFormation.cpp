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
    , nextConceptId_(0)
    , formationThreshold_(0.7f)
    , stabilityThreshold_(0.8f)
    , stabilityWindow_(20)
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
    // Check if pattern belongs to existing concept
    size_t existingConcept = getMatchingConcept(pattern);
    
    if (existingConcept != 0) {
        // Update existing concept
        updateConcept(existingConcept, pattern, features, reward);
        return existingConcept;
    }
    
    // Check if concept is ready to be "discovered"
    bool conceptReady = false;
    for (const auto& concept : concepts_) {
        if (!isConceptStable(concept.id)) {
            float similarity = computeSimilarity(pattern, concept.prototype);
            if (similarity > formationThreshold_) {
                // Concept is being formed - update prototype and check stability
                updatePrototype(concept.id, pattern);
                ConceptInstance instance;
                instance.pattern = pattern;
                instance.features = features;
                instance.observationCount = 1;
                instance.lastObserved = currentTime;
                instance.avgReward = reward;
                instance.stability = 1.0f;
                
                concepts_[concept.id].instances.push_back(instance);
                
                if (isConceptStable(concept.id)) {
                    conceptReady = true;
                    break;
                }
            }
        }
    }
    
    if (conceptReady) {
        // This pattern belongs to an in-progress concept - use that concept
        for (const auto& concept : concepts_) {
            if (isConceptStable(concept.id)) {
                updateConcept(concept.id, pattern, features, reward);
                return concept.id;
            }
        }
    }
    
    // New concept
    return createConcept(pattern, features, reward);
}

size_t ConceptFormation::getMatchingConcept(const std::vector<float>& pattern,
                                          float similarityThreshold) const {
    size_t bestMatch = 0;
    float bestSimilarity = 0.0f;
    
    for (const auto& concept : concepts_) {
        float similarity = computeSimilarity(pattern, concept.prototype);
        if (similarity > bestSimilarity && similarity > similarityThreshold) {
            bestSimilarity = similarity;
            bestMatch = concept.id;
        }
    }
    
    return bestMatch;
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
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (concept) {
        return concept->instances;
    }
    static const std::vector<ConceptInstance> empty;
    return empty;
}

float ConceptFormation::getConceptStability(size_t conceptId) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept || concept->instances.empty()) {
        return 0.0f;
    }
    
    float totalStability = 0.0f;
    for (const auto& instance : concept->instances) {
        totalStability += instance.stability;
    }
    
    return totalStability / concept->instances.size();
}

void ConceptFormation::mergeConcepts(size_t conceptA, size_t conceptB) {
    DiscoveredConcept* ca = const_cast<DiscoveredConcept*>(getConcept(conceptA));
    DiscoveredConcept* cb = const_cast<DiscoveredConcept*>(getConcept(conceptB));
    
    if (!ca || !cb) return;
    
    // Merge instances
    for (const auto& instance : cb->instances) {
        ca->instances.push_back(instance);
    }
    
    // Update prototype with weighted average
    if (!ca->prototype.empty() && !cb->prototype.empty()) {
        size_t size = std::max(ca->prototype.size(), cb->prototype.size());
        for (size_t i = 0; i < size; ++i) {
            float valA = (i < ca->prototype.size()) ? ca->prototype[i] : 0.0f;
            float valB = (i < cb->prototype.size()) ? cb->prototype[i] : 0.0f;
            ca->prototype[i] = (valA + valB) * 0.5f;
        }
    }
    
    // Merge associations
    for (size_t assoc : cb->associatedConceptIds) {
        ca->associatedConceptIds.push_back(assoc);
    }
    
    ca->totalObservations += cb->totalObservations;
    ca->avgStability = (ca->avgStability + cb->avgStability) * 0.5f;
    
    // Remove concept B
    auto it = std::find_if(concepts_.begin(), concepts_.end(),
                         [conceptB](const DiscoveredConcept& c) { return c.id == conceptB; });
    if (it != concepts_.end()) {
        concepts_.erase(it);
    }
    
    // Update concept IDs after removal
    for (auto& concept : concepts_) {
        if (concept.id > conceptB && concept.id <= std::max(conceptA, conceptB)) {
            concept.id--;
        }
    }
    
    nextConceptId_ = std::max(conceptA, conceptB);
    
    NLM_LOG_INFO("Merged concept " + std::to_string(conceptB) + " into concept " + std::to_string(conceptA));
}

void ConceptFormation::updateConcept(size_t conceptId, const std::vector<float>& newPattern,
                                    const std::vector<float>& features, float reward) {
    DiscoveredConcept* concept = const_cast<DiscoveredConcept*>(getConcept(conceptId));
    if (!concept) return;
    
    // Add new instance
    ConceptInstance instance;
    instance.pattern = newPattern;
    instance.features = features;
    instance.observationCount++;
    instance.lastObserved = nextConceptId_;  // Using conceptId as timestamp proxy
    instance.avgReward = (instance.avgReward * (instance.observationCount - 1) + reward) / instance.observationCount;
    instance.stability = (instance.stability * (instance.observationCount - 1) + computeSimilarity(newPattern, concept->prototype)) / instance.observationCount;
    
    concept->instances.push_back(instance);
    concept->totalObservations++;
    
    // Update prototype
    updatePrototype(conceptId, newPattern);
    
    // Update stability
    if (instance.observationCount > stabilityWindow_) {
        float totalStability = 0.0f;
        for (const auto& instance : concept->instances) {
            totalStability += instance.stability;
        }
        concept->avgStability = totalStability / concept->instances.size();
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

bool ConceptFormation::isNovel(const std::vector<float>& pattern,
                               float similarityThreshold) const {
    return getMatchingConcept(pattern, similarityThreshold) == 0;
}

size_t ConceptFormation::findConceptForPattern(const std::vector<float>& pattern) const {
    return getMatchingConcept(pattern, formationThreshold_);
}

void ConceptFormation::clear() {
    concepts_.clear();
    nextConceptId_ = 0;
    NLM_LOG_INFO("ConceptFormation cleared all concepts");
}

float ConceptFormation::getGeneralizationAbility(size_t conceptId) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept || concept->instances.empty()) return 0.0f;
    
    // Measure variation in instances
    if (concept->instances.size() < 2) return 1.0f;
    
    float totalVariation = 0.0f;
    for (size_t i = 0; i < concept->instances.size(); ++i) {
        for (size_t j = i + 1; j < concept->instances.size(); ++j) {
            float similarity = computeSimilarity(concept->instances[i].pattern, concept->instances[j].pattern);
            totalVariation += (1.0f - similarity);
        }
    }
    
    float maxPossibleVariation = (concept->instances.size() * (concept->instances.size() - 1)) / 2.0f;
    
    return (maxPossibleVariation > 0.0f) ? (1.0f - totalVariation / maxPossibleVariation) : 1.0f;
}

size_t ConceptFormation::createConcept(const std::vector<float>& pattern,
                                       const std::vector<float>& features,
                                       float reward) {
    DiscoveredConcept concept;
    concept.id = nextConceptId_++;
    concept.prototype = pattern;
    concept.categoryHint = "unknown";  // Will be derived from features
    
    // Derive category hint from features if possible
    if (!features.empty()) {
        size_t maxFeatureIndex = 0;
        float maxFeature = 0.0f;
        for (size_t i = 0; i < features.size(); ++i) {
            if (features[i] > maxFeature) {
                maxFeature = features[i];
                maxFeatureIndex = i;
            }
        }
        
        if (maxFeature > 0.7f) {
            // Map feature index to category
            switch (maxFeatureIndex % 5) {
                case 0: concept.categoryHint = "rewarding"; break;
                case 1: concept.categoryHint = "harmless"; break;
                case 2: concept.categoryHint = "dangerous"; break;
                case 3: concept.categoryHint = "novel"; break;
                case 4: concept.categoryHint = "useful"; break;
            }
        }
    }
    
    // Add first instance
    ConceptInstance instance;
    instance.pattern = pattern;
    instance.features = features;
    instance.observationCount = 1;
    instance.lastObserved = 0;  // Using concept ID as proxy
    instance.avgReward = reward;
    instance.stability = 1.0f;
    
    concept.instances.push_back(instance);
    concept.totalObservations = 1;
    concept.avgStability = 1.0f;
    
    concepts_.push_back(concept);
    
    NLM_LOG_INFO("Created new concept " + std::to_string(concept.id) + " with category: " + concept.categoryHint);
    
    return concept.id;
}

void ConceptFormation::updatePrototype(size_t conceptId, const std::vector<float>& newInstance) {
    DiscoveredConcept* concept = const_cast<DiscoveredConcept*>(getConcept(conceptId));
    if (!concept) return;
    
    if (concept->prototype.empty()) {
        concept->prototype = newInstance;
        return;
    }
    
    // Hebbian averaging - combine old prototype with new instance
    size_t size = std::max(concept->prototype.size(), newInstance.size());
    for (size_t i = 0; i < size; ++i) {
        float oldVal = (i < concept->prototype.size()) ? concept->prototype[i] : 0.0f;
        float newVal = (i < newInstance.size()) ? newInstance[i] : 0.0f;
        concept->prototype[i] = oldVal * 0.9f + newVal * 0.1f;
    }
}

bool ConceptFormation::isConceptStable(size_t conceptId) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept || concept->instances.empty()) return false;
    
    // Check stability through consistency
    if (concept->instances.size() < stabilityWindow_) return false;
    
    float totalStability = 0.0f;
    for (const auto& instance : concept->instances) {
        totalStability += instance.stability;
    }
    
    float avgStability = totalStability / concept->instances.size();
    
    return avgStability >= stabilityThreshold_;
}

} // namespace nlm
