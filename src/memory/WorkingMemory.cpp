#include "WorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <stdexcept>

namespace nlm {

struct WorkingMemory::Impl {
    // Stores neuron-value pairs with optional age for eviction
    struct Item {
        NeuronId neuron;
        float value;
        SimulationStep age;  // For LRU-style eviction
        
        Item(NeuronId n, float v) : neuron(n), value(v), age(0) {}
    };
    
    std::vector<Item> items;
    size_t capacity;
    
    Impl(size_t cap) : capacity(cap) {
        if (capacity == 0) {
            throw std::invalid_argument("WorkingMemory capacity must be positive");
        }
    }
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {
    NLM_LOG_INFO("WorkingMemory initialized with capacity " + std::to_string(pImpl->capacity));
}

WorkingMemory::~WorkingMemory() {
    NLM_LOG_INFO("WorkingMemory destroyed, stored " + std::to_string(pImpl->items.size()) + " items");
}

void WorkingMemory::store(NeuronId neuron, float value) {
    if (!neuron) {
        throw std::invalid_argument("Cannot store item with null neuron ID");
    }
    
    // Check if neuron already exists
    auto it = std::find_if(pImpl->items.begin(), pImpl->items.end(),
        [neuron](const Impl::Item& item) { return item.neuron == neuron; });
    
    if (it != pImpl->items.end()) {
        // Update existing item
        it->value = value;
        it->age = 0;  // Reset age since item was recently accessed
        NLM_LOG_DEBUG("WorkingMemory updated value for neuron " + std::to_string(neuron.getId()));
    } else {
        // Add new item if capacity allows
        if (pImpl->items.size() >= pImpl->capacity) {
            // Evict oldest item using simple LRU-style eviction
            size_t oldestIndex = 0;
            for (size_t i = 1; i < pImpl->items.size(); ++i) {
                if (pImpl->items[i].age > pImpl->items[oldestIndex].age) {
                    oldestIndex = i;
                }
            }
            NLM_LOG_DEBUG("WorkingMemory evicted neuron " + 
                         std::to_string(pImpl->items[oldestIndex].neuron.getId()) +
                         " due to capacity limit");
            pImpl->items.erase(pImpl->items.begin() + oldestIndex);
        }
        
        pImpl->items.emplace_back(neuron, value);
        NLM_LOG_DEBUG("WorkingMemory stored new value for neuron " + std::to_string(neuron.getId()));
    }
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    auto it = std::find_if(pImpl->items.begin(), pImpl->items.end(),
        [neuron](const Impl::Item& item) { return item.neuron == neuron; });
    
    if (it != pImpl->items.end()) {
        // Increment age for access tracking
        const_cast<Impl::Item&>(*it).age++;
        return it->value;
    }
    
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    return std::any_of(pImpl->items.begin(), pImpl->items.end(),
        [neuron](const Impl::Item& item) { return item.neuron == neuron; });
}

void WorkingMemory::clear() {
    size_t count = pImpl->items.size();
    pImpl->items.clear();
    if (count > 0) {
        NLM_LOG_INFO("WorkingMemory cleared " + std::to_string(count) + " items");
    }
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->capacity;
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->items.size();
}

void WorkingMemory::decay(float decayRate) {
    if (decayRate < 0.0f || decayRate > 1.0f) {
        throw std::invalid_argument("WorkingMemory decay rate must be between 0.0 and 1.0");
    }
    
    if (decayRate > 0.0f) {
        float totalDecay = 0.0f;
        for (auto& item : pImpl->items) {
            float oldValue = item.value;
            item.value *= (1.0f - decayRate);
            totalDecay += std::abs(item.value - oldValue);
        }
        
        if (totalDecay > 0.0f) {
            NLM_LOG_DEBUG("WorkingMemory applied decay rate " + std::to_string(decayRate) +
                         " affecting " + std::to_string(pImpl->items.size()) + " items");
        }
    }
}

} // namespace nlm
