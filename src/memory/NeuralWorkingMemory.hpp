#pragma once

#include "../core/Types/Types.hpp"
#include "../core/ErrorHandling/ErrorHandling.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

namespace nlm {

class NeuralWorkingMemory : public MemorySystem {
public:
    NeuralWorkingMemory();
    ~NeuralWorkingMemory();

    // Initialize with brain reference
    void initialize(Brain* brain) override;

    // Store information in working memory
    // The information is encoded as neural activity pattern
    void store(const std::vector<float>& pattern, float strength = 1.0f);

    // Store specific neuron activation
    void storeToNeuron(NeuronId neuron, float activation);

    // Retrieve current working memory content as activity levels
    std::vector<float> retrieve() const;

    // Check if specific neuron is part of working memory
    bool contains(NeuronId neuron) const;

    // Get activation level of a specific neuron in working memory
    float getNeuronActivation(NeuronId neuron) const;

    // Update working memory (maintenance and decay)
    void update(TimestepDuration dt) override;

    // Clear working memory
    void clear() override;

    // Get number of active memory traces
    size_t getActiveTraces() const { return activeTraces_.size(); }

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

    // Memory statistics
    void updateStats() override {
        memoryUsage_ = calculateMemoryUsage();
    }
    
    size_t getMemoryUsage() const override { return memoryUsage_; }
    size_t getMaxMemoryUsage() const override { return maxMemoryUsage_; }
    float getUsagePercentage() const override {
        if (maxMemoryUsage_ == 0) return 0.0f;
        return static_cast<float>(memoryUsage_) / static_cast<float>(maxMemoryUsage_) * 100.0f;
    }

private:
    // Create recurrent connection for maintenance
    void createRecurrentConnection(NeuronId from, NeuronId to, float strength);

    // Update recurrent connections for maintenance
    void updateRecurrentConnections();

    // Decay weak memory traces
    void decayWeakTraces();

    // Calculate memory usage in bytes
    size_t calculateMemoryUsage() const {
        size_t usage = 0;
        usage += memoryNeurons_.size() * sizeof(NeuronId);
        usage += memoryActivations_.size() * sizeof(float);
        usage += memoryTimestamps_.size() * sizeof(SimulationStep);
        usage += activeTraces_.size() * sizeof(size_t);
        usage += recurrentConnections_.size() * sizeof(std::pair<NeuronId, NeuronId>);
        usage += winners_.size() * sizeof(NeuronId);
        return usage;
    }

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
    
    // Memory statistics
    size_t memoryUsage_;
    size_t maxMemoryUsage_;
};

} // namespace nlm
