#include <iostream>
#include "NeuralRegion.hpp"
#include "../core/Random/Random.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <algorithm>
#include <random>

namespace nlm {

NeuralRegion::NeuralRegion(uint64_t id) : pImpl(new Impl(id)) {}

NeuralRegion::NeuralRegion(const NeuralRegion& other) : pImpl(new Impl(*other.pImpl)) {}

NeuralRegion& NeuralRegion::operator=(const NeuralRegion& other) {
    if (this != &other) {
        delete pImpl;
        pImpl = new Impl(*other.pImpl);
    }
    return *this;
}

NeuralRegion::~NeuralRegion() {
    delete pImpl;
}

NeuralRegion::NeuralRegion(NeuralRegion&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

NeuralRegion& NeuralRegion::operator=(NeuralRegion&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

uint64_t NeuralRegion::getId() const {
    return pImpl->id;
}

std::string NeuralRegion::getName() const {
    return pImpl->name;
}

void NeuralRegion::setName(const std::string& name) {
    pImpl->name = name;
}

size_t NeuralRegion::getNeuronCount() const {
    return pImpl->neurons.size();
}

size_t NeuralRegion::getSynapseCount() const {
    size_t count = 0;
    for (const auto& pair : pImpl->outgoingSynapses) {
        count += pair.second.size();
    }
    return count;
}

void NeuralRegion::addNeuron(std::unique_ptr<Neuron> neuron) {
    if (neuron) {
        pImpl->neurons.push_back(std::move(neuron));
    }
}

Neuron* NeuralRegion::getNeuron(NeuronId id) {
    for (auto& neuron : pImpl->neurons) {
        if (neuron->getId() == id) {
            return neuron.get();
        }
    }
    return nullptr;
}

const Neuron* NeuralRegion::getNeuron(NeuronId id) const {
    for (const auto& neuron : pImpl->neurons) {
        if (neuron->getId() == id) {
            return neuron.get();
        }
    }
    return nullptr;
}

const std::vector<std::unique_ptr<Neuron>>& NeuralRegion::getNeurons() const {
    return pImpl->neurons;
}

void NeuralRegion::addSynapse(std::unique_ptr<Synapse> synapse) {
    if (synapse) {
        auto source = synapse->getSourceNeuron();
        pImpl->outgoingSynapses[source].push_back(synapse->getId());
        pImpl->synapses.push_back(std::move(synapse));
    }
}

Synapse* NeuralRegion::getSynapse(SynapseId id) {
    for (auto& synapse : pImpl->synapses) {
        if (synapse->getId() == id) {
            return synapse.get();
        }
    }
    return nullptr;
}

const Synapse* NeuralRegion::getSynapse(SynapseId id) const {
    for (const auto& synapse : pImpl->synapses) {
        if (synapse->getId() == id) {
            return synapse.get();
        }
    }
    return nullptr;
}

const std::vector<std::unique_ptr<Synapse>>& NeuralRegion::getSynapses() const {
    return pImpl->synapses;
}

bool NeuralRegion::hasSynapse(NeuronId source, NeuronId target) const {
    for (const auto& synapse : pImpl->synapses) {
        if (synapse->getSourceNeuron() == source && synapse->getTargetNeuron() == target) {
            return true;
        }
    }
    return false;
}

void NeuralRegion::removeSynapse(SynapseId id) {
    // Find the synapse in the vector
    auto it = std::find_if(pImpl->synapses.begin(), pImpl->synapses.end(),
                          [&id](const auto& syn) { return syn->getId() == id; });
    if (it != pImpl->synapses.end()) {
        // Remove from outgoingSynapses
        auto source = (*it)->getSourceNeuron();
        auto& synapsesList = pImpl->outgoingSynapses[source];
        synapsapsesList.erase(std::remove(synapsesList.begin(), synapsesList.end(), id), synapsesList.end());
        
        // Remove from synapses vector
        pImpl->synapses.erase(it);
    }
}

void NeuralRegion::clear() {
    pImpl->neurons.clear();
    pImpl->synapses.clear();
    pImpl->outgoingSynapses.clear();
}

void NeuralRegion::initialize(RandomGenerator& rng) {
    // Initialize neurons
    for (auto& neuron : pImpl->neurons) {
        neuron->initialize(rng);
    }
    
    // Create random connections between neurons in this region
    size_t neuronCount = pImpl->neurons.size();
    if (neuronCount < 2) return;
    
    for (size_t i = 0; i < neuronCount; ++i) {
        for (size_t j = 0; j < neuronCount; ++j) {
            if (i == j) continue;
            
            float randVal = rng.random(0.0f, 1.0f);
            if (randVal < 0.1f) {  // 10% connection probability
                auto* sourceNeuron = pImpl->neurons[i].get();
                auto* targetNeuron = pImpl->neurons[j].get();
                
                auto synapseId = createUniqueId();
                float weight = rng.normal(0.5f, 0.1f);
                weight = std::max(0.0f, std::min(1.0f, weight)); // Clamp to [0, 1]
                
                auto synapse = std::make_unique<Synapse>(synapseId, sourceNeuron->getId(), targetNeuron->getId(),
                                                         weight, 1);
                synapse->enablePlasticity(true, true, false);
                
                addSynapse(std::move(synapse));
            }
        }
    }
}

NeuronId NeuralRegion::getNeuronId(size_t index) const {
    if (index < pImpl->neurons.size()) {
        return pImpl->neurons[index]->getId();
    }
    return INVALID_NEURON_ID;
}

void NeuralRegion::connectToRegion(NeuralRegion* targetRegion, RandomGenerator& rng) {
    if (!targetRegion) return;
    
    size_t sourceCount = pImpl->neurons.size();
    size_t targetCount = targetRegion->getNeuronCount();
    
    if (sourceCount == 0 || targetCount == 0) return;
    
    for (size_t i = 0; i < sourceCount; ++i) {
        auto* sourceNeuron = pImpl->neurons[i].get();
        
        for (size_t j = 0; j < targetCount; ++j) {
            float randVal = rng.random(0.0f, 1.0f);
            if (randVal < 0.05f) {  // 5% inter-region connection probability
                auto* targetNeuron = targetRegion->getNeuron(targetRegion->getNeuronId(j));
                if (!targetNeuron) continue;
                
                auto synapseId = createUniqueId();
                float weight = rng.normal(0.5f, 0.1f);
                weight = std::max(0.0f, std::min(1.0f, weight));
                
                auto synapse = std::make_unique<Synapse>(synapseId, sourceNeuron->getId(), targetNeuron->getId(),
                                                         weight, 2); // Longer delay for inter-region
                synapse->enablePlasticity(true, true, false);
                
                addSynapse(std::move(synapse));
            }
        }
    }
}

void NeuralRegion::update(float dt, RandomGenerator& rng) {
    // Update neurons
    for (auto& neuron : pImpl->neurons) {
        neuron->update(dt);
    }
    
    // Propagate spikes
    for (auto& neuron : pImpl->neurons) {
        if (neuron->hasSpike()) {
            size_t spikeCount = neuron->getSpikeCount();
            NeuronId sourceId = neuron->getId();
            
            // Find outgoing synapses
            auto it = pImpl->outgoingSynapses.find(sourceId);
            if (it != pImpl->outgoingSynapses.end()) {
                for (SynapseId synId : it->second) {
                    Synapse* synapse = getSynapse(synId);
                    if (synapse) {
                        // Propagate spike with delay
                        synapse->receiveSpike(spikeCount);
                    }
                }
            }
        }
    }
}

void NeuralRegion::logStatus() const {
    std::cout << "Neural Region " << pImpl->id;
    if (!pImpl->name.empty()) {
        std::cout << " (" << pImpl->name << ")";
    }
    std::cout << ":\n";
    std::cout << "  Neurons: " << pImpl->neurons.size() << "\n";
    std::cout << "  Synapses: " << getSynapseCount() << "\n";
    std::cout << "  Active neurons: " << getActiveNeuronCount() << "\n";
    std::cout << "  Firing neurons: " << getFiringNeuronCount() << "\n";
}

uint64_t NeuralRegion::createUniqueId() {
    // Simple implementation - in production, use a more robust ID generation
    static uint64_t counter = 0;
    return ++counter;
}

} // namespace nlm
