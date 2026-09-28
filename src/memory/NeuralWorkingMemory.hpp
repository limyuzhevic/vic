#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace nlm {

// NeuralWorkingMemory: Implements working memory using neural dynamics
// NOT a simple storage vector - uses persistent activity and recurrent connections
//
// Key mechanisms:
// - Persistent neural activity (reverberating activity)
// - Recurrent synaptic connections for maintaining information
// - Activity-dependent short-term plasticity
// - Neural competition for selective retention

class NeuralWorkingMemory {
public:
    /**
     * @brief Construct a new NeuralWorkingMemory object
     * Initializes memory with default capacity (100) and decay rate (0.01)
     */
    NeuralWorkingMemory();
    
    /**
     * @brief Destroy the NeuralWorkingMemory object
     * Cleans up any allocated memory and resources
     */
    ~NeuralWorkingMemory();
    
    /**
     * @brief Initialize the working memory with a brain reference
     * @param brain Pointer to the brain object that contains the neural population
     * The working memory will use this brain to access neurons and inject currents
     */
    void initialize(Brain* brain);
    
    /**
     * @brief Store information pattern in working memory
     * Encodes the input pattern as persistent neural activity using the brain's neurons
     * @param pattern Vector of activation values representing the information to store
     * @param strength Multiplier for the initial activation strength (default: 1.0f)
     * Higher strength values result in stronger persistent activity
     */
    void store(const std::vector<float>& pattern, float strength = 1.0f);
    
    /**
     * @brief Store specific neuron activation in working memory
     * Directly activates a specific neuron rather than encoding a pattern
     * @param neuron ID of the neuron to activate
     * @param activation Activation level to set (0.0 to 1.0 typical range)
     */
    void storeToNeuron(NeuronId neuron, float activation);
    
    /**
     * @brief Retrieve current working memory content as activation levels
     * @return std::vector<float> Vector of activation values for each stored item
     * The values are in the range 0.0 to 1.0, with higher values indicating stronger memory traces
     */
    std::vector<float> retrieve() const;
    
    /**
     * @brief Check if a specific neuron is part of the working memory
     * @param neuron ID of the neuron to check
     * @return bool true if neuron is in working memory, false otherwise
     */
    bool contains(NeuronId neuron) const;
    
    /**
     * @brief Get activation level of a specific neuron in working memory
     * @param neuron ID of the neuron to query
     * @return float Activation level of the neuron (0.0 if not in memory)
     */
    float getNeuronActivation(NeuronId neuron) const;
    
    /**
     * @brief Update working memory for one timestep
     * Maintains persistent activity through recurrent connections and applies decay
     * @param dt Time step duration in simulation steps
     */
    void update(TimestepDuration dt);
    
    /**
     * @brief Clear all working memory content
     * Resets memory to empty state, removing all stored patterns and connections
     */
    void clear();
    
    /**
     * @brief Get the number of active memory traces
     * @return size_t Count of memory traces above activity threshold
     */
    size_t getActiveTraces() const { return activeTraces_.size(); }
    
    /**
     * @brief Get the memory capacity (maximum number of items)
     * @return size_t Maximum number of memory items that can be stored
     */
    size_t getCapacity() const { return capacity_; }
    
    /**
     * @brief Set the memory capacity
     * @param cap Maximum number of memory items (must be > 0)
     */
    void setCapacity(size_t cap) { capacity_ = cap; }
    
    /**
     * @brief Get the decay rate for memory traces
     * @return float Decay rate per timestep (typically 0.01 for 1% decay per step)
     */
    float getDecayRate() const { return decayRate_; }
    
    /**
     * @brief Set the decay rate for memory traces
     * @param rate Decay rate per timestep (typically 0.01 for 1% decay per step)
     */
    void setDecayRate(float rate) { decayRate_ = rate; }
    
    /**
     * @brief Get neurons currently in working memory
     * @return const std::vector<NeuronId>& Vector of neuron IDs
     */
    const std::vector<NeuronId>& getMemoryNeurons() const { return memoryNeurons_; }
    
    /**
     * @brief Strengthen working memory representation (for rehearsal)
     * Increases activation of all memory traces to reinforce the memory
     * @param factor Multiplier for activation levels (typically > 1.0 for strengthening)
     */
    void strengthenMemory(float factor);
    
    /**
     * @brief Run competition between memory traces
     * Applies lateral inhibition to suppress weaker traces and amplify winners
     * This implements neural competition for selective attention/retention
     */
    void runCompetition();
    
    /**
     * @brief Check if this neuron is part of the winning population
     * @param neuron ID of the neuron to check
     * @return bool true if neuron is a winner from recent competition
     */
    bool isWinning(NeuronId neuron) const;
    
    /**
     * @brief Get overall memory activity level
     * @return float Average activation across all memory traces
     * Useful for monitoring overall memory load
     */
    float getMemoryActivity() const;
    
private:
    // Create recurrent connection for maintenance
    void createRecurrentConnection(NeuronId from, NeuronId to, float strength);
    
    // Update recurrent connections for maintenance
    void updateRecurrentConnections();
    
    // Decay weak memory traces
    void decayWeakTraces();
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    size_t capacity_;
    float decayRate_;
    
    // Memory content
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryActivations_;
    std::vector<SimulationStep> memoryTimestamps_;
    
    // Active memory traces
    std::vector<size_t> activeTraces_;
    
    // Recurrent connections for maintenance
    std::vector<std::pair<NeuronId, NeuronId>> recurrentConnections_;
    
    // Winner neurons (for competition)
    std::vector<NeuronId> winners_;
};

// AttentionalSelection: Neural attention through competition
// NOT Transformer attention - uses lateral inhibition and gain modulation
//
// Mechanisms:
// - Competition between neural populations
// - Salience-based selection
// - Recurrent amplification of winners
// - Inhibitory surround suppression

class AttentionalSelection {
public:
    /**
     * @brief Construct a new AttentionalSelection object
     */
    AttentionalSelection();
    
    /**
     * @brief Destroy the AttentionalSelection object
     */
    ~AttentionalSelection();
    
    /**
     * @brief Initialize with brain reference
     * @param brain Pointer to the brain object
     */
    void initialize(Brain* brain);
    
    /**
     * @brief Process competing sensory inputs and select winners
     * Implements neural competition using lateral inhibition
     * @param competitors Vector of neuron IDs that are competing for attention
     * @param globalInhibition Global inhibition strength (0.0 to 1.0, default: 0.5f)
     * @return std::vector<NeuronId> IDs of winning neural populations
     */
    std::vector<NeuronId> processCompetition(const std::vector<NeuronId>& competitors,
                                              float globalInhibition = 0.5f);
    
    /**
     * @brief Apply attention to a specific region (focus processing there)
     * @param region Region ID to attend to
     */
    void focusOnRegion(RegionId region);
    
    /**
     * @brief Release attention (allow all regions equal processing)
     */
    void releaseAttention();
    
    /**
     * @brief Get current attention focus
     * @return std::vector<RegionId> Vector of attended region IDs
     */
    std::vector<RegionId> getAttendedRegions() const;
    
    /**
     * @brief Set attention parameters
     * @param strength Inhibition strength for attentional competition
     */
    void setInhibitionStrength(float strength);
    
    /**
     * @brief Set attention parameters
     * @param strength Excitation strength for attended regions
     */
    void setExcitationStrength(float strength);
    
    /**
     * @brief Set attention parameters
     * @param threshold Activity threshold for winning populations
     */
    void setCompetitionThreshold(float threshold);
    
    /**
     * @brief Get inhibition level for a neuron
     * @param neuron Neuron ID to query
     * @return float Inhibition level (0.0 for winners, > 0 for losers)
     */
    float getInhibitionFor(NeuronId neuron) const;
    
    /**
     * @brief Get excitation level for a neuron
     * @param neuron Neuron ID to query
     * @return float Excitation level (0.0 for losers, > 0 for winners)
     */
    float getExcitationFor(NeuronId neuron) const;
    
    /**
     * @brief Update attention dynamics
     * Applies decay to salience and bias values over time
     * @param dt Time step duration
     */
    void update(TimestepDuration dt);
    
    /**
     * @brief Check if this neuron is currently attended
     * @param neuron Neuron ID to check
     * @return bool true if neuron is currently being attended
     */
    bool isAttended(NeuronId neuron) const;
    
    /**
     * @brief Get attention winners
     * @return const std::vector<NeuronId>& Vector of winning neuron IDs
     */
    const std::vector<NeuronId>& getWinners() const { return winners_; }
    
    /**
     * @brief Top-down attention (goal-directed)
     * Applies bias to neurons based on goals/intentions
     * @param neuron Neuron ID to bias
     * @param biasStrength Strength of the bias (positive = attract attention)
     */
    void applyTopDownBias(NeuronId neuron, float biasStrength);
    
    /**
     * @brief Bottom-up attention (stimulus-driven)
     * Applies salience to neurons based on stimulus properties
     * @param neuron Neuron ID to bias
     * @param salienceStrength Strength of the salience (positive = attract attention)
     */
    void applyBottomUpSalience(NeuronId neuron, float salienceStrength);
    
    /**
     * @brief Reset attention state
     * Clears all attention biases, winners, and attended regions
     */
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    float inhibitionStrength_;
    float excitationStrength_;
    float competitionThreshold_;
    
    std::vector<RegionId> attendedRegions_;
    std::vector<NeuronId> winners_;
    std::vector<float> neuronSalience_;
    
    // Biases/salience stored by NeuronId for correct lookup in processCompetition
    std::unordered_map<uint64_t, float> topDownBias_;
    std::unordered_map<uint64_t, float> bottomUpSalience_;
};

} // namespace nlm