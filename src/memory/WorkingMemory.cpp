#include "WorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct WorkingMemory::Impl {
    // Internal implementation details if needed
};

WorkingMemory::WorkingMemory()
    : pImpl(new Impl)
    , capacity_(100)
    , brain_(nullptr)
{
}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::initialize(Brain* brain) {
    brain_ = brain;
    NLM_LOG_INFO("WorkingMemory initialized");
}

void WorkingMemory::store(NeuronId neuron, float value) {
    items_[neuron] = value;
    decayRates_[neuron] = 0.01f; // Default decay rate
    
    // Inject current into neuron if brain exists
    if (brain_) {
        brain_->injectCurrent(neuron, value * 2.0f);
    }
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    auto it = items_.find(neuron);
    if (it != items_.end()) {
        return it->second;
    }
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    return items_.find(neuron) != items_.end();
}

void WorkingMemory::clear() {
    items_.clear();
    decayRates_.clear();
}

void WorkingMemory::decay(float decayRate) {
    for (auto& pair : items_) {
        pair.second *= (1.0f - decayRate);
        
        // Remove if too small
        if (pair.second < 0.01f) {
            items_.erase(pair.first);
            decayRates_.erase(pair.first);
        }
    }
}

} // namespace nlm