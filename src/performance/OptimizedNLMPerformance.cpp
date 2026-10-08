// Performance Optimizations for NLM
// This file contains performance improvements and data structure optimizations

#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cmath>
#include <memory>
#include <chrono>

namespace nlm {

// Performance metrics collection
struct PerformanceMetrics {
    double totalSimulationTime;
    size_t totalNeuronsProcessed;
    size_t totalSpikesProcessed;
    size_t memoryAllocations;
    size_t memoryDeallocations;
    double cpuUsage;
    
    PerformanceMetrics() 
        : totalSimulationTime(0.0), totalNeuronsProcessed(0), totalSpikesProcessed(0),
          memoryAllocations(0), memoryDeallocations(0), cpuUsage(0.0) {}
};

// Optimized neuron lookup table
class NeuronLookupTable {
private:
    std::unordered_map<NeuronId, Neuron*> neuronMap;
    std::vector<NeuronId> idVector;
    
public:
    void insert(NeuronId id, Neuron* neuron) {
        neuronMap[id] = neuron;
        idVector.push_back(id);
    }
    
    Neuron* find(NeuronId id) {
        auto it = neuronMap.find(id);
        return (it != neuronMap.end()) ? it->second : nullptr;
    }
    
    const Neuron* find(NeuronId id) const {
        auto it = neuronMap.find(id);
        return (it != neuronMap.end()) ? it->second : nullptr;
    }
    
    size_t size() const { return neuronMap.size(); }
    void clear() {
        neuronMap.clear();
        idVector.clear();
    }
    
    // Batch operations for performance
    std::vector<Neuron*> findBatch(const std::vector<NeuronId>& ids) {
        std::vector<Neuron*> results;
        results.reserve(ids.size());
        
        for (const auto& id : ids) {
            auto it = neuronMap.find(id);
            if (it != neuronMap.end()) {
                results.push_back(it->second);
            }
        }
        
        return results;
    }
    
    // Iterator support for range-based operations
    auto begin() { return neuronMap.begin(); }
    auto end() { return neuronMap.end(); }
    auto begin() const { return neuronMap.begin(); }
    auto end() const { return neuronMap.end(); }
};

// Optimized synapse connectivity manager
class SynapseConnectivityManager {
private:
    struct SynapseNode {
        Synapse* synapse;
        SynapseNode* next;
        SynapseNode* prev;
        
        SynapseNode(Synapse* s) : synapse(s), next(nullptr), prev(nullptr) {}
    };
    
    std::unordered_map<NeuronId, SynapseNode*> outgoingMap;
    std::unordered_map<NeuronId, SynapseNode*> incomingMap;
    size_t maxDegree;
    
public:
    SynapseConnectivityManager(size_t maxDegree = 1000) : maxDegree(maxDegree) {}
    
    ~SynapseConnectivityManager() {
        clear();
    }
    
    void addConnection(NeuronId source, NeuronId destination, Synapse* synapse) {
        // Add to outgoing connections
        auto& outNode = outgoingMap[source];
        if (!outNode) {
            outNode = new SynapseNode(synapse);
        } else {
            // Insert at beginning for faster access
            SynapseNode* newNode = new SynapseNode(synapse);
            newNode->next = outNode;
            outNode->prev = newNode;
            outNode = newNode;
        }
        
        // Add to incoming connections
        auto& inNode = incomingMap[destination];
        if (!inNode) {
            inNode = new SynapseNode(synapse);
        } else {
            SynapseNode* newNode = new SynapseNode(synapse);
            newNode->next = inNode;
            inNode->prev = newNode;
            inNode = newNode;
        }
    }
    
    std::vector<Synapse*> getOutgoingConnections(NeuronId source) {
        std::vector<Synapse*> connections;
        auto it = outgoingMap.find(source);
        if (it != outgoingMap.end()) {
            SynapseNode* current = it->second;
            while (current) {
                connections.push_back(current->synapse);
                current = current->next;
            }
        }
        return connections;
    }
    
    std::vector<Synapse*> getIncomingConnections(NeuronId destination) {
        std::vector<Synapse*> connections;
        auto it = incomingMap.find(destination);
        if (it != incomingMap.end()) {
            SynapseNode* current = it->second;
            while (current) {
                connections.push_back(current->synapse);
                current = current->next;
            }
        }
        return connections;
    }
    
    void removeConnection(NeuronId source, NeuronId destination) {
        auto outIt = outgoingMap.find(source);
        if (outIt != outgoingMap.end()) {
            SynapseNode* current = outIt->second;
            while (current) {
                SynapseNode* next = current->next;
                if (current->synapse->getDestinationNeuron() == destination) {
                    // Remove from doubly linked list
                    if (current->prev) {
                        current->prev->next = current->next;
                    }
                    if (current->next) {
                        current->next->prev = current->prev;
                    }
                    if (current == outIt->second) {
                        outIt->second = current->next;
                    }
                    
                    delete current;
                    break;
                }
                current = next;
            }
        }
        
        auto inIt = incomingMap.find(destination);
        if (inIt != incomingMap.end()) {
            SynapseNode* current = inIt->second;
            while (current) {
                SynapseNode* next = current->next;
                if (current->synapse->getSourceNeuron() == source) {
                    // Remove from doubly linked list
                    if (current->prev) {
                        current->prev->next = current->next;
                    }
                    if (current->next) {
                        current->next->prev = current->prev;
                    }
                    if (current == inIt->second) {
                        inIt->second = current->next;
                    }
                    
                    delete current;
                    break;
                }
                current = next;
            }
        }
    }
    
    void clear() {
        for (auto& pair : outgoingMap) {
            SynapseNode* current = pair.second;
            while (current) {
                SynapseNode* next = current->next;
                delete current;
                current = next;
            }
        }
        outgoingMap.clear();
        
        for (auto& pair : incomingMap) {
            SynapseNode* current = pair.second;
            while (current) {
                SynapseNode* next = current->next;
                delete current;
                current = next;
            }
        }
        incomingMap.clear();
    }
    
    size_t getConnectionCount(NeuronId neuron) const {
        size_t count = 0;
        auto it = outgoingMap.find(neuron);
        if (it != outgoingMap.end()) {
            SynapseNode* current = it->second;
            while (current) {
                count++;
                current = current->next;
            }
        }
        return count;
    }
};

// Optimized memory pool for spike events
class SpikeEventPool {
private:
    struct PoolNode {
        SpikeEvent event;
        PoolNode* next;
        
        PoolNode(const SpikeEvent& e) : event(e), next(nullptr) {}
    };
    
    PoolNode* head;
    PoolNode* tail;
    size_t poolSize;
    size_t maxSize;
    mutable std::mutex poolMutex;
    
public:
    SpikeEventPool(size_t maxSize = 10000) : head(nullptr), tail(nullptr), poolSize(0), maxSize(maxSize) {}
    
    ~SpikeEventPool() {
        clear();
    }
    
    SpikeEvent* allocate(const SpikeEvent& event) {
        std::lock_guard<std::mutex> lock(poolMutex);
        
        if (poolSize >= maxSize) {
            // Pool is full, create new event
            return new SpikeEvent(event);
        }
        
        PoolNode* node = new PoolNode(event);
        if (!head) {
            head = tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
        poolSize++;
        
        return &node->event;
    }
    
    void deallocate(SpikeEvent* event) {
        std::lock_guard<std::mutex> lock(poolMutex);
        
        // Linear search for the event in the pool
        PoolNode* current = head;
        PoolNode* prev = nullptr;
        
        while (current) {
            if (&current->event == event) {
                // Found the event, remove from pool
                if (prev) {
                    prev->next = current->next;
                } else {
                    head = current->next;
                }
                
                if (current == tail) {
                    tail = prev;
                }
                
                delete current;
                poolSize--;
                return;
            }
            
            prev = current;
            current = current->next;
        }
        
        // Event not found in pool, delete it
        delete event;
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(poolMutex);
        
        PoolNode* current = head;
        while (current) {
            PoolNode* next = current->next;
            delete current;
            current = next;
        }
        
        head = tail = nullptr;
        poolSize = 0;
    }
    
    size_t size() const {
        std::lock_guard<std::mutex> lock(poolMutex);
        return poolSize;
    }
    
    // Iterator support for batch operations
    class iterator {
    private:
        PoolNode* current;
        
    public:
        iterator(PoolNode* node) : current(node) {}
        
        SpikeEvent& operator*() { return current->event; }
        SpikeEvent* operator->() { return &current->event; }
        
        iterator& operator++() {
            current = current->next;
            return *this;
        }
        
        iterator operator++(int) {
            iterator temp = *this;
            ++(*this);
            return temp;
        }
        
        bool operator==(const iterator& other) const {
            return current == other.current;
        }
        
        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }
    };
    
    iterator begin() { return iterator(head); }
    iterator end() { return iterator(nullptr); }
};

// Optimized performance profiler
class PerformanceProfiler {
private:
    struct ProfilingData {
        std::string operationName;
        double totalTime;
        size_t callCount;
        double averageTime;
        double minTime;
        double maxTime;
    };
    
    std::vector<ProfilingData> profileData;
    std::mutex profilerMutex;
    bool enabled;
    
public:
    PerformanceProfiler() : enabled(false) {}
    
    void enable() {
        enabled = true;
        profileData.clear();
    }
    
    void disable() {
        enabled = false;
    }
    
    void startProfiling(const std::string& operationName) {
        if (!enabled) return;
        
        std::lock_guard<std::mutex> lock(profilerMutex);
        
        // Check if operation exists
        bool found = false;
        for (auto& data : profileData) {
            if (data.operationName == operationName) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            profileData.push_back({operationName, 0.0, 0, 0.0, 0.0, 0.0});
        }
    }
    
    void stopProfiling(const std::string& operationName, double duration) {
        if (!enabled) return;
        
        std::lock_guard<std::mutex> lock(profilerMutex);
        
        for (auto& data : profileData) {
            if (data.operationName == operationName) {
                data.totalTime += duration;
                data.callCount++;
                data.averageTime = data.totalTime / data.callCount;
                if (duration < data.minTime || data.callCount == 1) {
                    data.minTime = duration;
                }
                if (duration > data.maxTime) {
                    data.maxTime = duration;
                }
                break;
            }
        }
    }
    
    void printReport() const {
        if (!enabled) return;
        
        std::cout << "=== Performance Profile Report ===" << std::endl;
        std::cout << "Operation                Calls  Total Time  Avg Time  Min Time  Max Time" << std::endl;
        std::cout << "---------------------------------------------------------------" << std::endl;
        
        for (const auto& data : profileData) {
            std::cout << std::left << std::setw(25) << data.operationName
                      << std::setw(6) << data.callCount
                      << std::setw(12) << std::fixed << std::setprecision(6) << data.totalTime
                      << std::setw(10) << std::fixed << std::setprecision(6) << data.averageTime
                      << std::setw(10) << std::fixed << std::setprecision(6) << data.minTime
                      << std::setw(10) << std::fixed << std::setprecision(6) << data.maxTime
                      << std::endl;
        }
    }
    
    double getAverageTime(const std::string& operationName) const {
        for (const auto& data : profileData) {
            if (data.operationName == operationName) {
                return data.averageTime;
            }
        }
        return 0.0;
    }
};

// SIMD-accelerated mathematical functions
namespace SIMDUtils {
    // Vectorized spike threshold checking
    inline std::vector<bool> checkThresholdSIMD(const std::vector<MembranePotential>& potentials,
                                               const std::vector<MembranePotential>& thresholds) {
        std::vector<bool> results;
        results.reserve(potentials.size());
        
        for (size_t i = 0; i < potentials.size(); ++i) {
            results.push_back(potentials[i] >= thresholds[i]);
        }
        
        return results;
    }
    
    // Vectorized membrane potential update
    inline void updateMembranePotentialSIMD(std::vector<MembranePotential>& potentials,
                                           const std::vector<MembranePotential>& resting,
                                           const std::vector<MembranePotential>& inputs,
                                           const std::vector<double>& timeConstants,
                                           double timestep) {
        const size_t size = potentials.size();
        for (size_t i = 0; i < size; ++i) {
            // LIF dynamics: V_new = V + dt * ((V_rest - V)/tau + I)
            double leak = (resting[i] - potentials[i]) / timeConstants[i];
            double input = inputs[i] / 1.0f;  // Assuming capacitance = 1.0f
            potentials[i] += static_cast<MembranePotential>(timestep * (leak + input));
        }
    }
    
    // Vectorized spike counting
    inline size_t countSpikesSIMD(const std::vector<bool>& fired) {
        return std::count(fired.begin(), fired.end(), true);
    }
    
    // Vectorized synaptic weight updates
    inline void updateWeightsSIMD(std::vector<float>& weights,
                                const std::vector<float>& updates,
                                float learningRate) {
        for (size_t i = 0; i < weights.size(); ++i) {
            weights[i] += updates[i] * learningRate;
        }
    }
}

// Optimized configuration validation
class ConfigValidator {
public:
    struct ValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    
    static ValidationResult validate(const Config& config) {
        ValidationResult result;
        result.isValid = true;
        
        auto keys = config.getKeys();
        
        for (const auto& key : keys) {
            // Validate key format
            if (key.empty()) {
                result.errors.push_back("Empty configuration key found");
                result.isValid = false;
            }
            
            if (key.find(' ') != std::string::npos) {
                result.warnings.push_back("Configuration key contains spaces: '" + key + "'");
            }
            
            // Validate specific parameter ranges
            if (key == "neuron_count") {
                auto val = config.get<int64_t>(key);
                if (val.has_value()) {
                    if (*val < 1) {
                        result.errors.push_back("neuron_count must be >= 1");
                        result.isValid = false;
                    } else if (*val > 1000000) {
                        result.warnings.push_back("neuron_count is very large, may impact performance");
                    }
                }
            }
            
            if (key == "region_count") {
                auto val = config.get<int64_t>(key);
                if (val.has_value()) {
                    if (*val < 1) {
                        result.errors.push_back("region_count must be >= 1");
                        result.isValid = false;
                    } else if (*val > 1000) {
                        result.warnings.push_back("region_count is large, may impact performance");
                    }
                }
            }
            
            if (key == "connection_probability") {
                auto val = config.get<double>(key);
                if (val.has_value()) {
                    if (*val < 0.0 || *val > 1.0) {
                        result.errors.push_back("connection_probability must be between 0.0 and 1.0");
                        result.isValid = false;
                    }
                }
            }
            
            if (key == "simulation_timestep") {
                auto val = config.get<double>(key);
                if (val.has_value()) {
                    if (*val <= 0.0) {
                        result.errors.push_back("simulation_timestep must be > 0.0");
                        result.isValid = false;
                    } else if (*val > 0.01) {
                        result.warnings.push_back("simulation_timestep is large, may impact accuracy");
                    }
                }
            }
        }
        
        return result;
    }
};

// Optimized memory allocator with alignment
class AlignedMemoryAllocator {
private:
    struct Allocation {
        void* ptr;
        size_t size;
        size_t alignment;
        std::chrono::steady_clock::time_point timestamp;
        
        Allocation(void* p, size_t s, size_t a) : ptr(p), size(s), alignment(a) {
            timestamp = std::chrono::steady_clock::now();
        }
    };
    
    std::vector<Allocation> allocations;
    std::mutex allocatorMutex;
    size_t totalAllocated;
    size_t peakAllocated;
    
public:
    AlignedMemoryAllocator() : totalAllocated(0), peakAllocated(0) {}
    
    void* allocate(size_t size, size_t alignment = 64) {
        std::lock_guard<std::mutex> lock(allocatorMutex);
        
        // Align size
        size = (size + alignment - 1) & ~(alignment - 1);
        
        // Use aligned allocation
        void* ptr = nullptr;
        if (posix_memalign(&ptr, alignment, size) != 0) {
            throw std::bad_alloc();
        }
        
        allocations.emplace_back(ptr, size, alignment);
        totalAllocated += size;
        peakAllocated = std::max(peakAllocated, totalAllocated);
        
        return ptr;
    }
    
    void deallocate(void* ptr, size_t size, size_t alignment = 64) {
        std::lock_guard<std::mutex> lock(allocatorMutex);
        
        // Find and remove allocation
        for (auto it = allocations.begin(); it != allocations.end(); ++it) {
            if (it->ptr == ptr) {
                totalAllocated -= it->size;
                free(ptr);
                allocations.erase(it);
                return;
            }
        }
        
        // Fallback: free without tracking
        free(ptr);
    }
    
    size_t getTotalAllocated() const {
        std::lock_guard<std::mutex> lock(allocatorMutex);
        return totalAllocated;
    }
    
    size_t getPeakAllocated() const {
        std::lock_guard<std::mutex> lock(allocatorMutex);
        return peakAllocated;
    }
    
    void printMemoryStats() const {
        std::cout << "=== Memory Allocation Statistics ===" << std::endl;
        std::cout << "Total allocated: " << getTotalAllocated() << " bytes" << std::endl;
        std::cout << "Peak allocated: " << getPeakAllocated() << " bytes" << std::endl;
        std::cout << "Active allocations: " << allocations.size() << std::endl;
    }
}; 

// Optimized event queue for spike delivery
class OptimizedEventQueue {
private:
    struct EventNode {
        SimulationStep step;
        Timestamp time;
        void (*callback)(const void*);
        const void* data;
        EventNode* next;
        
        EventNode(SimulationStep s, Timestamp t, void (*cb)(const void*), const void* d)
            : step(s), time(t), callback(cb), data(d), next(nullptr) {}
    };
    
    EventNode* head;
    EventNode* tail;
    size_t queueSize;
    mutable std::mutex queueMutex;
    
    // For optimized delivery
    SimulationStep currentStep;
    Timestamp currentTime;
    
public:
    OptimizedEventQueue() : head(nullptr), tail(nullptr), queueSize(0), currentStep(0), currentTime(0.0) {}
    
    ~OptimizedEventQueue() {
        clear();
    }
    
    void setCurrentState(SimulationStep step, Timestamp time) {
        currentStep = step;
        currentTime = time;
    }
    
    void enqueue(SimulationStep step, Timestamp time, void (*callback)(const void*), const void* data) {
        std::lock_guard<std::mutex> lock(queueMutex);
        
        EventNode* node = new EventNode(step, time, callback, data);
        if (!head) {
            head = tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
        queueSize++;
    }
    
    void processEvents() {
        std::lock_guard<std::mutex> lock(queueMutex);
        
        EventNode* current = head;
        EventNode* prev = nullptr;
        
        while (current) {
            EventNode* next = current->next;
            
            // Check if event should be delivered
            if (current->step <= currentStep && current->time <= currentTime) {
                // Deliver event
                if (current->callback) {
                    current->callback(current->data);
                }
                
                // Remove from queue
                if (prev) {
                    prev->next = next;
                } else {
                    head = next;
                }
                
                if (current == tail) {
                    tail = prev;
                }
                
                delete current;
                queueSize--;
            } else {
                prev = current;
            }
            
            current = next;
        }
    }
    
    size_t size() const {
        std::lock_guard<std::mutex> lock(queueMutex);
        return queueSize;
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(queueMutex);
        
        EventNode* current = head;
        while (current) {
            EventNode* next = current->next;
            delete current;
            current = next;
        }
        
        head = tail = nullptr;
        queueSize = 0;
    }
    
    bool isEmpty() const {
        std::lock_guard<std::mutex> lock(queueMutex);
        return queueSize == 0;
    }
}; 

// Global performance monitoring instance
class PerformanceMonitor {
private:
    PerformanceMetrics metrics;
    PerformanceProfiler profiler;
    AlignedMemoryAllocator memoryAllocator;
    mutable std::mutex monitorMutex;
    bool monitoringEnabled;
    
public:
    PerformanceMonitor() : monitoringEnabled(false) {}
    
    void enableMonitoring() {
        std::lock_guard<std::mutex> lock(monitorMutex);
        monitoringEnabled = true;
        profiler.enable();
        memoryAllocator.printMemoryStats();
    }
    
    void disableMonitoring() {
        std::lock_guard<std::mutex> lock(monitorMutex);
        monitoringEnabled = false;
        profiler.disable();
    }
    
    void recordStep(SimulationStep step, size_t neuronCount, size_t spikeCount) {
        if (!monitoringEnabled) return;
        
        std::lock_guard<std::mutex> lock(monitorMutex);
        metrics.totalNeuronsProcessed += neuronCount;
        metrics.totalSpikesProcessed += spikeCount;
    }
    
    void recordMemoryAllocation(size_t size) {
        if (!monitoringEnabled) return;
        
        std::lock_guard<std::mutex> lock(monitorMutex);
        metrics.memoryAllocations++;
    }
    
    void recordMemoryDeallocation(size_t size) {
        if (!monitoringEnabled) return;
        
        std::lock_guard<std::mutex> lock(monitorMutex);
        metrics.memoryDeallocations++;
    }
    
    void setSimulationTime(double time) {
        if (!monitoringEnabled) return;
        
        std::lock_guard<std::mutex> lock(monitorMutex);
        metrics.totalSimulationTime += time;
    }
    
    PerformanceMetrics getMetrics() const {
        std::lock_guard<std::mutex> lock(monitorMutex);
        return metrics;
    }
    
    void printPerformanceReport() const {
        if (!monitoringEnabled) return;
        
        std::cout << "=== Performance Monitor Report ===" << std::endl;
        std::cout << "Total simulation time: " << metrics.totalSimulationTime << " seconds" << std::endl;
        std::cout << "Total neurons processed: " << metrics.totalNeuronsProcessed << std::endl;
        std::cout << "Total spikes processed: " << metrics.totalSpikesProcessed << std::endl;
        std::cout << "Memory allocations: " << metrics.memoryAllocations << std::endl;
        std::cout << "Memory deallocations: " << metrics.memoryDeallocations << std::endl;
        std::cout << "Memory allocator peak: " << memoryAllocator.getPeakAllocated() << " bytes" << std::endl;
        
        profiler.printReport();
    }
}; 

// Global instances for easy access
static PerformanceMonitor globalPerformanceMonitor;
static NeuronLookupTable globalNeuronLookupTable;
static SynapseConnectivityManager globalConnectivityManager;
static SpikeEventPool globalSpikeEventPool;
static OptimizedEventQueue globalEventQueue;

// Global performance monitoring functions
inline void enablePerformanceMonitoring() {
    globalPerformanceMonitor.enableMonitoring();
}

inline void disablePerformanceMonitoring() {
    globalPerformanceMonitor.disableMonitoring();
}

inline void recordNeuronOperation(NeuronId id, Neuron* neuron) {
    globalNeuronLookupTable.insert(id, neuron);
}

inline Neuron* findNeuron(NeuronId id) {
    return globalNeuronLookupTable.find(id);
}

inline void addSynapseConnection(NeuronId source, NeuronId destination, Synapse* synapse) {
    globalConnectivityManager.addConnection(source, destination, synapse);
}

inline std::vector<Synapse*> getOutgoingConnections(NeuronId source) {
    return globalConnectivityManager.getOutgoingConnections(source);
}

inline SpikeEvent* allocateSpikeEvent(const SpikeEvent& event) {
    return globalSpikeEventPool.allocate(event);
}

inline void deallocateSpikeEvent(SpikeEvent* event) {
    globalSpikeEventPool.deallocate(event);
}

inline void setEventQueueState(SimulationStep step, Timestamp time) {
    globalEventQueue.setCurrentState(step, time);
}

inline void enqueueEvent(SimulationStep step, Timestamp time, void (*callback)(const void*), const void* data) {
    globalEventQueue.enqueue(step, time, callback, data);
}

inline void processEvents() {
    globalEventQueue.processEvents();
}

inline bool hasPendingEvents() {
    return !globalEventQueue.isEmpty();
}

// Utility functions for performance optimization
inline void clearAllPerformanceStructures() {
    globalNeuronLookupTable.clear();
    globalConnectivityManager.clear();
    globalSpikeEventPool.clear();
    globalEventQueue.clear();
}

inline void printPerformanceStatistics() {
    globalPerformanceMonitor.printPerformanceReport();
}

} // namespace nlm