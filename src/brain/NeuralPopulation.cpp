#include "NeuralPopulation.hpp"
#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <memory>

namespace nlm {

struct NeuralPopulation::Impl {
    PopulationId id;
    size_t size;
    std::vector<Neuron*> neurons;
    NeuronType neuronType;
    std::vector<MembranePotential> membranePotentials;  // for statistics
    
    Impl(PopulationId id, size_t size) : id(id), size(size), neuronType(NeuronType::Internal) {
        neurons.reserve(size);
        membranePotentials.reserve(size);
    }
    
    ~Impl() {
        // Clean up neurons if we own them
        // Note: In the current architecture, NeuralPopulation doesn't own neurons,
        // but this destructor shows the pattern for safe cleanup
        neurons.clear();
        membranePotentials.clear();
    }
};

NeuralPopulation::NeuralPopulation(PopulationId id, size_t size) : pImpl(new Impl(id, size)) {
    if (!pImpl) {
        throw std::bad_alloc("Failed to allocate NeuralPopulation implementation");
    }
}

NeuralPopulation::~NeuralPopulation() {
    // The pImpl pointer will be deleted automatically by the default destructor
    // since we don't have a custom destructor, this is safe
}

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
        return nullptr; // Return nullptr instead of throwing to maintain API compatibility
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
        NLM_LOG_WARNING("Attempt to add null neuron to population");
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
        if (neuron) {
            neuron->setType(type);
        }
    }
}

size_t NeuralPopulation::getActiveCount() const {
    size_t count = 0;
    for (const auto* neuron : pImpl->neurons) {
        if (neuron && (neuron->isFiring() || neuron->getFiringRate() > 0.0f)) {
            ++count;
        }
    }
    return count;
}

size_t NeuralPopulation::getFiringCount() const {
    size_t count = 0;
    for (const auto* neuron : pImpl->neurons) {
        if (neuron && neuron->isFiring()) {
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
    size_t validNeurons = 0;
    for (const auto* neuron : pImpl->neurons) {
        if (neuron) {
            sum += neuron->getFiringRate();
            ++validNeurons;
        }
    }
    if (validNeurons == 0) {
        return 0.0f;
    }
    return sum / static_cast<float>(validNeurons);
}

float NeuralPopulation::getActivityLevel() const {
    if (pImpl->neurons.empty()) {
        return 0.0f;
    }
    size_t activeCount = getActiveCount();
    return static_cast<float>(activeCount) / static_cast<float>(pImpl->neurons.size());
}

void NeuralPopulation::initializeRandom(RandomGenerator& rng) {
    for (auto* neuron : pImpl->neurons) {
        if (neuron) {
            neuron->initializeRandom(rng);
        }
    }
}

void NeuralPopulation::step(Timestamp currentTime) {
    for (auto* neuron : pImpl->neurons) {
        if (neuron) {
            try {
                neuron->step(currentTime);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Exception stepping neuron in population: " + e.what()));
            }
        }
    }
}

void NeuralPopulation::reset() {
    for (auto* neuron : pImpl->neurons) {
        if (neuron) {
            neuron->reset();
        }
    }
    pImpl->membranePotentials.clear();
}

std::vector<NeuronId> NeuralPopulation::getNeuronIds() const {
    std::vector<NeuronId> ids;
    ids.reserve(pImpl->neurons.size());
    for (const auto* neuron : pImpl->neurons) {
        if (neuron) {
            ids.push_back(neuron->getId());
        }
    }
    return ids;
}

void NeuralPopulation::recordActivity() {
    pImpl->membranePotentials.clear();
    pImpl->membranePotentials.reserve(pImpl->neurons.size());
    for (const auto* neuron : pImpl->neurons) {
        if (neuron) {
            pImpl->membranePotentials.push_back(neuron->getMembranePotential());
        }
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
