#pragma once

#include "../core/Types/Types.hpp"
#include "IMemorySystem.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Brain;
class BrainMemoryAdapter;

// NeuralWorkingMemory: Implements working memory using neural dynamics
// NOT a simple storage vector - uses persistent activity and recurrent connections
//
// Key mechanisms:
// - Persistent neural activity (reverberating activity)
// - Recurrent synaptic connections for maintaining information
// - Activity-dependent short-term plasticity
// - Neural competition for selective retention
//
// Now implements IWorkingMemory interface for better integration

class NeuralWorkingMemory : public IWorkingMemory {
public:
    NeuralWorkingMemory();
    ~NeuralWorkingMemory() override;

    // IMemorySystem interface implementation
    void initialize(Brain* owner) override;
    void update(TimestepDuration dt) override;
    void clear() override;
    Brain* getOwner() const override { return brain_; }
    bool isInitialized() const override { return brain_ != nullptr; }

    // IWorkingMemory interface implementation
    void store(const std::vector<float>& pattern, float strength) override;
    void storeToNeuron(NeuronId neuron, float activation) override;
    std::vector<float> retrieve() const override;
    bool contains(NeuronId neuron) const override;
    float getNeuronActivation(NeuronId neuron) const override;
    size_t getCapacity() const override { return capacity_; }
    void setCapacity(size_t cap) override { capacity_ = cap; }
    float getDecayRate() const override { return decayRate_; }
    void setDecayRate(float rate) override { decayRate_ = rate; }
    size_t getActiveTraces() const override { return activeTraces_.size(); }
    void runCompetition() override;
    bool isWinning(NeuronId neuron) const override;
    float getMemoryActivity() const override;
    void strengthenMemory(float factor) override;
    const std::vector<NeuronId>& getMemoryNeurons() const override { return memoryNeurons_; }

    // Legacy methods (for compatibility)
    void initialize(Brain* brain) { initialize(brain); }
    void update(TimestepDuration dt) { update(dt); }
    void clear() { clear(); }
    size_t getActiveTraces() const { return getActiveTraces(); }
    bool isWinning(NeuronId neuron) const { return isWinning(neuron); }
    float getMemoryActivity() const { return getMemoryActivity(); }
    void strengthenMemory(float factor) { strengthenMemory(factor); }
    const std::vector<NeuronId>& getMemoryNeurons() const { return getMemoryNeurons(); }

    // Set adapter for clean brain interaction
    void setAdapter(std::shared_ptr<BrainMemoryAdapter> adapter) { adapter_ = adapter; }

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
    std::shared_ptr<BrainMemoryAdapter> adapter_;
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

class AttentionalSelection : public virtual IMemorySystem {
public:
    AttentionalSelection();
    ~AttentionalSelection() override;

    // IMemorySystem interface implementation
    void initialize(Brain* owner) override;
    void update(TimestepDuration dt) override;
    void clear() override;
    Brain* getOwner() const override { return brain_; }
    bool isInitialized() const override { return brain_ != nullptr; }

    // Legacy methods (for compatibility)
    void initialize(Brain* brain) { initialize(brain); }
    void update(TimestepDuration dt) { update(dt); }
    void clear() { clear(); }

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