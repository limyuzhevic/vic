#include "ConceptFormation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>
#include <set>
#include <deque>

namespace nlm {

// Implementation struct for ConceptFormation
struct ConceptFormation::Impl {
    Brain* brain;
    std::unordered_map<size_t, std::vector<float>> conceptPrototypes;  // id -> prototype
    std::unordered_map<size_t, std::vector<ConceptInstance>> conceptInstances;  // id -> instances
    std::unordered_map<size_t, float> conceptStabilities;  // id -> stability
    std::unordered_map<size_t, float> conceptAverageRewards;  // id -> avg reward
    std::unordered_map<size_t, size_t> conceptObservationCounts;  // id -> observation count
    
    // Pattern similarity lookup (pattern hash -> concept id)
    std::vector<std::vector<float>> patternDatabase;
    std::vector<size_t> patternConceptMapping;
    
    // Additional internal tracking
    size_t nextConceptId;
    
    Impl() : brain(nullptr), nextConceptId(1) {}
};

ConceptFormation::ConceptFormation() : pImpl(new Impl), brain_(nullptr), nextConceptId_(1),
    formationThreshold_(0.7f), stabilityThreshold_(0.8f), stabilityWindow_(10) {
}

ConceptFormation::~ConceptFormation() = default;

void ConceptFormation::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("ConceptFormation initialized");
}

Brain* ConceptFormation::getBrain() const {
    return pImpl->brain;
}

size_t ConceptFormation::presentExperience(const std::vector<float>& pattern,
                                        const std::vector<float>& features,
                                        float reward,
                                        SimulationStep currentTime) {
    // Check if this matches an existing concept
    size_t matchingConcept = getMatchingConcept(pattern);
    
    if (matchingConcept > 0) {
        // Update existing concept
        updateConcept(matchingConcept, pattern, features, reward);
        return matchingConcept;
    }
    
    // Check if pattern is stable enough to form a new concept
    if (isNovel(pattern) && isPatternStableEnough(pattern)) {
        size_t newConceptId = createConcept(pattern, features, reward);
        NLM_LOG_DEBUG("ConceptFormation: Created new concept " + std::to_string(newConceptId));
        return newConceptId;
    }
    
    // Too novel, return 0 to indicate uncertainty
    return 0;
}

void ConceptFormation::updateConcept(size_t conceptId, const std::vector<float>& newPattern,
                                const std::vector<float>& features, float reward) {
    // Update concept prototype using Hebbian averaging
    auto& prototype = pImpl->conceptPrototypes[conceptId];
    if (prototype.empty()) {
        prototype = newPattern;
    } else {
        for (size_t i = 0; i < std::min(prototype.size(), newPattern.size()); ++i) {
            prototype[i] = (prototype[i] * 0.9f) + (newPattern[i] * 0.1f);
        }
    }
    
    // Add new instance
    ConceptInstance instance;
    instance.pattern = newPattern;
    instance.features = features;
    instance.observationCount = pImpl->conceptObservationCounts[conceptId] + 1;
    instance.lastObserved = currentTime;
    instance.avgReward = (pImpl->conceptAverageRewards[conceptId] * 
                         (instance.observationCount - 1) + reward) / instance.observationCount;
    
    // Calculate stability
    float stability = computePatternStability(newPattern, conceptId);
    instance.stability = stability;
    
    pImpl->conceptInstances[conceptId].push_back(instance);
    
    // Update statistics
    pImpl->conceptObservationCounts[conceptId] = instance.observationCount;
    pImpl->conceptAverageRewards[conceptId] = instance.avgReward;
    
    // Update concept stability
    if (pImpl->conceptStabilities.find(conceptId) != pImpl->conceptStabilities.end()) {
        pImpl->conceptStabilities[conceptId] = calculateConceptStability(conceptId);
    }
}

float ConceptFormation::computePatternStability(const std::vector<float>& pattern, size_t conceptId) const {
    // For now, use pattern consistency as stability measure
    // In real implementation, would check consistency across multiple observations
    
    if (pImpl->conceptInstances.find(conceptId) == pImpl->conceptInstances.end()) {
        return 0.5f;  // Default stability
    }
    
    const auto& instances = pImpl->conceptInstances.at(conceptId);
    
    if (instances.empty()) return 0.5f;
    
    // Calculate average stability of similar instances
    float totalStability = 0.0f;
    size_t count = 0;
    
    for (const auto& instance : instances) {
        float sim = computeSimilarity(pattern, instance.pattern);
        totalStability += sim * instance.stability;
        count++;
    }
    
    return (count > 0) ? (totalStability / count) : 0.5f;
}

bool ConceptFormation::isPatternStableEnough(const std::vector<float>& pattern) const {
    // Check if pattern has been observed multiple times to be stable
    // For now, just check if pattern appears in database
    
    // Simple implementation: check pattern database
    for (size_t i = 0; i < pImpl->patternDatabase.size(); ++i) {
        if (computeSimilarity(pattern, pImpl->patternDatabase[i]) > 0.8f) {
            return true;
        }
    }
    
    return false;
}

size_t ConceptFormation::createConcept(const std::vector<float>& pattern,
                                    const std::vector<float>& features,
                                    float reward) {
    size_t conceptId = nextConceptId_++;
    
    // Store prototype
    pImpl->conceptPrototypes[conceptId] = pattern;
    
    // Create initial instance
    ConceptInstance instance;
    instance.pattern = pattern;
    instance.features = features;
    instance.observationCount = 1;
    instance.lastObserved = 0;
    instance.avgReward = reward;
    instance.stability = 1.0f;  // New concepts start with high stability
    
    pImpl->conceptInstances[conceptId].push_back(instance);
    
    // Initialize statistics
    pImpl->conceptStabilities[conceptId] = 1.0f;
    pImpl->conceptAverageRewards[conceptId] = reward;
    pImpl->conceptObservationCounts[conceptId] = 1;
    
    return conceptId;
}

size_t ConceptFormation::getMatchingConcept(const std::vector<float>& pattern,
                                        float similarityThreshold) const {
    size_t bestMatch = 0;
    float bestSimilarity = 0.0f;
    
    for (const auto& pair : pImpl->conceptPrototypes) {
        size_t conceptId = pair.first;
        const auto& prototype = pair.second;
        
        float similarity = computeSimilarity(pattern, prototype);
        
        if (similarity > similarityThreshold && similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestMatch = conceptId;
        }
    }
    
    return bestMatch;
}

float ConceptFormation::computeSimilarity(const std::vector<float>& a,
                                    const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < a.size(); ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dotProduct / (std::sqrt(normA) * std::sqrt(normB));
}

bool ConceptFormation::isNovel(const std::vector<float>& pattern,
                            float similarityThreshold) const {
    return getMatchingConcept(pattern, similarityThreshold) == 0;
}

size_t ConceptFormation::findConceptForPattern(const std::vector<float>& pattern) const {
    return getMatchingConcept(pattern);
}

void ConceptFormation::mergeConcepts(size_t conceptA, size_t conceptB) {
    // Merge concept B into concept A, then delete concept B
    if (conceptA == conceptB || conceptB == 0) return;
    
    auto& instancesA = pImpl->conceptInstances[conceptA];
    auto& instancesB = pImpl->conceptInstances[conceptB];
    
    // Merge instances
    instancesA.insert(instancesA.end(), instancesB.begin(), instancesB.end());
    
    // Update prototype
    auto& prototypeA = pImpl->conceptPrototypes[conceptA];
    auto& prototypeB = pImpl->conceptPrototypes[conceptB];
    
    for (size_t i = 0; i < std::min(prototypeA.size(), prototypeB.size()); ++i) {
        prototypeA[i] = (prototypeA[i] + prototypeB[i]) * 0.5f;
    }
    
    // Update statistics
    pImpl->conceptObservationCounts[conceptA] += pImpl->conceptObservationCounts[conceptB];
    pImpl->conceptAverageRewards[conceptA] = 
        (pImpl->conceptAverageRewards[conceptA] * pImpl->conceptObservationCounts[conceptA - 
        pImpl->conceptObservationCounts[conceptB]] +
         pImpl->conceptAverageRewards[conceptB] * pImpl->conceptObservationCounts[conceptB]) /
        pImpl->conceptObservationCounts[conceptA];
    
    // Update stability
    float avgStability = (pImpl->conceptStabilities[conceptA] + pImpl->conceptStabilities[conceptB]) * 0.5f;
    pImpl->conceptStabilities[conceptA] = avgStability;
    
    // Remove concept B
    pImpl->conceptPrototypes.erase(conceptB);
    pImpl->conceptInstances.erase(conceptB);
    pImpl->conceptStabilities.erase(conceptB);
    pImpl->conceptAverageRewards.erase(conceptB);
    pImpl->conceptObservationCounts.erase(conceptB);
}

void ConceptFormation::clear() {
    pImpl->conceptPrototypes.clear();
    pImpl->conceptInstances.clear();
    pImpl->conceptStabilities.clear();
    pImpl->conceptAverageRewards.clear();
    pImpl->conceptObservationCounts.clear();
    pImpl->patternDatabase.clear();
    pImpl->patternConceptMapping.clear();
}

float HackConceptFormation::calculateConceptStability(size_t conceptId) const {
    // Calculate overall concept stability from its instances
    const auto& it = pImpl->conceptInstances.find(conceptId);
    if (it == pImpl->conceptInstances.end() || it.second.empty()) return 0.0f;
    
    const auto& instances = it.second;
    float totalStability = 0.0f;
    
    for (const auto& instance : instances) {
        totalStability += instance.stability;
    }
    
    return totalStability / instances.size();
}

float HackConceptFormation::getGeneralizationAbility(size_t conceptId) const {
    // Calculate how well concept handles variations
    const auto& it = pImpl->conceptInstances.find(conceptId);
    if (it == pImpl->conceptInstances.end() || it.second.empty()) return 0.0f;
    
    const auto& instances = it.second;
    float totalSimilarity = 0.0f;
    size_t count = 0;
    
    for (const auto& instance : instances) {
        // Test how well prototype matches instance
        float sim = computeSimilarity(instance.pattern, prototype);
        totalSimilarity += sim;
        count++;
    }
    
    return (count > 0) ? (totalSimilarity / count) : 0.0f;
}

// Additional utility methods
float HackConceptFormation::computeSimilarity(const std::vector<float>& a,
                                       const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < a.size(); ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dotProduct / (std::sqrt(normA) * std::sqrt(normB));
}

// These functions need to be added to the ConceptFormation header first
// discoveredConcept ConceptFormation::getConcept(size_t conceptId) const {
//     const auto& it = pImpl->conceptPrototypes.find(conceptId);
//     if (it != pImpl->conceptPrototypes.end()) {
//         DiscoveredConcept concept;
//         concept.id = conceptId;
//         concept.prototype = it.second;
//         concept.totalObservations = pImpl->conceptObservationCounts[conceptId];
//         concept.avgStability = pImpl->conceptStabilities[conceptId];
//         concept.avgReward = pImpl->conceptAverageRewards[conceptId];
//         
//         auto instancesIt = pImpl->conceptInstances.find(conceptId);
//         if (instancesIt != pImpl->conceptInstances.end()) {
//             concept.instances = instancesIt.second;
//         }
//         
//         return concept;
//     }
//     return DiscoveredConcept();
// }

// std::vector<float> ConceptFormation::getConceptPrototype(size_t conceptId) const {
//     const auto& it = pImpl->conceptPrototypes.find(conceptId);
//     if (it != pImpl->conceptPrototypes.end()) {
//         return it.second;
//     }
//     return std::vector<float>();
// }

// const std::vector<ConceptInstance>& ConceptFormation::getConceptInstances(size_t conceptId) const {
//     const auto& it = pImpl->conceptInstances.find(conceptId);
//     if (it != pImpl->conceptInstances.end()) {
//         return it.second;
//     }
//     static const std::vector<ConceptInstance> empty;
//     return empty;
// }

// float ConceptFormation::getConceptStability(size_t conceptId) const {
//     const auto& it = pImpl->conceptStabilities.find(conceptId);
//     if (it != pImpl->conceptStabilities.end()) {
//         return it.second;
//     }
//     return 0.0f;
// }

ConceptFormation::presentExperience ConceptFormation::presentExperience;
ConceptFormation::updateConcept ConceptFormation::updateConcept;
ConceptFormation::computePatternStability ConceptFormation::computePatternStability;
ConceptFormation::isPatternStableEnough ConceptFormation::isPatternStableEnough;
ConceptFormation::createConcept ConceptFormation::createConcept;
ConceptFormation::getMatchingConcept ConceptFormation::getMatchingConcept;
ConceptFormation::computeSimilarity ConceptFormation::computeSimilarity;
ConceptFormation::isNovel ConceptFormation::isNovel;
ConceptFormation::findConceptForPattern ConceptFormation::findConceptForPattern;
ConceptFormation::mergeConcepts ConceptFormation::mergeConcepts;
ConceptFormation::clear ConceptFormation::clear;
ConceptFormation::getBrain ConceptFormation::getBrain;
ConceptFormation::calculateConceptStability HackConceptFormation::calculateConceptStability;
ConceptFormation::getGeneralizationAbility HackConceptFormation::getGeneralizationAbility;

} // namespace nlm
