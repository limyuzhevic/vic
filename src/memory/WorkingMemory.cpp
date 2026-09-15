// Working Memory - Simple implementation
// This delegates to NeuralWorkingMemory for neural dynamics

#include "Memory.hpp"
#include "NeuralWorkingMemory.hpp"
#include <algorithm>

namespace nlm {

struct WorkingMemory::Impl {
    NeuralWorkingMemory neuralWorkingMemory;
    size_t capacity;
    
    Impl(size_t cap) : capacity(cap) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // Delegate to neural working memory
    pImpl->neuralWorkingMemory.storeToNeuron(neuron, value);
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    return pImpl->neuralWorkingMemory.getNeuronActivation(neuron);
}

bool WorkingMemory::contains(NeuronId neuron) const {
    return pImpl->neuralWorkingMemory.contains(neuron);
}

void WorkingMemory::clear() {
    pImpl->neuralWorkingMemory.clear();
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->neuralWorkingMemory.getCapacity();
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->neuralWorkingMemory.getActiveTraces();
}

void WorkingMemory::decay(float decayRate) {
    // Neural working memory handles decay internally
}

void WorkingMemory::consolidate(float threshold) {
    // Neural working memory handles consolidation internally
}

void WorkingMemory::update(SimulationStep timestep) {
    // Neural working memory updates internally
}

} // namespace nlm