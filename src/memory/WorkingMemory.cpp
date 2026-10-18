#include "Memory.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct WorkingMemory::Impl {
    // Simple storage for working memory
    std::unordered_map<NeuronId, float> memoryMap;
    std::vector<NeuronId> accessOrder;
    
    Impl() {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // TODO PHASE 2: Implement real storage with neural dynamics
    pImpl->memoryMap[neuron] = value;
    pImpl->accessOrder.push_back(neuron);
    
    // Keep access order bounded
    if (pImpl->accessOrder.size() > getCapacity()) {
        pImpl->accessOrder.erase(pImpl->accessOrder.begin());
    }
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    auto it = pImpl->memoryMap.find(neuron);
    if (it != pImpl->memoryMap.end()) {
        return it->second;
    }
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    return pImpl->memoryMap.find(neuron) != pImpl->memoryMap.end();
}

void WorkingMemory::clear() {
    pImpl->memoryMap.clear();
    pImpl->accessOrder.clear();
}

size_t WorkingMemory::getCapacity() const {
    return 100; // Default capacity
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->memoryMap.size();
}

void WorkingMemory::decay(float decayRate) {
    for (auto& pair : pImpl->memoryMap) {
        pair.second *= (1.0f - decayRate);
        if (pair.second < 0.001f) {
            pImpl->memoryMap.erase(pair.first);
        }
    }
}

} // namespace nlm