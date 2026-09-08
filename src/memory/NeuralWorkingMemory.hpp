#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

class NeuralWorkingMemory {
public:
    NeuralWorkingMemory();
    ~NeuralWorkingMemory();
    
    void initialize(Brain* brain);
    
    // Store information as neural activity pattern
    void store(const std::vector<float>& pattern, float strength = 1.0f);
    
    // Store specific neuron activation
    void storeToNeuron(NeuronId neuron, float activation);
    
    std::vector<float> retrieve() const;
    bool contains(NeuronId neuron) const;
    float getNeuronActivation(NeuronId neuron) const;
    
    // Update with maintenance dynamics and decay
    void update(TimestepDuration dt);
    
    void clear();
    
    size_t getActiveTraces() const { return memoryNeurons_.size(); }
    size_t getCapacity() const { return capacity_; }
    void setCapacity(size_t cap) { capacity_ = cap; }
    
    float getDecayRate() const { return decayRate_; }
    void setDecayRate(float rate) { decayRate_ = rate; }
    
    const std::vector<NeuronId>& getMemoryNeurons() const { return memoryNeurons_; }
    
    void strengthenMemory(float factor);
    void runCompetition();
    bool isWinning(NeuronId neuron) const;
    float getMemoryActivity() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    size_t capacity_;
    float decayRate_;
    
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryActivations_;
    std::vector<SimulationStep> memoryTimestamps_;
    std::vector<size_t> activeTraces_;
    std::vector<std::pair<NeuronId, NeuronId>> recurrentConnections_;
    std::vector<NeuronId> winners_;
};

} // namespace nlm
