#include "NeuralPopulation.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>

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
    return pImpl->id;
}

size_t NeuralPopulation::getSize() const {
    return pImpl->size;
}

bool NeuralPopulation::isEmpty() const {
    return pImpl->neurons.empty();
}

Neuron* NeuralPopulation::getNeuron(NeuronIndex index) {
    try {
        if (pImpl->neurons.empty() || index >= pImpl->neurons.size()) {
            return nullptr;
        }
        // Additional validation for null pointer
        if (!pImpl->neurons[index]) {
            return nullptr;
        }
        return pImpl->neurons[index];
    } catch (...) {
        return nullptr; // Handle any errors gracefully
    }
}

const Neuron* NeuralPopulation::getNeuron(NeuronIndex index) const {
    try {
        if (pImpl->neurons.empty() || index >= pImpl->neurons.size()) {
            return nullptr;
        }
        // Additional validation for null pointer
        if (!pImpl->neurons[index]) {
            return nullptr;
        }
        return pImpl->neurons[index];
    } catch (...) {
        return nullptr; // Handle any errors gracefully
    }
}

void NeuralPopulation::addNeuron(Neuron* neuron) {
    try {
        if (!neuron) {
            return; // Don't add null neurons
        }
        pImpl->neurons.push_back(neuron);
        if (pImpl->size < pImpl->neurons.size()) {
            pImpl->size = pImpl->neurons.size();
        }
    } catch (...) {
        // Handle memory allocation errors gracefully
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
    try {
        if (pImpl->neurons.empty()) {
            return 0.0f;
        }
        float sum = 0.0f;
        for (const auto* neuron : pImpl->neurons) {
            if (!neuron) {
                continue; // Skip null neurons
            }
            sum += neuron->getFiringRate();
        }
        return sum / static_cast<float>(pImpl->neurons.size());
    } catch (...) {
        return 0.0f; // Handle any errors gracefully
    }
}

float NeuralPopulation::getActivityLevel() const {
    try {
        if (pImpl->neurons.empty()) {
            return 0.0f;
        }
        return static_cast<float>(getActiveCount()) / static_cast<float>(pImpl->neurons.size());
    } catch (...) {
        return 0.0f; // Handle any errors gracefully
    }
}

void NeuralPopulation::initializeRandom(RandomGenerator& rng) {
    for (auto* neuron : pImpl->neurons) {
        neuron->initializeRandom(rng);
    }
}

void NeuralPopulation::step(Timestamp currentTime) {
    try {
        // Validate input time
        if (std::isnan(currentTime) || std::isinf(currentTime)) {
            return; // Invalid time, skip
        }
        
        for (auto* neuron : pImpl->neurons) {
            if (neuron) {
                neuron->step(currentTime);
            }
        }
    } catch (...) {
        // Handle any errors gracefully
    }
}

void NeuralPopulation::reset() {
    try {
        for (auto* neuron : pImpl->neurons) {
            if (neuron) {
                neuron->reset();
            }
        }
        pImpl->membranePotentials.clear();
    } catch (...) {
        // Handle any errors gracefully
    }
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
    try {
        pImpl->membranePotentials.clear();
        pImpl->membranePotentials.reserve(pImpl->neurons.size());
        for (const auto* neuron : pImpl->neurons) {
            if (neuron) {
                pImpl->membranePotentials.push_back(neuron->getMembranePotential());
            }
        }
    } catch (...) {
        // Handle any errors gracefully
    }
}

float NeuralPopulation::getMeanMembranePotential() const {
    try {
        if (pImpl->membranePotentials.empty()) {
            return 0.0f;
        }
        float sum = std::accumulate(pImpl->membranePotentials.begin(), 
                                     pImpl->membranePotentials.end(), 0.0f);
        return sum / static_cast<float>(pImpl->membranePotentials.size());
    } catch (...) {
        return 0.0f; // Handle any errors gracefully
    }
}

float NeuralPopulation::getVarianceMembranePotential() const {
    try {
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
    } catch (...) {
        return 0.0f; // Handle any errors gracefully
    }
}

} // namespace nlm
