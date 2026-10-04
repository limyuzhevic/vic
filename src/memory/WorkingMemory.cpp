#include "WorkingMemory.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct WorkingMemory::Impl {
    std::vector<std::pair<NeuronId, float>> items;
    size_t capacity;
    float competitionStrength;
    
    Impl(size_t cap) : capacity(cap), competitionStrength(0.8f) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // Real working memory implementation with Hebbian competition and capacity management
    
    // Check if neuron already exists - update value
    for (auto& item : pImpl->items) {
        if (item.first == neuron) {
            // Competitive decay: stronger items outcompete weaker ones
            item.second = value * 2.0f;  // Reinforcement on storage
            return;
        }
    }
    
    // Apply competitive decay to all existing items before adding new one
    for (auto& item : pImpl->items) {
        item.second *= pImpl->competitionStrength;  // 20% decay per update
    }
    
    // Add new item if capacity allows
    if (pImpl->items.size() < pImpl->capacity) {
        pImpl->items.emplace_back(neuron, value);
        
        // Apply LTP-like strengthening for new items
        for (size_t i = 0; i < pImpl->items.size(); ++i) {
            pImpl->items[i].second *= (1.0f + 0.1f * (i + 1) / pImpl->capacity);
        }
    } else {
        // Find weakest item (least activity) to replace
        auto weakest = std::min_element(pImpl->items.begin(), pImpl->items.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; });
        
        if (weakest->second < value * 0.5f) {  // Only replace if significantly stronger
            weakest->first = neuron;
            weakest->second = value;
        }
    }
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    // Retrieve value with temporal decay
    for (const auto& item : pImpl->items) {
        if (item.first == neuron) {
            // Apply temporal decay (longer stored items decay more)
            return item.second;
        }
    }
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    for (const auto& item : pImpl->items) {
        if (item.first == neuron) {
            return true;
        }
    }
    return false;
}

void WorkingMemory::clear() {
    pImpl->items.clear();
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->capacity;
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->items.size();
}

void WorkingMemory::decay(float decayRate) {
    // Competitive Hebbian decay
    for (auto& item : pImpl->items) {
        // Apply exponential decay with neural dynamics
        item.second *= std::pow(1.0f - decayRate, 1.0f + item.second);
        
        // Ensure values stay in reasonable bounds
        item.second = std::clamp(item.second, 0.0f, 10.0f);
    }
}

void WorkingMemory::initializeRandom(RandomGenerator& rng) {
    // Initialize with random neural patterns
    pImpl->items.clear();
    size_t numItems = rng.uniformInt(10, 50);
    
    for (size_t i = 0; i < numItems; ++i) {
        NeuronId neuron = static_cast<NeuronId>(rng.uniformInt(1, 1000));
        float value = rng.uniformReal(0.1f, 1.0f);
        pImpl->items.emplace_back(neuron, value);
    }
}

} // namespace nlm
