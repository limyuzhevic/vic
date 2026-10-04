#include "Memory.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/STDP.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <queue>

namespace nlm {

// Semantic Memory Neural Implementation
struct SemanticMemory::Impl {
    Brain* brain;
    
    // Semantic regions in the brain
    std::vector<NeuronId> semanticPopulation;
    std::vector<std::vector<float>> conceptPrototypes;
    std::vector<float> conceptActivations;
    
    // Hebbian clustering for concept discovery
    std::vector<std::vector<size_t>> conceptClusters;
    std::vector<size_t> conceptLeaders;
    
    // Concept-Region mapping for consolidation
    std::unordered_map<size_t, RegionId> conceptRegionMap;
    
    // Temporal stability tracking
    std::vector<float> conceptStability;
    std::vector<size_t> observationCounts;
    
    // Integration with other systems
    ConceptFormation* conceptFormation;
    NeuralPrediction* predictionSystem;
    Neuromodulator* neuromodulator;
    
    // Similarity threshold for concept formation
    float conceptThreshold;
    
    Impl() : brain(nullptr), conceptFormation(nullptr), 
             predictionSystem(nullptr), neuromodulator(nullptr),
             conceptThreshold(0.7f) {
        semanticPopulation.reserve(1000);
        conceptPrototypes.reserve(100);
        conceptActivations.reserve(100);
        conceptStability.reserve(100);
        observationCounts.reserve(100);
    }
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {
    // Initialize neural population for semantic memory
    if (pImpl) {
        // Create semantic cortical region
        pImpl->semanticPopulation.reserve(1000);
        pImpl->conceptPrototypes.reserve(100);
        pImpl->conceptActivations.reserve(100);
        pImpl->conceptStability.reserve(100);
        pImpl->observationCounts.reserve(100);
    }
}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    if (!pImpl || !pImpl->brain) return;
    
    // Create neural pattern from fact key
    std::vector<float> pattern(100, 0.0f);  // 100-dimensional semantic vector
    for (size_t i = 0; i < std::min(key.size(), pattern.size()); ++i) {
        pattern[i] = static_cast<float>(key[i]) / 255.0f;
    }
    
    // Add value information
    if (value.size() < pattern.size()) {
        for (size_t i = 0; i < value.size(); ++i) {
            pattern[50 + i] = static_cast<float>(value[i]) / 255.0f;
        }
    }
    
    // Store as neural pattern with Hebbian learning
    storePattern(pattern, key, value);
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    if (!pImpl || !pImpl->brain) return "";
    
    // Create pattern for lookup
    std::vector<float> pattern(100, 0.0f);
    for (size_t i = 0; i < std::min(key.size(), pattern.size()); ++i) {
        pattern[i] = static_cast<float>(key[i]) / 255.0f;
    }
    
    // Find matching concept
    size_t conceptId = findMatchingConcept(pattern);
    if (conceptId != 0) {
        auto& prototypes = pImpl->conceptPrototypes;
        if (conceptId <= prototypes.size()) {
            // Reconstruct value from prototype
            std::string value;
            for (size_t i = 50; i < std::min(50 + 50, prototypes[conceptId].size()); ++i) {
                value += static_cast<char>(prototypes[conceptId][i] * 255.0f);
            }
            return value;
        }
    }
    
    return "";
}

bool SemanticMemory::hasFact(const std::string& key) const {
    return !retrieveFact(key).empty();
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    std::vector<std::pair<std::string, std::string>> facts;
    
    if (!pImpl) return facts;
    
    for (size_t i = 0; i < pImpl->conceptPrototypes.size(); ++i) {
        // Reconstruct key and value from concept prototype
        const auto& prototype = pImpl->conceptPrototypes[i];
        std::string key, value;
        
        for (size_t j = 0; j < std::min(50, prototype.size()); ++j) {
            key += static_cast<char>(prototype[j] * 255.0f);
        }
        for (size_t j = 50; j < std::min(100, prototype.size()); ++j) {
            value += static_cast<char>(prototype[j] * 255.0f);
        }
        
        if (!key.empty()) {
            facts.emplace_back(key, value);
        }
    }
    
    return facts;
}

void SemanticMemory::clear() {
    if (!pImpl) return;
    
    pImpl->semanticPopulation.clear();
    pImpl->conceptPrototypes.clear();
    pImpl->conceptActivations.clear();
    pImpl->conceptClusters.clear();
    pImpl->conceptLeaders.clear();
    pImpl->conceptRegionMap.clear();
    pImpl->conceptStability.clear();
    pImpl->observationCounts.clear();
}

void SemanticMemory::storePattern(const std::vector<float>& pattern, 
                                  const std::string& key, 
                                  const std::string& value) {
    if (!pImpl || pattern.empty()) return;
    
    // Find matching concept using Hebbian clustering
    size_t matchingConcept = findMatchingConcept(pattern);
    
    if (matchingConcept == 0) {
        // Create new concept
        size_t conceptId = createNewConcept(pattern, key, value);
        
        if (conceptId > 0) {
            // Update prototype using Hebbian averaging
            if (pImpl->brain && pImpl->conceptFormation) {
                pImpl->conceptFormation->updateConcept(conceptId, pattern);
            }
            
            // Consolidate concept into neural region
            consolidateConcept(conceptId);
        }
    } else {
        // Update existing concept
        updateExistingConcept(matchingConcept, pattern);
    }
}

size_t SemanticMemory::createNewConcept(const std::vector<float>& pattern, 
                                        const std::string& key,
                                        const std::string& value) {
    if (!pImpl) return 0;
    
    // Check if concept is novel enough
    bool isNovel = true;
    for (const auto& prototype : pImpl->conceptPrototypes) {
        if (computePatternSimilarity(pattern, prototype) > pImpl->conceptThreshold) {
            isNovel = false;
            break;
        }
    }
    
    if (!isNovel) {
        // Find closest concept
        size_t bestMatch = 0;
        float bestSimilarity = 0.0f;
        
        for (size_t i = 0; i < pImpl->conceptPrototypes.size(); ++i) {
            float sim = computePatternSimilarity(pattern, pImpl->conceptPrototypes[i]);
            if (sim > bestSimilarity) {
                bestSimilarity = sim;
                bestMatch = i;
            }
        }
        return bestMatch + 1;  // +1 for 1-based indexing
    }
    
    // Create new concept entry
    size_t conceptId = pImpl->conceptPrototypes.size();
    pImpl->conceptPrototypes.push_back(pattern);
    pImpl->conceptActivations.push_back(0.0f);
    pImpl->conceptStability.push_back(0.0f);
    pImpl->observationCounts.push_back(1);
    
    // Add to Hebbian cluster
    size_t leader = conceptId;
    for (size_t i = 0; i < pImpl->conceptLeaders.size(); ++i) {
        float sim = computePatternSimilarity(pattern, pImpl->conceptPrototypes[pImpl->conceptLeaders[i]]);
        if (sim > pImpl->conceptThreshold) {
            leader = pImpl->conceptLeaders[i];
            pImpl->conceptClusters[i].push_back(conceptId);
            break;
        }
    }
    
    if (leader == conceptId) {
        pImpl->conceptLeaders.push_back(conceptId);
        pImpl->conceptClusters.push_back(std::vector<size_t>{conceptId});
    }
    
    return conceptId + 1;  // +1 for 1-based indexing
}

size_t SemanticMemory::findMatchingConcept(const std::vector<float>& pattern) const {
    if (!pImpl || pImpl->conceptPrototypes.empty()) return 0;
    
    size_t bestMatch = 0;
    float bestSimilarity = pImpl->conceptThreshold;
    
    for (size_t i = 0; i < pImpl->conceptPrototypes.size(); ++i) {
        float similarity = computePatternSimilarity(pattern, pImpl->conceptPrototypes[i]);
        
        // Boost similarity for frequently observed concepts
        float boost = 0.5f * std::log(1.0f + pImpl->observationCounts[i]);
        similarity = std::min(1.0f, similarity + boost);
        
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestMatch = i + 1;  // +1 for 1-based indexing
        }
    }
    
    return bestMatch;
}

void SemanticMemory::updateExistingConcept(size_t conceptId, const std::vector<float>& newPattern) {
    if (!pImpl || conceptId == 0 || conceptId > pImpl->conceptPrototypes.size()) return;
    
    size_t idx = conceptId - 1;  // Convert to 0-based
    auto& prototype = pImpl->conceptPrototypes[idx];
    
    // Update prototype using Hebbian averaging
    for (size_t i = 0; i < prototype.size() && i < newPattern.size(); ++i) {
        prototype[i] = prototype[i] * 0.9f + newPattern[i] * 0.1f;
        prototype[i] = std::clamp(prototype[i], 0.0f, 1.0f);
    }
    
    // Update stability
    pImpl->conceptStability[idx] = (pImpl->conceptStability[idx] * pImpl->observationCounts[idx] + 1.0f) / 
                                  (pImpl->observationCounts[idx] + 1);
    pImpl->observationCounts[idx]++;
    
    // Consolidate if stable enough
    if (pImpl->conceptStability[idx] > 0.8f && pImpl->observationCounts[idx] > 10) {
        consolidateConcept(conceptId);
    }
}

void SemanticMemory::consolidateConcept(size_t conceptId) {
    if (!pImpl || conceptId == 0 || conceptId > pImpl->conceptPrototypes.size()) return;
    
    size_t idx = conceptId - 1;
    
    // Create dedicated neural region for stable concepts
    RegionId regionId = pImpl->brain->addRegion("semantic_cortex");
    pImpl->conceptRegionMap[conceptId] = regionId;
    
    // Create attractor neurons for the concept
    auto& prototype = pImpl->conceptPrototypes[idx];
    
    // Create symmetric connections (Hebbian) for pattern completion
    std::vector<NeuronId> attractorNeurons;
    for (size_t i = 0; i < prototype.size() && i < 10; ++i) {  // Limit to 10 neurons
        float activation = prototype[i] * 10.0f;  // Scale to neuron range
        if (auto* region = pImpl->brain->getRegion(regionId)) {
            // This would normally create actual neurons - simplified for example
        }
    }
}

float SemanticMemory::computePatternSimilarity(const std::vector<float>& a,
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

} // namespace nlm
