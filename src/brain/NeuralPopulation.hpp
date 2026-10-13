#pragma once

#include "../core/Types/Types.hpp"
#include "Neuron.hpp"
#include <vector>
#include <memory>

namespace nlm {

// NeuralPopulation: A group of neurons with shared properties
// PLACEHOLDER - Phase 2 will implement population-level dynamics

class NeuralPopulation {
public:
    /**
     * @brief Create neural population with ID and size
     * @param id Unique identifier for the population
     * @param size Number of neurons in this population
     */
    NeuralPopulation(PopulationId id, size_t size);
    
    /**
     * @brief Destructor
     */
    ~NeuralPopulation();
    
    /**
     * @brief Copy constructor deleted (populations are not copyable)
     */
    NeuralPopulation(const NeuralPopulation&) = delete;
    
    /**
     * @brief Copy assignment operator deleted (populations are not copyable)
     */
    NeuralPopulation& operator=(const NeuralPopulation&) = delete;
    
    /**
     * @brief Move constructor
     * @param other Population to move from
     */
    NeuralPopulation(NeuralPopulation&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * @param other Population to move from
     * @return Reference to this population
     */
    NeuralPopulation& operator=(NeuralPopulation&&) noexcept;
    
    /**
     * @brief Get population identifier
     * @return Population ID
     */
    PopulationId getId() const;
    
    /**
     * @brief Get population size
     * @return Number of neurons in population
     */
    size_t getSize() const;
    
    /**
     * @brief Check if population is empty
     * @return true if population has no neurons, false otherwise
     */
    bool isEmpty() const;
    
    /**
     * @brief Get neuron by index
     * @param index Index of neuron (0-based)
     * @return Pointer to neuron at specified index
     */
    Neuron* getNeuron(NeuronIndex index);
    
    /**
     * @brief Get neuron by index (const version)
     * @param index Index of neuron (0-based)
     * @return Const pointer to neuron at specified index
     */
    const Neuron* getNeuron(NeuronIndex index) const;
    
    /**
     * @brief Add neuron to population
     * @param neuron Pointer to neuron to add
     */
    void addNeuron(Neuron* neuron);
    
    /**
     * @brief Get all neurons in population
     * @return Const reference to vector of neuron pointers
     */
    const std::vector<Neuron*>& getNeurons() const;
    
    /**
     * @brief Get all neurons in population (mutable version)
     * @return Vector of neuron pointers (non-const)
     */
    std::vector<Neuron*> getAllNeurons();
    
    /**
     * @brief Get neuron type for this population
     * @return All neurons in population share this type
     */
    NeuronType getNeuronType() const;
    
    /**
     * @brief Set neuron type for this population
     * @param type Neuron type to set for all neurons
     */
    void setNeuronType(NeuronType type);
    
    /**
     * @brief Get number of active neurons
     * @return Count of neurons in Active or Refractory state
     */
    size_t getActiveCount() const;
    
    /**
     * @brief Get number of firing neurons
     * @return Count of neurons in Refractory state (just fired)
     */
    size_t getFiringCount() const;
    
    /**
     * @brief Get average firing rate
     * @return Average firing rate in Hz across all neurons
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Get activity level
     * @return Fraction of active neurons (0.0 to 1.0)
     */
    float getActivityLevel() const;  // fraction of active neurons
    
    /**
     * @brief Initialize all neurons randomly
     * @param rng Random number generator for biological variability
     * @details Calls initializeRandom() for each neuron in population
     */
    void initializeRandom(class RandomGenerator& rng);
    
    /**
     * @brief Step all neurons with specific timestep
     * @param currentTime Current simulation time
     * @param dt Timestep duration in milliseconds
     * @details Advances all neurons by one simulation timestep using their
     *          LIF stepLIF() method with the specified timestep duration
     */
    void step(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Reset all neurons
     * @details Calls reset() for each neuron in population
     */
    void reset();
    
    /**
     * @brief Get neuron IDs in this population
     * @return Vector of neuron IDs
     */
    std::vector<NeuronId> getNeuronIds() const;
    
    /**
     * @brief Record current activity for statistics
     * @details Captures current membrane potentials for statistical analysis
     */
    void recordActivity();  // Record current state for statistics
    
    /**
     * @brief Get mean membrane potential
     * @return Mean membrane potential across all neurons
     */
    float getMeanMembranePotential() const;
    
    /**
     * @brief Get variance of membrane potential
     * @return Variance of membrane potential across all neurons
     */
    float getVarianceMembranePotential() const;
    
private:
    /**
     * @brief Private implementation structure for NeuralPopulation
     * @details Internal implementation details to reduce header size and improve compilation times
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
