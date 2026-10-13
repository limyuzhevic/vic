#pragma once

#include "../interfaces/IComponents.hpp"
#include "../core/Types/Types.hpp"
#include <vector>
#include <unordered_map>

namespace nlm {

// WorkingMemoryImpl: Implementation of IWorkingMemorySystem
class WorkingMemoryImpl : public IWorkingMemorySystem {
public:
    WorkingMemoryImpl(size_t capacity = 100);
    ~WorkingMemoryImpl() override;
    
    // IMemorySystem interface
    void initialize(IBrainInterface* brain) override;
    void store(NeuronId neuron, float value) override;
    float retrieve(NeuronId neuron) const override;
    bool contains(NeuronId neuron) const override;
    void clear() override;
    size_t getCapacity() const override;
    size_t getCurrentSize() const override;
    void decay(float decayRate) override;
    void update(double timestep) override;
    std::vector<NeuronId> getActiveTraces() const override;
    std::vector<NeuronId> getMemoryNeurons() const override;
    
    // IWorkingMemorySystem specific methods
    void storeToNeuron(NeuronId neuron, float value) override;
    float getStoredValue(NeuronId neuron) const override;
    void setCapacity(size_t capacity) override;
    void setDecayRate(float rate) override;
    void setCompeteFactor(float factor) override;
    void strengthenMemory(float factor) override;
    void runCompetition() override;
    bool isWinning(NeuronId neuron) const override;
    float getMemoryActivity() const override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// EpisodicMemoryImpl: Implementation of IEpisodicMemorySystem
class EpisodicMemoryImpl : public IEpisodicMemorySystem {
public:
    EpisodicMemoryImpl(size_t maxEpisodes = 1000);
    ~EpisodicMemoryImpl() override;
    
    // IMemorySystem interface
    void initialize(IBrainInterface* brain) override;
    void store(NeuronId neuron, float value) override;
    float retrieve(NeuronId neuron) const override;
    bool contains(NeuronId neuron) const override;
    void clear() override;
    size_t getCapacity() const override;
    size_t getCurrentSize() const override;
    void decay(float decayRate) override;
    void update(double timestep) override;
    std::vector<NeuronId> getActiveTraces() const override;
    std::vector<NeuronId> getMemoryNeurons() const override;
    
    // IEpisodicMemorySystem specific methods
    void storeEpisode(const EpisodicMemoryItem& episode) override;
    const EpisodicMemoryItem* getEpisode(SimulationStep timestamp) const override;
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const override;
    void replayEpisode(const EpisodicMemoryItem& episode) override;
    void consolidate(float relevanceThreshold) override;
    void setMaxEpisodes(size_t max) override;
    size_t getEpisodeCount() const override;
    size_t getReplayInterval() const override;
    void setReplayInterval(size_t interval) override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// AssociativeMemoryImpl: Implementation of IAssociativeMemorySystem
class AssociativeMemoryImpl : public IAssociativeMemorySystem {
public:
    AssociativeMemoryImpl();
    ~AssociativeMemoryImpl() override;
    
    // IMemorySystem interface
    void initialize(IBrainInterface* brain) override;
    void store(NeuronId neuron, float value) override;
    float retrieve(NeuronId neuron) const override;
    bool contains(NeuronId neuron) const override;
    void clear() override;
    size_t getCapacity() const override;
    size_t getCurrentSize() const override;
    void decay(float decayRate) override;
    void update(double timestep) override;
    std::vector<NeuronId> getActiveTraces() const override;
    std::vector<NeuronId> getMemoryNeurons() const override;
    
    // IAssociativeMemorySystem specific methods
    void associate(NeuronId a, NeuronId b, float strength) override;
    float getAssociationStrength(NeuronId a, NeuronId b) const override;
    std::vector<NeuronId> getAssociations(NeuronId neuron) const override;
    void updateAssociation(NeuronId a, NeuronId b, float delta) override;
    void setMaxAssociations(size_t max) override;
    void setAssociationDecay(float decay) override;
    float getAssociationDecay() const override;
    void setStrengtheningRate(float rate) override;
    float getStrengtheningRate() const override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
