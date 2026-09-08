#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    
    // Pattern neuron cache: maps from pattern hash to neuron ID
    std::unordered_map<size_t, NeuronId> patternNeuronCache;
    
    // Association network: strength between neuron pairs
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
    
    // Pattern index: maps neuron ID to pattern vector
    std::unordered_map<NeuronId, std::vector<float>> neuronPatternMap;
    
    Impl() : brain(nullptr) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
{
    // Preallocate some common patterns
    commonPatterns_.reserve(10);
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralAssociativeMemory initialized");
}

void NeuralAssociativeMemory::associate(const std::vector<float>& patternA,
                                       const std::vector<float>& patternB,
                                       float strength) {
    if (patternA.empty() || patternB.empty() || !brain_) return;
    
    // Find or create neurons for the patterns
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA == INVALID_NEURON_ID || neuronB == INVALID_NEURON_ID) return;
    
    // Check if association already exists
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            // Update existing association strength
            std::get<2>(assoc) = std::clamp(
                std::get<2>(assoc) + strength * 0.1f, 
                0.0f, 1.0f);
            return;
        }
    }
    
    // Create new association
    pImpl->associations.emplace_back(neuronA, neuronB, strength);
    
    // Strengthen both neurons (Hebbian-like learning)
    strengthenPatternNeurons({neuronA, neuronB}, strength * 0.5f);
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    if (!brain_ || episode.sensoryState.empty() || episode.resultingSensoryState.empty()) return;
    
    // Create associations between current and predicted states
    float predictiveStrength = 0.8f;
    associate(episode.sensoryState, episode.resultingSensoryState, predictiveStrength);
    
    // Associate actions with outcomes
    if (!episode.sensoryState.empty()) {
        std::vector<float> actionPattern(episode.sensoryState.size(), 0.0f);
        
        // Encode action type
        switch (episode.action) {
            case ActionType::MoveForward: actionPattern[0] = 1.0f; break;
            case ActionType::MoveBackward: actionPattern[1] = 1.0f; break;
            case ActionType::TurnLeft: actionPattern[2] = 1.0f; break;
            case ActionType::TurnRight: actionPattern[3] = 1.0f; break;
            case ActionType::Interact: actionPattern[4] = 1.0f; break;
            case ActionType::Wait: actionPattern[5] = 1.0f; break;
        }
        
        associate(actionPattern, episode.resultingSensoryState, 0.7f);
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(
    const std::vector<float>& queryPattern,
    size_t maxResults) const {
    std::vector<std::vector<float>> results;
    
    if (queryPattern.empty() || !brain_) return results;
    
    // Find the best matching pattern neuron
    NeuronId queryNeuron = INVALID_NEURON_ID;
    float bestSimilarity = 0.0f;
    
    for (const auto& pair : pImpl->neuronPatternMap) {
        float sim = computeSimilarity(queryPattern, pair.second);
        if (sim > bestSimilarity) {
            bestSimilarity = sim;
            queryNeuron = pair.first;
        }
    }
    
    if (bestSimilarity < 0.3f) return results;  // Not close enough
    
    // Retrieve all strongly associated patterns
    std::vector<std::pair<float, std::vector<float>>> scoredPatterns;
    
    for (const auto& assoc : pImpl->associations) {
        NeuronId otherNeuron;
        
        if (std::get<0>(assoc) == queryNeuron) {
            otherNeuron = std::get<1>(assoc);
        } else if (std::get<1>(assoc) == queryNeuron) {
            otherNeuron = std::get<0>(assoc);
        } else {
            continue;
        }
        
        if (otherNeuron == INVALID_NEURON_ID) continue;
        
        auto it = pImpl->neuronPatternMap.find(otherNeuron);
        if (it != pImpl->neuronPatternMap.end()) {
            float strength = std::get<2>(assoc);
            
            // Apply activation based on similarity to query
            if (bestSimilarity > 0.7f) {
                strength *= bestSimilarity;
            }
            
            scoredPatterns.emplace_back(strength, it->second);
        }
    }
    
    // Sort by strength (descending)
    std::sort(scoredPatterns.begin(), scoredPatterns.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top results
    for (size_t i = 0; i < std::min(maxResults, scoredPatterns.size()); ++i) {
        results.push_back(scoredPatterns[i].second);
    }
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB) const {
    if (patternA.empty() || patternB.empty() || !brain_) return 0.0f;
    
    // Find neurons for the patterns
    NeuronId neuronA = INVALID_NEURON_ID;
    NeuronId neuronB = INVALID_NEURON_ID;
    
    float bestSimA = 0.0f;
    float bestSimB = 0.0f;
    
    for (const auto& pair : pImpl->neuronPatternMap) {
        float simA = computeSimilarity(patternA, pair.second);
        float simB = computeSimilarity(patternB, pair.second);
        
        if (simA > bestSimA) {
            bestSimA = simA;
            neuronA = pair.first;
        }
        if (simB > bestSimB) {
            bestSimB = simB;
            neuronB = pair.first;
        }
    }
    
    if (bestSimA < 0.3f || bestSimB < 0.3f) return 0.0f;
    
    // Look up association strength
    for (const auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            return std::get<2>(assoc);
        }
    }
    
    return 0.0f;
}

void NeuralAssociativeMemory::updateAssociation(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB,
    float delta) {
    if (patternA.empty() || patternB.empty() || !brain_) return;
    
    // Find neurons
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA == INVALID_NEURON_ID || neuronB == INVALID_NEURON_ID) return;
    
    // Update association
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            return;
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::spreadActivation(
    const std::vector<float>& cuePattern,
    size_t steps) const {
    std::vector<std::vector<float>> activatedPatterns;
    std::set<NeuronId> visitedNeurons;
    
    if (cuePattern.empty()) return activatedPatterns;
    
    // Find starting neuron for the cue
    NeuronId currentNeuron = INVALID_NEURON_ID;
    float bestSimilarity = 0.0f;
    
    for (const auto& pair : pImpl->neuronPatternMap) {
        float sim = computeSimilarity(cuePattern, pair.second);
        if (sim > bestSimilarity) {
            bestSimilarity = sim;
            currentNeuron = pair.first;
        }
    }
    
    if (currentNeuron == INVALID_NEURON_ID) return activatedPatterns;
    
    activatedPatterns.push_back(pImpl->neuronPatternMap.at(currentNeuron));
    visitedNeurons.insert(currentNeuron);
    
    // Spread activation through association network
    for (size_t step = 0; step < steps; ++step) {
        std::vector<NeuronId> nextNeurons;
        
        // Find all neurons associated with current neuron
        for (const auto& assoc : pImpl->associations) {
            NeuronId neighbor = INVALID_NEURON_ID;
            
            if (std::get<0>(assoc) == currentNeuron && 
                visitedNeurons.find(std::get<1>(assoc)) == visitedNeurons.end()) {
                neighbor = std::get<1>(assoc);
            } else if (std::get<1>(assoc) == currentNeuron &&
                       visitedNeurons.find(std::get<0>(assoc)) == visitedNeurons.end()) {
                neighbor = std::get<0>(assoc);
            }
            
            if (neighbor != INVALID_NEURON_ID) {
                nextNeurons.push_back(neighbor);
                visitedNeurons.insert(neighbor);
            }
        }
        
        // Activate neighbors with decaying strength
        for (const auto& neighbor : nextNeurons) {
            float activation = std::get<2>(pImpl->associations.back()) * 0.5f;
            
            // Find pattern for this neuron
            auto it = pImpl->neuronPatternMap.find(neighbor);
            if (it != pImpl->neuronPatternMap.end()) {
                activatedPatterns.push_back(it->second);
            }
        }
        
        // Pick the most strongly associated neuron
        if (!nextNeurons.empty()) {
            currentNeuron = nextNeurons[0];
        } else {
            break;
        }
    }
    
    return activatedPatterns;
}

void NeuralAssociativeMemory::clear() {
    pImpl->patternNeuronCache.clear();
    pImpl->associations.clear();
    pImpl->neuronPatternMap.clear();
    commonPatterns_.clear();
}

NeuronId NeuralAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    if (pattern.empty() || !brain_) return INVALID_NEURON_ID;
    
    // First check cache
    size_t hash = computePatternHash(pattern);
    
    auto it = pImpl->patternNeuronCache.find(hash);
    if (it != pImpl->patternNeuronCache.end()) {
        return it->second;
    }
    
    // Check for similar pattern
    for (const auto& pair : pImpl->neuronPatternMap) {
        if (computeSimilarity(pattern, pair.second) > 0.8f) {
            pImpl->patternNeuronCache[hash] = pair.first;
            return pair.first;
        }
    }
    
    // Create new neuron for this pattern
    NeuronId newId = generateNeuronId();
    
    // Create neuron if it doesn't exist
    if (!neuronExists(newId)) {
        createPatternNeuron(newId, pattern);
    }
    
    // Store in maps
    pImpl->neuronPatternMap[newId] = pattern;
    pImpl->patternNeuronCache[hash] = newId;
    
    // Add to common patterns list for quick access
    commonPatterns_.push_back(pattern);
    
    return newId;
}

float NeuralAssociativeMemory::computeSimilarity(const std::vector<float>& a,
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
    
    return dotProduct / std::sqrt(normA * normB);
}

void NeuralAssociativeMemory::strengthenPatternNeurons(const std::vector<NeuronId>& neurons,
                                                      float amount) {
    if (!brain_) return;
    
    for (const auto& neuronId : neurons) {
        // Find the region containing this neuron
        for (const auto& region : brain_->getRegions()) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : *neurons) {
                if (neuron && neuron->getId() == neuronId) {
                    // Apply strengthening current
                    float strengtheningCurrent = amount * 10.0f;
                    neuron->injectCurrent(strengtheningCurrent);
                    break;
                }
            }
        }
    }
}

bool NeuralAssociativeMemory::neuronExists(NeuronId neuronId) const {
    for (const auto& region : brain_->getRegions()) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : *neurons) {
            if (neuron && neuron->getId() == neuronId) {
                return true;
            }
        }
    }
    return false;
}

void NeuralAssociativeMemory::createPatternNeuron(NeuronId neuronId, const std::vector<float>& pattern) {
    // Find or create region for this neuron
    RegionId regionId(1);  // Default to region 1
    
    // Try to get existing region or create new one
    NeuralRegion* region = brain_->getRegion(regionId);
    if (!region) {
        region = brain_->addRegion();
        regionId = region->getId();
    }
    
    // Create a new population for pattern neurons
    PopulationId popId = region->addPopulation(1, NeuronType::Internal);
    NeuralPopulation* population = region->getPopulation(popId);
    
    if (population) {
        auto neurons = population->getNeurons();
        for (auto* neuron : *neurons) {
            if (neuron && neuron->getId() == neuronId) {
                // Set initial activation based on pattern
                if (!pattern.empty()) {
                    float initialActivation = pattern[0];
                    neuron->injectCurrent(initialActivation * 5.0f);
                }
                break;
            }
        }
    }
}

NeuronId NeuralAssociativeMemory::generateNeuronId() {
    // Generate a unique neuron ID
    static uint64_t neuronCounter = 30000;
    return NeuronId(neuronCounter++);
}

size_t NeuralAssociativeMemory::computePatternHash(const std::vector<float>& pattern) const {
    std::hash<float> hasher;
    size_t seed = 0;
    
    for (float val : pattern) {
        seed ^= hasher(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    
    return seed;
}

} // namespace nlm
