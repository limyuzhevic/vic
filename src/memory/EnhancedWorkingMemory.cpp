#include "EnhancedWorkingMemory.hpp"
#include "../core/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <chrono>
#include <random>

namespace nlm {

struct EnhancedWorkingMemory::Impl {
    class Brain* brain;
    float baseCapacity;
    float capacityMultiplier;
    float decayRate;
    
    // Item storage with competition
    std::vector<std::pair<NeuronId, float>> items;
    float currentCapacity;
    
    // Competition and prioritization
    float competitionThreshold;
    bool competitionEnabled;
    std::vector<NeuronId> dominantItems;
    
    // Consolidation
    bool consolidationEnabled;
    size_t consolidationInterval;
    size_t consolidationCycles;
    std::vector<std::vector<float>> consolidationHistory;
    
    // Learning and adaptation
    bool dynamicCapacityEnabled;
    float learningEfficiency;
    float encodingEfficiency;
    float retrievalEfficiency;
    std::vector<float> capacityHistory;
    
    // Timing
    size_t stepsSinceConsolidation;
    std::chrono::high_resolution_clock::time_point lastConsolidation;
    
    Impl() : brain(nullptr), baseCapacity(100.0f), capacityMultiplier(2.0f),
             decayRate(0.05f), currentCapacity(100.0f), competitionThreshold(0.3f),
             competitionEnabled(true), consolidationEnabled(true),
             consolidationInterval(100), consolidationCycles(0),
             dynamicCapacityEnabled(true), learningEfficiency(0.0f),
             encodingEfficiency(0.0f), retrievalEfficiency(0.0f),
             stepsSinceConsolidation(0) {}
};

EnhancedWorkingMemory::EnhancedWorkingMemory() : pImpl(std::make_unique<Impl>()) {}

EnhancedWorkingMemory::~EnhancedWorkingMemory() = default;

void EnhancedWorkingMemory::initialize(class Brain* brain, float baseCapacity,
                                       float capacityMultiplier, float decayRate) {
    pImpl->brain = brain;
    pImpl->baseCapacity = baseCapacity;
    pImpl->capacityMultiplier = capacityMultiplier;
    pImpl->decayRate = decayRate;
    
    // Calculate initial capacity
    pImpl->currentCapacity = baseCapacity * capacityMultiplier;
    
    // Initialize settings
    pImpl->competitionThreshold = 0.3f;
    pImpl->competitionEnabled = true;
    pImpl->consolidationEnabled = true;
    pImpl->consolidationInterval = 100;
    pImpl->consolidationCycles = 0;
    pImpl->dynamicCapacityEnabled = true;
    
    // Initialize timing
    pImpl->stepsSinceConsolidation = 0;
    pImpl->lastConsolidation = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO("Enhanced Working Memory initialized");
    NLM_LOG_INFO("Base capacity: " + std::to_string(baseCapacity));
    NLM_LOG_INFO("Dynamic capacity multiplier: " + std::to_string(capacityMultiplier));
    NLM_LOG_INFO("Decay rate: " + std::to_string(decayRate));
}

void EnhancedWorkingMemory::storeWithCompetition(NeuronId neuron, float activation,
                                               const std::vector<float>& context) {
    if (!pImpl->competitionEnabled) {
        // Simple storage without competition
        items.emplace_back(neuron, activation);
        return;
    }
    
    // Check if item already exists
    auto it = std::find_if(items.begin(), items.end(),
                          [neuron](const auto& item) { return item.first == neuron; });n
    if (it != items.end()) {
        // Update existing item
        it->second = activation;
    } else {
        // Add new item with competition
        items.emplace_back(neuron, activation);
        
        // Apply competition: remove low activation items
        applyCompetition();
    }
    
    // Update dynamic capacity
    updateDynamicCapacity();
    
    // Apply decay
    applyDecay();
    
    // Check for consolidation
    pImpl->stepsSinceConsolidation++;
    if (pImpl->consolidationEnabled && pImpl->stepsSinceConsolidation >= pImpl->consolidationInterval) {
        consolidate();
    }
}

void EnhancedWorkingMemory::applyCompetition() {
    // Sort by activation
    std::sort(items.begin(), items.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Determine capacity limit based on current capacity
    size_t maxItems = static_cast<size_t>(pImpl->currentCapacity);
    
    // Keep top items
    if (items.size() > maxItems) {
        items.resize(maxItems);
    }
    
    // Update dominant items
    pImpl->dominantItems.clear();
    for (size_t i = 0; i < std::min(size_t(10), items.size()); ++i) {
        pImpl->dominantItems.push_back(items[i].first);
    }
}

void EnhancedWorkingMemory::updateDynamicCapacity() {
    if (!pImpl->dynamicCapacityEnabled) {
        return;
    }
    
    // Calculate current memory load
    float load = getMemoryLoad();
    
    // Adjust capacity based on load
    if (load > 0.8f) {
        // High load: reduce capacity
        pImpl->currentCapacity *= 0.9f;
    } else if (load < 0.3f) {
        // Low load: increase capacity
        pImpl->currentCapacity *= 1.1f;
    }
    
    // Clamp to reasonable bounds
    pImpl->currentCapacity = std::clamp(pImpl->currentCapacity,
                                       pImpl->baseCapacity * 0.5f,
                                       pImpl->baseCapacity * capacityMultiplier * 2.0f);
}

void EnhancedWorkingMemory::applyDecay() {
    // Apply exponential decay to all items
    for (auto& item : items) {
        item.second *= (1.0f - pImpl->decayRate);
    }
    
    // Remove items with very low activation
    items.erase(std::remove_if(items.begin(), items.end(),
                              [this](const auto& item) { return item.second < 0.01f; }),
                items.end());
}

void EnhancedWorkingMemory::consolidate() {
    if (!pImpl->consolidationEnabled) {
        return;
    }
    
    // Consolidate items: strengthen dominant items
    for (auto& item : items) {
        // Find item's position
        auto it = std::find_if(items.begin(), items.end(),
                              [item](const auto& entry) { return entry.first == item.first; });n        
        if (it != items.end()) {
            // Strengthen item
            it->second *= 1.2f;
            
            // Store in consolidation history
            consolidationHistory.push_back(std::vector<float>(10, it->second));
        }
    }
    
    // Update consolidation statistics
    pImpl->consolidationCycles++;
    
    // Reset counter
    pImpl->stepsSinceConsolidation = 0;
    
    // Record consolidation time
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->lastConsolidation);
    pImpl->lastConsolidation = now;
    
    NLM_LOG_DEBUG("Consolidation completed (cycles: " + std::to_string(pImpl->consolidationCycles) +
                ", duration: " + std::to_string(duration.count()) + "s)");
}

std::vector<std::pair<NeuronId, float>> EnhancedWorkingMemory::getTopItems(size_t count) const {
    // Sort by activation and return top items
    std::vector<std::pair<NeuronId, float>> sortedItems = items;
    std::sort(sortedItems.begin(), sortedItems.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Return top items
    if (count > sortedItems.size()) {
        count = sortedItems.size();
    }
    
    return std::vector<std::pair<NeuronId, float>>(sortedItems.begin(),
                                                  sortedItems.begin() + count);
}

float EnhancedWorkingMemory::getDynamicCapacity() const {
    return pImpl->currentCapacity;
}

float EnhancedWorkingMemory::getMemoryLoad() const {
    if (items.empty()) {
        return 0.0f;
    }
    
    // Calculate load as ratio of items to capacity
    float load = items.size() / pImpl->currentCapacity;
    return std::min(1.0f, load);
}

float EnhancedWorkingMemory::getEncodingEfficiency() const {
    if (items.empty()) {
        return 0.0f;
    }
    
    // Calculate encoding efficiency based on average activation
    float avgActivation = std::accumulate(items.begin(), items.end(), 0.0f,
                                         [](float sum, const auto& item) { return sum + item.second; }) /
                         items.size();
    
    return avgActivation / 10.0f; // Normalize to [0,1]
}

float EnhancedWorkingMemory::getRetrievalEfficiency() const {
    if (items.empty()) {
        return 0.0f;
    }
    
    // Calculate retrieval efficiency based on top item dominance
    if (items.size() < 2) {
        return 1.0f; // Perfect efficiency with single item
    }
    
    float topActivation = items[0].second;
    float secondActivation = items[1].second;
    
    // Higher ratio means better efficiency
    return topActivation / (topActivation + secondActivation);
}

void EnhancedWorkingMemory::clear(bool clearAll) {
    if (clearAll) {
        items.clear();
        pImpl->dominantItems.clear();
        pImpl->consolidationHistory.clear();
    } else {
        // Keep only high activation items
        items.erase(std::remove_if(items.begin(), items.end(),
                                  [this](const auto& item) { return item.second < 0.1f; }),
                    items.end());
    }
}

void EnhancedWorkingMemory::clearLowActivation(float threshold) {
    items.erase(std::remove_if(items.begin(), items.end(),
                              [threshold](const auto& item) { return item.second < threshold; }),
                items.end());
}

void EnhancedWorkingMemory::enableDynamicCapacity(bool enable) {
    pImpl->dynamicCapacityEnabled = enable;
}

void EnhancedWorkingMemory::enableContentCompetition(bool enable) {
    pImpl->competitionEnabled = enable;
}

void EnhancedWorkingMemory::enableConsolidation(bool enable) {
    pImpl->consolidationEnabled = enable;
}

void EnhancedWorkingMemory::setBaseCapacity(float capacity) {
    pImpl->baseCapacity = capacity;
    // Recalculate current capacity
    updateDynamicCapacity();
}

void EnhancedWorkingMemory::setCapacityMultiplier(float multiplier) {
    pImpl->capacityMultiplier = multiplier;
    updateDynamicCapacity();
}

void EnhancedWorkingMemory::setCompetitionThreshold(float threshold) {
    pImpl->competitionThreshold = threshold;
}

void EnhancedWorkingMemory::setConsolidationInterval(size_t interval) {
    pImpl->consolidationInterval = interval;
}

float EnhancedWorkingMemory::getLearningProgress() const {
    // Calculate learning progress based on consolidation cycles
    if (pImpl->consolidationCycles == 0) {
        return 0.0f;
    }
    
    // Progress based on consolidation experience
    return std::min(1.0f, pImpl->consolidationCycles * 0.1f);
}

size_t EnhancedWorkingMemory::getConsolidationCycles() const {
    return pImpl->consolidationCycles;
}

EnhancedWorkingMemory::MemoryStats EnhancedWorkingMemory::getMemoryStats() const {
    MemoryStats stats;
    
    stats.totalItems = items.size();
    stats.currentLoad = getMemoryLoad();
    
    // Calculate average activation
    if (!items.empty()) {
        float sum = std::accumulate(items.begin(), items.end(), 0.0f,
                                   [](float sum, const auto& item) { return sum + item.second; });
        stats.averageActivation = sum / items.size();
    } else {
        stats.averageActivation = 0.0f;
    }
    
    stats.competitionIntensity = items.size() > 1 ? 
                                (items[0].second / items[1].second) : 1.0f;
    stats.consolidationProgress = getLearningProgress();
    stats.dominantItems = pImpl->dominantItems;
    
    return stats;
}

} // namespace nlm