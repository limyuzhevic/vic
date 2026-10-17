// Object Pool Implementation for Neural Simulation Project
// This file provides object pooling patterns for neurons, synapses, and other simulation objects
// to improve allocation efficiency and reduce memory fragmentation

#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <mutex>

namespace nlm {

// Base Object Pool Template
// Provides a generic object pool that can be specialized for any type
// Following the classic object pool pattern with fast allocation/deallocation

template<typename T, size_t PoolSize = 1000>
class ObjectPool {
public:
    ObjectPool() : nextIndex_(0) {
        // Pre-allocate objects
        objects_.reserve(PoolSize);
        for (size_t i = 0; i < PoolSize; ++i) {
            objects_.emplace_back(std::make_unique<T>());
            freeList_.push_back(i);
        }
    }
    
    ~ObjectPool() = default;
    
    // Disable copying
    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;
    
    // Allocate an object from the pool
    T* allocate() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (freeList_.empty()) {
            // Pool is exhausted, create a new object (should not happen with proper sizing)
            auto obj = std::make_unique<T>();
            T* ptr = obj.get();
            allocatedObjects_.push_back(std::move(obj));
            return ptr;
        }
        
        // Get index from free list
        size_t index = freeList_.back();
        freeList_.pop_back();
        
        T* ptr = objects_[index].get();
        allocatedObjects_.push_back(std::move(objects_[index]));
        objects_[index].reset(); // Clear the slot
        
        return ptr;
    }
    
    // Deallocate an object back to the pool
    void deallocate(T* obj) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Find the object in allocatedObjects_
        auto it = std::find_if(allocatedObjects_.begin(), allocatedObjects_.end(),
                             [obj](const std::unique_ptr<T>&& ptr) {
                                 return ptr.get() == obj;
                             });
        
        if (it != allocatedObjects_.end()) {
            // Return object to objects_ pool
            *it = std::make_unique<T>(); // Recreate object at same slot
            freeList_.push_back(std::distance(allocatedObjects_.begin(), it));
            allocatedObjects_.erase(it);
        }
    }
    
    // Check if pool has available objects
    bool hasAvailable() const {
        return !freeList_.empty();
    }
    
    // Get number of allocated objects
    size_t allocatedCount() const {
        return allocatedObjects_.size();
    }
    
    // Get total pool size
    size_t poolSize() const {
        return PoolSize;
    }
    
    // Get memory usage
    size_t memoryUsage() const {
        size_t total = 0;
        for (const auto& obj : allocatedObjects_) {
            total += sizeof(T);
        }
        return total;
    }
    
    // Clear all allocated objects (for emergency scenarios)
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Move all allocated objects back to pool
        for (auto& obj : allocatedObjects_) {
            // Reset the object (calls destructor)
            obj.reset();
            // Add to free list (this is a simplification)
        }
        
        allocatedObjects_.clear();
        
        // Refill free list
        freeList_.clear();
        for (size_t i = 0; i < PoolSize; ++i) {
            freeList_.push_back(i);
        }
    }
    
private:
    std::vector<std::unique_ptr<T>> objects_;
    std::vector<size_t> freeList_;
    std::vector<std::unique_ptr<T>> allocatedObjects_;
    mutable std::mutex mutex_;
    size_t nextIndex_;
};

// Neuron Object Pool Specialization
// Provides specialized object pooling for neurons with initialization support
class NeuronObjectPool {
public:
    NeuronObjectPool(size_t poolSize = 1000) : poolSize_(poolSize) {
        // Pre-allocate neurons
        neurons_.reserve(poolSize_);
        for (size_t i = 0; i < poolSize_; ++i) {
            neurons_.emplace_back(std::make_unique<Neuron>(i));
            freeList_.push_back(i);
        }
    }
    
    ~NeuronObjectPool() = default;
    
    // Allocate a neuron with initial state
    NeuronId allocate(const NeuronState& initialState) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (freeList_.empty()) {
            // Pool is exhausted, create a new neuron
            NeuronId id = static_cast<NeuronId>(nextNeuronId_.fetch_add(1));
            auto neuron = std::make_unique<Neuron>(id);
            initializeNeuron(neuron.get(), initialState);
            neurons_.push_back(std::move(neuron));
            return id;
        }
        
        // Get ID from free list
        size_t index = freeList_.back();
        NeuronId id = static_cast<NeuronId>(index);
        freeList_.pop_back();
        
        // Get neuron from pool
        auto neuron = std::make_unique<Neuron>(id);
        initializeNeuron(neuron.get(), initialState);
        neurons_.push_back(std::move(neuron));
        
        return id;
    }
    
    // Deallocate a neuron
    void deallocate(NeuronId id) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Find neuron in neurons_
        auto it = std::find_if(neurons_.begin(), neurons_.end(),
                             [id](const std::unique_ptr<Neuron>&& ptr) {
                                 return ptr->getId() == id;
                             });
        
        if (it != neurons_.end()) {
            // Return to free list
            freeList_.push_back(id);
            neurons_.erase(it);
        }
    }
    
    // Get a neuron by ID
    Neuron* get(NeuronId id) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = std::find_if(neurons_.begin(), neurons_.end(),
                             [id](const std::unique_ptr<Neuron>&& ptr) {
                                 return ptr->getId() == id;
                             });
        
        if (it != neurons_.end()) {
            return it->get();
        }
        return nullptr;
    }
    
    // Get all neurons
    const std::vector<std::unique_ptr<Neuron>>& getAllNeurons() const {
        return neurons_;
    }
    
    // Check if neuron pool has available slots
    bool hasAvailable() const {
        return !freeList_.empty();
    }
    
    // Get number of allocated neurons
    size_t allocatedCount() const {
        return neurons_.size() - freeList_.size();
    }
    
    // Reserve additional capacity
    void reserve(size_t additionalCapacity) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        size_t currentCapacity = neurons_.capacity();
        if (currentCapacity < poolSize_ + additionalCapacity) {
            neurons_.reserve(poolSize_ + additionalCapacity);
        }
    }
    
    // Clear all neurons (emergency scenario)
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Return all neurons to free list
        for (const auto& neuron : neurons_) {
            freeList_.push_back(neuron->getId());
        }
        
        neurons_.clear();
        
        // Refill free list
        freeList_.clear();
        for (size_t i = 0; i < poolSize_; ++i) {
            freeList_.push_back(i);
        }
        
        // Reset next neuron ID
        nextNeuronId_ = poolSize_;
    }
    
    // Get memory usage
    size_t memoryUsage() const {
        size_t total = 0;
        for (const auto& neuron : neurons_) {
            total += sizeof(Neuron);
        }
        return total;
    }
    
private:
    void initializeNeuron(Neuron* neuron, const NeuronState& state) {
        // Initialize neuron with provided state
        neuron->setMembranePotential(state.membranePotential);
        neuron->setThreshold(state.threshold);
        neuron->setFiringState(state.firingState);
        neuron->setFiringRate(state.firingRate);
        neuron->setRefractoryPeriod(state.refractoryPeriod);
        neuron->setLeakConductance(state.leakConductance);
        neuron->setRestingPotential(state.restingPotential);
        neuron->setResetPotential(state.resetPotential);
        
        // Initialize other fields
        neuron->getState().adaptationVariable = state.adaptationVariable;
        neuron->getState().lastSpikeTime = state.lastSpikeTime;
        neuron->getState().refractoryRemaining = state.refractoryRemaining;
        neuron->getState().plasticityFlags = state.plasticityFlags;
    }
    
    std::vector<std::unique_ptr<Neuron>> neurons_;
    std::vector<NeuronId> freeList_;
    mutable std::mutex mutex_;
    size_t poolSize_;
    std::atomic<NeuronId> nextNeuronId_;
};

// Synapse Object Pool Specialization
// Provides specialized object pooling for synapses with connection support
class SynapseObjectPool {
public:
    SynapseObjectPool(size_t poolSize = 5000) : poolSize_(poolSize) {
        // Pre-allocate synapses
        synapses_.reserve(poolSize_);
        for (size_t i = 0; i < poolSize_; ++i) {
            SynapseId id = static_cast<SynapseId>(i);
            synapses_.emplace_back(std::make_unique<Synapse>(id, 0, 0));
            freeList_.push_back(id);
        }
    }
    
    ~SynapseObjectPool() = default;
    
    // Allocate a synapse with initial state
    SynapseId allocate(NeuronId source, NeuronId destination, const SynapseState& initialState) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (freeList_.empty()) {
            // Pool is exhausted, create a new synapse
            SynapseId id = static_cast<SynapseId>(nextSynapseId_.fetch_add(1));
            auto synapse = std::make_unique<Synapse>(id, source, destination);
            initializeSynapse(synapse.get(), initialState);
            synapses_.push_back(std::move(synapse));
            return id;
        }
        
        // Get ID from free list
        SynapseId id = freeList_.back();
        freeList_.pop_back();
        
        // Get synapse from pool
        auto synapse = std::make_unique<Synapse>(id, source, destination);
        initializeSynapse(synapse.get(), initialState);
        synapses_.push_back(std::move(synapse));
        
        return id;
    }
    
    // Deallocate a synapse
    void deallocate(SynapseId id) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Find synapse in synapses_
        auto it = std::find_if(synapses_.begin(), synapses_.end(),
                             [id](const std::unique_ptr<Synapse>&& ptr) {
                                 return ptr->getId() == id;
                             });
        
        if (it != synapses_.end()) {
            // Return to free list
            freeList_.push_back(id);
            synapses_.erase(it);
        }
    }
    
    // Get a synapse by ID
    Synapse* get(SynapseId id) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = std::find_if(synapses_.begin(), synapses_.end(),
                             [id](const std::unique_ptr<Synapse>&& ptr) {
                                 return ptr->getId() == id;
                             });
        
        if (it != synapses_.end()) {
            return it->get();
        }
        return nullptr;
    }
    
    // Get all synapses
    const std::vector<std::unique_ptr<Synapse>>& getAllSynapses() const {
        return synapses_;
    }
    
    // Check if synapse pool has available slots
    bool hasAvailable() const {
        return !freeList_.empty();
    }
    
    // Get number of allocated synapses
    size_t allocatedCount() const {
        return synapses_.size() - freeList_.size();
    }
    
    // Reserve additional capacity
    void reserve(size_t additionalCapacity) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        size_t currentCapacity = synapses_.capacity();
        if (currentCapacity < poolSize_ + additionalCapacity) {
            synapses_.reserve(poolSize_ + additionalCapacity);
        }
    }
    
    // Clear all synapses (emergency scenario)
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Return all synapses to free list
        for (const auto& synapse : synapses_) {
            freeList_.push_back(synapse->getId());
        }
        
        synapses_.clear();
        
        // Refill free list
        freeList_.clear();
        for (size_t i = 0; i < poolSize_; ++i) {
            freeList_.push_back(static_cast<SynapseId>(i));
        }
        
        // Reset next synapse ID
        nextSynapseId_ = poolSize_;
    }
    
    // Get memory usage
    size_t memoryUsage() const {
        size_t total = 0;
        for (const auto& synapse : synapses_) {
            total += sizeof(Synapse);
        }
        return total;
    }
    
    // Find synapses by source neuron
    std::vector<SynapseId> findSynapsesBySource(NeuronId source) const {
        std::vector<SynapseId> result;
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (const auto& synapse : synapses_) {
            if (synapse->getSourceNeuron() == source) {
                result.push_back(synapse->getId());
            }
        }
        
        return result;
    }
    
    // Find synapses by destination neuron
    std::vector<SynapseId> findSynapsesByDestination(NeuronId destination) const {
        std::vector<SynapseId> result;
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (const auto& synapse : synapses_) {
            if (synapse->getDestinationNeuron() == destination) {
                result.push_back(synapse->getId());
            }
        }
        
        return result;
    }
    
private:
    void initializeSynapse(Synapse* synapse, const SynapseState& state) {
        // Initialize synapse with provided state
        synapse->setWeight(state.weight);
        synapse->setDelay(state.delay);
        synapse->setType(state.type);
        synapse->enablePlasticity(state.plasticityFlags.hebbian,
                                state.plasticityFlags.stdp,
                                state.plasticityFlags.reward_modulated);
        
        // Initialize other fields
        synapse->setEligibilityTrace(0.0f);
        synapse->setEfficacy(1.0f);
    }
    
    std::vector<std::unique_ptr<Synapse>> synapses_;
    std::vector<SynapseId> freeList_;
    mutable std::mutex mutex_;
    size_t poolSize_;
    std::atomic<SynapseId> nextSynapseId_;
};

// MemoryManager - Centralized memory management
// Provides high-level memory management for all simulation objects
class MemoryManager {
public:
    MemoryManager() : neuronPool_(10000), synapsePool_(50000) {
        // Initialize pools with reasonable sizes
    }
    
    ~MemoryManager() {
        // Clear all pools
        neuronPool_.clear();
        synapsePool_.clear();
    }
    
    // Get neuron pool
    NeuronObjectPool& getNeuronPool() {
        return neuronPool_;
    }
    
    // Get synapse pool
    SynapseObjectPool& getSynapsePool() {
        return synapsePool_;
    }
    
    // Allocate neuron with initial state
    NeuronId allocateNeuron(const NeuronState& initialState = NeuronState()) {
        return neuronPool_.allocate(initialState);
    }
    
    // Allocate synapse with initial state
    SynapseId allocateSynapse(NeuronId source, NeuronId destination,
                           const SynapseState& initialState = SynapseState()) {
        return synapsePool_.allocate(source, destination, initialState);
    }
    
    // Deallocate neuron
    void deallocateNeuron(NeuronId id) {
        neuronPool_.deallocate(id);
    }
    
    // Deallocate synapse
    void deallocateSynapse(SynapseId id) {
        synapsePool_.deallocate(id);
    }
    
    // Get neuron by ID
    Neuron* getNeuron(NeuronId id) {
        return neuronPool_.get(id);
    }
    
    // Get synapse by ID
    Synapse* getSynapse(SynapseId id) {
        return synapsePool_.get(id);
    }
    
    // Get all neurons
    const std::vector<std::unique_ptr<Neuron>>& getAllNeurons() const {
        return neuronPool_.getAllNeurons();
    }
    
    // Get all synapses
    const std::vector<std::unique_ptr<Synapse>>& getAllSynapses() const {
        return synapsePool_.getAllSynapses();
    }
    
    // Reserve additional capacity for neurons
    void reserveNeurons(size_t count) {
        neuronPool_.reserve(count);
    }
    
    // Reserve additional capacity for synapses
    void reserveSynapses(size_t count) {
        synapsePool_.reserve(count);
    }
    
    // Clear all objects (emergency scenario)
    void clear() {
        neuronPool_.clear();
        synapsePool_.clear();
    }
    
    // Get memory statistics
    struct MemoryStats {
        size_t neuronCount;
        size_t synapseCount;
        size_t neuronMemoryUsage;
        size_t synapseMemoryUsage;
        float utilizationRate;
    };
    
    MemoryStats getMemoryStats() const {
        MemoryStats stats;
        stats.neuronCount = neuronPool_.allocatedCount();
        stats.synapseCount = synapsePool_.allocatedCount();
        stats.neuronMemoryUsage = neuronPool_.memoryUsage();
        stats.synapseMemoryUsage = synapsePool_.memoryUsage();
        
        size_t totalAllocated = stats.neuronMemoryUsage + stats.synapseMemoryUsage;
        size_t totalPoolSize = neuronPool_.poolSize() * sizeof(Neuron) +
                              synapsePool_.poolSize() * sizeof(Synapse);
        
        if (totalPoolSize > 0) {
            stats.utilizationRate = static_cast<float>(totalAllocated) / static_cast<float>(totalPoolSize);
        } else {
            stats.utilizationRate = 0.0f;
        }
        
        return stats;
    }
    
    // Check if pools have available capacity
    bool hasAvailableCapacity() const {
        return neuronPool_.hasAvailable() && synapsePool_.hasAvailable();
    }
    
    // Emergency reserve additional capacity
    void reserveAdditionalCapacity(size_t neuronCount, size_t synapseCount) {
        neuronPool_.reserve(neuronCount);
        synapsePool_.reserve(synapseCount);
    }
    
    // Get free list sizes
    size_t getFreeNeuronCount() const {
        return neuronPool_.freeList_.size();
    }
    
    size_t getFreeSynapseCount() const {
        return synapsePool_.freeList_.size();
    }
    
private:
    NeuronObjectPool neuronPool_;
    SynapseObjectPool synapsePool_;
};

// Global memory manager instance
inline MemoryManager& getGlobalMemoryManager() {
    static MemoryManager manager;
    return manager;
}

// Helper functions for backward compatibility
inline NeuronId allocate_neuron(const NeuronState& state = NeuronState()) {
    return getGlobalMemoryManager().allocateNeuron(state);
}

inline SynapseId allocate_synapse(NeuronId source, NeuronId destination,
                               const SynapseState& state = SynapseState()) {
    return getGlobalMemoryManager().allocateSynapse(source, destination, state);
}

inline void deallocate_neuron(NeuronId id) {
    getGlobalMemoryManager().deallocateNeuron(id);
}

inline void deallocate_synapse(SynapseId id) {
    getGlobalMemoryManager().deallocateSynapse(id);
}

inline Neuron* get_neuron(NeuronId id) {
    return getGlobalMemoryManager().getNeuron(id);
}

inline Synapse* get_synapse(SynapseId id) {
    return getGlobalMemoryManager().getSynapse(id);
}

} // namespace nlm
