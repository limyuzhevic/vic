// NeuralAssociativeMemory Implementation
// Phase 6: Pattern Association and Memory System
// Stores and retrieves pattern associations based on partial cues

#include "NeuralAssociativeMemory.hpp"
#include "../brain/Brain.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <queue>
#include <limits>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    // Pattern storage: maps pattern IDs to activation patterns
    std::unordered_map<MemoryId, std::vector<float>> patterns;
    
    // Association weights between patterns
    std::unordered_map<std::pair<MemoryId, MemoryId>, float> associations;
    
    // Activation thresholds for pattern retrieval
    static constexpr float RETRIEVAL_THRESHOLD = 0.5f;
    static constexpr float ACTIVATION_DECAY = 0.95f;  // Decay per timestep
    static constexpr float MAX_PATTERN_SIZE = 1000.0f;  // Maximum pattern vector size
    
    // Hebbian learning parameters
    static constexpr float HEBBIAN_LEARNING_RATE = 0.01f;
    static constexpr float CLEARING_THRESHOLD = 0.3f;  // For interference management
    
    // Pattern activation states
    std::unordered_map<MemoryId, float> activations;
    
    // Cue pattern for retrieval
    std::vector<float> currentCue;
    
    // Statistics
    size_t patternCount = 0;
    size_t associationCount = 0;
    size_t successfulRetrievals = 0;
    
    Impl() : patternCount(0), associationCount(0), successfulRetrievals(0) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory(std::shared_ptr<Brain> brain)
    : pImpl(std::make_unique<Impl>()), brain(brain) {
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

NeuralAssociativeMemory::NeuralAssociativeMemory(NeuralAssociativeMemory&& other) noexcept
    : pImpl(std::move(other.pImpl)), brain(std::move(other.brain)) {
}

NeuralAssociativeMemory& NeuralAssociativeMemory::operator=(NeuralAssociativeMemory&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        brain = std::move(other.brain);
    }
    return *this;
}

void NeuralAssociativeMemory::initialize(std::shared_ptr<Brain> brain) {
    this->brain = brain;
    
    // Initialize with empty state
    pImpl->patterns.clear();
    pImpl->associations.clear();
    pImpl->activations.clear();
    pImpl->currentCue.clear();
    
    pImpl->patternCount = 0;
    pImpl->associationCount = 0;
    pImpl->successfulRetrievals = 0;
    
    NLM_LOG_INFO("NeuralAssociativeMemory initialized");
}

MemoryId NeuralAssociativeMemory::storePattern(const std::vector<float>& pattern, 
                                                const std::string& description) {
    // Normalize and validate pattern
    std::vector<float> normalizedPattern = normalizePattern(pattern);
    
    // Generate unique ID
    MemoryId id(static_cast<MemoryId::ValueType>(++pImpl->patternCount));
    
    // Store pattern with metadata
    pImpl->patterns[id] = normalizedPattern;
    pImpl->activations[id] = 1.0f;  // Mark as active
    
    // Create description mapping (simplified - could use external map)
    if (!description.empty()) {
        // In a full implementation, this would map to pattern descriptions
    }
    
    NLM_LOG_INFO("Stored pattern " + std::to_string(id.index()) + 
                " with " + std::to_string(normalizedPattern.size()) + " dimensions");
    
    return id;
}

std::vector<float> NeuralAssociativeMemory::normalizePattern(const std::vector<float>& pattern) {
    if (pattern.empty()) {
        return {};
    }
    
    // Find max absolute value for normalization
    float maxVal = 0.0f;
    for (float val : pattern) {
        maxVal = std::max(maxVal, std::abs(val));
    }
    
    if (maxVal < 1e-6f) {  // Near-zero pattern
        return pattern;
    }
    
    // Normalize to [-1, 1] range
    std::vector<float> normalized;
    normalized.reserve(pattern.size());
    for (float val : pattern) {
        normalized.push_back(val / maxVal);
    }
    
    // Limit pattern size to prevent explosion
    if (normalized.size() > MAX_PATTERN_SIZE) {
        normalized.resize(static_cast<size_t>(MAX_PATTERN_SIZE));
    }
    
    return normalized;
}

bool NeuralAssociativeMemory::associatePatterns(MemoryId pattern1, MemoryId pattern2, 
                                               float strength) {
    if (!pImpl->patterns.count(pattern1) || !pImpl->patterns.count(pattern2)) {
        return false;  // One or both patterns don't exist
    }
    
    // Ensure symmetric association
    pImpl->associations[{pattern1, pattern2}] = strength;
    pImpl->associations[{pattern2, pattern1}] = strength;
    
    pImpl->associationCount++;
    
    // Update activations
    pImpl->activations[pattern1] = 1.0f;
    pImpl->activations[pattern2] = 1.0f;
    
    NLM_LOG_INFO("Associated patterns " + std::to_string(pattern1.index()) + 
                " and " + std::to_string(pattern2.index()) + 
                " with strength " + std::to_string(strength));
    
    return true;
}

std::vector<MemoryId> NeuralAssociativeMemory::retrievePattern(const std::vector<float>& cue, 
                                                               float threshold) {
    // Set current cue
    pImpl->currentCue = cue;
    
    // Normalize cue
    std::vector<float> normalizedCue = normalizePattern(cue);
    if (normalizedCue.empty()) {
        return {};  // No cue provided
    }
    
    // Calculate similarity to all stored patterns
    std::vector<std::pair<MemoryId, float>> similarities;
    
    for (const auto& entry : pImpl->patterns) {
        MemoryId patternId = entry.first;
        const std::vector<float>& pattern = entry.second;
        
        // Calculate cosine similarity
        float similarity = cosineSimilarity(normalizedCue, pattern);
        
        // Apply activation weighting
        float activation = pImpl->activations[patternId];
        float weightedSimilarity = similarity * activation;
        
        if (weightedSimilarity >= threshold) {
            similarities.emplace_back(patternId, weightedSimilarity);
        }
    }
    
    // Sort by similarity (highest first)
    std::sort(similarities.begin(), similarities.end(),
              [](const auto& a, const auto& b) {
                  return a.second > b.second;
              });
    
    // Return pattern IDs above threshold
    std::vector<MemoryId> retrievedIds;
    for (const auto& pair : similarities) {
        retrievedIds.push_back(pair.first);
        
        // Record successful retrieval for learning
        pImpl->successfulRetrievals++;
    }
    
    // Decay activations
    decayActivations();
    
    // Boost retrieved pattern activations
    for (MemoryId id : retrievedIds) {
        pImpl->activations[id] = std::min(1.0f, pImpl->activations[id] * 1.2f);
    }
    
    NLM_LOG_INFO("Retrieved " + std::to_string(retrievedIds.size()) + " patterns from cue (threshold: " +
                std::to_string(threshold) + ")");
    
    return retrievedIds;
}

float NeuralAssociativeMemory::cosineSimilarity(const std::vector<float>& a, 
                                               const std::vector<float>& b) {
    if (a.empty() || b.empty()) {
        return 0.0f;
    }
    
    size_t minSize = std::min(a.size(), b.size());
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < minSize; ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    float denominator = std::sqrt(normA) * std::sqrt(normB);
    if (denominator < 1e-6f) {
        return 0.0f;
    }
    
    return dotProduct / denominator;
}

void NeuralAssociativeMemory::decayActivations() {
    for (auto& activation : pImpl->activations) {
        activation.second *= pImpl->ACTIVATION_DECAY;
        
        // Remove deactivated patterns
        if (activation.second < CLEARING_THRESHOLD) {
            // Clear associations for deactivated patterns
            for (auto it = pImpl->associations.begin(); it != pImpl->associations.end(); ) {
                if (it->first.first == activation.first || it->first.second == activation.first) {
                    it = pImpl->associations.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }
}

void NeuralAssociativeMemory::applyHebbianLearning(const std::vector<NeuronId>& activeNeurons) {
    if (activeNeurons.size() < 2) {
        return;  // Need at least two active neurons for learning
    }
    
    // Convert neuron IDs to pattern space (simplified - could use neural activity patterns)
    std::vector<float> pattern;
    pattern.reserve(activeNeurons.size());
    
    for (NeuronId neuronId : activeNeurons) {
        // Use neuron ID as feature value (in real implementation, would use neural activity)
        pattern.push_back(1.0f);
    }
    
    // Store pattern if new
    MemoryId patternId = storePattern(pattern, "Hebbian-learned pattern");
    
    // Create associations with existing patterns
    for (const auto& existingPattern : pImpl->patterns) {
        if (existingPattern.first != patternId) {
            // Calculate similarity and create weak association
            float similarity = cosineSimilarity(pattern, existingPattern.second);
            if (similarity > CLEARING_THRESHOLD) {
                associatePatterns(patternId, existingPattern.first, 
                                similarity * HEBBIAN_LEARNING_RATE);
            }
        }
    }
}

void NeuralAssociativeMemory::update(float timestep) {
    // Update pattern activations and decay
    decayActivations();
    
    // Update pattern completion based on current brain state
    if (brain) {
        // Get current neural activity pattern
        std::vector<NeuronId> activeNeurons = getActiveNeurons();
        if (activeNeurons.size() > 0) {
            applyHebbianLearning(activeNeurons);
        }
    }
}

std::vector<NeuronId> NeuralAssociativeMemory::getActiveNeurons() {
    std::vector<NeuronId> activeNeurons;
    
    if (!brain) {
        return activeNeurons;
    }
    
    // Scan all regions and populations for active neurons
    for (const auto& region : brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (neuron->isFiring() || neuron->getFiringRate() > 0.1f) {
                    activeNeurons.push_back(neuron->getId());
                }
            }
        }
    }
    
    return activeNeurons;
}

MemoryId NeuralAssociativeMemory::getPatternId(const std::string& description) {
    // Simplified pattern ID retrieval (in full implementation would use description map)
    // For now, return first pattern with matching description (placeholder)
    return MemoryId(0);  // Placeholder
}

std::vector<float> NeuralAssociativeMemory::getPattern(MemoryId id) const {
    auto it = pImpl->patterns.find(id);
    if (it != pImpl->patterns.end()) {
        return it->second;
    }
    return {};  // Pattern not found
}

float NeuralAssociativeMemory::getPatternActivation(MemoryId id) const {
    auto it = pImpl->activations.find(id);
    if (it != pImpl->activations.end()) {
        return it->second;
    }
    return 0.0f;  // No activation
}

void NeuralAssociativeMemory::clear() {
    pImpl->patterns.clear();
    pImpl->associations.clear();
    pImpl->activations.clear();
    pImpl->currentCue.clear();
    
    pImpl->patternCount = 0;
    pImpl->associationCount = 0;
    pImpl->successfulRetrievals = 0;
    
    NLM_LOG_INFO("NeuralAssociativeMemory cleared");
}

size_t NeuralAssociativeMemory::getPatternCount() const {
    return pImpl->patternCount;
}

size_t NeuralAssociativeMemory::getAssociationCount() const {
    return pImpl->associationCount;
}

size_t NeuralAssociativeMemory::getSuccessfulRetrievals() const {
    return pImpl->successfulRetrievals;
}

float NeuralAssociativeMemory::getRetrievalRate() const {
    if (pImpl->patternCount == 0) {
        return 0.0f;
    }
    return static_cast<float>(pImpl->successfulRetrievals) / pImpl->patternCount;
}

void NeuralAssociativeMemory::enableForLearning(bool enable) {
    // Enable or disable Hebbian learning
    // In full implementation, this would control learning rules
    NLM_LOG_INFO("NeuralAssociativeMemory learning " + std::string(enable ? "enabled" : "disabled"));
}

void NeuralAssociativeMemory::setPatternSizeLimit(size_t limit) {
    // Set maximum pattern size (placeholder)
    // In full implementation, would enforce pattern size constraints
    NLM_LOG_INFO("Pattern size limit set to " + std::to_string(limit));
}

void NeuralAssociativeMemory::setSimilarityThreshold(float threshold) {
    // Set similarity threshold for retrieval (placeholder)
    // In full implementation, would use this threshold in retrieval
    NLM_LOG_INFO("Similarity threshold set to " + std::to_string(threshold));
}

std::string NeuralAssociativeMemory::getStats() const {
    std::ostringstream oss;
    oss << "=== NeuralAssociativeMemory Statistics ===\n";
    oss << "Patterns stored: " << pImpl->patternCount << "\n";
    oss << "Associations: " << pImpl->associationCount << "\n";
    oss << "Successful retrievals: " << pImpl->successfulRetrievals << "\n";
    oss << "Retrieval rate: " << getRetrievalRate() * 100.0f << "%\n";
    oss << "Current cue size: " << pImpl->currentCue.size() << "\n";
    oss << "Active patterns: " << pImpl->activations.size() << "\n";
    
    return oss.str();
}

} // namespace nlm
