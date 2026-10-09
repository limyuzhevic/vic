#include "NeuralPopulation.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

struct NeuralPopulation::Impl {
    PopulationId id;
    size_t size;
    std::vector<std::unique_ptr<Neuron>> neurons;
    NeuronType neuronType;
    std::vector<MembranePotential> membranePotentials;  // for statistics
    
    Impl(PopulationId id, size_t size) : id(id), size(size), neuronType(NeuronType::Internal) {
        // Auto-create neurons with proper IDs
        neurons.reserve(size);
        for (size_t i = 0; i < size; ++i) {
            NeuronId neuronId(i + 1);  // Start from 1 (avoid 0 which is invalid)
            neurons.emplace_back(std::make_unique<Neuron>(neuronId));
        }
    }
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
    return pImpl->neurons[index].get();
}

const Neuron* NeuralPopulation::getNeuron(NeuronIndex index) const {
    if (index >= pImpl->neurons.size()) {
        return nullptr;
    }
    return pImpl->neurons[index].get();
}

void NeuralPopulation::addNeuron(std::unique_ptr<Neuron> neuron) {
    if (neuron) {
        pImpl->neurons.push_back(std::move(neuron));
        if (pImpl->size < pImpl->neurons.size()) {
            pImpl->size = pImpl->neurons.size();
        }
    }
}

// Legacy method for backward compatibility - transfers ownership
void NeuralPopulation::addNeuron(Neuron* neuron) {
    if (neuron) {
        // Transfer ownership of raw pointer to unique_ptr
        addNeuron(std::unique_ptr<Neuron>(neuron));
    }
}

const std::vector<Neuron*>& NeuralPopulation::getNeurons() const {
    // Return a copy for thread safety - maintain backward compatibility
    std::vector<Neuron*> result;
    result.reserve(pImpl->neurons.size());
    for (const auto& neuron : pImpl->neurons) {
        result.push_back(neuron.get());
    }
    return result;
}

const std::vector<std::unique_ptr<Neuron>>& NeuralPopulation::getNeuronSmartPtrs() const {
    return pImpl->neurons;
}

NeuronType NeuralPopulation::getNeuronType() const {
    return pImpl->neuronType;
}

void NeuralPopulation::setNeuronType(NeuronType type) {
    pImpl->neuronType = type;
    for (auto& neuron : pImpl->neurons) {
        neuron->setType(type);
    }
}

size_t NeuralPopulation::getActiveCount() const {
    size_t count = 0;
    for (const auto& neuron : pImpl->neurons) {
        if (neuron->isFiring() || neuron->getFiringRate() > 0.0f) {
            ++count;
        }
    }
    return count;
}

size_t NeuralPopulation::getFiringCount() const {
    size_t count = 0;
    for (const auto& neuron : pImpl->neurons) {
        if (neuron->isFiring()) {
            ++count;
        }
    }
    return count;
}

float NeuralPopulation::getAverageFiringRate() const {
    if (pImpl->neurons.empty()) {
        return 0.0f;
    }
    float sum = 0.0f;
    for (const auto& neuron : pImpl->neurons) {
        sum += neuron->getFiringRate();
    }
    return sum / static_cast<float>(pImpl->neurons.size());
}

float NeuralPopulation::getActivityLevel() const {
    if (pImpl->neurons.empty()) {
        return 0.0f;
    }
    return static_cast<float>(getActiveCount()) / static_cast<float>(pImpl->neurons.size());
}

void NeuralPopulation::initializeRandom(RandomGenerator& rng) {
    for (auto& neuron : pImpl->neurons) {
        neuron->initializeRandom(rng);
    }
}

void NeuralPopulation::step(Timestamp currentTime) {
    for (auto& neuron : pImpl->neurons) {
        neuron->step(currentTime);
    }
}

void NeuralPopulation::reset() {
    for (auto& neuron : pImpl->neurons) {
        neuron->reset();
    }
    pImpl->membranePotentials.clear();
}

std::vector<NeuronId> NeuralPopulation::getNeuronIds() const {
    std::vector<NeuronId> ids;
    ids.reserve(pImpl->neurons.size());
    for (const auto& neuron : pImpl->neurons) {
        ids.push_back(neuron->getId());
    }
    return ids;
}

void NeuralPopulation::recordActivity() {
    pImpl->membranePotentials.clear();
    pImpl->membranePotentials.reserve(pImpl->neurons.size());
    for (const auto& neuron : pImpl->neurons) {
        pImpl->membranePotentials.push_back(neuron->getMembranePotential());
    }
}

float NeuralPopulation::getMeanMembranePotential() const {
    if (pImpl->membranePotentials.empty()) {
        return 0.0f;
    }
    float sum = std::accumulate(pImpl->membranePotentials.begin(), 
                                 pImpl->membranePotentials.end(), 0.0f);
    return sum / static_cast<float>(pImpl->membranePotentials.size());
}

float NeuralPopulation::getVarianceMembranePotential() const {
    if (pImpl->membranePotentials.size() < 2) {
        return 0.0f;
    }
    float mean = getMeanMembranePotential();
    float sumSquares = 0.0f;
    for (float v : pImpl->membranePotentials) {
        float diff = v - mean;
        sumSquares += diff * diff;
    }
    return sumSquares / static_cast<float>(pImpl->membranePotentials.size() - 1);
}

} // namespace nlm
