#include "WorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct WorkingMemory::Impl {
    // Memory trace: neuron -> activation with age and importance
    struct MemoryTrace {
        NeuronId neuron;
        float activation;
        SimulationStep age;
        float importance;
        float predictedReward;
        float eligibilityTrace;
        
        MemoryTrace() : neuron(), activation(0.0f), age(0), importance(0.0f), predictedReward(0.0f), eligibilityTrace(0.0f) {}
        MemoryTrace(NeuronId n, float act) : neuron(n), activation(act), age(0), importance(1.0f), predictedReward(0.0f), eligibilityTrace(0.0f) {}
    };
    
    std::vector<MemoryTrace> traces;
    size_t capacity;
    float decayRate;
    float consolidationThreshold;
    float learningRate;
    
    // Performance optimization
    std::unordered_map<NeuronId, size_t> neuronIndexMap;
    
    Impl(size_t cap) : capacity(cap), decayRate(0.01f), consolidationThreshold(0.7f), learningRate(0.01f) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {
    NLM_LOG_INFO("WorkingMemory initialized with neural integration");
}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    store(neuron, value, 1.0f); // Default importance
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    auto it = pImpl->neuronIndexMap.find(neuron);
    if (it != pImpl->neuronIndexMap.end()) {
        return pImpl->traces[it->second].activation;
    }
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    return pImpl->neuronIndexMap.find(neuron) != pImpl->neuronIndexMap.end();
}

void WorkingMemory::decay(float decayRate) {
    for (auto& trace : pImpl->traces) {
        trace.activation *= (1.0f - decayRate);
    }
}

} // namespace nlm
