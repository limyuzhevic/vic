#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : capacity_(1000)
    , integrateHebbian_(true)
{
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(std::shared_ptr<Brain> brain) {
    brain_ = brain;
    patterns_.clear();
    patterns_.reserve(capacity_);
    NLM_LOG_INFO("NeuralAssociativeMemory initialized");
}

void NeuralAssociativeMemory::reset() {
    patterns_.clear();
    currentActivations_.clear();
    NLM_LOG_INFO("NeuralAssociativeMemory reset");
}

void NeuralAssociativeMemory::storePattern(const std::vector<NeuronId>& pattern, float strength) {
    if (pattern.empty()) return;
    
    // Check if pattern already exists
    size_t existingIndex = findPatternIndex(pattern);
    if (existingIndex != -1) {
        // Update existing pattern
        patterns_[existingIndex].strength = strength;
        patterns_[existingIndex].lastAccessed = 0.0f;
        patterns_[existingIndex].accessCount++;
    } else {
        // Add new pattern if within capacity
        if (patterns_.size() < capacity_) {
            PatternEntry entry;
            entry.pattern = pattern;
            entry.strength = strength;
            entry.lastAccessed = 0.0f;
            entry.accessCount = 1;
            patterns_.push_back(entry);
        } else {
            // Prune weakest pattern and add new one
            pruneWeakPatterns();
            PatternEntry entry;
            entry.pattern = pattern;
            entry.strength = strength;
            entry.lastAccessed = 0.0f;
            entry.accessCount = 1;
            patterns_.push_back(entry);
        }
    }
}

void NeuralAssociativeMemory::recallPattern(const std::vector<NeuronId>& pattern, std::vector<float>& activations) {
    activations.clear();
    activations.resize(pattern.size(), 0.0f);
    
    for (const auto& entry : patterns_) {
        // Compute similarity with stored pattern
        float similarity = computePatternSimilarity(entry, pattern);
        
        if (similarity > 0.5f) {  // Threshold for meaningful recall
            for (size_t i = 0; i < pattern.size(); ++i) {
                activations[i] += entry.strength * similarity;
            }
        }
    }
}

void NeuralAssociativeMemory::updatePattern(const std::vector<NeuronId>& pattern, float learningRate) {
    size_t index = findPatternIndex(pattern);
    if (index != -1) {
        // Hebbian-like update
        patterns_[index].strength += learningRate * (1.0f - patterns_[index].strength);
        patterns_[index].lastAccessed = 0.0f;
    }
}

void NeuralAssociativeMemory::integrateWithHebbian(bool enable) {
    integrateHebbian_ = enable;
}

void NeuralAssociativeMemory::processHebbianSpikes(const std::vector<NeuronId>& preNeurons, const std::vector<NeuronId>& postNeurons) {
    if (!integrateHebbian_) return;
    
    // Create pattern from pre-post pair
    std::vector<NeuronId> pattern;
    pattern.reserve(preNeurons.size() + postNeurons.size());
    pattern.insert(pattern.end(), preNeurons.begin(), preNeurons.end());
    pattern.insert(pattern.end(), postNeurons.begin(), postNeurons.end());
    
    // Update pattern strength based on co-activation
    float newStrength = std::min(1.0f, static_cast<float>(preNeurons.size() + postNeurons.size()) * 0.01f);
    storePattern(pattern, newStrength);
}

size_t NeuralAssociativeMemory::getPatternCount() const {
    return patterns_.size();
}

size_t NeuralAssociativeMemory::getPatternCapacity() const {
    return capacity_;
}

void NeuralAssociativeMemory::setPatternCapacity(size_t capacity) {
    capacity_ = capacity;
    pruneWeakPatterns();
}

float NeuralAssociativeMemory::getPatternStrength(const std::vector<NeuronId>& pattern) const {
    size_t index = findPatternIndex(pattern);
    if (index != -1) {
        return patterns_[index].strength;
    }
    return 0.0f;
}

void NeuralAssociativeMemory::increaseStrength(const std::vector<NeuronId>& pattern, float amount) {
    size_t index = findPatternIndex(pattern);
    if (index != -1) {
        patterns_[index].strength = std::min(1.0f, patterns_[index].strength + amount);
    }
}

void NeuralAssociativeMemory::decreaseStrength(const std::vector<NeuronId>& pattern, float amount) {
    size_t index = findPatternIndex(pattern);
    if (index != -1) {
        patterns_[index].strength = std::max(0.0f, patterns_[index].strength - amount);
    }
}

size_t NeuralAssociativeMemory::getTotalActivations() const {
    size_t total = 0;
    for (const auto& entry : patterns_) {
        total += entry.accessCount;
    }
    return total;
}

float NeuralAssociativeMemory::getAverageActivationStrength() const {
    if (patterns_.empty()) return 0.0f;
    
    float totalStrength = 0.0f;
    for (const auto& entry : patterns_) {
        totalStrength += entry.strength;
    }
    
    return totalStrength / patterns_.size();
}

void NeuralAssociativeMemory::pruneWeakPatterns() {
    // Remove weakest 10% of patterns to maintain capacity
    if (patterns_.size() <= capacity_) return;
    
    std::sort(patterns_.begin(), patterns_.end(),
              [](const PatternEntry& a, const PatternEntry& b) {
                  return a.strength < b.strength;
              });
    
    size_t removeCount = patterns_.size() - capacity_;
    patterns_.erase(patterns_.begin(), patterns_.begin() + removeCount);
}

size_t NeuralAssociativeMemory::findPatternIndex(const std::vector<NeuronId>& pattern) const {
    for (size_t i = 0; i < patterns_.size(); ++i) {
        if (patterns_[i].pattern == pattern) {
            return i;
        }
    }
    return -1;
}

float NeuralAssociativeMemory::computePatternSimilarity(const PatternEntry& entry, const std::vector<NeuronId>& pattern) const {
    if (entry.pattern.size() != pattern.size()) return 0.0f;
    
    size_t matches = 0;
    for (size_t i = 0; i < entry.pattern.size(); ++i) {
        if (entry.pattern[i] == pattern[i]) {
            matches++;
        }
    }
    
    return static_cast<float>(matches) / entry.pattern.size();
}

} // namespace nlm

