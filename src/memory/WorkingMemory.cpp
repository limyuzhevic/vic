// WorkingMemory implementation using pImpl pattern
namespace nlm {

struct WorkingMemory::Impl {
    // Capacity and current state
    size_t capacity;
    size_t currentSize;
    
    // Storage
    std::vector<NeuronId> neurons;
    std::vector<float> values;
    std::vector<SimulationStep> timestamps;
    
    // Decay rate
    float decayRate;
    
    Impl() : capacity(100), currentSize(0), decayRate(0.01f) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // Find existing entry
    auto it = std::find(neurons.begin(), neurons.end(), neuron);
    
    if (it != neurons.end()) {
        size_t idx = std::distance(neurons.begin(), it);
        values[idx] = value;
        timestamps[idx] = 0;
    } else if (currentSize < capacity) {
        neurons.push_back(neuron);
        values.push_back(value);
        timestamps.push_back(0);
        currentSize++;
    } else {
        // Capacity full - replace weakest entry
        size_t weakestIdx = 0;
        float weakestValue = values[0];
        
        for (size_t i = 1; i < currentSize; ++i) {
            if (values[i] < weakestValue) {
                weakestIdx = i;
                weakestValue = values[i];
            }
        }
        
        neurons[weakestIdx] = neuron;
        values[weakestIdx] = value;
        timestamps[weakestIdx] = 0;
    }
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    auto it = std::find(neurons.begin(), neurons.end(), neuron);
    if (it != neurons.end()) {
        size_t idx = std::distance(neurons.begin(), it);
        return values[idx];
    }
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    return std::find(neurons.begin(), neurons.end(), neuron) != neurons.end();
}

void WorkingMemory::clear() {
    neurons.clear();
    values.clear();
    timestamps.clear();
    currentSize = 0;
}

size_t WorkingMemory::getCapacity() const {
    return capacity;
}

size_t WorkingMemory::getCurrentSize() const {
    return currentSize;
}

void WorkingMemory::decay(float decayRate) {
    if (decayRate <= 0.0f) return;
    
    for (size_t i = 0; i < currentSize; ++i) {
        values[i] *= (1.0f - decayRate);
        timestamps[i]++;
    }
    
    // Remove entries below threshold
    std::vector<size_t> toRemove;
    for (size_t i = 0; i < currentSize; ++i) {
        if (values[i] < 0.01f) {
            toRemove.push_back(i);
        }
    }
    
    // Remove in reverse order
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        neurons.erase(neurons.begin() + *it);
        values.erase(values.begin() + *it);
        timestamps.erase(timestamps.begin() + *it);
    }
    
    currentSize = neurons.size();
}

} // namespace nlm