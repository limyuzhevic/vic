#include "../memory/Memory.hpp"

namespace nlm {

// WorkingMemory implementation - placeholder for Phase 2
class WorkingMemory::Impl {
public:
    size_t capacity = 1000;
    size_t currentSize = 0;
    std::vector<std::pair<NeuronId, float>> storage;
};

WorkingMemory::WorkingMemory() : pImpl(new Impl) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // Remove existing entry if present
    auto it = std::find_if(pImpl->storage.begin(), pImpl->storage.end(),
                         [neuron](const auto& item) { return item.first == neuron; });
    if (it != pImpl->storage.end()) {
        pImpl->storage.erase(it);
    }
    
    pImpl->storage.push_back({neuron, value});
    pImpl->currentSize = std::min(pImpl->currentSize + 1, pImpl->capacity);
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    auto it = std::find_if(pImpl->storage.begin(), pImpl->storage.end(),
                         [neuron](const auto& item) { return item.first == neuron; });
    if (it != pImpl->storage.end()) {
        return it->second;
    }
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    return std::any_of(pImpl->storage.begin(), pImpl->storage.end(),
                      [neuron](const auto& item) { return item.first == neuron; });
}

void WorkingMemory::clear() {
    pImpl->storage.clear();
    pImpl->currentSize = 0;
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->capacity;
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->currentSize;
}

void WorkingMemory::decay(float decayRate) {
    for (auto& item : pImpl->storage) {
        item.second *= (1.0f - decayRate);
    }
}

} // namespace nlm
