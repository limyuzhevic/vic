/*
 * This is an automatically generated header file for the NLM brain region management.
 * This file provides a clean interface for managing neural regions within the NLM brain system.
 * 
 * Generated to improve code organization and reduce compilation times.
 */

#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>

namespace nlm {

/**
 * @class NeuralRegion
 * @brief Represents a functional region of the brain with specialized neuron populations
 * 
 * Neural regions are modular components of the brain that contain specific
 * types of neurons organized for particular functions. Each region can have
 * multiple populations of neurons with different properties and connectivity.
 */
class NeuralRegion {
public:
    NeuralRegion(RegionId id, const std::string& name);
    ~NeuralRegion() = default;
    
    // Non-copyable, movable
    NeuralRegion(const NeuralRegion&) = delete;
    NeuralRegion& operator=(const NeuralRegion&) = delete;
    NeuralRegion(NeuralRegion&&) noexcept = default;
    NeuralRegion& operator=(NeuralRegion&&) noexcept = default;
    
    /**
     * @brief Get the unique identifier of this region
     */
    RegionId getId() const { return id_; }
    
    /**
     * @brief Get the name of this region
     */
    const std::string& getName() const { return name_; }
    
    /**
     * @brief Get the total number of neurons in this region
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get the number of populations in this region
     */
    size_t getPopulationCount() const { return populations_.size(); }
    
    /**
     * @brief Get the total number of synapses in this region
     */
    size_t getSynapseCount() const;
    
    /**
     * @brief Get the average synaptic weight in this region
     */
    float getAverageSynapticWeight() const;
    
    /**
     * @brief Get the number of active neurons in this region
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get the average firing rate across all neurons in this region
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Add a new neuron population to this region
     * 
     * @param neuronCount Number of neurons to create in this population
     * @param type Type of neurons to create (Sensory, Internal, Motor, etc.)
     * @return PopulationId of the newly created population
     */
    PopulationId addPopulation(size_t neuronCount, NeuronType type);
    
    /**
     * @brief Get a population by its ID
     */
    class NeuralPopulation* getPopulation(PopulationId id);
    
    const class NeuralPopulation* getPopulation(PopulationId id) const;
    
    /**
     * @brief Get all populations in this region
     */
    const std::vector<std::unique_ptr<class NeuralPopulation>>& getPopulations() const { return populations_; }
    
    /**
     * @brief Get all neurons in this region
     */
    std::vector<Neuron*> getAllNeurons();
    
    const std::vector<Neuron*> getAllNeurons() const;
    
    /**
     * @brief Get all synapses in this region
     */
    std::vector<class Synapse*> getSynapses();
    
    const std::vector<class Synapse*> getSynapses() const;
    
    /**
     * @brief Get all synapses originating from a specific neuron
     */
    std::vector<class Synapse*> getSynapsesFrom(NeuronId neuronId);
    
    const std::vector<class Synapse*> getSynapsesFrom(NeuronId neuronId) const;
    
    /**
     * @brief Get all synapses targeting a specific neuron
     */
     std::vector<class Synapse*> getSynapsesTo(NeuronId neuronId);
    
    const std::vector<class Synapse*> getSynapsesTo(NeuronId neuronId) const;
    
    /**
     * @brief Initialize the region with random connectivity
     * 
     * Creates random synaptic connections between neurons in this region
     * based on the specified connection probability.
     */
    void initializeRandomConnectivity(class RandomGenerator& rng, 
                                     float connectionProbability,
                                     float maxWeight,
                                     float minWeight);
    
    /**
     * @brief Reset all neurons in this region to their initial state
     */
    void reset();
    
    /**
     * @brief Get the neuron with the specified ID
     */
    Neuron* getNeuron(NeuronId id);
    
    const Neuron* getNeuron(NeuronId id) const;
    
    /**
     * @brief Remove a neuron from the region (for structural plasticity)
     */
    void removeNeuron(NeuronId id);
    
    /**
     * @brief Remove a population from the region
     */
    void removePopulation(PopulationId id);
    
    /**
     * @brief Get the underlying brain reference for cross-region operations
     */
    void setBrainReference(class Brain* brain) { brainRef_ = brain; }

private:
    RegionId id_;
    std::string name_;
    std::vector<std::unique_ptr<class NeuralPopulation>> populations_;
    class Brain* brainRef_;
    
    friend class Brain; // Brain can access private members
};
