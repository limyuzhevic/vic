// Add missing methods to ConceptFormation class

#include "ConceptFormation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct ConceptFormation::Impl {
    Brain* brain;
    std::vector<DiscoveredConcept> concepts;
    size_t nextConceptId;
    
    float formationThreshold;
    float stabilityThreshold;
    size_t stabilityWindow;
    
    Impl() : brain(nullptr), nextConceptId(1), formationThreshold(0.7f), 
             stabilityThreshold(0.8f), stabilityWindow(10) {}
};

ConceptFormation::ConceptFormation()
    : pImpl(std::make_unique<Impl>())
    , brain_(nullptr)
{
}

ConceptFormation::~ConceptFormation() = default;

void ConceptFormation::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    pImpl->concepts.clear();
    pImpl->nextConceptId = 1;
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
        // Update existing concept with new instance
        updateConcept(matchingConcept, pattern, features, reward);
        return matchingConcept;
    }
    
    // Too early to tell if it's a new concept
    if (pImpl->concepts.empty()) return 0;
    
    // Create new concept
    DiscoveredConcept newConcept;
    newConcept.id = pImpl->nextConceptId++;
    newConcept.prototype = pattern;
    
    ConceptInstance instance;
    instance.pattern = pattern;
    instance.features = features;
    instance.observationCount = 1;
    instance.lastObserved = currentTime;
    instance.avgReward = reward;
    instance.stability = 1.0f / pImpl->stabilityWindow;
    
    newConcept.instances.push_back(instance);
    pImpl->concepts.push_back(newConcept);
    
    return newConcept.id;
}

size_t ConceptFormation::getMatchingConcept(const std::vector<float>& pattern,
                                          float similarityThreshold) const {
    if (pattern.empty() || pImpl->concepts.empty()) return 0;
    
    size_t bestMatch = 0;
    float bestSimilarity = 0.0f;
    
    for (const auto& concept : pImpl->concepts) {
        float similarity = computeSimilarity(pattern, concept.prototype);
        
        if (similarity > bestSimilarity && similarity >= similarityThreshold) {
            bestSimilarity = similarity;
            bestMatch = concept.id;
        }
    }
    
    return bestMatch;
}

const DiscoveredConcept* ConceptFormation::getConcept(size_t conceptId) const {
    for (const auto& concept : pImpl->concepts) {
        if (concept.id == conceptId) return &concept;
    }
    return nullptr;
}

std::vector<float> ConceptFormation::getConceptPrototype(size_t conceptId) const {
    for (const auto& concept : pImpl->concepts) {
        if (concept.id == conceptId) return concept.prototype;
    }
    return std::vector<float>();
}

const std::vector<ConceptInstance>& ConceptFormation::getConceptInstances(size_t conceptId) const {
    static std::vector<ConceptInstance> empty;
    
    for (const auto& concept : pImpl->concepts) {
        if (concept.id == conceptId) return concept.instances;
    }
    
    return empty;
}

float ConceptFormation::getConceptStability(size_t conceptId) const {
    for (const auto& concept : pImpl->concepts) {
        if (concept.id == conceptId) {
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

void ConceptFormation::updateConcept(size_t conceptId, const std::vector<float>& newPattern,
                                     const std::vector<float>& features, float reward) {
    for (auto& concept : pImpl->concepts) {
        if (concept.id == conceptId) {
            ConceptInstance instance;
            instance.pattern = newPattern;
            instance.features = features;
            instance.observationCount++;
            instance.lastObserved = 0;  // current step
            instance.avgReward = (instance.avgReward * (instance.observationCount - 1) + reward) / instance.observationCount;
            instance.stability = (instance.stability * (instance.observationCount - 1) + 1.0f) / instance.observationCount;
            
            concept.instances.push_back(instance);
            
            // Update prototype based on instances
            if (concept.instances.size() > 1) {
                concept.prototype = newPattern;
                // TODO: Implement prototype update with stability weighting
            }
            
            break;
        }
    }
}

float ConceptFormation::computeSimilarity(const std::vector<float>& a,
                                      const std::vector<float>& b) const {
    if (a.empty() || b.empty() || a.size() != b.size()) return 0.0f;
    
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < a.size(); ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA == 0.0f || normB == 0.0f) return 0.0f;
    
    return dotProduct / std::sqrt(normA * normB);
}

void ConceptFormation::updateConcept(double timestep) {
    // Update concept formation with current neural state
    // This method allows concept formation to be updated in the brain loop
    
    // Get current neural state from working memory
    std::vector<float> currentState;
    if (auto* workingMemory = pImpl->brain->getWorkingMemory()) {
        currentState = workingMemory->retrieve();
    }
    
    if (!currentState.empty()) {
        // Present current state to concept formation
        presentExperience(currentState, std::vector<float>(), 0.0f, 0);
    }
}

} // namespace nlm
