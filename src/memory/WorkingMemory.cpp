#include "WorkingMemory.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>

namespace nlm {

WorkingMemory::WorkingMemory() : pImpl(nullptr) {
    // This constructor will be called by factory or explicit creation
    // Implement here if needed
}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    if (!pImpl) return;
    
    // Check for existing item
    for (auto& item : pImpl->items) {
        if (item.first == neuron) {
            item.second = value;
            return;
        }
    }
    
    // Add new item if capacity allows
    if (pImpl->items.size() < pImpl->capacity) {
        pImpl->items.emplace_back(neuron, value);
    }
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    if (!pImpl) return 0.0f;
    
    for (const auto& item : pImpl->items) {
        if (item.first == neuron) {
            return item.second;
        }
    }
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    if (!pImpl) return false;
    
    return std::any_of(pImpl->items.begin(), pImpl->items.end(),
                      [&neuron](const auto& item) { return item.first == neuron; });
}

void WorkingMemory::clear() {
    if (!pImpl) return;
    
    pImpl->items.clear();
}

size_t WorkingMemory::getCapacity() const {
    return pImpl ? pImpl->capacity : 0;
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl ? pImpl->items.size() : 0;
}

void WorkingMemory::decay(float decayRate) {
    if (!pImpl) return;
    
    for (auto& item : pImpl->items) {
        item.second *= (1.0f - decayRate);
    }
}

} // namespace nlm
