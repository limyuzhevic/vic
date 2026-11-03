#include "NeuralPopulation.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

struct NeuralPopulation::Impl {
    PopulationId id;
    size_t size;
    std::vector<std::unique_ptr<Neuron>> neurons;  // OWNING smart pointers
    NeuronType neuronType;
    std::vector<MembranePotential> membranePotentials;  // for statistics
    
    Impl(PopulationId id, size_t size) : id(id), size(size), neuronType(NeuronType::Internal) {};
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
    pImpl->neurons.clear();
    pImpl->neurons.reserve(pImpl->size);
    for (size_t i = 0; i < pImpl->size; ++i) {
        auto neuron = std::make_unique<Neuron>(NeuronId(startId.index() + i));
        neuron->setType(pImpl->neuronType);
        pImpl->neurons.push_back(std::move(neuron));
    }
}

void NeuralPopulation::addNeuron(std::unique_ptr<Neuron> neuron) {
    pImpl->neurons.push_back(std::move(neuron));
    if (pImpl->size < pImpl->neurons.size()) {
        pImpl->size = pImpl->neurons.size();
    }
}

// Note: This API should be updated to return unique_ptrs or Neuron* for better memory management
// For now, we'll keep this for compatibility but it's not ideal
const std::vector<Neuron*>& NeuralPopulation::getNeurons() const {
    // WARNING: This returns raw pointers to owned neurons - caller must not delete them
    // This is kept for API compatibility but should be deprecated in favor of ownership
    static std::vector<Neuron*> rawPtrs;
    rawPtrs.clear();
    rawPtrs.reserve(pImpl->neurons.size());
    for (const auto& neuronPtr : pImpl->neurons) {
        rawPtrs.push_back(neuronPtr.get());
    }
    return rawPtrs;
}

NeuronType NeuralPopulation::getNeuronType() const {
    return pImpl->neuronType;
}

void NeuralPopulation::setNeuronType(NeuronType type) {
    pImpl->neuronType = type;
    for (auto& neuronPtr : pImpl->neurons) {
        neuronPtr->setType(type);
    }
}

size_t NeuralPopulation::getActiveCount() const {
    size_t count = 0;
    for (const auto& neuronPtr : pImpl->neurons) {
        if (neuronPtr->isFiring() || neuronPtr->getFiringRate() > 0.0f) {
            ++count;
        }
    }
    return count;
}

size_t NeuralPopulation::getFiringCount() const {
    size_t count = 0;
    for (const auto& neuronPtr : pImpl->neurons) {
        if (neuronPtr->isFiring()) {
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
    for (const auto& neuronPtr : pImpl->neurons) {
        sum += neuronPtr->getFiringRate();
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
    for (auto& neuronPtr : pImpl->neurons) {
        neuronPtr->initializeRandom(rng);
    }
}

void NeuralPopulation::step(Timestamp currentTime) {
    for (auto& neuronPtr : pImpl->neurons) {
        neuronPtr->step(currentTime);
    }
}

void NeuralPopulation::reset() {
    for (auto& neuronPtr : pImpl->neurons) {
        neuronPtr->reset();
    }
    pImpl->membranePotentials.clear();
}

std::vector<NeuronId> NeuralPopulation::getNeuronIds() const {
    std::vector<NeuronId> ids;
    ids.reserve(pImpl->neurons.size());
    for (const auto& neuronPtr : pImpl->neurons) {
        ids.push_back(neuronPtr->getId());
    }
    return ids;
}

void NeuralPopulation::recordActivity() {
    pImpl->membranePotentials.clear();
    pImpl->membranePotentials.reserve(pImpl->neurons.size());
    for (const auto& neuronPtr : pImpl->neurons) {
        pImpl->membranePotentials.push_back(neuronPtr->getMembranePotential());
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
