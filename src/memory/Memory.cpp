#include "Memory.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <unordered_map>

namespace nlm {

// Working Memory Implementation
struct WorkingMemory::Impl {
    std::vector<std::pair<NeuronId, float>> items;
    size_t capacity;
    
    // Neural learning components
    std::unordered_map<NeuronId, float> recencyScore;
    std::unordered_map<NeuronId, float> activationScore;
    float totalActivation;
    
    // Implement sophisticated working memory with capacity limits
    Impl(size_t cap) : capacity(cap), totalActivation(0.0f) {
        // Initialize with reasonable defaults
        recencyScore.reserve(cap);
        activationScore.reserve(cap);
    }
    
    // Enhanced storage with neural prioritization
    void store(NeuronId neuron, float value) {
        // Check if item already exists
        for (auto& item : items) {
            if (item.first == neuron) {
                // Update existing item with neural learning
                float oldValue = item.second;
                item.second = value;
                
                // Update neural scores based on change
                float change = std::abs(value - oldValue);
                recencyScore[neuron] += change * 2.0f; // Recency weighted by change
                activationScore[neuron] = std::max(activationScore[neuron], value);
                
                totalActivation += change;
                return;
            }
        }
        
        // Handle capacity limits with eviction policy
        evictIfNecessary();
        
        // Store new item with neural initialization
        items.emplace_back(neuron, value);
        recencyScore[neuron] = 1.0f; // Fresh storage gets high recency
        activationScore[neuron] = std::abs(value);
        totalActivation += std::abs(value);
    }
    
    // Evict items based on neural prioritization (LRU with activation weighting)
    void evictIfNecessary() {
        if (items.size() < capacity) return;
        
        // Calculate eviction scores based on recency and activation
        std::vector<std::pair<size_t, float>> evictionScores;
        for (size_t i = 0; i < items.size(); ++i) {
            NeuronId neuron = items[i].first;
            float recency = recencyScore[neuron];
            float activation = activationScore[neuron];
            float score = recency * 0.6f + activation * 0.4f; // Weighted combination
            evictionScores.emplace_back(i, score);
        }
        
        // Sort by eviction score (lower score = should be evicted first)
        std::sort(evictionScores.begin(), evictionScores.end(), 
                 [](const auto& a, const auto& b) { return a.second < b.second; });
        
        // Evict lowest-scoring item
        size_t evictIndex = evictionScores[0].first;
        NeuronId evictNeuron = items[evictIndex].first;
        
        // Remove from all data structures
        recencyScore.erase(evictNeuron);
        activationScore.erase(evictNeuron);
        totalActivation -= std::abs(items[evictIndex].second);
        items.erase(items.begin() + evictIndex);
    }
    
    float retrieve(NeuronId neuron) const {
        for (const auto& item : items) {
            if (item.first == neuron) {
                return item.second;
            }
        }
        return 0.0f;
    }
    
    bool contains(NeuronId neuron) const {
        return recencyScore.find(neuron) != recencyScore.end();
    }
    
    void clear() {
        items.clear();
        recencyScore.clear();
        activationScore.clear();
        totalActivation = 0.0f;
    }
    
    size_t getCapacity() const {
        return capacity;
    }
    
    size_t getCurrentSize() const {
        return items.size();
    }
    
    // Enhanced decay with neural dynamics
    void decay(float decayRate) {
        if (decayRate <= 0.0f) return;
        
        // Neural decay: different decay rates based on activation levels
        for (auto& item : items) {
            float activation = activationScore[item.first];
            float itemDecay = decayRate * (1.0f + activation); // Higher activation = faster decay
            item.second *= (1.0f - itemDecay);
            
            // Update activation scores
            activationScore[item.first] = std::max(activationScore[item.first] * (1.0f - decayRate), 0.0f);
        }
        
        // Clean up zero-valued items
        items.erase(std::remove_if(items.begin(), items.end(),
                                 [](const auto& item) { return std::abs(item.second) < 0.001f; }),
                   items.end());
    }
    
    // Get items sorted by neural activation (for retrieval priority)
    std::vector<std::pair<NeuronId, float>> getSortedItems() const {
        std::vector<std::pair<size_t, float>> scores;
        for (size_t i = 0; i < items.size(); ++i) {
            NeuronId neuron = items[i].first;
            float recency = recencyScore.at(neuron);
            float activation = activationScore.at(neuron);
            float score = recency * 0.6f + activation * 0.4f;
            scores.emplace_back(i, score);
        }
        
        // Sort by score (descending)
        std::sort(scores.begin(), scores.end(),
                 [](const auto& a, const auto& b) { return a.second > b.second; });
        
        std::vector<std::pair<NeuronId, float>> result;
        for (const auto& score : scores) {
            result.push_back(items[score.first]);
        }
        
        return result;
    }
    
    // Get total activation level
    float getTotalActivation() const {
        return totalActivation;
    }
    
    // Neural interference mitigation
    void applyNeuralSaturation(float saturationThreshold) {
        for (auto& item : items) {
            float activation = activationScore[item.first];
            if (activation > saturationThreshold) {
                // Reduce item value to prevent saturation
                item.second *= saturationThreshold / activation;
                activationScore[item.first] = saturationThreshold;
            }
        }
    }
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // Phase 2: Implement real storage with capacity limits
    // Enhanced neural working memory with sophisticated prioritization
    
    pImpl->store(neuron, value);
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    return pImpl->retrieve(neuron);
}

bool WorkingMemory::contains(NeuronId neuron) const {
    return pImpl->contains(neuron);
}

void WorkingMemory::clear() {
    pImpl->clear();
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->getCapacity();
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->getCurrentSize();
}

void WorkingMemory::decay(float decayRate) {
    pImpl->decay(decayRate);
}

// Episodic Memory Implementation
struct EpisodicMemory::Impl {
    std::vector<EpisodicMemoryItem> episodes;
    size_t maxEpisodes;
    
    // Neural consolidation components
    std::vector<float> episodeStrengths;
    std::vector<float> consolidationScores;
    float averageConsolidation;
    
    Impl(size_t max) : maxEpisodes(max), averageConsolidation(0.0f) {}
    
    // Enhanced episode storage with neural indexing
    void storeEpisode(const EpisodicMemoryItem& episode) {
        // Generate neural index for the episode
        size_t neuralIndex = generateNeuralIndex(episode);
        
        // Check for interference with existing episodes
        float interference = calculateInterference(episode, neuralIndex);
        
        // Store episode with strength tracking
        if (episodes.size() >= maxEpisodes) {
            evictWeakestEpisode();
        }
        
        episodes.push_back(episode);
        
        // Calculate and store episode strength
        float strength = calculateEpisodeStrength(episode, interference);
        episodeStrengths.push_back(strength);
        consolidationScores.push_back(0.0f); // Fresh episode, no consolidation yet
        
        // Update average consolidation
        updateAverageConsolidation();
        
        // Apply neural consolidation
        neuralConsolidate(episode, neuralIndex);
    }
    
    // Generate neural index for efficient retrieval
    size_t generateNeuralIndex(const EpisodicMemoryItem& episode) const {
        if (episode.neurons.empty() || episode.values.empty()) {
            return 0;
        }
        
        // Create hash-like index from neural pattern
        float hash = 0.0f;
        for (size_t i = 0; i < std::min<size_t>(10, episode.neurons.size()); ++i) {
            hash += episode.neurons[i].value * (i + 1);
        }
        
        for (size_t i = 0; i < std::min<size_t>(10, episode.values.size()); ++i) {
            hash += episode.values[i] * (i + 1) * 100.0f;
        }
        
        return static_cast<size_t>(std::abs(hash)) % maxEpisodes;
    }
    
    // Calculate interference with existing episodes
    float calculateInterference(const EpisodicMemoryItem& newEpisode, size_t index) const {
        if (episodes.empty()) return 0.0f;
        
        float totalInterference = 0.0f;
        for (size_t i = 0; i < episodes.size(); ++i) {
            if (i == episodes.size() - 1 && index >= maxEpisodes) continue; // Skip if we just evicted
            
            float similarity = calculateSimilarity(newEpisode, episodes[i]);
            totalInterference += similarity * episodeStrengths[i];
        }
        
        return totalInterference;
    }
    
    // Calculate similarity between episodes
    float calculateSimilarity(const EpisodicMemoryItem& a, const EpisodicMemoryItem& b) const {
        if (a.neurons.size() != b.neurons.size() || a.values.size() != b.values.size()) {
            return 0.0f;
        }
        
        float similarity = 0.0f;
        for (size_t i = 0; i < a.neurons.size(); ++i) {
            if (a.neurons[i] == b.neurons[i]) {
                similarity += std::abs(a.values[i] - b.values[i]);
            }
        }
        
        return 1.0f - (similarity / static_cast<float>(a.neurons.size())); // Normalize to [0, 1]
    }
    
    // Calculate episode strength
    float calculateEpisodeStrength(const EpisodicMemoryItem& episode, float interference) const {
        float baseStrength = 1.0f;
        
        // Stronger if more neurons involved
        baseStrength *= std::min(1.0f, static_cast<float>(episode.neurons.size()) / 50.0f);
        
        // Stronger if higher activation values
        float maxVal = *std::max_element(episode.values.begin(), episode.values.end(),
                                        [](float a, float b) { return std::abs(a) < std::abs(b); });
        baseStrength *= std::min(1.0f, std::abs(maxVal) * 2.0f);
        
        // Weaker with higher interference
        baseStrength *= (1.0f - interference * 0.5f);
        
        return std::max(0.0f, baseStrength);
    }
    
    // Evict weakest episode based on strength
    void evictWeakestEpisode() {
        if (episodes.empty()) return;
        
        size_t weakestIndex = 0;
        float weakestStrength = episodeStrengths[0];
        
        for (size_t i = 1; i < episodes.size(); ++i) {
            if (episodeStrengths[i] < weakestStrength) {
                weakestStrength = episodeStrengths[i];
                weakestIndex = i;
            }
        }
        
        // Remove from all structures
        episodes.erase(episodes.begin() + weakestIndex);
        episodeStrengths.erase(episodeStrengths.begin() + weakestIndex);
        consolidationScores.erase(consolidationScores.begin() + weakestIndex);
    }
    
    // Update average consolidation score
    void updateAverageConsolidation() {
        if (consolidationScores.empty()) return;
        
        float total = std::accumulate(consolidationScores.begin(), consolidationScores.end(), 0.0f);
        averageConsolidation = total / consolidationScores.size();
    }
    
    // Neural consolidation process
    void neuralConsolidate(const EpisodicMemoryItem& episode, size_t index) {
        // Simulate synaptic consolidation
        float consolidation = calculateConsolidationPotential(episode);
        consolidationScores[index] = consolidation;
        
        // Apply consolidation effects
        if (consolidation > 0.7f) {
            // Strong consolidation: enhance memory trace
            enhanceMemoryTrace(episode, index);
        }
        
        // Update consolidation over time
        scheduleReConsolidation(index);
    }
    
    // Calculate consolidation potential
    float calculateConsolidationPotential(const EpisodicMemoryItem& episode) const {
        float potential = 0.5f; // Base potential
        
        // Higher for more salient features
        float maxActivation = *std::max_element(episode.values.begin(), episode.values.end(),
                                               [](float a, float b) { return std::abs(a) < std::abs(b); });
        potential += std::abs(maxActivation) * 0.3f;
        
        // Higher for longer sequences
        potential += std::min(0.2f, static_cast<float>(episode.neurons.size()) / 100.0f);
        
        return std::min(1.0f, potential);
    }
    
    // Enhance memory trace
    void enhanceMemoryTrace(const EpisodicMemoryItem& episode, size_t index) {
        // In real implementation, this would:
        // 1. Strengthen synaptic connections
        // 2. Increase neural firing patterns
        // 3. Enhance retrieval cues
        // 4. Reduce interference from similar memories
        
        // For now, simulate by increasing strength
        episodeStrengths[index] = std::min(1.0f, episodeStrengths[index] * 1.5f);
    }
    
    // Schedule re-consolidation
    void scheduleReConsolidation(size_t index) {
        // In real implementation, this would schedule future consolidation sessions
        // For now, just mark as needing consolidation
        consolidationScores[index] = 0.1f; // Reset consolidation score
    }
    
    EpisodicMemoryItem retrieveEpisode(size_t index) const {
        if (index < episodes.size()) {
            return episodes[index];
        }
        return EpisodicMemoryItem();
    }
    
    size_t getEpisodeCount() const {
        return episodes.size();
    }
    
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const {
        std::vector<EpisodicMemoryItem> result;
        size_t start = count > episodes.size() ? 0 : episodes.size() - count;
        for (size_t i = start; i < episodes.size(); ++i) {
            result.push_back(episodes[i]);
        }
        return result;
    }
    
    void clear() {
        episodes.clear();
        episodeStrengths.clear();
        consolidationScores.clear();
        averageConsolidation = 0.0f;
    }
    
    // Enhanced consolidation with neural mechanisms
    void consolidate(float relevanceThreshold) {
        if (episodes.empty()) return;
        
        // Neural consolidation: gradually stabilize memories
        for (size_t i = 0; i < episodes.size(); ++i) {
            float strength = episodeStrengths[i];
            
            // Apply consolidation based on relevance
            if (strength >= relevanceThreshold) {
                // Strong consolidation: stabilize memory
                consolidationScores[i] = std::min(1.0f, consolidationScores[i] + 0.1f);
                
                // Enhance memory trace
                if (consolidationScores[i] > 0.8f) {
                    episodeStrengths[i] = std::min(1.0f, episodeStrengths[i] * 1.2f);
                }
            } else {
                // Weak memory: may be pruned
                if (consolidationScores[i] < 0.3f) {
                    episodeStrengths[i] *= 0.8f; // Weaken
                }
            }
        }
        
        // Update averages
        updateAverageConsolidation();
        
        // Clean up weak memories
        cleanupWeakMemories();
    }
    
    // Clean up weak memories
    void cleanupWeakMemories() {
        if (episodes.empty()) return;
        
        // Remove memories with very low strength and low consolidation
        std::vector<size_t> toRemove;
        for (size_t i = 0; i < episodes.size(); ++i) {
            if (episodeStrengths[i] < 0.1f && consolidationScores[i] < 0.2f) {
                toRemove.push_back(i);
            }
        }
        
        // Remove in reverse order to maintain indices
        for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
            size_t index = *it;
            episodes.erase(episodes.begin() + index);
            episodeStrengths.erase(episodeStrengths.begin() + index);
            consolidationScores.erase(consolidationScores.begin() + index);
        }
        
        updateAverageConsolidation();
    }
    
    // Get average consolidation level
    float getAverageConsolidation() const {
        return averageConsolidation;
    }
    
    // Get episode strengths for analysis
    std::vector<float> getEpisodeStrengths() const {
        return episodeStrengths;
    }
    
    // Get consolidation scores for analysis
    std::vector<float> getConsolidationScores() const {
        return consolidationScores;
    }
};

EpisodicMemory::EpisodicMemory() : pImpl(new Impl(1000)) {}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    pImpl->storeEpisode(episode);
}

EpisodicMemoryItem EpisodicMemory::retrieveEpisode(size_t index) const {
    return pImpl->retrieveEpisode(index);
}

size_t EpisodicMemory::getEpisodeCount() const {
    return pImpl->getEpisodeCount();
}

std::vector<EpisodicMemoryItem> EpisodicMemory::getRecentEpisodes(size_t count) const {
    return pImpl->getRecentEpisodes(count);
}

void EpisodicMemory::clear() {
    pImpl->clear();
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    // Phase 2: Implement real consolidation
    // Neural memory consolidation with multiple mechanisms
    
    pImpl->consolidate(relevanceThreshold);
}

} // namespace nlm
