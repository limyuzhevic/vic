#pragma once

#include "../performance/Performance.hpp"
#include <vector>
#include <string>

namespace nlm {

// Concrete implementation of memory pool strategy (malloc/free)
class MallocMemoryPoolStrategy : public MemoryPoolStrategy {
public:
    MallocMemoryPoolStrategy() = default;
    ~MallocMemoryPoolStrategy() override = default;
    
    void* allocate(size_t size) override {
        return std::malloc(size);
    }
    
    void deallocate(void* ptr, size_t size) override {
        std::free(ptr);
    }
    
    size_t getMemoryUsage() const override {
        return memoryUsage_;
    }
    
    size_t getAllocatedCount() const override {
        return allocatedCount_;
    }
    
    void reset() override {
        memoryUsage_ = 0;
        allocatedCount_ = 0;
    }
    
    const std::string& getName() const override {
        static const std::string name = "MallocMemoryPool";
        return name;
    }
    
    bool isEnabled() const override {
        return enabled_;
    }
    
    void setEnabled(bool enabled) {
        enabled_ = enabled;
    }
    
private:
    size_t memoryUsage_ = 0;
    size_t allocatedCount_ = 0;
    bool enabled_ = true;
};

// Concrete implementation of event queue strategy (FIFO)
class FIFOEventQueueStrategy : public EventQueueStrategy {
public:
    FIFOEventQueueStrategy() = default;
    ~FIFOEventQueueStrategy() override = default;
    
    void enqueueSpike(NeuronId neuronId, Timestamp time) override {
        events_.push_back({neuronId, time});
    }
    
    bool dequeueSpike(NeuronId& neuronId, Timestamp& time) override {
        if (events_.empty()) return false;
        
        auto event = events_.front();
        events_.pop_front();
        neuronId = event.neuronId;
        time = event.time;
        return true;
    }
    
    bool hasEvents() const override {
        return !events_.empty();
    }
    
    size_t getEventCount() const override {
        return events_.size();
    }
    
    void clear() override {
        events_.clear();
    }
    
    const std::string& getName() const override {
        static const std::string name = "FIFOEventQueue";
        return name;
    }
    
private:
    struct SpikeEvent {
        NeuronId neuronId;
        Timestamp time;
    };
    std::deque<SpikeEvent> events_;
};

// Concrete implementation of connectivity strategy (adjacency list)
class AdjacencyListConnectivityStrategy : public ConnectivityStrategy {
public:
    AdjacencyListConnectivityStrategy() = default;
    ~AdjacencyListConnectivityStrategy() override = default;
    
    SynapseId addConnection(NeuronId source, NeuronId target,
                           float weight = 1.0f, Delay delay = 1) override {
        SynapseId id = nextSynapseId_++;
        connections_[id] = {source, target, weight, delay, {}};
        adjacencyList_[source].push_back(id);
        adjacencyListIncoming_[target].push_back(id);
        return id;
    }
    
    bool removeConnection(SynapseId id) override {
        auto it = connections_.find(id);
        if (it == connections_.end()) return false;
        
        NeuronId source = it->second.sourceNeuron;
        NeuronId target = it->second.destinationNeuron;
        
        // Remove from adjacency lists
        auto& outgoing = adjacencyList_[source];
        outgoing.erase(std::remove(outgoing.begin(), outgoing.end(), id), outgoing.end());
        
        auto& incoming = adjacencyListIncoming_[target];
        incoming.erase(std::remove(incoming.begin(), incoming.end(), id), incoming.end());
        
        connections_.erase(it);
        return true;
    }
    
    std::vector<SynapseId> getOutgoingConnections(NeuronId source) const override {
        auto it = adjacencyList_.find(source);
        return (it != adjacencyList_.end()) ? it->second : std::vector<SynapseId>{};
    }
    
    std::vector<SynapseId> getIncomingConnections(NeuronId target) const override {
        auto it = adjacencyListIncoming_.find(target);
        return (it != adjacencyListIncoming_.end()) ? it->second : std::vector<SynapseId>{};
    }
    
    float getConnectionWeight(SynapseId id) const override {
        auto it = connections_.find(id);
        return (it != connections_.end()) ? it->second.weight : 0.0f;
    }
    
    void setConnectionWeight(SynapseId id, float weight) override {
        auto it = connections_.find(id);
        if (it != connections_.end()) {
            it->second.weight = weight;
        }
    }
    
    size_t getTotalConnectionCount() const override {
        return connections_.size();
    }
    
    const std::string& getName() const override {
        static const std::string name = "AdjacencyListConnectivity";
        return name;
    }
    
    bool isEnabled() const override {
        return enabled_;
    }
    
    void setEnabled(bool enabled) {
        enabled_ = enabled;
    }
    
private:
    struct SynapseData {
        NeuronId sourceNeuron;
        NeuronId destinationNeuron;
        float weight;
        Delay delay;
        PlasticityFlags plasticityFlags;
    };
    
    std::map<SynapseId, SynapseData> connections_;
    std::map<NeuronId, std::vector<SynapseId>> adjacencyList_;
    std::map<NeuronId, std::vector<SynapseId>> adjacencyListIncoming_;
    SynapseId nextSynapseId_ = 1;
    bool enabled_ = true;
};

// Concrete implementation of performance strategy factory
class DefaultPerformanceStrategyFactory : public PerformanceStrategyFactory {
public:
    DefaultPerformanceStrategyFactory() = default;
    ~DefaultPerformanceStrategyFactory() override = default;
    
    std::unique_ptr<MemoryPoolStrategy> createMemoryPoolStrategy(
        const std::string& type,
        const std::vector<float>& parameters) override {
        if (type == "malloc") {
            auto strategy = std::make_unique<MallocMemoryPoolStrategy>();
            if (!parameters.empty()) {
                strategy->setEnabled(parameters[0] != 0.0f);
            }
            return strategy;
        }
        return nullptr;
    }
    
    std::unique_ptr<EventQueueStrategy> createEventQueueStrategy(
        const std::string& type,
        const std::vector<float>& parameters) override {
        if (type == "fifo") {
            return std::make_unique<FIFOEventQueueStrategy>();
        }
        return nullptr;
    }
    
    std::unique_ptr<ConnectivityStrategy> createConnectivityStrategy(
        const std::string& type,
        const std::vector<float>& parameters) override {
        if (type == "adjacency_list") {
            auto strategy = std::make_unique<AdjacencyListConnectivityStrategy>();
            if (!parameters.empty()) {
                strategy->setEnabled(parameters[0] != 0.0f);
            }
            return strategy;
        }
        return nullptr;
    }
    
    std::vector<std::string> getAvailableMemoryPoolTypes() const override {
        return {"malloc"};
    }
    
    std::vector<std::string> getAvailableEventQueueTypes() const override {
        return {"fifo"};
    }
    
    std::vector<std::string> getAvailableConnectivityTypes() const override {
        return {"adjacency_list"};
    }
    
private:
    std::map<std::string, std::vector<float>> defaultParameters_ = {
        {"malloc", {1.0f}},
        {"fifo", {}},
        {"adjacency_list", {1.0f}}
    };
};

} // namespace nlm
