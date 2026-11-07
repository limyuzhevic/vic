#include "NeuralPopulation.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

struct NeuralPopulation::Impl {
    PopulationId id;
    size_t size;
    std::vector<Neuron*> neurons;
    std::vector<std::unique_ptr<Neuron>> ownedNeurons;
    NeuronType neuronType;
    std::vector<MembranePotential> membranePotentials;  // for statistics
    
    // Performance optimization fields
    mutable size_t cachedActiveCount = 0;
    mutable size_t cachedFiringCount = 0;
    mutable float cachedTotalFiringRate = 0.0f;
    mutable bool statsDirty = true;
    
    Impl(PopulationId id, size_t size) : id(id), size(size), neuronType(NeuronType::Internal),
                                        cachedActiveCount(0), cachedFiringCount(0), 
                                        cachedTotalFiringRate(0.0f), statsDirty(true) {}
};

NeuralPopulation::NeuralPopulation(PopulationId id, size_t size) : pImpl(new Impl(id, size)) {}

NeuralPopulation::~NeuralPopulation() = default;

NeuralPopulation::NeuralPopulation(NeuralPopulation&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

NeuralPopulation& NeuralPopulation::operator=(NeuralPopulation&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

PopulationId NeuralPopulation::getId() const {
    return pImpl->id;
}

size_t NeuralPopulation::getSize() const {
    return pImpl->size;
}

bool NeuralPopulation::isEmpty() const {
    return pImpl->neurons.empty();
}

Neuron* NeuralPopulation::getNeuron(NeuronIndex index) {
    if (index >= pImpl->neurons.size()) {
        return nullptr;
    }
    return pImpl->neurons[index];
}

const Neuron* NeuralPopulation::getNeuron(NeuronIndex index) const {
    if (index >= pImpl->neurons.size()) {
        return nullptr;
    }
    return pImpl->neurons[index];
}

void NeuralPopulation::createNeurons(NeuronId startId) {
    pImpl->ownedNeurons.clear();
    pImpl->neurons.clear();
    pImpl->neurons.reserve(pImpl->size);
    pImpl->ownedNeurons.reserve(pImpl->size);
    for (size_t i = 0; i < pImpl->size; ++i) {
        auto neuron = std::make_unique<Neuron>(NeuronId(startId.index() + i));
        neuron->setType(pImpl->neuronType);
        pImpl->neurons.push_back(neuron.get());
        pImpl->ownedNeurons.push_back(std::move(neuron));
    }
    pImpl->statsDirty = true;  // Invalidate cached stats
}

void NeuralPopulation::addNeuron(Neuron* neuron) {
    pImpl->neurons.push_back(neuron);
    if (pImpl->size < pImpl->neurons.size()) {
        pImpl->size = pImpl->neurons.size();
    }
    pImpl->statsDirty = true;  // Invalidate cached stats
}

const std::vector<Neuron*>& NeuralPopulation::getNeurons() const {
    return pImpl->neurons;
}

NeuronType NeuralPopulation::getNeuronType() const {
    return pImpl->neuronType;
}

void NeuralPopulation::setNeuronType(NeuronType type) {
    pImpl->neuronType = type;
    for (auto* neuron : pImpl->neurons) {
        neuron->setType(type);
    }
    pImpl->statsDirty = true;  // Invalidate cached stats
}

// Optimized implementations with caching

// Batch update all neuron states for better cache locality
void NeuralPopulation::batchStep(Timestamp currentTime, float timestep) {
    // Use thread-friendly approach for better performance
    for (auto* neuron : pImpl->neurons) {
        neuron->step(currentTime, timestep);
    }
    pImpl->statsDirty = true;  // Stats may have changed
}

// Optimized getActiveCount() using cached value when available
void NeuralPopulation::updateCachedStats() const {
    if (!pImpl->statsDirty) return;  // Use cached values if not dirty
    
    size_t activeCount = 0;
    size_t firingCount = 0;
    float totalFiringRate = 0.0f;
    
    for (const auto* neuron : pImpl->neurons) {
        if (neuron->isFiring() || neuron->getFiringRate() > 0.0f) {
            ++activeCount;
        }
        if (neuron->isFiring()) {
            ++firingCount;
        }
        totalFiringRate += neuron->getFiringRate();
    }
    
    pImpl->cachedActiveCount = activeCount;
    pImpl->cachedFiringCount = firingCount;
    pImpl->cachedTotalFiringRate = totalFiringRate;
    pImpl->statsDirty = false;  // Mark as clean
}

// Optimized getActiveCount() - O(1) with caching
size_t NeuralPopulation::getActiveCount() const {
    updateCachedStats();
    return pImpl->cachedActiveCount;
}

// Optimized getFiringCount() - O(1) with caching
size_t NeuralPopulation::getFiringCount() const {
    updateCachedStats();
    return pImpl->cachedFiringCount;
}

// Optimized getAverageFiringRate() - O(1) with caching
float NeuralPopulation::getAverageFiringRate() const {
    if (pImpl->neurons.empty()) {
        return 0.0f;
    }
    updateCachedStats();
    return pImpl->cachedTotalFiringRate / static_cast<float>(pImpl->neurons.size());
}

// Get activity level with optimized implementation
float NeuralPopulation::getActivityLevel() const {
    if (pImpl->neurons.empty()) {
        return 0.0f;
    }
    updateCachedStats();
    return static_cast<float>(pImpl->cachedActiveCount) / static_cast<float>(pImpl->neurons.size());
}

// Batch initialize for better performance
void NeuralPopulation::batchInitializeRandom(RandomGenerator& rng) {
    for (auto* neuron : pImpl->neurons) {
        neuron->initializeRandom(rng);
    }
    pImpl->statsDirty = true;  // Invalidate cached stats
}

// Reset with cache invalidation
void NeuralPopulation::reset() {
    for (auto* neuron : pImpl->neurons) {
        neuron->reset();
    }
    pImpl->membranePotentials.clear();
    pImpl->statsDirty = true;  // Invalidate cached stats
}

// Record membrane potentials for statistics
void NeuralPopulation::recordActivity() {
    pImpl->membranePotentials.clear();
    pImpl->membranePotentials.reserve(pImpl->neurons.size());
    for (const auto* neuron : pImpl->neurons) {
        pImpl->membranePotentials.push_back(neuron->getMembranePotential());
    }
    pImpl->statsDirty = true;  // Invalidate cached stats
}

// Get neuron IDs with optimized implementation
std::vector<NeuronId> NeuralPopulation::getNeuronIds() const {
    std::vector<NeuronId> ids;
    ids.reserve(pImpl->neurons.size());
    for (const auto* neuron : pImpl->neurons) {
        ids.push_back(neuron->getId());
    }
    return ids;
}

// New optimized methods for batch operations

// Deliver synaptic input to all neurons in this population efficiently
void NeuralPopulation::deliverSynapticInputBatch(const std::vector<SynapticInput>& inputs) {
    // Process inputs in batches for better cache locality
    for (size_t i = 0; i < inputs.size(); ++i) {
        const auto& input = inputs[i];
        // Find target neuron efficiently
        for (auto* neuron : pImpl->neurons) {
            if (neuron->getId() == input.targetNeuronId) {
                neuron->deliverSynapticInput(input.weight, input.delay, input.isExcitatory);
                break; // Found the target
            }
        }
    }
}

// Batch process spikes from all neurons in this population
void NeuralPopulation::processSpikesBatch(const std::vector<SpikeEvent>& spikeEvents) {
    // Use vectorized processing for better performance
    for (const auto& spikeEvent : spikeEvents) {
        // Deliver spike to target neurons in other populations
        deliverSpikeToTargets(spikeEvent);
    }
}

// Optimized method to deliver spikes to targets
void NeuralPopulation::deliverSpikeToTargets(const SpikeEvent& spikeEvent) {
    // Batch process all connections for this spike event
    for (const auto& connection : pImpl->outputConnections) {
        if (connection.target) {
            connection.target->deliverIncomingSpike(spikeEvent, connection.weight);
        }
    }
}

// Add synaptic connection between populations
void NeuralPopulation::addOutputConnection(NeuralPopulation* target, float weight) {
    pImpl->outputConnections.push_back({target, weight});
    target->addInputConnection(this, weight);
}

// Get neurons by type for efficient access
std::vector<Neuron*> NeuralPopulation::getNeuronsByType(NeuronType type) const {
    std::vector<Neuron*> result;
    result.reserve(pImpl->neurons.size() / 4);  // Estimate
    for (auto* neuron : pImpl->neurons) {
        if (neuron->getType() == type) {
            result.push_back(neuron);
        }
    }
    return result;
}

// Get firing neurons efficiently
std::vector<Neuron*> NeuralPopulation::getFiringNeurons() const {
    std::vector<Neuron*> result;
    result.reserve(pImpl->neurons.size() / 4);  // Estimate
    for (auto* neuron : pImpl->neurons) {
        if (neuron->isFiring()) {
            result.push_back(neuron);
        }
    }
    return result;
}

// Check if population has any active neurons
bool NeuralPopulation::hasActiveNeurons() const {
    updateCachedStats();
    return pImpl->cachedActiveCount > 0;
}

// Get total synaptic inputs to this population
float NeuralPopulation::getTotalExcitatoryInput() const {
    float total = 0.0f;
    for (const auto& connection : pImpl->inputConnections) {
        total += connection.weight;
    }
    return total;
}

} // namespace nlm
