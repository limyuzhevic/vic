#pragma once

#include <string>
#include <vector>
#include <shared_mutex>
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

/**
 * @brief Experiment class for measuring synaptic changes over time
 * 
 * Records and analyzes synaptic weight changes during learning experiments.
 * Thread-safe for concurrent access during simulation.
 */
class LearningExperiment {
public:
    /**
     * @brief Constructor for learning experiment
     * 
     * @param brain Shared pointer to the brain being tested
     * @param seed Random seed for reproducible measurements
     */
    LearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed);
    
    /**
     * @brief Record initial synaptic state
     * 
     * Captures initial weights and neuron activity before learning begins.
     */
    void recordInitialState();
    
    /**
     * @brief Record final synaptic state
     * 
     * Captures final weights and neuron activity after learning completes.
     */
    void recordFinalState();
    
    /**
     * @brief Compute and display learning statistics
     * 
     * Analyzes synaptic changes and determines if learning occurred.
     */
    void computeStatistics();
    
    /**
     * @brief Get initial synapse count
     * 
     * @return Number of synapses at experiment start
     */
    size_t getInitialSynapseCount() const { return initialSynapseCount; }
    
    /**
     * @brief Get most active neurons during experiment
     * 
     * @return Vector of neuron IDs that were most active
     */
    const std::vector<NeuronId>& getMostActiveNeurons() const { return mostActiveNeurons; }
    
private:
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    mutable std::shared_mutex mutex;
};

/**
 * @brief Runs basic neural connectivity test
 * 
 * Injects current into neurons and verifies spike propagation through the network.
 * 
 * @param brain Shared pointer to the brain being tested
 */
void runBasicConnectivityTest(std::shared_ptr<Brain> brain);

/**
 * @brief Runs plasticity learning experiment
 * 
 * Applies repeated input patterns to stimulate synaptic learning via STDP and Hebbian rules.
 * 
 * @param brain Shared pointer to the brain being tested
 */
void runPlasticityExperiment(std::shared_ptr<Brain> brain);

/**
 * @brief Runs STDP verification test
 * 
 * Verifies that spike-timing dependent plasticity produces expected weight changes.
 * 
 * @param brain Shared pointer to the brain being tested
 */
void runStdpVerification(std::shared_ptr<Brain> brain);

} // namespace nlm