// src/memory/WorkingMemory.cpp
// Implementation of working memory: temporary active storage of information

#include "WorkingMemory.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace nlm {

struct WorkingMemory::Impl {
    // Working memory storage
    std::unordered_map<NeuronId, float> storedValues;
    std::unordered_map<NeuronId, Timestamp> decayTimestamps;
    
    // Working memory parameters
    size_t capacity;                    // Maximum number of items
    float decayRate;                    // Decay rate per timestep (0-1)
    float maxValue;                     // Maximum value that can be stored
    float minValue;                     // Minimum value that can be stored
    
    // Memory management
    size_t accessCount;                 // Number of times memory accessed
    float totalActivity;                // Recent activity level (0-1)
    Timestamp lastUpdateTime;
    
    // Competition parameters
    bool enableCompetition;             // Whether items compete for attention
    float attentionBias;                // Bias toward certain items (0-1)
    
    Impl() : capacity(1000), decayRate(0.001f), maxValue(1.0f), minValue(-1.0f),
             accessCount(0), totalActivity(0.0f), lastUpdateTime(0.0f),
             enableCompetition(true), attentionBias(0.5f) {
        storedValues.reserve(capacity);
        decayTimestamps.reserve(capacity);
    }
};

WorkingMemory::WorkingMemory() : pImpl(new Impl()) {
    NLM_LOG_INFO("WorkingMemory initialized");
    NLM_LOG_INFO("  Capacity: " + std::to_string(pImpl->capacity));
    NLM_LOG_INFO("  Decay rate: " + std::to_string(pImpl->decayRate));
    NLM_LOG_INFO("  Competition enabled: " + std::to_string(pImpl->enableCompetition));
}

WorkingMemory::~WorkingMemory() {
    delete pImpl;
}

// Move constructor
WorkingMemory::WorkingMemory(WorkingMemory&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

// Move assignment operator
WorkingMemory& WorkingMemory::operator=(WorkingMemory&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

void WorkingMemory::store(NeuronId neuron, float value) {
    if (!pImpl) {
        NLM_LOG_ERROR("Cannot store value: WorkingMemory not initialized");
        return;
    }
    
    // Clamp value to valid range
    float clampedValue = std::clamp(value, pImpl->minValue, pImpl->maxValue);
    
    // Check capacity and remove oldest items if necessary
    if (pImpl->storedValues.size() >= pImpl->capacity && 
        pImpl->storedValues.find(neuron) == pImpl->storedValues.end()) {
        // Simple eviction: remove item with lowest activity/age
        evictLeastRecentlyUsed();
    }
    
    // Store the value
    pImpl->storedValues[neuron] = clampedValue;
    pImpl->decayTimestamps[neuron] = getCurrentTimestamp();
    
    // Update access count
    pImpl->accessCount++;
    
    NLM_LOG_DEBUG("Stored value for neuron " + std::to_string(neuron) + ": " + std::to_string(clampedValue));
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    if (!pImpl) {
        NLM_LOG_ERROR("Cannot retrieve value: WorkingMemory not initialized");
        return 0.0f;
    }
    
    auto it = pImpl->storedValues.find(neuron);
    if (it == pImpl->storedValues.end()) {
        NLM_LOG_WARNING("Attempted to retrieve non-existent neuron " + std::to_string(neuron));
        return 0.0f;
    }
    
    // Apply decay based on time since last update
    float decayFactor = calculateDecayFactor(it->second);
    float retrievedValue = it->second * decayFactor;
    
    // Update access count
    pImpl->accessCount++;
    
    NLM_LOG_DEBUG("Retrieved value for neuron " + std::to_string(neuron) + ": " + std::to_string(retrievedValue));
    return retrievedValue;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    if (!pImpl) return false;
    
    auto it = pImpl->storedValues.find(neuron);
    return it != pImpl->storedValues.end();
}

void WorkingMemory::clear() {
    if (!pImpl) return;
    
    size_t oldSize = pImpl->storedValues.size();
    pImpl->storedValues.clear();
    pImpl->decayTimestamps.clear();
    
    NLM_LOG_INFO("Cleared working memory: removed " + std::to_string(oldSize) + " items");
}

size_t WorkingMemory::getCapacity() const {
    return pImpl ? pImpl->capacity : 0;
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl ? pImpl->storedValues.size() : 0;
}

void WorkingMemory::decay(float decayRate) {
    if (!pImpl) return;
    
    // Update decay rate
    pImpl->decayRate = std::clamp(decayRate, 0.0f, 1.0f);
    
    // Apply decay to all stored values
    float totalDecay = 1.0f - std::pow(1.0f - pImpl->decayRate, 1.0f); // Decay per timestep
    
    for (auto& pair : pImpl->storedValues) {
        pair.second *= (1.0f - totalDecay);
    }
    
    NLM_LOG_INFO("Applied decay to working memory: rate = " + std::to_string(pImpl->decayRate));
}

void WorkingMemory::updateActivity(float activity) {
    if (!pImpl) return;
    
    pImpl->totalActivity = std::clamp(activity, 0.0f, 1.0f);
}

float WorkingMemory::getTotalActivity() const {
    return pImpl ? pImpl->totalActivity : 0.0f;
}

void WorkingMemory::setCapacity(size_t capacity) {
    if (!pImpl) return;
    
    if (capacity < pImpl->storedValues.size()) {
        NLM_LOG_WARNING("Cannot reduce capacity below current size: " + 
                       std::to_string(pImpl->storedValues.size()) + " items stored");
        return;
    }
    
    pImpl->capacity = capacity;
    pImpl->storedValues.reserve(capacity);
    
    NLM_LOG_INFO("Working memory capacity set to: " + std::to_string(capacity));
}

void WorkingMemory::setDecayRate(float decayRate) {
    if (!pImpl) return;
    
    if (decayRate < 0.0f) {
        NLM_LOG_WARNING("Decay rate cannot be negative, using absolute value");
        decayRate = std::abs(decayRate);
    }
    
    pImpl->decayRate = std::clamp(decayRate, 0.0f, 1.0f);
    NLM_LOG_INFO("Working memory decay rate set to: " + std::to_string(pImpl->decayRate));
}

float WorkingMemory::getAverageActivity() const {
    if (!pImpl || pImpl->storedValues.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& pair : pImpl->storedValues) {
        sum += std::abs(pair.second);
    }
    
    return sum / pImpl->storedValues.size();
}

std::vector<std::pair<NeuronId, float>> WorkingMemory::getAllItems() const {
    if (!pImpl) return {};
    
    std::vector<std::pair<NeuronId, float>> items;
    items.reserve(pImpl->storedValues.size());
    
    for (const auto& pair : pImpl->storedValues) {
        float decayFactor = calculateDecayFactor(pair.second);
        items.emplace_back(pair.first, pair.second * decayFactor);
    }
    
    return items;
}

void WorkingMemory::evictLeastRecentlyUsed() {
    if (!pImpl || pImpl->storedValues.empty()) return;
    
    // Find the item with the lowest value (simple heuristic)
    auto it = std::min_element(pImpl->storedValues.begin(), pImpl->storedValues.end(),
                              [](const auto& a, const auto& b) {
                                  return std::abs(a.second) < std::abs(b.second);
                              });
    
    if (it != pImpl->storedValues.end()) {
        NLM_LOG_INFO("Evicting neuron " + std::to_string(it->first) + 
                    " from working memory (value: " + std::to_string(it->second) + ")");
        pImpl->storedValues.erase(it);
        pImpl->decayTimestamps.erase(it->first);
    }
}

float WorkingMemory::calculateDecayFactor(float value) const {
    if (!pImpl) return 1.0f;
    
    // Simple exponential decay based on value
    return std::exp(-pImpl->decayRate * getCurrentTimestamp());
}

Timestamp WorkingMemory::getCurrentTimestamp() const {
    // In a real implementation, this would use the simulation clock
    // For now, we'll use a simple counter
    static Timestamp timestamp = 0;
    return timestamp++;
}

} // namespace nlm
