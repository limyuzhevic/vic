#include "NeuralPopulation.hpp"
#include <algorithm>
#include <numeric>
#include <sstream>
#include <stdexcept>

namespace nlm {

struct NeuralPopulation::Impl {
    PopulationId id;
    size_t size;
    std::vector<Neuron*> neurons;
    NeuronType neuronType;
    std::vector<MembranePotential> membranePotentials;  // for statistics
    
    Impl(PopulationId id, size_t size) : id(id), size(size), neuronType(NeuronType::Internal) {}
};

NeuralPopulation::NeuralPopulation(PopulationId id, size_t size) : pImpl(new Impl(id, size)) {
    if (size == 0) {
        NLM_LOG_WARNING(std::string("NeuralPopulation created with zero size (id: ") + std::to_string(id.index()));
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
    if (index >= pImpl->neurons.size()) {
        NLM_LOG_WARNING(std::string("Neuron access out of range in population ") + std::to_string(pImpl->id.index()) +
                       ". Requested index: " + std::to_string(index) + ", size: " + std::to_string(pImpl->neurons.size()));
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

void NeuralPopulation::addNeuron(Neuron* neuron) {
    if (!neuron) {
        NLM_LOG_ERROR("Attempted to add null neuron to population");
        return;
    }
    
    pImpl->neurons.push_back(neuron);
    if (pImpl->size < pImpl->neurons.size()) {
        pImpl->size = pImpl->neurons.size();
    }
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
}

size_t NeuralPopulation::getActiveCount() const {
    size_t count = 0;
    for (const auto* neuron : pImpl->neurons) {
        if (neuron->isFiring() || neuron->getFiringRate() > 0.0f) {
            ++count;
        }
    }
    return count;
}

size_t NeuralPopulation::getFiringCount() const {
    size_t count = 0;
    for (const auto* neuron : pImpl->neurons) {
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
    for (const auto* neuron : pImpl->neurons) {
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
    for (auto* neuron : pImpl->neurons) {
        neuron->initializeRandom(rng);
    }
}

void NeuralPopulation::step(Timestamp currentTime) {
    // Validate population state before stepping - optimized early exits
    if (pImpl->neurons.empty()) {
        NLM_LOG_WARNING("Attempting to step empty neural population");
        return;
    }
    
    // Check for null neurons with batch processing
    size_t i = 0;
    while (i < pImpl->neurons.size()) {
        if (!pImpl->neurons[i]) {
            NLM_LOG_ERROR(std::string("Null neuron found at index ") + std::to_string(i) + 
                         " in population " + std::to_string(pImpl->id.index()));
            pImpl->neurons.erase(pImpl->neurons.begin() + i);
            // Don't increment i, new element shifted into current position
        } else {
            ++i;
        }
    }
    
    // Batch step all neurons - cache local references
    Neuron** neuronPtrs = pImpl->neurons.data();
    size_t neuronCount = pImpl->neurons.size();
    
    for (size_t i = 0; i < neuronCount; ++i) {
        neuronPtrs[i]->step(currentTime);
    }
}

void NeuralPopulation::reset() {
    for (auto* neuron : pImpl->neurons) {
        neuron->reset();
    }
    pImpl->membranePotentials.clear();
}

std::vector<NeuronId> NeuralPopulation::getNeuronIds() const {
    std::vector<NeuronId> ids;
    ids.reserve(pImpl->neurons.size());
    for (const auto* neuron : pImpl->neurons) {
        ids.push_back(neuron->getId());
    }
    return ids;
}

void NeuralPopulation::recordActivity() {
    // Optimized activity recording - reserve memory upfront
    size_t neuronCount = pImpl->neurons.size();
    if (neuronCount == 0) {
        pImpl->membranePotentials.clear();
        return;
    }
    
    // Reserve space to avoid reallocations
    pImpl->membranePotentials.clear();
    pImpl->membranePotentials.reserve(neuronCount);
    
    // Batch extract membrane potentials
    Neuron** neuronPtrs = pImpl->neurons.data();
    for (size_t i = 0; i < neuronCount; ++i) {
        pImpl->membranePotentials.push_back(neuronPtrs[i]->getMembranePotential());
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
