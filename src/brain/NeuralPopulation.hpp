#pragma once

#include "../core/Types/Types.hpp"
#include "Neuron.hpp"
#include <vector>
#include <algorithm>
#include <numeric>

namespace nlm {

// NeuralPopulation: A group of neurons with shared properties
// PLACEHOLDER - Phase 2 will implement population-level dynamics

class NeuralPopulation {
public:
    // Create population with ID and size
    NeuralPopulation(PopulationId id, size_t size) : id(id), size(size), neuronType(NeuronType::Internal) {}
    
    ~NeuralPopulation() = default;
    
    // Disable copying, enable moving
    NeuralPopulation(const NeuralPopulation&) = delete;
    NeuralPopulation& operator=(const NeuralPopulation&) = delete;
    NeuralPopulation(NeuralPopulation&& other) noexcept : id(other.id), size(other.size), 
        neurons(std::move(other.neurons)), neuronType(other.neuronType),
        membranePotentials(std::move(other.membranePotentials)) {
        other.id = INVALID_POPULATION_ID;
        other.size = 0;
    }
    NeuralPopulation& operator=(NeuralPopulation&& other) noexcept {
        if (this != &other) {
            id = other.id;
            size = other.size;
            neurons = std::move(other.neurons);
            neuronType = other.neuronType;
            membranePotentials = std::move(other.membranePotentials);
            other.id = INVALID_POPULATION_ID;
            other.size = 0;
        }
        return *this;
    }
    
    // Identity
    PopulationId getId() const { return id; }
    
    // Size
    size_t getSize() const { return size; }
    bool isEmpty() const { return neurons.empty(); }
    
    // Neuron access
    Neuron* getNeuron(NeuronIndex index) {
        if (index >= neurons.size()) {
            return nullptr;
        }
        return neurons[index];
    }
    
    const Neuron* getNeuron(NeuronIndex index) const {
        if (index >= neurons.size()) {
            return nullptr;
        }
        return neurons[index];
    }
    
    // Add neuron to population
    void addNeuron(Neuron* neuron) {
        neurons.push_back(neuron);
        if (size < neurons.size()) {
            size = neurons.size();
        }
    }
    
    // Get all neurons
    const std::vector<Neuron*>& getNeurons() const { return neurons; }
    
    // Population type (all neurons in population share same type)
    NeuronType getNeuronType() const { return neuronType; }
    void setNeuronType(NeuronType type) {
        neuronType = type;
        for (auto* neuron : neurons) {
            neuron->setType(type);
        }
    }
    
    // Statistics
    size_t getActiveCount() const {
        size_t count = 0;
        for (const auto* neuron : neurons) {
            if (neuron->isFiring() || neuron->getFiringRate() > 0.0f) {
                ++count;
            }
        }
        return count;
    }
    
    size_t getFiringCount() const {
        size_t count = 0;
        for (const auto* neuron : neurons) {
            if (neuron->isFiring()) {
                ++count;
            }
        }
        return count;
    }
    
    float getAverageFiringRate() const {
        if (neurons.empty()) {
            return 0.0f;
        }
        float sum = 0.0f;
        for (const auto* neuron : neurons) {
            sum += neuron->getFiringRate();
        }
        return sum / static_cast<float>(neurons.size());
    }
    
    float getActivityLevel() const {
        if (neurons.empty()) {
            return 0.0f;
        }
        return static_cast<float>(getActiveCount()) / static_cast<float>(neurons.size());
    }
    
    // Initialize all neurons randomly
    void initializeRandom(RandomGenerator& rng);
    
    // Step all neurons
    void step(Timestamp currentTime);
    
    // Reset all neurons
    void reset();
    
    // Get neuron IDs in this population
    std::vector<NeuronId> getNeuronIds() const {
        std::vector<NeuronId> ids;
        ids.reserve(neurons.size());
        for (const auto* neuron : neurons) {
            ids.push_back(neuron->getId());
        }
        return ids;
    }
    
    // Statistics tracking
    void recordActivity() {
        membranePotentials.clear();
        membranePotentials.reserve(neurons.size());
        for (const auto* neuron : neurons) {
            membranePotentials.push_back(neuron->getMembranePotential());
        }
    }
    
    float getMeanMembranePotential() const {
        if (membranePotentials.empty()) {
            return 0.0f;
        }
        float sum = std::accumulate(membranePotentials.begin(), 
                                     membranePotentials.end(), 0.0f);
        return sum / static_cast<float>(membranePotentials.size());
    }
    
    float getVarianceMembranePotential() const {
        if (membranePotentials.size() < 2) {
            return 0.0f;
        }
        float mean = getMeanMembranePotential();
        float sumSquares = 0.0f;
        for (float v : membranePotentials) {
            float diff = v - mean;
            sumSquares += diff * diff;
        }
        return sumSquares / static_cast<float>(membranePotentials.size() - 1);
    }
    
private:
    // NeuralPopulation implementation details (moved from pImpl)
    PopulationId id;
    size_t size;
    std::vector<Neuron*> neurons;
    NeuronType neuronType;
    std::vector<MembranePotential> membranePotentials;  // for statistics
};

} // namespace nlm
