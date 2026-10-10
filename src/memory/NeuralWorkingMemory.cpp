// NeuralWorkingMemory.cpp (original placeholder implementation in src/memory/)

#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Maintenance connections (recurrent)
    std::vector<std::pair<NeuronId, NeuronId>> maintenanceSynapses;
    
    // Memory trace ages
    std::vector<SimulationStep> traceAges;
    
    Impl() : brain(nullptr) {}
};

NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
{
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralWorkingMemory initialized");
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) return;
    
    // Find neurons to encode this pattern
    size_t neuronsNeeded = std::min(pattern.size(), memoryNeurons_.size());
    
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        NeuronId neuron = memoryNeurons_[i % memoryNeurons_.size()];
        float activation = pattern[i] * strength;
        
        // Set neuron activation
        if (auto* n = brain_->getRegion(neuron.getId() / 1000)->getAllNeurons()) {
            for (auto* nn : *n) {
                if (nn->getId() == neuron) {
                    nn->injectCurrent(activation * 5.0f);
                }
            }
        }
    }
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    if (!brain_) return;
    
    auto* targetNeuron = brain_->getNeuron(neuron);
    if (!targetNeuron) return;
    
    targetNeuron->setMembranePotential(activation * 20.0f);
    targetNeuron->injectCurrent(activation * 30.0f);
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
    std::vector<float> result;
    
    if (brain_) {
        auto allNeurons = brain_->getAllNeuronIds();
        result.resize(allNeurons.size(), 0.0f);
    }
    
    return result;
}

bool NeuralWorkingMemory::contains(NeuronId neuron) const {
    for (NeuronId id : memoryNeurons_) {
        if (id == neuron) return true;
    }
    return false;
}

float NeuralWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    return 0.0f;  // Simplified implementation
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    // Simplified update - decay memory traces
    for (auto& age : pImpl->traceAges) {
        age += dt;
        if (age > 100.0f) {
            age = 100.0f;
        }
    }
}

void NeuralWorkingMemory::clear() {
    pImpl->brain = nullptr;
    brain_ = nullptr;
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    pImpl->maintenanceSynapses.clear();
    pImpl->traceAges.clear();
}

} // namespace nlm
