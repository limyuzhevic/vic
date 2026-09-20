#pragma once

#include "BrainMemoryAdapter.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Default implementation of BrainMemoryAdapter
class DefaultBrainMemoryAdapter::Impl {
public:
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

DefaultBrainMemoryAdapter::DefaultBrainMemoryAdapter() : pImpl(new Impl) {}

DefaultBrainMemoryAdapter::~DefaultBrainMemoryAdapter() = default;

void DefaultBrainMemoryAdapter::injectCurrent(Brain* brain, NeuronId neuron, MembranePotential current) {
    brain->injectCurrent(neuron, current);
}

Neuron* DefaultBrainMemoryAdapter::getNeuron(Brain* brain, NeuronId neuronId) {
    if (!brain) return nullptr;
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            if (neuron->getId() == neuronId) {
                return neuron;
            }
        }
    }
    return nullptr;
}

NeuralRegion* DefaultBrainMemoryAdapter::getRegion(Brain* brain, RegionId regionId) {
    if (!brain) return nullptr;
    return brain->getRegion(regionId);
}

std::vector<Neuron*> DefaultBrainMemoryAdapter::getNeurons(NeuralRegion* region) {
    if (!region) return std::vector<Neuron*>();
    return region->getAllNeurons();
}

Brain* DefaultBrainMemoryAdapter::getBrain() const {
    return pImpl->brain;
}

void DefaultBrainMemoryAdapter::setBrain(Brain* brain) {
    pImpl->brain = brain;
}

} // namespace nlm
