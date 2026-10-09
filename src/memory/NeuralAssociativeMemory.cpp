#include "NeuralAssociativeMemory.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <numeric>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    // Primary storage: mapping from pattern hash to associations
    std::unordered_map<size_t, std::vector<std::pair<std::vector<NeuronId>, float>>> patternAssociations;
    
    // Secondary storage: mapping from pattern to list of associations for fast lookup
    std::unordered_map<size_t, std::vector<std::pair<size_t, float>>> reverseIndex;
    
    // Statistics
    size_t totalAssociations;
    float totalStrength;
    float minStrength;
    float maxStrength;
    
    // Brain reference
    Brain* brain;
    
    // Decay settings
    float decayRate;
    float consolidationThreshold;
    
    Impl() : totalAssociations(0), totalStrength(0.0f), 
             minStrength(1.0f), maxStrength(0.0f), brain(nullptr),
             decayRate(0.001f), consolidationThreshold(0.5f) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory() : pImpl(std::make_unique<Impl>()) {}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
}

void NeuralAssociativeMemory::associatePattern(const std::vector<NeuronId>& patternA, 
                                               const std::vector<NeuronId>& patternB,
                                               float initialStrength) {
    if (patternA.empty() || patternB.empty() || pImpl->brain == nullptr) {
        return;
    }
    
    size_t hashA = hashPattern(patternA);
    size_t hashB = hashPattern(patternB);
    
    // Update association in forward mapping
    pImpl->patternAssociations[hashA].push_back({patternB, initialStrength});
    pImpl->patternAssociations[hashB].push_back({patternA, initialStrength});
    
    // Update reverse index
    pImpl->reverseIndex[hashA].push_back({hashB, initialStrength});
    pImpl->reverseIndex[hashB].push_back({hashA, initialStrength});
    
    // Update statistics
    pImpl->totalAssociations++;
    pImpl->totalStrength += initialStrength;
    pImpl->minStrength = std::min(pImpl->minStrength, initialStrength);
    pImpl->maxStrength = std::max(pImpl->maxStrength, initialStrength);
}

std::vector<NeuronId> NeuralAssociativeMemory::getAssociatedPattern(const std::vector<NeuronId>& pattern) const {
    if (pattern.empty() || pImpl->patternAssociations.empty()) {
        return {};
    }
    
    size_t hash = hashPattern(pattern);
    auto it = pImpl->patternAssociations.find(hash);
    if (it == pImpl->patternAssociations.end() || it->second.empty()) {
        return {};
    }
    
    // Return the pattern with the highest association strength
    const auto& associations = it->second;
    auto strongest = std::max_element(associations.begin(), associations.end(),
                                     [](const auto& a, const auto& b) { return a.second < b.second; });
    
    return strongest->first;
}

float NeuralAssociativeMemory::calculatePatternSimilarity(const std::vector<NeuronId>& patternA,
                                                         const std::vector<NeuronId>& patternB) const {
    return calculateCosineSimilarity(patternA, patternB);
}

void NeuralAssociativeMemory::updateAssociation(const std::vector<NeuronId>& patternA,
                                                 const std::vector<NeuronId>& patternB,
                                                 float delta) {
    size_t hashA = hashPattern(patternA);
    size_t hashB = hashPattern(patternB);
    
    // Update forward mappings
    auto updateForward = [delta](auto& associations) {
        for (auto& assoc : associations) {
            if (std::abs(assoc.second) > 0.001f) {
                assoc.second += delta;
            }
        }
    };
    
    auto itA = pImpl->patternAssociations.find(hashA);
    auto itB = pImpl->patternAssociations.find(hashB);
    
    if (itA != pImpl->patternAssociations.end()) {
        updateForward(itA->second);
    }
    
    if (itB != pImpl->patternAssociations.end()) {
        updateForward(itB->second);
    }
}

std::vector<std::pair<std::vector<NeuronId>, float>> NeuralAssociativeMemory::getPatternAssociations(
    const std::vector<NeuronId>& pattern) const {
    std::vector<std::pair<std::vector<NeuronId>, float>> associations;
    
    if (pattern.empty()) {
        return associations;
    }
    
    size_t hash = hashPattern(pattern);
    auto it = pImpl->patternAssociations.find(hash);
    if (it != pImpl->patternAssociations.end()) {
        associations = it->second;
    }
    
    return associations;
}

size_t NeuralAssociativeMemory::getAssociationCount() const {
    return pImpl->totalAssociations;
}

std::vector<std::pair<std::vector<NeuronId>, std::vector<NeuronId>>> NeuralAssociativeMemory::getAllAssociations() const {
    std::vector<std::pair<std::vector<NeuronId>, std::vector<NeuronId>>> associations;
    
    for (const auto& outer : pImpl->patternAssociations) {
        for (const auto& assoc : outer.second) {
            // Avoid duplicates by checking if we've already added this association
            bool duplicate = false;
            for (const auto& existing : associations) {
                if ((existing.first == outer.second[0] && existing.second == assoc.first) ||
                    (existing.first == assoc.first && existing.second == outer.second[0])) {
                    duplicate = true;
                    break;
                }
            }
            if (!duplicate) {
                associations.push_back({outer.second[0], assoc.first});
            }
        }
    }
    
    return associations;
}

float NeuralAssociativeMemory::getAverageAssociationStrength() const {
    if (pImpl->totalAssociations == 0) {
        return 0.0f;
    }
    return pImpl->totalStrength / pImpl->totalAssociations;
}

std::vector<std::pair<std::pair<std::vector<NeuronId>, std::vector<NeuronId>>, float>> 
NeuralAssociativeMemory::getStrongestAssociations(size_t count) const {
    std::vector<std::pair<std::pair<std::vector<NeuronId>, std::vector<NeuronId>>, float>> associations;
    
    // Collect all unique associations with their strengths
    std::unordered_set<size_t> processed;
    
    for (const auto& outer : pImpl->patternAssociations) {
        for (const auto& assoc : outer.second) {
            // Create a unique key for this association
            size_t key = std::hash<NeuronId>{}((outer.first + assoc.first) % 2);
            
            if (processed.find(key) == processed.end()) {
                processed.insert(key);
                
                // Find a representative pattern for this association
                std::vector<NeuronId> patternA = {};
                std::vector<NeuronId> patternB = {};
                
                // Get the first pattern from outer mapping
                for (const auto& assoc2 : outer.second) {
                    if (assoc2.first == assoc.first) {
                        patternA = outer.second[0].first;
                        patternB = assoc.first;
                        break;
                    }
                }
                
                associations.push_back({{patternA, patternB}, assoc.second});
            }
        }
    }
    
    // Sort by strength in descending order
    std::sort(associations.begin(), associations.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Limit to requested count
    if (count < associations.size()) {
        associations.resize(count);
    }
    
    return associations;
}

void NeuralAssociativeMemory::decayAssociations(float decayRate) {
    pImpl->decayRate = decayRate;
    
    float totalDecay = 1.0f - decayRate;
    
    for (auto& outer : pImpl->patternAssociations) {
        for (auto& assoc : outer.second) {
            assoc.second *= totalDecay;
        }
    }
}

void NeuralAssociativeMemory::clear() {
    pImpl->patternAssociations.clear();
    pImpl->reverseIndex.clear();
    pImpl->totalAssociations = 0;
    pImpl->totalStrength = 0.0f;
    pImpl->minStrength = 1.0f;
    pImpl->maxStrength = 0.0f;
}

NeuralAssociativeMemory::MemoryStats NeuralAssociativeMemory::getStatistics() const {
    MemoryStats stats;
    
    if (pImpl->totalAssociations == 0) {
        stats.totalAssociations = 0;
        stats.averageStrength = 0.0f;
        stats.minStrength = 0.0f;
        stats.maxStrength = 0.0f;
        stats.patternSimilarity = 0.0f;
        return stats;
    }
    
    stats.totalAssociations = pImpl->totalAssociations;
    stats.averageStrength = getAverageAssociationStrength();
    stats.minStrength = pImpl->minStrength;
    stats.maxStrength = pImpl->maxStrength;
    
    // Calculate average pattern similarity for this memory system
    stats.patternSimilarity = 0.0f;
    
    return stats;
}

size_t NeuralAssociativeMemory::hashPattern(const std::vector<NeuronId>& pattern) {
    if (pattern.empty()) {
        return 0;
    }
    
    // Simple hash based on sum of neuron IDs
    size_t hash = 0;
    for (size_t i = 0; i < pattern.size(); ++i) {
        hash ^= std::hash<NeuronId>{}(pattern[i]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    
    return hash;
}

float NeuralAssociativeMemory::calculateCosineSimilarity(const std::vector<NeuronId>& patternA,
                                                         const std::vector<NeuronId>& patternB) {
    if (patternA.empty() || patternB.empty()) {
        return 0.0f;
    }
    
    // Create binary vectors for patterns (neuron presence/absence)
    std::unordered_set<NeuronId> setA(patternA.begin(), patternA.end());
    std::unordered_set<NeuronId> setB(patternB.begin(), patternB.end());
    
    // Calculate intersection
    size_t intersection = 0;
    for (const auto& neuron : setA) {
        if (setB.find(neuron) != setB.end()) {
            intersection++;
        }
    }
    
    // Calculate cosine similarity
    float magnitudeA = static_cast<float>(setA.size());
    float magnitudeB = static_cast<float>(setB.size());
    
    if (magnitudeA == 0.0f || magnitudeB == 0.0f) {
        return 0.0f;
    }
    
    return intersection / std::sqrt(magnitudeA * magnitudeB);
}

} // namespace nlm