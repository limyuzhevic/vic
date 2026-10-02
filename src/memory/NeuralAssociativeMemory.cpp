// NeuralAssociativeMemory.cpp - Enhanced Neural Associative Memory Implementation

#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"
#include "../memory/MemorySystemIntegration.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <chrono>
#include <thread>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    
    // Pattern neurons for associative recall
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons;
    
    Impl() : brain(nullptr) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , associationStrength_(1.0f)
    , interferenceLevel_(0.0f)
    , plasticityInfluence_(0.0f)
{
    NLM_LOG_INFO("NeuralAssociativeMemory: Created enhanced associative memory system");
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() {
    NLM_LOG_INFO("NeuralAssociativeMemory: Destroyed enhanced associative memory system");
}

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Initialize enhanced associative memory components
    setupEnhancedStorage();
    setupInterferenceManagement();
    
    NLM_LOG_INFO("NeuralAssociativeMemory: Initialized with brain reference and enhanced systems");
}

void NeuralAssociativeMemory::setupEnhancedStorage() {
    // Set up efficient storage for associations
    associations_.clear();
    patternNeurons_.clear();
    
    // Set up cache for frequently accessed associations
    associationCache_.clear();
    highPriorityAssociations_.clear();
    
    NLM_LOG_INFO("NeuralAssociativeMemory: Enhanced storage system initialized");
}

void NeuralAssociativeMemory::setupInterferenceManagement() {
    // Set up interference management
    interferenceMap_.clear();
    spreadingActivation_ = 0.0f;
    
    NLM_LOG_INFO("NeuralAssociativeMemory: Interference management initialized");
}

void NeuralAssociativeMemory::associate(const std::vector<float>& patternA,
                                        const std::vector<float>& patternB,
                                        float strength) {
    // Create association between two patterns
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA != INVALID_NEURON_ID && neuronB != INVALID_NEURON_ID) {
        // Check if association exists
        for (auto& assoc : associations_) {
            if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
                (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
                // Update strength
                std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + strength * 0.1f, 0.0f, 1.0f);
                return;
            }
        }
        
        // Create new association
        associations_.emplace_back(neuronA, neuronB, strength);
        
        // Check for potential interference
        detectInterference(neuronA, neuronB);
    }
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    if (!episode.sensoryState.empty() && !episode.resultingSensoryState.empty()) {
        // Create association based on experience
        associate(episode.sensoryState, episode.resultingSensoryState, 0.8f);
        
        // Associate action with resulting state
        if (!episode.sensoryState.empty()) {
            // Create action-specific pattern
            std::vector<float> actionPattern(episode.sensoryState.size(), 0.0f);
            actionPattern[0] = static_cast<float>(episode.action) / 10.0f;
            
            associate(actionPattern, episode.resultingSensoryState, 0.7f);
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(
    const std::vector<float>& queryPattern,
    size_t maxResults) const {
    
    NeuronId queryNeuron;
    float bestSim = 0.0f;
    
    // Find best matching pattern neuron
    for (size_t i = 0; i < patternNeurons_.size(); ++i) {
        float sim = computeSimilarity(queryPattern, patternNeurons_[i].second);
        if (sim > bestSim) {
            bestSim = sim;
            queryNeuron = patternNeurons_[i].first;
        }
    }
    
    if (bestSim < 0.3f) {
        return {};
    }
    
    // Find associated patterns
    std::vector<std::vector<float>> results;
    std::vector<std::pair<float, std::vector<float>>> scored;
    
    for (const auto& assoc : associations_) {
        NeuronId otherNeuron;
        
        if (std::get<0>(assoc) == queryNeuron) {
            otherNeuron = std::get<1>(assoc);
        } else if (std::get<1>(assoc) == queryNeuron) {
            otherNeuron = std::get<0>(assoc);
        }
        
        if (otherNeuron != INVALID_NEURON_ID) {
            for (const auto& pn : patternNeurons_) {
                if (pn.first == otherNeuron) {
                    float strength = std::get<2>(assoc);
                    scored.emplace_back(strength, pn.second);
                    break;
                }
            }
        }
    }
    
    // Sort by strength
    std::sort(scored.begin(), scored.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    for (size_t i = 0; i < std::min(maxResults, scored.size()); ++i) {
        results.push_back(scored[i].second);
    }
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB) const {
    
    NeuronId neuronA, neuronB;
    float bestSimA = 0.0f, bestSimB = 0.0f;
    
    for (const auto& pn : patternNeurons_) {
        float simA = computeSimilarity(patternA, pn.second);
        float simB = computeSimilarity(patternB, pn.second);
        
        if (simA > bestSimA) {
            bestSimA = simA;
            neuronA = pn.first;
        }
        if (simB > bestSimB) {
            bestSimB = simB;
            neuronB = pn.first;
        }
    }
    
    if (bestSimA < 0.3f || bestSimB < 0.3f) {
        return 0.0f;
    }
    
    for (const auto& assoc : associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            return std::get<2>(assoc);
        }
    }
    
    return 0.0f;
}

void NeuralAssociativeMemory::updateAssociation(const std::vector<float>& patternA,
                                                const std::vector<float>& patternB,
                                                float delta) {
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    for (auto& assoc : associations_) {
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
    
    std::vector<std::vector<float>> activated;
    std::set<NeuronId> visited;
    
    // Find initial pattern neuron
    NeuronId currentNeuron;
    for (const auto& pn : patternNeurons_) {
        if (computeSimilarity(cuePattern, pn.second) > 0.5f) {
            currentNeuron = pn.first;
            activated.push_back(pn.second);
            visited.insert(currentNeuron);
            break;
        }
    }
    
    if (currentNeuron == INVALID_NEURON_ID) {
        return activated;
    }
    
    // Spread through association network
    for (size_t step = 0; step < steps; ++step) {
        std::vector<NeuronId> nextNeurons;
        
        for (const auto& assoc : associations_) {
            NeuronId otherNeuron = INVALID_NEURON_ID;
            
            if (std::get<0>(assoc) == currentNeuron && visited.find(std::get<1>(assoc)) == visited.end()) {
                otherNeuron = std::get<1>(assoc);
            } else if (std::get<1>(assoc) == currentNeuron && visited.find(std::get<0>(assoc)) == visited.end()) {
                otherNeuron = std::get<0>(assoc);
            }
            
            if (otherNeuron != INVALID_NEURON_ID && std::get<2>(assoc) > 0.3f) {
                nextNeurons.push_back(otherNeuron);
                visited.insert(otherNeuron);
                
                // Find pattern for this neuron
                for (const auto& pn : patternNeurons_) {
                    if (pn.first == otherNeuron) {
                        activated.push_back(pn.second);
                        break;
                    }
                }
            }
        }
        
        if (nextNeurons.empty()) break;
        
        // Pick highest associated next neuron
        currentNeuron = nextNeurons[0];
    }
    
    return activated;
}

void NeuralAssociativeMemory::clear() {
    patternNeurons_.clear();
    associations_.clear();
    interferenceMap_.clear();
    associationCache_.clear();
    highPriorityAssociations_.clear();
}

NeuronId NeuralAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    // Check if we already have a similar pattern
    for (const auto& pn : patternNeurons_) {
        if (computeSimilarity(pattern, pn.second) > 0.8f) {
            return pn.first;
        }
    }
    
    // Create new pattern neuron
    NeuronId newId(patternNeurons_.size() + 30000);
    patternNeurons_.push_back({newId, pattern});
    return newId;
}

float NeuralAssociativeMemory::computeSimilarity(const std::vector<float>& a,
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

void NeuralAssociativeMemory::detectInterference(NeuronId neuronA, NeuronId neuronB) {
    // Detect interference between patterns
    std::vector<NeuronId> similarNeurons;
    
    // Find neurons that are similar to both
    for (const auto& pn : patternNeurons_) {
        if (pn.first != neuronA && pn.first != neuronB) {
            float simToA = computeSimilarity(pn.second, getPatternFromNeuron(neuronA));
            float simToB = computeSimilarity(pn.second, getPatternFromNeuron(neuronB));
            
            if (simToA > 0.7f && simToB > 0.7f) {
                similarNeurons.push_back(pn.first);
            }
        }
    }
    
    if (!similarNeurons.empty()) {
        // Update interference map
        interferenceMap_[neuronA] = similarNeurons.size();
        interferenceMap_[neuronB] = similarNeurons.size();
        
        // Update overall interference level
        interferenceLevel_ = std::min(1.0f, interferenceLevel_ + 0.1f);
        
        NLM_LOG_WARN("NeuralAssociativeMemory: Interference detected between neurons " +
                     std::to_string(neuronA) + " and " + std::to_string(neuronB));
    }
}

void NeuralAssociativeMemory::resolveInterference(const std::vector<float>& pattern) {
    // Resolve interference with existing patterns
    NeuronId patternNeuron = findPatternNeuron(pattern);
    
    if (patternNeuron != INVALID_NEURON_ID) {
        // Check if this neuron is in interference map
        if (interferenceMap_.find(patternNeuron) != interferenceMap_.end()) {
            // Reduce interference by strengthening the pattern
            interferenceMap_[patternNeuron] = 0;
            
            // Reduce overall interference level
            interferenceLevel_ = std::max(0.0f, interferenceLevel_ - 0.2f);
            
            NLM_LOG_INFO("NeuralAssociativeMemory: Interference resolved for neuron " +
                         std::to_string(patternNeuron));
        }
    }
}

std::vector<float> NeuralAssociativeMemory::getPatternFromNeuron(NeuronId neuron) const {
    // Get pattern from neuron
    for (const auto& pn : patternNeurons_) {
        if (pn.first == neuron) {
            return pn.second;
        }
    }
    return {};
}

void NeuralAssociativeMemory::cacheAssociation(size_t index, bool highPriority) {
    // Cache association for faster access
    if (index >= associations_.size()) {
        return;
    }
    
    // Check if already cached
    if (associationCache_.find(index) != associationCache_.end()) {
        return;
    }
    
    // Add to cache
    associationCache_[index] = associations_[index];
    
    if (highPriority) {
        highPriorityAssociations_.insert(index);
    }
}

void NeuralAssociativeMemory::optimizeCache() {
    // Optimize cache for better performance
    if (associationCache_.size() > config_.cacheSize) {
        // Remove least recently used entries
        size_t toRemove = associationCache_.size() - config_.cacheSize;
        auto it = associationCache_.begin();
        for (size_t i = 0; i < toRemove && it != associationCache_.end(); ++i, ++it) {
            if (highPriorityAssociations_.find(i) == highPriorityAssociations_.end()) {
                associationCache_.erase(std::next(associationCache_.begin(), i));
                highPriorityAssociations_.erase(i);
            }
        }
    }
}

void NeuralAssociativeMemory::setAssociationStrength(float strength) {
    associationStrength_ = strength;
    
    // Update all associations
    for (auto& assoc : associations_) {
        std::get<2>(assoc) *= strength;
    }
}

void NeuralAssociativeMemory::setInterferenceThreshold(float threshold) {
    interferenceThreshold_ = threshold;
}

void NeuralAssociativeMemory::setPlasticityInfluence(float influence) {
    plasticityInfluence_ = influence;
}

void NeuralAssociativeMemory::autoTune(float performanceThreshold) {
    // Auto-tune associative memory configuration
    analyzePerformance();
    
    float currentScore = getPerformanceScore();
    
    if (currentScore < performanceThreshold) {
        // Adjust configuration for better performance
        if (associationStrength_ > 0.5f) {
            associationStrength_ *= 0.9f;
        }
        if (interferenceLevel_ > 0.5f) {
            interferenceLevel_ *= 0.9f;
        }
    } else {
        // Improve configuration for better performance
        if (associationStrength_ < 1.0f) {
            associationStrength_ *= 1.1f;
        }
        if (interferenceLevel_ < 0.2f) {
            interferenceLevel_ *= 1.1f;
        }
    }
}

void NeuralAssociativeMemory::analyzePerformance() {
    // Analyze performance metrics
    size_t associationCount = associations_.size();
    size_t cacheHits = 0;
    
    // Calculate cache hit rate
    for (const auto& [index, assoc] : associationCache_) {
        if (index < associations_.size() && assoc == associations_[index]) {
            cacheHits++;
        }
    }
    
    if (!associationCache_.empty()) {
        float cacheHitRate = static_cast<float>(cacheHits) / static_cast<float>(associationCache_.size());
        NLM_LOG_INFO("NeuralAssociativeMemory: Cache hit rate: " + std::to_string(cacheHitRate * 100.0f) + "%");
    }
    
    if (associationCount > 1000) {
        NLM_LOG_WARN("NeuralAssociativeMemory: High association count: " + std::to_string(associationCount));
    }
}

float NeuralAssociativeMemory::getPerformanceScore() const {
    float associationCountScore = std::min(1.0f, 1000.0f / static_cast<float>(associations_.size()));
    float interferenceScore = 1.0f - interferenceLevel_;
    float cacheScore = std::min(1.0f, static_cast<float>(associationCache_.size()) / 100.0f);
    
    return (associationCountScore + interferenceScore + cacheScore) / 3.0f;
}

} // namespace nlm
