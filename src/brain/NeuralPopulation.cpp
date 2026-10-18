#include "NeuralPopulation.hpp"
#include "StatisticsManager.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

// Global statistics manager instance
static StatisticsManager statsManager;

struct NeuralPopulation::Impl {
    PopulationId id;
    size_t size;
    std::vector<NeuronId> neuronIds;
    std::vector<NeuronType> neuronTypes;
    std::vector<MembranePotential> neuronMembranePotentials;
    std::vector<FiringState> neuronFiringStates;
    std::vector<FiringRate> neuronFiringRates;
    NeuronType neuronType;
    
    // Integrated statistics for optimized computation
    std::vector<float> soaFiringRates;
    std::vector<float> soaMembranePotentials;
    std::vector<uint8_t> soaFiringStates;
    
    Impl(PopulationId id, size_t size) : id(id), size(size), neuronType(NeuronType::Internal) {}
};

NeuralPopulation::NeuralPopulation(PopulationId id, size_t size) : pImpl(new Impl(id, size)) {
    // Initialize SoA arrays for statistics collection
    pImpl->soaFiringRates.resize(size);
    pImpl->soaMembranePotentials.resize(size);
    pImpl->soaFiringStates.resize(size);
    std::fill(pImpl->soaFiringRates.begin(), pImpl->soaFiringRates.end(), 0.0f);
    std::fill(pImpl->soaMembranePotentials.begin(), pImpl->soaMembranePotentials.end(), 0.0f);
    std::fill(pImpl->soaFiringStates.begin(), pImpl->soaFiringStates.end(), 0);
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
    return pImpl->neuronIds.empty();
}

Neuron* NeuralPopulation::getNeuron(NeuronIndex index) {
    if (index >= pImpl->neuronIds.size()) {
        return nullptr;
    }
    // TODO: This would require a neuron lookup system
    // For now, return nullptr to avoid breaking compilation
    return nullptr;
}

const Neuron* NeuralPopulation::getNeuron(NeuronIndex index) const {
    if (index >= pImpl->neuronIds.size()) {
        return nullptr;
    }
    return nullptr;  // Placeholder
}

const std::vector<Neuron*>& NeuralPopulation::getNeurons() const {
    // TODO: Return empty vector for now since we're using SoA
    // This will be properly implemented once neurons are integrated
    static const std::vector<Neuron*> empty;
    return empty;
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
    for (size_t i = 0; i < pImpl->neuronFiringRates.size(); ++i) {
        if (pImpl->neuronFiringRates[i] > 0.0f) {
            ++count;
        }
    }
    return count;
}

size_t NeuralPopulation::getFiringCount() const {
    size_t count = 0;
    // Since we don't have direct firing state access, use an approximation
    // TODO: Implement proper SoA firing state access
    return count;
}

float NeuralPopulation::getAverageFiringRate() const {
    if (pImpl->neuronFiringRates.empty()) {
        return 0.0f;
    }
    float sum = 0.0f;
    for (float rate : pImpl->neuronFiringRates) {
        sum += rate;
    }
    return sum / static_cast<float>(pImpl->neuronFiringRates.size());
}

float NeuralPopulation::getActivityLevel() const {
    if (pImpl->neuronFiringRates.empty()) {
        return 0.0f;
    }
    return static_cast<float>(getActiveCount()) / static_cast<float>(pImpl->neuronFiringRates.size());
}

void NeuralPopulation::initializeRandom(RandomGenerator& rng) {
    // SoA implementation - would need to access neuron objects through a lookup system
    // For now, placeholder implementation
    (void)rng;  // Avoid unused parameter warning
}

void NeuralPopulation::step(Timestamp currentTime) {
    // SoA implementation - would need to access neuron objects through a lookup system
    // For now, placeholder implementation
    (void)currentTime;  // Avoid unused parameter warning
}

void NeuralPopulation::reset() {
    // SoA implementation
    // Reset neuron states through neuron lookup system
    // Reset membrane potentials
    pImpl->neuronMembranePotentials.clear();
    pImpl->neuronFiringStates.clear();
    pImpl->neuronFiringRates.clear();
}

std::vector<NeuronId> NeuralPopulation::getNeuronIds() const {
    std::vector<NeuronId> ids;
    ids.reserve(pImpl->neuronIds.size());
    for (const auto& id : pImpl->neuronIds) {
        ids.push_back(id);
    }
    return ids;
}

// Update SoA arrays with current neuron states
    statsManager.initializePopulationStats(popStats, pImpl->size);
    
    // Populate SoA arrays from neuron objects
    for (size_t i = 0; i < pImpl->size; ++i) {
        popStats.soaFiringRates[i] = pImpl->neuronFiringRates[i];
        popStats.soaMembranePotentials[i] = static_cast<float>(pImpl->neuronMembranePotentials[i]);
        popStats.soaFiringStates[i] = static_cast<uint8_t>(pImpl->neuronFiringStates[i]);
    }
    
    // Update statistics manager
    statsManager.updatePopulationStats(popStats, 
                                     popStats.soaFiringRates,
                                     popStats.soaMembranePotentials,
                                     popStats.soaFiringStates);
}

// Create temporary PopulationStats object for SoA statistics computation
    PopulationStats tempStats;
    statsManager.initializePopulationStats(tempStats, pImpl->size);
    
    // Populate SoA arrays from neuron states
    for (size_t i = 0; i < pImpl->size; ++i) {
        tempStats.soaFiringRates[i] = pImpl->neuronFiringRates[i];
        tempStats.soaMembranePotentials[i] = static_cast<float>(pImpl->neuronMembranePotentials[i]);
        tempStats.soaFiringStates[i] = static_cast<uint8_t>(pImpl->neuronFiringStates[i]);
    }
    
    // Update statistics using SoA access
    statsManager.updatePopulationStats(tempStats,
                                     tempStats.soaFiringRates,
                                     tempStats.soaMembranePotentials,
                                     tempStats.soaFiringStates);
    
    return tempStats.meanMembranePotential;
}

float NeuralPopulation::getVarianceMembranePotential() const {
    // Use StatisticsManager for optimized variance computation
    PopulationStats tempStats;
    statsManager.initializePopulationStats(tempStats, pImpl->size);
    
    // Populate SoA arrays from neuron states
    for (size_t i = 0; i < pImpl->size; ++i) {
        tempStats.soaFiringRates[i] = pImpl->neuronFiringRates[i];
        tempStats.soaMembranePotentials[i] = static_cast<float>(pImpl->neuronMembranePotentials[i]);
        tempStats.soaFiringStates[i] = static_cast<uint8_t>(pImpl->neuronFiringStates[i]);
    }
    
    // Update statistics using SoA access
    statsManager.updatePopulationStats(tempStats,
                                     tempStats.soaFiringRates,
                                     tempStats.soaMembranePotentials,
                                     tempStats.soaFiringStates);
    
    return tempStats.varianceMembranePotential;
}

} // namespace nlm
