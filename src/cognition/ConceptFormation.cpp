// ConceptFormation implementation
#include "ConceptFormation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct ConceptFormation::Impl {
    Brain* brain;
    
    // Concept prototypes
    std::vector<std::vector<float>> conceptPrototypes;
    
    // Pattern associations
    std::vector<std::pair<std::vector<float>, std::vector<float>>> patternAssociations;
    
    // Concept weights for pattern matching
    std::vector<float> conceptWeights;
    
    Impl() : brain(nullptr) {}
};

ConceptFormation::ConceptFormation()
    : pImpl(new Impl)
    , conceptLearningRate_(0.1f)
    , conceptStabilityThreshold_(0.8f)
    , minPatternSimilarity_(0.3f)
{
}

ConceptFormation::~ConceptFormation() = default;

void ConceptFormation::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("ConceptFormation initialized");
}

void ConceptFormation::discoverConcepts(const std::vector<float>& sensoryPattern,
                                       SimulationStep step) {
    if (!pImpl->brain) return;
    
    // Check if this pattern is similar to existing concepts
    size_t bestConcept = findClosestConcept(sensoryPattern);
    
    if (bestConcept < pImpl->conceptPrototypes.size()) {
        // Update existing concept
        std::vector<float>& prototype = pImpl->conceptPrototypes[bestConcept];
        for (size_t i = 0; i < sensoryPattern.size() && i < prototype.size(); ++i) {
            prototype[i] = prototype[i] * (1.0f - conceptLearningRate_) + 
                         sensoryPattern[i] * conceptLearningRate_;
        }
        
        // Update concept weight based on pattern consistency
        pImpl->conceptWeights[bestConcept] = 
            std::min(1.0f, pImpl->conceptWeights[bestConcept] + 0.01f);
    } else {
        // Create new concept
        createConcept(sensoryPattern, step);
    }
}

size_t ConceptFormation::findClosestConcept(const std::vector<float>& pattern) const {
    if (pImpl->conceptPrototypes.empty()) {
        return 0;
    }
    
    float bestSimilarity = minPatternSimilarity_;
    size_t bestConcept = 0;
    
    for (size_t i = 0; i < pImpl->conceptPrototypes.size(); ++i) {
        float similarity = computeSimilarity(pattern, pImpl->conceptPrototypes[i]);
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestConcept = i;
        }
    }
    
    return bestConcept;
}

std::vector<size_t> ConceptFormation::getActiveConcepts(const std::vector<float>& pattern) const {
    std::vector<size_t> activeConcepts;
    
    for (size_t i = 0; i < pImpl->conceptPrototypes.size(); ++i) {
        if (computeSimilarity(pattern, pImpl->conceptPrototypes[i]) > conceptStabilityThreshold_) {
            activeConcepts.push_back(i);
        }
    }
    
    return activeConcepts;
}

std::vector<float> ConceptFormation::getConceptRepresentation(size_t conceptId) const {
    if (conceptId < pImpl->conceptPrototypes.size()) {
        return pImpl->conceptPrototypes[conceptId];
    }
    return std::vector<float>();
}

float ConceptFormation::getConceptStability(size_t conceptId) const {
    if (conceptId < pImpl->conceptWeights.size()) {
        return pImpl->conceptWeights[conceptId];
    }
    return 0.0f;
}

void ConceptFormation::updateConcept(const std::vector<float>& oldConcept,
                                    const std::vector<float>& newPattern,
                                    float learningRate) {
    for (size_t i = 0; i < oldConcept.size() && i < newPattern.size(); ++i) {
        // Smooth update
        pImpl->conceptPrototypes[i][i] = oldConcept[i] * (1.0f - learningRate) + 
                                         newPattern[i] * learningRate;
    }
}

void ConceptFormation::associatePatterns(const std::vector<float>& patternA,
                                        const std::vector<float>& patternB) {
    // Record association between similar patterns
    patternAssociations.emplace_back(patternA, patternB);
}

void ConceptFormation::applyConceptToActionSelection(ActionType action,
                                                   std::vector<float>& state) const {
    // Modify action selection based on concept relationships
    // This would integrate with the planning system
}

float ConceptFormation::computeConceptDistance(size_t conceptA, size_t conceptB) const {
    if (conceptA >= pImpl->conceptPrototypes.size() || 
        conceptB >= pImpl->conceptPrototypes.size()) {
        return 1.0f;
    }
    
    return 1.0f - computeSimilarity(pImpl->conceptPrototypes[conceptA],
                                  pImpl->conceptPrototypes[conceptB]);
}

std::vector<size_t> ConceptFormation::getConceptHierarchy() const {
    // Return concepts in order of stability (most stable first)
    std::vector<size_t> hierarchy(pImpl->conceptWeights.size());
    for (size_t i = 0; i < pImpl->conceptWeights.size(); ++i) {
        hierarchy[i] = i;
    }
    
    // Sort by stability (descending)
    std::sort(hierarchy.begin(), hierarchy.end(),
              [this](size_t a, size_t b) {
                  return pImpl->conceptWeights[a] > pImpl->conceptWeights[b];
              });
    
    return hierarchy;
}

void ConceptFormation::integrateWithMemory(const EpisodicMemoryItem& episode) {
    // Use episodic memory to refine concepts
    // Extract patterns from episode and update concept prototypes
    if (!episode.sensoryState.empty()) {
        // This would integrate with concept formation
    }
}

void ConceptFormation::reset() {
    pImpl->conceptPrototypes.clear();
    pImpl->conceptWeights.clear();
    pImpl->patternAssociations.clear();
}

void ConceptFormation::createConcept(const std::vector<float>& pattern, SimulationStep step) {
    // Create new concept from pattern
    pImpl->conceptPrototypes.push_back(pattern);
    pImpl->conceptWeights.push_back(0.5f);  // Initial weight
    
    NLM_LOG_INFO("New concept created with " + std::to_string(pattern.size()) + " features");
}

float ConceptFormation::computeSimilarity(const std::vector<float>& a,
                                       const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    // Cosine similarity
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

} // namespace nlm
