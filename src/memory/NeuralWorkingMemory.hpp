#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <queue>

namespace nlm;

// Working memory trace for biologically realistic storage
struct WorkingMemoryTrace {
    NeuronId neuralId;                    // Primary encoding neuron
    std::vector<NeuronId> population;      // Encoding neural population
    std::vector<float> populationActivity; // Activity levels of population neurons
    float persistentActivity;              // Maintained persistent firing rate
    SimulationStep age;                    // Trace age in simulation steps
    float noveltyScore;                     // Recency/familiarity measure
    SimulationStep lastReactivation;        // Last time trace was reinforced
    float consolidationWeight;              // Weight for trace integration
    float traceStrength;                    // Current trace strength (0-1)
    float synapticResource;                 // Synaptic resources allocated to this trace
    bool isConsolidated;                    // Whether trace is fully consolidated
    std::vector<float> spikeHistory;        // Recent spike patterns for pattern matching
    
    WorkingMemoryTrace();
};

class NeuralWorkingMemory {
public:
    NeuralWorkingMemory();
    ~NeuralWorkingMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Store information in working memory
    // The information is encoded as neural activity pattern
    void store(const std::vector<float>& pattern, float strength = 1.0f);

    // Store specific neuron activation with biological dynamics
    void storeToNeuron(NeuronId neuron, float activation);

    // Retrieve current working memory content as activity levels
    std::vector<float> retrieve() const;

    // Check if specific neuron is part of working memory
    bool contains(NeuronId neuron) const;

    // Get activation level of a specific neuron in working memory
    float getNeuronActivation(NeuronId neuron) const;

    // Update working memory (maintenance and decay)
    void update(TimestepDuration dt);

    // Clear working memory
    void clear();

    // Get number of active memory traces
    size_t getActiveTraces() const { return pImpl->traces.size(); }

    // Get capacity
    size_t getCapacity() const { return capacity_; }
    void setCapacity(size_t cap) { capacity_ = cap; }

    // Decay rate for memory traces
    float getDecayRate() const { return decayRate_; }
    void setDecayRate(float rate) { decayRate_ = rate; }

    // Get neurons currently in working memory
    const std::vector<NeuronId>& getMemoryNeurons() const { return memoryNeurons_; }

    // Strengthen working memory representation (for rehearsal)
    void strengthenMemory(float factor);

    // Competition between memory traces
    void runCompetition();

    // Is this neuron part of the winning population?
    bool isWinning(NeuronId neuron) const;

    // Get overall memory activity level
    float getMemoryActivity() const;

    // Get current memory load and capacity statistics
    float getMemoryLoad() const { return pImpl->currentLoad; }
    float getMaximumCapacity() const { return pImpl->maximumCapacity; }

    // Novelty detection and forgetting mechanisms
    std::vector<NeuronId> getNovelNeurons() const;
    void forgetWeakTraces();

    // Trace consolidation and decay mechanisms
    void consolidateTraces();
    void decayTraces(TimestepDuration dt);

    // Neural resource management
    void allocateNeuralResources(size_t count);
    void releaseNeuralResources(const std::vector<NeuronId>& neurons);

private:
    // Create recurrent connections for maintenance
    void createRecurrentConnections(const std::vector<NeuronId>& neurons, float strength);
    void createRecurrentConnection(NeuronId from, NeuronId to, float strength);

    // Update recurrent connections for maintenance
    void updateRecurrentConnections();

    // Trace management and dynamics
    void updateTraceStatistics(WorkingMemoryTrace& trace);
    void reactivateTrace(WorkingMemoryTrace& trace);
    void selectEncodingPopulation(const WorkingMemoryTrace& trace);

    // Competition and attention mechanisms
    void runWinnerTakeAllCompetition();
    void applyLateralInhibition();
    void applyGlobalInhibition();

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    size_t capacity_;
    float decayRate_;
    
    // Legacy memory content (for backward compatibility)
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryActivations_;
    std::vector<SimulationStep> memoryTimestamps_;
    
    // Active memory traces (new biologically realistic system)
    std::vector<size_t> activeTraces_;  // Indices into pImpl->traces
    
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
    AttentionalSelection();
    ~AttentionalSelection();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Process competing sensory inputs and select winners
    // Returns IDs of winning neural populations
    std::vector<NeuronId> processCompetition(const std::vector<NeuronId>& competitors,
                                              float globalInhibition = 0.5f);

    // Apply attention to a specific region (focus processing there)
    void focusOnRegion(RegionId region);

    // Release attention (allow all regions equal processing)
    void releaseAttention();

    // Get current attention focus
    std::vector<RegionId> getAttendedRegions() const;

    // Set attention parameters
    void setInhibitionStrength(float strength);
    void setExcitationStrength(float strength);
    void setCompetitionThreshold(float threshold);

    // Get inhibition level for a neuron
    float getInhibitionFor(NeuronId neuron) const;

    // Get excitation level for a neuron
    float getExcitationFor(NeuronId neuron) const;

    // Update attention dynamics
    void update(TimestepDuration dt);

    // Is this neuron currently attended?
    bool isAttended(NeuronId neuron) const;

    // Get attention winners
    const std::vector<NeuronId>& getWinners() const { return winners_; }

    // Top-down attention (goal-directed)
    void applyTopDownBias(NeuronId neuron, float biasStrength);

    // Bottom-up attention (stimulus-driven)
    void applyBottomUpSalience(NeuronId neuron, float salienceStrength);

    // Reset attention
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