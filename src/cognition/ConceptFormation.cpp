// ConceptFormation implementation
#include "ConceptFormation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct ConceptFormation::Impl {
    float similarityThreshold;
    float stabilityThreshold;
    size_t stabilityWindow;
    size_t conceptMemorySize;
    
    Impl() : similarityThreshold(0.7f), stabilityThreshold(0.8f), stabilityWindow(10), conceptMemorySize(100) {}
};

ConceptFormation::ConceptFormation()
    : pImpl(new Impl)
    , brain_(nullptr)
    , nextConceptId_(1)
    , formationThreshold_(0.7f)
    , stabilityThreshold_(0.8f)
    , stabilityWindow_(10)
{
    pImpl->similarityThreshold = formationThreshold_;
    pImpl->stabilityThreshold = stabilityThreshold_;
    pImpl->stabilityWindow = stabilityWindow_;
}

ConceptFormation::~ConceptFormation() = default;

void ConceptFormation::initialize(Brain* brain) {
    pImpl = std::make_unique<Impl>();
    pImpl->similarityThreshold = formationThreshold_;
    pImpl->stabilityThreshold = stabilityThreshold_;
    pImpl->stabilityWindow = stabilityWindow_;
    
    brain_ = brain;
    NLM_LOG_INFO("ConceptFormation initialized");
}

size_t ConceptFormation::presentExperience(const std::vector<float>& pattern,
                                           const std::vector<float>& features,
                                           float reward,
                                           SimulationStep currentTime) {
    if (pattern.empty()) return 0;
    
    // Check if this matches an existing concept
    size_t matchingConcept = getMatchingConcept(pattern);
    
    if (matchingConcept > 0) {
        // Update existing concept
        updateConcept(matchingConcept, pattern, features, reward);
        return matchingConcept;
    }
    
    // Check if it's too early to tell
    if (concepts_.empty()) {
        return 0;  // Too early to form concepts
    }
    
    // Check if it's novel (doesn't belong to any concept)
    if (isNovel(pattern)) {
        // Create new concept
        size_t newConceptId = createConcept(pattern, features, reward);
        return newConceptId;
    }
    
    return 0;  // Belongs to some concept but below threshold
}

size_t ConceptFormation::getMatchingConcept(const std::vector<float>& pattern,
                                           float similarityThreshold) const {
    if (pattern.empty() || concepts_.empty()) return 0;
    
    float bestSimilarity = 0.0f;
    size_t bestMatch = 0;
    
    for (const auto& concept : concepts_) {
        float similarity = computeSimilarity(pattern, concept.prototype);
        
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestMatch = concept.id;
        }
    }
    
    if (bestSimilarity >= similarityThreshold) {
        return bestMatch;
    }
    
    return 0;
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
    if (!concept) return {};
    return concept->prototype;
}

const std::vector<ConceptInstance>& ConceptFormation::getConceptInstances(size_t conceptId) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept) return {};
    return concept->instances;
}

float ConceptFormation::getConceptStability(size_t conceptId) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept || concept->instances.empty()) return 0.0f;
    
    float totalStability = 0.0f;
    for (const auto& instance : concept->instances) {
        totalStability += instance.stability;
    }
    
    return totalStability / concept->instances.size();
}

void ConceptFormation::mergeConcepts(size_t conceptA, size_t conceptB) {
    const DiscoveredConcept* conceptA_ptr = getConcept(conceptA);
    const DiscoveredConcept* conceptB_ptr = getConcept(conceptB);
    
    if (!conceptA_ptr || !conceptB_ptr) return;
    
    // Create merged concept
    DiscoveredConcept mergedConcept = *conceptA_ptr;
    
    // Merge instances
    for (const auto& instance : conceptB_ptr->instances) {
        mergedConcept.instances.push_back(instance);
    }
    
    // Update prototype
    if (!mergedConcept.instances.empty()) {
        std::vector<float> newPrototype(mergedConcept.instances[0].pattern.size(), 0.0f);
        for (const auto& instance : mergedConcept.instances) {
            for (size_t i = 0; i < instance.pattern.size(); ++i) {
                newPrototype[i] += instance.pattern[i];
            }
        }
        
        for (size_t i = 0; i < newPrototype.size(); ++i) {
            newPrototype[i] /= mergedConcept.instances.size();
        }
        
        mergedConcept.prototype = newPrototype;
    }
    
    // Replace old concepts with merged one
    auto itA = std::find_if(concepts_.begin(), concepts_.end(),
                          [conceptA](const DiscoveredConcept& c) { return c.id == conceptA; });n    auto itB = std::find_if(concepts_.begin(), concepts_.end(),
                          [conceptB](const DiscoveredConcept& c) { return c.id == conceptB; });
    
    if (itA != concepts_.end() && itB != concepts_.end()) {
        // Update references in other concepts
        for (auto& concept : concepts_) {
            if (concept.id == conceptA || concept.id == conceptB) {
                concept.associatedConceptIds.erase(
                    std::remove_if(concept.associatedConceptIds.begin(), concept.associatedConceptIds.end(),
                                 [conceptB](size_t id) { return id == conceptB; }),
                    concept.associatedConceptIds.end()
                );
            }
        }
        
        // Replace with merged concept
        *itA = mergedConcept;
        concepts_.erase(itB);
        
        // Update IDs
        for (auto& concept : concepts_) {
            if (concept.id >= conceptB) {
                concept.id--;
            }
        }
        
        mergedConcept.id = conceptB;
        *itB = mergedConcept;
    }
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
    
    // Create new instance
    ConceptInstance instance;
    instance.pattern = newPattern;
    instance.features = features;
    instance.observationCount = concept->totalObservations + 1;
    instance.lastObserved = concept->totalObservations;  // Use index as timestamp for simplicity
    instance.avgReward = (concept->avgReward * (concept->totalObservations - 1) + reward) / concept->totalObservations;
    instance.stability = calculateStability(instance.pattern, concept->prototype);
    
    concept->instances.push_back(instance);
    concept->totalObservations++;
    concept->avgStability = (concept->avgStability * (concept->totalObservations - 1) + instance.stability) / concept->totalObservations;
    
    // Update prototype
    updatePrototype(conceptId, newPattern);
}

float ConceptFormation::computeSimilarity(const std::vector<float>& a,
                                        const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        totalDiff += std::abs(a[i] - b[i]);
    }
    
    float avgDiff = totalDiff / a.size();
    return std::max(0.0f, 1.0f - avgDiff / 20.0f);  // Normalize similarity
}

bool ConceptFormation::isNovel(const std::vector<float>& pattern,
                               float similarityThreshold) const {
    return getMatchingConcept(pattern, similarityThreshold) == 0;
}

size_t ConceptFormation::findConceptForPattern(const std::vector<float>& pattern) const {
    return getMatchingConcept(pattern);
}

void ConceptFormation::clear() {
    concepts_.clear();
    nextConceptId_ = 1;
}

float ConceptFormation::getGeneralizationAbility(size_t conceptId) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept) return 0.0f;
    
    // Generalization ability is based on stability and diversity of instances
    float stability = concept->avgStability;
    float diversity = concept->instances.size() > 1 ? 1.0f : 0.5f;
    
    return (stability * 0.7f) + (diversity * 0.3f);
}

size_t ConceptFormation::createConcept(const std::vector<float>& pattern,
                                       const std::vector<float>& features,
                                       float reward) {
    DiscoveredConcept concept;
    concept.id = nextConceptId_++;
    concept.prototype = pattern;
    concept.features = features;
    concept.categoryHint = deriveCategoryHint(pattern);
    concept.totalObservations = 1;
    concept.avgStability = 1.0f;  // New concepts start stable
    
    ConceptInstance instance;
    instance.pattern = pattern;
    instance.features = features;
    instance.observationCount = 1;
    instance.lastObserved = 0;
    instance.avgReward = reward;
    instance.stability = 1.0f;
    
    concept.instances.push_back(instance);
    
    concepts_.push_back(concept);
    
    // Limit number of concepts
    if (concepts_.size() > pImpl->conceptMemorySize) {
        // Remove least stable concept
        std::sort(concepts_.begin(), concepts_.end(),
                 [](const DiscoveredConcept& a, const DiscoveredConcept& b) {
                     return a.avgStability < b.avgStability;
                 });
        concepts_.erase(concepts_.begin());
    }
    
    return concept.id;
}

float ConceptFormation::calculateStability(const std::vector<float>& newPattern,
                                           const std::vector<float>& existingPrototype) const {
    if (existingPrototype.empty()) return 1.0f;
    
    float similarity = computeSimilarity(newPattern, existingPrototype);
    return similarity;  // Stability is similarity to prototype
}

void ConceptFormation::updatePrototype(size_t conceptId, const std::vector<float>& newInstance) {
    DiscoveredConcept* concept = nullptr;
    for (auto& c : concepts_) {
        if (c.id == conceptId) {
            concept = &c;
            break;
        }
    }
    
    if (!concept) return;
    
    // Hebbian averaging for prototype
    if (concept->instances.size() > 1) {
        std::vector<float> newPrototype(concept->instances[0].pattern.size(), 0.0f);
        for (const auto& instance : concept->instances) {
            for (size_t i = 0; i < instance.pattern.size(); ++i) {
                newPrototype[i] += instance.pattern[i] * instance.stability;
            }
        }
        
        float totalWeight = 0.0f;
        for (const auto& instance : concept->instances) {
            totalWeight += instance.stability;
        }
        
        if (totalWeight > 0.0f) {
            for (size_t i = 0; i < newPrototype.size(); ++i) {
                newPrototype[i] /= totalWeight;
            }
            
            concept->prototype = newPrototype;
        }
    }
}

bool ConceptFormation::isConceptStable(size_t conceptId) const {
    const DiscoveredConcept* concept = getConcept(conceptId);
    if (!concept) return false;
    
    return concept->avgStability >= pImpl->stabilityThreshold;
}

std::string ConceptFormation::deriveCategoryHint(const std::vector<float>& pattern) const {
    // Simple category derivation based on pattern characteristics
    if (pattern.empty()) return "unknown";
    
    float positiveSum = 0.0f;
    float negativeSum = 0.0f;
    
    for (float val : pattern) {
        if (val > 0) positiveSum += val;
        else negativeSum += val;
    }
    
    if (positiveSum > -negativeSum * 0.5f) {
        return "positive";
    } else if (negativeSum > positiveSum * 0.5f) {
        return "negative";
    } else {
        return "neutral";
    }
}

} // namespace nlm