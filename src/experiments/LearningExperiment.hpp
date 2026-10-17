// NLM (熙然) - Neural Learning Machine
// Learning Experiment class
// Implements learning measurement and analysis for neural systems

#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <vector>
#include <numeric>
#include <cmath>

namespace nlm {

/**
 * @brief LearningExperiment class for measuring and analyzing neural learning
 * 
 * This class provides a structured way to measure synaptic changes and neural activity
 * across simulation runs to quantify learning effects. It records initial and final
 * neural states, computes statistical analysis of synaptic weights, and determines
 * whether measurable learning has occurred.
 * 
 * @note This class uses the existing Brain interface to access neural data
 *       and the NLM logging system for output.
 */
class LearningExperiment {
public:
    /**
     * @brief Constructor for the learning experiment
     * 
     * @param brain Shared pointer to the brain being analyzed
     * @param seed Random seed for reproducible results
     */
    explicit LearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed);
    
    /**
     * @brief Default destructor
     */
    ~LearningExperiment() = default;
    
    // Disable copying
    LearningExperiment(const LearningExperiment&) = delete;
    LearningExperiment& operator=(const LearningExperiment&) = delete;
    
    // Enable moving
    LearningExperiment(LearningExperiment&&) noexcept = default;
    LearningExperiment& operator=(LearningExperiment&&) noexcept = default;
    
    /**
     * @brief Record the initial state of the neural system
     * 
     * Captures pre-simulation data including:
     * - Total synapse count
     * - Initial synaptic weights from the first brain region
     * - Basic statistics about initial weights
     */
    void recordInitialState();
    
    /**
     * @brief Record the final state after simulation
     * 
     * Captures post-simulation data including:
     * - Final synaptic weights from the first brain region
     * - Most active neurons during simulation
     * - Final spike statistics
     */
    void recordFinalState();
    
    /**
     * @brief Compute comprehensive learning statistics
     * 
     * Performs detailed analysis of synaptic changes and neural activity:
     * - Weight statistics (initial/final means, delta)
     * - Synaptic changes (strengthened/weakened/unchanged)
     * - Spike activity analysis
     * - Learning detection and summary
     */
    void computeStatistics();
    
    // Getters for experiment data
    
    /**
     * @brief Get the initial synapse count
     * @return Number of synapses at start of experiment
     */
    size_t getInitialSynapseCount() const { return initialSynapseCount; }
    
    /**
     * @brief Get initial synaptic weights
     * @return Vector of initial synaptic weights
     */
    const std::vector<float>& getInitialWeights() const { return initialWeights; }
    
    /**
     * @brief Get final synaptic weights
     * @return Vector of final synaptic weights
     */
    const std::vector<float>& getFinalWeights() const { return finalWeights; }
    
    /**
     * @brief Get most active neurons from simulation
     * @return Vector of most active neuron IDs
     */
    const std::vector<NeuronId>& getMostActiveNeurons() const { return mostActiveNeurons; }
    
private:
    // Experiment data
    std::shared_ptr<Brain> brain;           ///< Shared brain reference
    uint64_t seed;                          ///< Experiment seed
    size_t initialSynapseCount;             ///< Synapse count at start
    std::vector<float> initialWeights;      ///< Synaptic weights at start
    std::vector<float> finalWeights;        ///< Synaptic weights at end
    std::vector<NeuronId> mostActiveNeurons; ///< Most active neurons during simulation
    
    // Helper methods
    
    /**
     * @brief Calculate mean of a vector of floats
     * @param values Vector of values to average
     * @return Mean value
     */
    float calculateMean(const std::vector<float>& values) const;
    
    /**
     * @brief Count synapses that changed significantly
     * @param initial Initial weights
     * @param final Final weights
     * @return Pair of (strengthened, weakened) counts
     */
    std::pair<size_t, size_t> countSignificantChanges(
        const std::vector<float>& initial,
        const std::vector<float>& final) const;
    
    /**
     * @brief Determine if learning has occurred
     * @param initialMean Initial mean weight
     * @param finalMean Final mean weight
     * @param strengthened Count of strengthened synapses
     * @param weakened Count of weakened synapses
     * @return True if learning occurred
     */
    bool hasLearningOccurred(float initialMean, float finalMean,
                            size_t strengthened, size_t weakened) const;
};

} // namespace nlm
