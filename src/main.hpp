// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This header file declares the main components for Phase 2:
// - Leaky Integrate-and-Fire (LIF) neuron dynamics
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity rules
// - Structural plasticity (synaptogenesis/pruning)

#ifndef NLM_MAIN_HPP
#define NLM_MAIN_HPP

#include <memory>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include <numeric>

// Forward declarations
namespace nlm {
    class Brain;
    class Neuron;
    class Synapse;
    class Config;
    class Logger;
    class SimulationClock;
}

using namespace nlm;

/**
 * @brief Demonstrates measurable synaptic changes through experience
 * 
 * This struct implements an experimental framework for testing
 * learning in neural networks by measuring synaptic weight changes
 * over time in response to repeated input patterns.
 * 
 * @details The LearningExperiment tracks:
 * - Initial synaptic weights before experience
 * - Final synaptic weights after experience  
 * - Spike activity patterns
 * - Statistical analysis of weight changes
 * - Detection of learning (weight modification)
 * 
 * This struct is used to experimentally verify that neural networks
 * can learn through experience by modifying their synaptic weights.
 * The learning experiment is a key component of Phase 2, which
 * focuses on real neural computation mechanisms.
 */
struct LearningExperiment {
    std::shared_ptr<Brain> brain;           ///< Neural substrate being tested
    uint64_t seed;                          ///< Random seed for reproducibility
    size_t initialSynapseCount;             ///< Number of synapses at start
    std::vector<float> initialWeights;     ///< Synaptic weights before experience
    std::vector<float> finalWeights;        ///< Synaptic weights after experience
    std::vector<NeuronId> mostActiveNeurons;///< Most active neurons during experiment

    /**
     * @brief Constructor for LearningExperiment
     * 
     * @param b Neural substrate (brain) to test
     * @param s Random seed for reproducible experiments
     */
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s);

    /**
     * @brief Record initial synaptic state before learning experiment
     * 
     * @details This method:
     * 1. Records the total number of synapses
     * 2. Collects synaptic weights from the first brain region
     * 3. Calculates and logs statistical summary of initial weights
     * 4. Prepares baseline for comparison after experience
     */
    void recordInitialState();

    /**
     * @brief Record final synaptic state after learning experiment
     * 
     * @details This method:
     * 1. Records final synaptic weights from the first brain region
     * 2. Identifies the most active neurons during the experiment
     * 3. Calculates and logs total spike activity
     * 4. Prepares final weights for statistical analysis
     */
    void recordFinalState();

    /**
     * @brief Analyze and report learning experiment results
     * 
     * @details This method:
     * 1. Computes statistical changes in synaptic weights
     * 2. Counts significant weight changes (strengthened/weakened)
     * 3. Analyzes spike activity patterns
     * 4. Determines if learning occurred
     * 5. Generates comprehensive report of findings
     */
    void computeStatistics();
};

/**
 * @brief Display program banner with phase information
 * 
 * @details Shows the NLM (熙然) Neural Learning Machine banner
 * and provides an overview of Phase 2 capabilities including:
 * - Real LIF neuron dynamics
 * - Event-driven spike propagation
 * - STDP and Hebbian plasticity
 * - Structural plasticity
 */
void printBanner();

/**
 * @brief Test 1: Basic neural connectivity verification
 * 
 * @details This test:
 * 1. Injects strong current into 10 neurons
 * 2. Runs neural simulation for 50 steps
 * 3. Measures spike propagation through network
 * 4. Provides detailed diagnostic output for troubleshooting
 * 
 * This test verifies that the basic neural connectivity
 * is functional and that spikes can propagate through
 * the network, which is essential for all neural computations.
 * 
 * @param brain Neural substrate to test
 */
void runBasicConnectivityTest(std::shared_ptr<Brain> brain);

/**
 * @brief Test 2: Plasticity learning experiment
 * 
 * @details This test:
 * 1. Creates a LearningExperiment instance
 * 2. Records initial synaptic state
 * 3. Enables Hebbian and STDP plasticity
 * 4. Applies 1000 steps of repeated input patterns
 * 5. Records final synaptic state
 * 6. Computes and displays learning statistics
 * 
 * This test verifies that neural networks can learn
 * through experience by modifying synaptic weights.
 * It demonstrates both Hebbian and STDP plasticity
 * mechanisms working together.
 * 
 * @param brain Neural substrate to test
 */
void runPlasticityExperiment(std::shared_ptr<Brain> brain);

/**
 * @brief Test 3: STDP verification experiment
 * 
 * @details This test:
 * 1. Selects 5 synapses for detailed monitoring
 * 2. Records initial synaptic weights
 * 3. Enables only STDP plasticity
 * 4. Creates correlated pre-post activity (LTP)
 * 5. Records final synaptic weights
 * 6. Analyzes weight changes to verify STDP
 * 
 * This test specifically verifies the Spike-Timing-Dependent
 * Plasticity (STDP) rule, where a presynaptic spike followed
 * by a postsynaptic spike strengthens the synapse.
 * 
 * @param brain Neural substrate to test
 */
void runStdpVerification(std::shared_ptr<Brain> brain);

/**
 * @brief Print program banner with Phase 2 capabilities
 * 
 * @details Shows NLM Neural Learning Machine banner
 * and provides overview of Phase 2 capabilities
 */
inline void printBanner();

#endif // NLM_MAIN_HPP