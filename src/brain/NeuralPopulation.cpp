// NeuralPopulation.cpp - Safe implementation with proper error handling
#include "NeuralPopulation.hpp"
#include <algorithm>
#include <numeric>
#include <memory>
#include <stdexcept>
#include "../core/Random/Random.hpp"

namespace nlm {

struct NeuralPopulation::Impl {
    PopulationId id;
    size_t size;
    std::vector<std::unique_ptr<Neuron>> neurons;
    const std::vector<Neuron*>& neurons_const_view;  // Safe view of neurons
    NeuronType neuronType;
    std::vector<MembranePotential> membranePotentials;  // for statistics
    
    Impl(PopulationId id, size_t size) : id(id), size(size), neurons(), neurons_const_view(neurons), neuronType(NeuronType::Internal) {}
};

NeuralPopulation::NeuralPopulation(PopulationId id, size_t size) : pImpl(new Impl(id, size)) {
    if (size == 0) {
        throw std::invalid_argument("NeuralPopulation size must be positive");
    }
}

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
    if (!isValidNeuronIndex(index)) {
        return nullptr;
    }
    return pImpl->neurons[index].get();
}

const Neuron* NeuralPopulation::getNeuron(NeuronIndex index) const {
    if (!isValidNeuronIndex(index)) {
        return nullptr;
    }
    return pImpl->neurons[index].get();
}

void NeuralPopulation::addNeuron(std::unique_ptr<Neuron> neuron) {
    if (!neuron) {
        throw std::invalid_argument("Cannot add null neuron to population");
    }
    pImpl->neurons.push_back(std::move(neuron));
    if (pImpl->size < pImpl->neurons.size()) {
        pImpl->size = pImpl->neurons.size();
    }
}

const std::vector<Neuron*>& NeuralPopulation::getNeurons() const {
    return pImpl->neurons_const_view;
}

NeuronType NeuralPopulation::getNeuronType() const {
    return pImpl->neuronType;
}

void NeuralPopulation::setNeuronType(NeuronType type) {
    pImpl->neuronType = type;
    for (auto& neuronPtr : pImpl->neurons) {
        if (neuronPtr) {
            neuronPtr->setType(type);
        }
    }
}

size_t NeuralPopulation::getActiveCount() const {
    size_t count = 0;
    for (const auto& neuronPtr : pImpl->neurons) {
        if (!neuronPtr) continue;
        const Neuron* neuron = neuronPtr.get();
        if (neuron->isFiring() || neuron->getFiringRate() > 0.0f) {
            ++count;
        }
    }
    return count;
}

size_t NeuralPopulation::getFiringCount() const {
    size_t count = 0;
    for (const auto& neuronPtr : pImpl->neurons) {
        if (!neuronPtr) continue;
        const Neuron* neuron = neuronPtr.get();
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
    size_t validCount = 0;
    for (const auto& neuronPtr : pImpl->neurons) {
        if (!neuronPtr) continue;
        const Neuron* neuron = neuronPtr.get();
        sum += neuron->getFiringRate();
        ++validCount;
    }
    if (validCount == 0) return 0.0f;
    return sum / static_cast<float>(validCount);
}

float NeuralPopulation::getActivityLevel() const {
    if (pImpl->neurons.empty()) {
        return 0.0f;
    }
    return static_cast<float>(getActiveCount()) / static_cast<float>(pImpl->neurons.size());
}

void NeuralPopulation::initializeRandom(RandomGenerator& rng) {
    for (auto& neuronPtr : pImpl->neurons) {
        if (neuronPtr) {
            neuronPtr->initializeRandom(rng);
        }
    }
}

void NeuralPopulation::step(Timestamp currentTime) {
    for (auto& neuronPtr : pImpl->neurons) {
        if (neuronPtr) {
            neuronPtr->step(currentTime);
        }
    }
}

void NeuralPopulation::reset() {
    for (auto& neuronPtr : pImpl->neurons) {
        if (neuronPtr) {
            neuronPtr->reset();
        }
    }
    pImpl->membranePotentials.clear();
}

std::vector<NeuronId> NeuralPopulation::getNeuronIds() const {
    std::vector<NeuronId> ids;
    ids.reserve(pImpl->neurons.size());
    for (const auto& neuronPtr : pImpl->neurons) {
        if (!neuronPtr) continue;
        const Neuron* neuron = neuronPtr.get();
        ids.push_back(neuron->getId());
    }
    return ids;
}

void NeuralPopulation::recordActivity() {
    pImpl->membranePotentials.clear();
    pImpl->membranePotentials.reserve(pImpl->neurons.size());
    for (const auto& neuronPtr : pImpl->neurons) {
        if (!neuronPtr) continue;
        const Neuron* neuron = neuronPtr.get();
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

bool NeuralPopulation::isValidNeuronIndex(NeuronIndex index) const {
    return index < pImpl->neurons.size();
}

size_t NeuralPopulation::getSafeNeuronCount() const {
    return pImpl->neurons.size();
}

float NeuralPopulation::getWeightedActivityLevel() const {
    if (pImpl->neurons.empty()) {
        return 0.0f;
    }
    float sum = 0.0f;
    size_t validCount = 0;
    for (const auto& neuronPtr : pImpl->neurons) {
        if (!neuronPtr) continue;
        const Neuron* neuron = neuronPtr.get();
        sum += neuron->getMembranePotential();
        ++validCount;
    }
    if (validCount == 0) return 0.0f;
    return sum / static_cast<float>(validCount);
}

} // namespace nlm