#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace nlm {

/**
 * @brief NeuralWorkingMemory: Implements working memory using neural dynamics
 * 
 * NOT a simple storage vector - uses persistent activity and recurrent connections.
 * 
 * Working memory is a cognitive system that temporarily stores and manipulates
 * information for cognitive tasks. Unlike simple storage, neural working memory
 * maintains information through persistent neural activity patterns.
 * 
 * Key mechanisms:
 * - Persistent neural activity (reverberating activity)
 * - Recurrent synaptic connections for maintaining information
 * - Activity-dependent short-term plasticity
 * - Neural competition for selective retention
 * 
 * @note This implementation represents Phase 6 of the NLM architecture,
 * providing a biologically plausible model of working memory.
 */
class NeuralWorkingMemory {
public:
    /**
     * @brief Default constructor
     */
    NeuralWorkingMemory();
    
    /**
     * @brief Destructor
     */
    ~NeuralWorkingMemory();

    /**
     * @brief Initialize with brain reference
     * 
     * @param brain Pointer to the brain instance that this memory system belongs to
     */
    void initialize(Brain* brain);

    /**
     * @brief Store information in working memory
     * 
     * Encodes information as neural activity pattern that can be maintained
     * through recurrent connections and persistent activity.
     * 
     * @param pattern Neural activity pattern to store (values represent activation levels)
     * @param strength Initial strength/stability of the memory trace (default: 1.0f)
     */
    void store(const std::vector<float>& pattern, float strength = 1.0f);

    /**
     * @brief Store specific neuron activation
     * 
     * Directly encodes activation level for a specific neuron in working memory.
     * 
     * @param neuron ID of the neuron to store activation for
     * @param activation Activation level to store (0.0 to 1.0)
     */
    void storeToNeuron(NeuronId neuron, float activation);

    /**
     * @brief Retrieve current working memory content as activity levels
     * 
     * @return Vector of activation levels representing the current working memory content
     */
    std::vector<float> retrieve() const;

    /**
     * @brief Check if specific neuron is part of working memory
     * 
     * @param neuron Neuron ID to check
     * @return true if neuron is part of working memory, false otherwise
     */
    bool contains(NeuronId neuron) const;

    /**
     * @brief Get activation level of a specific neuron in working memory
     * 
     * @param neuron Neuron ID to query
     * @return Current activation level of the neuron
     */
    float getNeuronActivation(NeuronId neuron) const;

    /**
     * @brief Update working memory (maintenance and decay)
     * 
     * Maintains memory traces through recurrent connections and applies decay
     * to prevent indefinite persistence.
     * 
     * @param dt Time step duration for the update
     */
    void update(TimestepDuration dt);

    /**
     * @brief Clear working memory
     * 
     * Removes all memory traces and resets the working memory system.
     */
    void clear();

    /**
     * @brief Get number of active memory traces
     * 
     * @return Number of active memory traces currently stored
     */
    size_t getActiveTraces() const { return activeTraces_.size(); }

    /**
     * @brief Get memory capacity
     * 
     * @return Maximum number of memory traces that can be stored
     */
    size_t getCapacity() const { return capacity_; }
    
    /**
     * @brief Set memory capacity
     * 
     * @param cap New capacity in number of memory traces
     */
    void setCapacity(size_t cap) { capacity_ = cap; }

    /**
     * @brief Get decay rate for memory traces
     * 
     * @return Decay rate (0.0 to 1.0, where higher values decay faster)
     */
    float getDecayRate() const { return decayRate_; }
    
    /**
     * @brief Set decay rate for memory traces
     * 
     * @param rate New decay rate (0.0 to 1.0)
     */
    void setDecayRate(float rate) { decayRate_ = rate; }

    /**
     * @brief Get neurons currently in working memory
     * 
     * @return Vector of neuron IDs that are part of working memory
     */
    const std::vector<NeuronId>& getMemoryNeurons() const { return memoryNeurons_; }

    /**
     * @brief Strengthen working memory representation (for rehearsal)
     * 
     * Increases the activation of all memory traces by the given factor,
     * useful for rehearsal and consolidation processes.
     * 
     * @param factor Multiplicative factor to strengthen activations (typically > 1.0)
     */
    void strengthenMemory(float factor);

    /**
     * @brief Competition between memory traces
     * 
     * Implements competitive dynamics where neurons with higher activation
     * inhibit those with lower activation, leading to selective retention.
     */
    void runCompetition();

    /**
     * @brief Check if neuron is part of winning population
     * 
     * @param neuron Neuron ID to check
     * @return true if neuron is among the winning population, false otherwise
     */
    bool isWinning(NeuronId neuron) const;

    /**
     * @brief Get overall memory activity level
     * 
     * @return Average activation level across all memory traces
     */
    float getMemoryActivity() const;

private:
    /**
     * @brief Create recurrent connection for maintenance
     * 
     * Establishes a persistent connection between neurons to maintain memory traces
     * through recurrent activity.
     * 
     * @param from Source neuron ID
     * @param to Target neuron ID
     * @param strength Connection strength
     */
    void createRecurrentConnection(NeuronId from, NeuronId to, float strength);

    /**
     * @brief Update recurrent connections for maintenance
     * 
     * Applies maintenance currents through recurrent connections to sustain
     * persistent activity in working memory.
     */
    void updateRecurrentConnections();

    /**
     * @brief Decay weak memory traces
     * 
     * Gradually reduces activation of weak memory traces below a threshold,
     * implementing forgetting processes.
     */
    void decayWeakTraces();

    struct Impl;
    std::unique_ptr<Impl> pImpl;  ///< Private implementation (PIMPL pattern)

    Brain* brain_;                   ///< Reference to parent brain
    size_t capacity_;                ///< Maximum number of memory traces
    float decayRate_;                ///< Decay rate for memory traces (0.0 to 1.0)
    
    // Memory content
    std::vector<NeuronId> memoryNeurons_;        ///< Neurons currently in working memory
    std::vector<float> memoryActivations_;      ///< Activation levels of memory traces
    std::vector<SimulationStep> memoryTimestamps_; ///< Ages of memory traces
    
    // Active memory traces
    std::vector<size_t> activeTraces_;          ///< Indices of active memory traces
    
    // Recurrent connections for maintenance
    std::vector<std::pair<NeuronId, NeuronId>> recurrentConnections_;  ///< Connections for maintenance
    
    // Winner neurons (for competition)
    std::vector<NeuronId> winners_;             ///< Neurons that won competition
};

/**
 * @brief AttentionalSelection: Neural attention through competition
 * 
 * NOT Transformer attention - uses lateral inhibition and gain modulation.
 * 
 * Attentional selection is a fundamental cognitive process that allows
 * organisms to focus processing resources on relevant information while
 * suppressing irrelevant input. This implementation uses neural mechanisms
 * similar to those found in biological systems.
 * 
 * Key mechanisms:
 * - Competition between neural populations
 * - Salience-based selection
 * - Recurrent amplification of winners
 * - Inhibitory surround suppression
 */
class AttentionalSelection {
public:
    /**
     * @brief Default constructor
     */
    AttentionalSelection();
    
    /**
     * @brief Destructor
     */
    ~AttentionalSelection();

    /**
     * @brief Initialize with brain reference
     * 
     * @param brain Pointer to the brain instance
     */
    void initialize(Brain* brain);

    /**
     * @brief Process competing sensory inputs and select winners
     * 
     * Implements competitive selection among neural populations based on
     * their current activity levels and attentional signals.
     * 
     * @param competitors Vector of competing neuron IDs
     * @param globalInhibition Global inhibition strength (0.0 to 1.0)
     * @return Vector of winning neuron IDs
     */
    std::vector<NeuronId> processCompetition(const std::vector<NeuronId>& competitors,
                                              float globalInhibition = 0.5f);

    /**
     * @brief Apply attention to a specific region (focus processing there)
     * 
     * @param region Region ID to attend to
     */
    void focusOnRegion(RegionId region);

    /**
     * @brief Release attention (allow all regions equal processing)
     */
    void releaseAttention();

    /**
     * @brief Get current attention focus
     * 
     * @return Vector of region IDs currently being attended to
     */
    std::vector<RegionId> getAttendedRegions() const;

    /**
     * @brief Set attention parameters
     * 
     * @param strength Inhibition strength
     * @param excitation Excitation strength
     * @param threshold Competition threshold
     */
    void setInhibitionStrength(float strength);
    void setExcitationStrength(float strength);
    void setCompetitionThreshold(float threshold);

    /**
     * @brief Get inhibition level for a neuron
     * 
     * @param neuron Neuron ID to query
     * @return Inhibition level for the neuron
     */
    float getInhibitionFor(NeuronId neuron) const;

    /**
     * @brief Get excitation level for a neuron
     * 
     * @param neuron Neuron ID to query
     * @return Excitation level for the neuron
     */
    float getExcitationFor(NeuronId neuron) const;

    /**
     * @brief Update attention dynamics
     * 
     * Updates attentional states based on current neural activity and
     * decays attentional biases over time.
     * 
     * @param dt Time step duration
     */
    void update(TimestepDuration dt);

    /**
     * @brief Check if neuron is currently attended
     * 
     * @param neuron Neuron ID to check
     * @return true if neuron is currently attended, false otherwise
     */
    bool isAttended(NeuronId neuron) const;

    /**
     * @brief Get attention winners
     * 
     * @return Vector of neuron IDs that won attention competition
     */
    const std::vector<NeuronId>& getWinners() const { return winners_; }

    /**
     * @brief Apply top-down attention (goal-directed)
     * 
     * @param neuron Neuron ID to apply bias to
     * @param biasStrength Strength of top-down bias
     */
    void applyTopDownBias(NeuronId neuron, float biasStrength);

    /**
     * @brief Apply bottom-up attention (stimulus-driven)
     * 
     * @param neuron Neuron ID to apply salience to
     * @param salienceStrength Strength of bottom-up salience
     */
    void applyBottomUpSalience(NeuronId neuron, float salienceStrength);

    /**
     * @brief Reset attention
     * 
     * Clears all attentional states and resets the attention system.
     */
    void reset();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;  ///< Private implementation (PIMPL pattern)

    Brain* brain_;                   ///< Reference to parent brain
    float inhibitionStrength_;       ///< Strength of inhibitory competition
    float excitationStrength_;       ///< Strength of excitatory amplification
    float competitionThreshold_;     ///< Threshold for winning competition
    
    std::vector<RegionId> attendedRegions_;  ///< Regions currently being attended to
    std::vector<NeuronId> winners_;          ///< Neurons that won attention competition
    std::vector<float> neuronSalience_;      ///< Bottom-up salience values for neurons
    
    // Biases/salience stored by NeuronId for correct lookup in processCompetition
    std::unordered_map<uint64_t, float> topDownBias_;   ///< Top-down attentional biases
    std::unordered_map<uint64_t, float> bottomUpSalience_; ///< Bottom-up salience values
};

} // namespace nlm