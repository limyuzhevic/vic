#include "BasePimpl.hpp"
#include "NeuralPopulation.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

struct NeuralPopulation::Impl {
    PopulationId id;
    size_t size;
    std::vector<Neuron*> neurons;
    NeuronType neuronType;
    std::vector<MembranePotential> membranePotentials;  // for statistics
    
    Impl(PopulationId id, size_t size) : id(id), size(size), neuronType(NeuronType::Internal) {}
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
    return pImpl->get()->id;
}

size_t NeuralPopulation::getSize() const {
    return pImpl->get()->size;
}

bool NeuralPopulation::isEmpty() const {
    return pImpl->get()->neurons.empty();
}

Neuron* NeuralPopulation::getNeuron(NeuronIndex index) {
    if (index >= pImpl->get()->neurons.size()) {
        return nullptr;
    }
    return pImpl->get()->neurons[index];
}

const Neuron* NeuralPopulation::getNeuron(NeuronIndex index) const {
    if (index >= pImpl->get()->neurons.size()) {
        return nullptr;
    }
    return pImpl->get()->neurons[index];
}

void NeuralPopulation::addNeuron(Neuron* neuron) {
    pImpl->get()->neurons.push_back(neuron);
    if (pImpl->get()->size < pImpl->get()->neurons.size()) {
        pImpl->get()->size = pImpl->get()->neurons.size();
    }
}

const std::vector<Neuron*>& NeuralPopulation::getNeurons() const {
    return pImpl->get()->neurons;
}

NeuronType NeuralPopulation::getNeuronType() const {
    return pImpl->get()->neuronType;
}

void NeuralPopulation::setNeuronType(NeuronType type) {
    pImpl->get()->neuronType = type;
    for (auto* neuron : pImpl->get()->neurons) {
        neuron->setType(type);
    }
}

size_t NeuralPopulation::getActiveCount() const {
    size_t count = 0;
    for (const auto* neuron : pImpl->get()->neurons) {
        if (neuron->isFiring() || neuron->getFiringRate() > 0.0f) {
            ++count;
        }
    }
    return count;
}

size_t NeuralPopulation::getFiringCount() const {
    size_t count = 0;
    for (const auto* neuron : pImpl->get()->neurons) {
        if (neuron->isFiring()) {
            ++count;
        }
    }
    return count;
}

float NeuralPopulation::getAverageFiringRate() const {
    if (pImpl->get()->neurons.empty()) {
        return 0.0f;
    }
    float sum = 0.0f;
    for (const auto* neuron : pImpl->get()->neurons) {
        sum += neuron->getFiringRate();
    }
    return sum / static_cast<float>(pImpl->get()->neurons.size());
}

float NeuralPopulation::getActivityLevel() const {
    if (pImpl->get()->neurons.empty()) {
        return 0.0f;
    }
    return static_cast<float>(getActiveCount()) / static_cast<float>(pImpl->get()->neurons.size());
}

void NeuralPopulation::initializeRandom(RandomGenerator& rng) {
    for (auto* neuron : pImpl->get()->neurons) {
        neuron->initializeRandom(rng);
    }
}

void NeuralPopulation::step(Timestamp currentTime) {
    for (auto* neuron : pImpl->get()->neurons) {
        neuron->step(currentTime);
    }
}

void NeuralPopulation::reset() {
    for (auto* neuron : pImpl->get()->neurons) {
        neuron->reset();
    }
    pImpl->get()->membranePotentials.clear();
}

std::vector<NeuronId> NeuralPopulation::getNeuronIds() const {
    std::vector<NeuronId> ids;
    ids.reserve(pImpl->get()->neurons.size());
    for (const auto* neuron : pImpl->get()->neurons) {
        ids.push_back(neuron->getId());
    }
    return ids;
}

void NeuralPopulation::recordActivity() {
    pImpl->get()->membranePotentials.clear();
    pImpl->get()->membranePotentials.reserve(pImpl->get()->neurons.size());
    for (const auto* neuron : pImpl->get()->neurons) {
        pImpl->get()->membranePotentials.push_back(neuron->getMembranePotential());
    }
}

float NeuralPopulation::getMeanMembranePotential() const {
    if (pImpl->get()->membranePotentials.empty()) {
        return 0.0f;
    }
    float sum = std::accumulate(pImpl->get()->membranePotentials.begin(), 
                                 pImpl->get()->membranePotentials.end(), 0.0f);
    return sum / static_cast<float>(pImpl->get()->membranePotentials.size());
}

float NeuralPopulation::getVarianceMembranePotential() const {
    if (pImpl->get()->membranePotentials.size() < 2) {
        return 0.0f;
    }
    float mean = getMeanMembranePotential();
    float sumSquares = 0.0f;
    for (float v : pImpl->get()->membranePotentials) {
        float diff = v - mean;
        sumSquares += diff * diff;
    }
    return sumSquares / static_cast<float>(pImpl->get()->membranePotentials.size() - 1);
}

} // namespace nlm
