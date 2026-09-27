#pragma once

#include "OptimizedSpikeSystem.hpp"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <mutex>
#include <chrono>

namespace nlm {

class SynapticIndex {
public:
    struct NeuronConnection {
        NeuronId neuronId;
        float weight;
        Delay delay;
        bool isExcitatory;
        bool active;
        
        NeuronConnection() : neuronId(), weight(0.0f), delay(1), isExcitatory(true), active(false) {}
    };

    SynapticIndex();
    ~SynapticIndex();

    // Disable copying, enable moving
    SynapticIndex(const SynapticIndex&) = delete;
    SynapticIndex& operator=(const SynapticIndex&) = delete;
    SynapticIndex(SynapticIndex&&) noexcept;
    SynapticIndex& operator=(SynapticIndex&&) noexcept;

    // Index management
    void addConnection(const NeuronId& source, const NeuronId& destination, float weight, 
                       Delay delay, bool isExcitatory);
    void removeConnection(const NeuronId& source, const NeuronId& destination);
    void clear();

    // Optimized spike delivery
    void deliverSpike(const NeuronId& source, float sourceTime, 
                      SimulationStep currentStep, Timestamp currentTime);
    void batchDeliverSpikes(const std::vector<NeuronId>& sources, 
                           SimulationStep currentStep, Timestamp currentTime);

    // Lookup optimizations
    const std::vector<NeuronConnection>* getConnections(const NeuronId& neuron) const;
    size_t getConnectionCount(const NeuronId& neuron) const;
    bool hasConnections(const NeuronId& neuron) const;

    // Statistics
    size_t getTotalConnections() const;
    size_t getActiveConnections() const;
    std::string getStatistics() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
