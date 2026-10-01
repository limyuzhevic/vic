#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <set>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    
    // Pattern storage for associations
    struct Association {
        std::vector<float> patternA;
        std::vector<float> patternB;
        float strength;
        Timestamp timestamp;
        
        Association(const std::vector<float>& a, const std::vector<float>& b, float s)
            : patternA(a), patternB(b), strength(s), timestamp(0.0f) {}
    };
    
    std::vector<Association> associations;
    
    // Pattern completion
    struct PatternNode {
        std::vector<float> pattern;
        std::set<NeuronId> connectedNeurons;
        float activationLevel;
        
        PatternNode(const std::vector<float>& p) 
            : pattern(p), activationLevel(0.0f) {}
    };
    
    std::vector<PatternNode> patternNodes;
    
    Impl() : brain(nullptr) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
{
    NLM_LOG_INFO("NeuralAssociativeMemory created");
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("NeuralAssociativeMemory initialized");
}

void NeuralAssociativeMemory::associate(const std::vector<float>& patternA,
                                       const std::vector<float>& patternB,
                                       float strength) {
    // Create association
    Association assoc(patternA, patternB, strength);
    
    // Normalize patterns
    std::vector<float> normA = normalize(patternA);
    std::vector<float> normB = normalize(patternB);
    
    // Update existing association if similar pattern exists
    bool found = false;
    for (auto& existing : pImpl->associations) {
        if (computeSimilarity(normA, existing.patternA) > 0.9f &&
            computeSimilarity(normB, existing.patternB) > 0.9f) {
            // Update strength
            existing.strength = (existing.strength + strength) / 2.0f;
            existing.timestamp = getCurrentTime();
            found = true;
            break;
        }
    }
    
    if (!found) {
        pImpl->associations.push_back({normA, normB, strength, getCurrentTime()});
        
        // Create pattern nodes for distributed representation
        createPatternNode(normA);
        createPatternNode(normB);
        
        // Link neurons between patterns
        linkPatterns(normA, normB, strength);
    }
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    // Extract patterns from episode
    std::vector<float> sensoryPattern = extractSensoryPattern(episode);
    std::vector<float> rewardPattern(1, episode.reward);
    std::vector<float> actionPattern = extractActionPattern(episode);
    
    // Create associations
    if (!sensoryPattern.empty() && !rewardPattern.empty()) {
        associate(sensoryPattern, rewardPattern, 0.8f);
    }
    
    if (!sensoryPattern.empty() && !actionPattern.empty()) {
        associate(sensoryPattern, actionPattern, 0.6f);
    }
    
    // Temporal associations: sequence of patterns
    if (!actionPattern.empty() && !rewardPattern.empty()) {
        associate(actionPattern, rewardPattern, 0.7f);
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(
    const std::vector<float>& pattern,
    size_t maxResults) const {
    
    std::vector<std::pair<float, size_t>> scores;
    
    // Compute similarity to stored associations
    for (size_t i = 0; i < pImpl->associations.size(); ++i) {
        const auto& assoc = pImpl->associations[i];
        
        float simToA = computeSimilarity(pattern, assoc.patternA);
        float simToB = computeSimilarity(pattern, assoc.patternB);
        
        // Consider both patterns
        float score = std::max(simToA, simToB);
        if (score > 0.3f) {
            scores.emplace_back(score, i);
        }
    }
    
    // Sort by similarity
    std::sort(scores.begin(), scores.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top results
    std::vector<std::vector<float>> results;
    for (size_t i = 0; i < std::min(maxResults, scores.size()); ++i) {
        size_t idx = scores[i].second;
        if (scores[i].first > 0.4f) {
            // Return the associated pattern
            results.push_back(pImpl->associations[idx].patternB);
        }
    }
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB) const {
    
    for (const auto& assoc : pImpl->associations) {
        float simA = computeSimilarity(patternA, assoc.patternA);
        float simB = computeSimilarity(patternB, assoc.patternB);
        
        if (simA > 0.8f && simB > 0.8f) {
            return assoc.strength;
        }
    }
    
    return 0.0f;
}

void NeuralAssociativeMemory::updateAssociation(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB,
    float newStrength) {
    
    for (auto& assoc : pImpl->associations) {
        float simA = computeSimilarity(patternA, assoc.patternA);
        float simB = computeSimilarity(patternB, assoc.patternB);
        
        if (simA > 0.9f && simB > 0.9f) {
            assoc.strength = newStrength;
            assoc.timestamp = getCurrentTime();
            break;
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::spreadActivation(
    const std::vector<float>& pattern,
    float activationThreshold) const {
    
    std::vector<std::vector<float>> results;
    
    // Find similar patterns
    auto similar = retrieve(pattern, 5);
    
    for (const auto& assocPattern : similar) {
        float activation = computeSimilarity(pattern, normalize(assocPattern));
        
        if (activation >= activationThreshold) {
            results.push_back(assocPattern);
        }
    }
    
    return results;
}

void NeuralAssociativeMemory::clear() {
    pImpl->associations.clear();
    pImpl->patternNodes.clear();
    NLM_LOG_INFO("NeuralAssociativeMemory cleared");
}

void NeuralAssociativeMemory::createPatternNode(const std::vector<float>& pattern) {
    bool found = false;
    for (auto& node : pImpl->patternNodes) {
        if (computeSimilarity(node.pattern, pattern) > 0.8f) {
            found = true;
            break;
        }
    }
    
    if (!found) {
        pImpl->patternNodes.emplace_back(pattern);
    }
}

void NeuralAssociativeMemory::linkPatterns(const std::vector<float>& patternA,
                                           const std::vector<float>& patternB,
                                           float strength) {
    if (!pImpl->brain) return;
    
    // Find pattern neurons
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA && neuronB && neuronA != neuronB) {
        // Create bidirectional connections
        pImpl->brain->addInterRegionConnection(
            neuronA, neuronB, strength * 0.5f, 1);
        pImpl->brain->addInterRegionConnection(
            neuronB, neuronA, strength * 0.5f, 1);
    }
}

NeuronId NeuralAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    // Create a unique ID from pattern hash
    uint64_t hash = 0;
    for (float val : pattern) {
        hash = hash * 31 + static_cast<uint64_t>(val * 1000);
    }
    
    // Check if we already have a neuron for this pattern
    for (const auto& node : pImpl->patternNodes) {
        if (computeSimilarity(node.pattern, pattern) > 0.9f) {
            return NeuronId(hash % 10000);
        }
    }
    
    return NeuronId();
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
    
    if (normA > 0.0f && normB > 0.0f) {
        return dotProduct / std::sqrt(normA * normB);
    }
    
    return 0.0f;
}

std::vector<float> NeuralAssociativeMemory::normalize(const std::vector<float>& vec) const {
    std::vector<float> result = vec;
    float norm = 0.0f;
    
    for (float val : result) {
        norm += val * val;
    }
    
    if (norm > 0.0f) {
        float sqrtNorm = std::sqrt(norm);
        for (auto& val : result) {
            val /= sqrtNorm;
        }
    }
    
    return result;
}

std::vector<float> NeuralAssociativeMemory::extractSensoryPattern(const EpisodicMemoryItem& episode) const {
    std::vector<float> pattern;
    
    // Extract vision data
    if (!episode.sensoryState.empty()) {
        for (const auto& signal : episode.sensoryState) {
            pattern.push_back(signal);
        }
    }
    
    // Add reward as part of pattern
    pattern.push_back(episode.reward);
    
    // Add action type as encoded value
    pattern.push_back(static_cast<float>(episode.actionType));
    
    return pattern;
}

std::vector<float> NeuralAssociativeMemory::extractActionPattern(const EpisodicMemoryItem& episode) const {
    std::vector<float> pattern(6, 0.0f);  // One-hot encoding of action types
    
    // Convert action type to one-hot
    size_t actionIdx = static_cast<size_t>(episode.actionType);
    if (actionIdx < pattern.size()) {
        pattern[actionIdx] = 1.0f;
    }
    
    return pattern;
}

Timestamp NeuralAssociativeMemory::getCurrentTime() const {
    // Return current simulation time
    // In real implementation, would get from brain clock
    return 0.0f;
}

} // namespace nlm
