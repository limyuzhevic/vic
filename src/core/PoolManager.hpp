#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <memory>
#include <vector>
#include <queue>
#include <mutex>
#include <algorithm>

namespace nlm {

/**
 * @class ObjectPool
 * @brief Generic object pool for managing reusable objects
 * 
 * Object pools reduce memory allocation overhead by reusing objects
 * instead of allocating/deallocating them individually. This is particularly
 * beneficial for neuron and synapse objects in neural simulations where
 * thousands of objects are created and destroyed during runtime.
 * 
 * @tparam T The object type to pool
 * @tparam Args Constructor argument types
 * 
 * @author NLM Research Team
 * @version 0.1.0
 */
template<typename T, typename... Args>
class ObjectPool {
public:
    /**
     * @brief Construct a new ObjectPool object
     * 
     * @param initialCapacity Initial number of objects to pre-allocate
     * @param maxCapacity Maximum number of objects allowed in pool
     */
    ObjectPool(size_t initialCapacity = 1000, size_t maxCapacity = 10000)
        : maxCapacity_(maxCapacity)
    {
        reserve(initialCapacity);
    }
    
    /**
     * @brief Destroy the ObjectPool object
     * 
     * All returned objects must have been returned to the pool before destruction.
     */
    ~ObjectPool() = default;
    
    // Disable copying
    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;
    
    /**
     * @brief Construct a new object in the pool
     * 
     * @tparam ConstructorArgs Types of constructor arguments
     * @param args Arguments to pass to constructor
     * @return T* Pointer to newly created object
     * 
     * @note This method uses perfect forwarding for constructor arguments
     * @see returnObject()
     */
    template<typename... ConstructorArgs>
    T* newObject(ConstructorArgs&&... args) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (freeList_.empty()) {
            if (objects_.size() >= maxCapacity_) {
                return nullptr; // Pool exhausted
            }
            objects_.emplace_back(std::forward<ConstructorArgs>(args)...);
            return objects_.back().get();
        }
        
        T* ptr = freeList_.front().get();
        freeList_.pop();
        return ptr;
    }
    
    /**
     * @brief Return an object to the pool for reuse
     * 
     * @param obj Pointer to object to return
     * 
     * @note Object must have been allocated from this pool
     * @see newObject()
     */
    void deleteObject(T* obj) {
        if (!obj) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Verify object was allocated from this pool (basic check)
        auto it = std::find_if(objects_.begin(), objects_.end(),
            [obj](const std::unique_ptr<T>& ptr) { return ptr.get() == obj; });n        
        if (it != objects_.end()) {
            freeList_.push(it->release());
            objects_.erase(it);
        }
        // Note: In production, consider adding object tracking for better validation
    }
    
    /**
     * @brief Get the number of allocated objects
     */
    size_t getAllocatedCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return objects_.size() - freeList_.size();
    }
    
    /**
     * @brief Get the total number of objects ever allocated
     */
    size_t getTotalAllocatedCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return objects_.size();
    }
    
    /**
     * @brief Get the current number of free objects
     */
    size_t getFreeCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return freeList_.size();
    }
    
    /**
     * @brief Reserve space for additional objects
     * 
     * @param capacity Number of objects to reserve
     */
    void reserve(size_t capacity) {
        objects_.reserve(capacity);
    }
    
    /**
     * @brief Clear the pool and release all objects
     */
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        freeList_ = std::queue<std::unique_ptr<T>>();
        objects_.clear();
    }
    
    /**
     * @brief Check if pool has reached maximum capacity
     */
    bool isFull() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return objects_.size() >= maxCapacity_;
    }
    
    /**
     * @brief Check if pool has available objects
     */
    bool hasAvailable() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return !freeList_.empty() || objects_.size() < maxCapacity_;
    }

private:
    std::vector<std::unique_ptr<T>> objects_;              // All allocated objects
    std::queue<std::unique_ptr<T>> freeList_;              // Free objects for reuse
    mutable std::mutex mutex_;                             // Thread safety
    size_t maxCapacity_;                                   // Maximum allowed objects
};

/**
 * @class NeuronPool
 * @brief Specialized object pool for Neuron objects
 * 
 * This class provides optimized pooling for Neuron objects with
 * specialization for common neural simulation patterns.
 * 
 * @author NLM Research Team
 * @version 0.1.0
 */
class NeuronPool : public ObjectPool<Neuron> {
public:
    /**
     * @brief Create a neuron with common initialization
     * 
     * @param id Neuron identifier
     * @return Neuron* Pointer to new neuron
     * 
     * @note This method performs common initialization steps that are
     *       frequently performed when creating neurons in simulations.
     */
    Neuron* createNeuron(NeuronId id) {
        Neuron* neuron = newObject(id);
        if (neuron) {
            // Perform common initialization
            neuron->setType(NeuronType::Internal);
            neuron->setMembranePotential(-70.0f);  // Default resting potential
            neuron->setThreshold(-55.0f);          // Default threshold
            neuron->setRestingPotential(-70.0f);   // Default resting potential
            neuron->setResetPotential(-75.0f);     // Default reset potential
            neuron->setFiringState(FiringState::Resting);
            neuron->enablePlasticity(true, true, false);
        }
        return neuron;
    }
    
    /**
     * @brief Create a sensory neuron with specialization
     * 
     * @param id Neuron identifier
     * @return Neuron* Pointer to new sensory neuron
     */
    Neuron* createSensoryNeuron(NeuronId id) {
        Neuron* neuron = createNeuron(id);
        if (neuron) {
            neuron->setType(NeuronType::Sensory);
            neuron->setRestingPotential(-60.0f);  // Higher resting potential for sensory
        }
        return neuron;
    }
    
    /**
     * @brief Create a motor neuron with specialization
     * 
     * @param id Neuron identifier
     * @return Neuron* Pointer to new motor neuron
     */
    Neuron* createMotorNeuron(NeuronId id) {
        Neuron* neuron = createNeuron(id);
        if (neuron) {
            neuron->setType(NeuronType::Motor);
            neuron->setRestingPotential(-50.0f);  // Even higher for motor
            neuron->setThreshold(-45.0f);         // Lower threshold for motor
        }
        return neuron;
    }
    
    /**
     * @brief Create an inhibitory neuron
     * 
     * @param id Neuron identifier
     * @return Neuron* Pointer to new inhibitory neuron
     */
    Neuron* createInhibitoryNeuron(NeuronId id) {
        Neuron* neuron = createNeuron(id);
        if (neuron) {
            neuron->setType(NeuronType::Inhibitory);
            neuron->setRestingPotential(-65.0f);
            neuron->setThreshold(-60.0f);         // Lower threshold for inhibitory
        }
        return neuron;
    }
};

/**
 * @class SynapsePool
 * @brief Specialized object pool for Synapse objects
 * 
 * Provides optimized pooling for Synapse objects with initialization
 * optimizations specific to neural connectivity patterns.
 * 
 * @author NLM Research Team
 * @version 0.1.0
 */
class SynapsePool : public ObjectPool<Synapse> {
public:
    /**
     * @brief Create a synapse with common parameters
     * 
     * @param source Source neuron ID
     * @param destination Destination neuron ID
     * @param weight Initial synaptic weight
     * @param delay Synaptic delay in milliseconds
     * @return Synapse* Pointer to new synapse
     */
    Synapse* createSynapse(NeuronId source, NeuronId destination, 
                          SynapticWeight weight = 0.5f, Delay delay = 1) {
        Synapse* synapse = newObject(source, destination, weight, delay);
        if (synapse) {
            // Enable common plasticity mechanisms
            synapse->enablePlasticity(true, true, true);
            
            // Set common parameters based on delay
            if (delay > 5) {
                synapse->setDelay(delay);
            }
            
            // Normalize weight to typical range
            float normalizedWeight = std::clamp(weight, -1.0f, 1.0f);
            synapse->setWeight(normalizedWeight);
        }
        return synapse;
    }
    
    /**
     * @brief Create an excitatory synapse
     * 
     * @param source Source neuron ID
     * @param destination Destination neuron ID
     * @param weight Synaptic weight (positive)
     * @return Synapse* Pointer to new excitatory synapse
     */
    Synapse* createExcitatorySynapse(NeuronId source, NeuronId destination, 
                                    SynapticWeight weight = 0.8f) {
        Synapse* synapse = createSynapse(source, destination, weight, 1);
        if (synapse) {
            synapse->setType(SynapseType::Excitatory);
        }
        return synapse;
    }
    
    /**
     * @brief Create an inhibitory synapse
     * 
     * @param source Source neuron ID
     * @param destination Destination neuron ID
     * @param weight Synaptic weight (negative)
     * @return Synapse* Pointer to new inhibitory synapse
     */
    Synapse* createInhibitorySynapse(NeuronId source, NeuronId destination, 
                                    SynapticWeight weight = -0.5f) {
        Synapse* synapse = createSynapse(source, destination, weight, 1);
        if (synapse) {
            synapse->setType(SynapseType::Inhibitory);
        }
        return synapse;
    }
    
    /**
     * @brief Create a modulatory synapse
     * 
     * @param source Source neuron ID
     * @param destination Destination neuron ID
     * @param weight Synaptic weight for modulation
     * @return Synapse* Pointer to new modulatory synapse
     */
    Synapse* createModulatorySynapse(NeuronId source, NeuronId destination, 
                                    SynapticWeight weight = 0.3f) {
        Synapse* synapse = createSynapse(source, destination, weight, 2);
        if (synapse) {
            synapse->setType(SynapseType::Modulatory);
        }
        return synapse;
    }
};

/**
 * @class PoolManager
 * @brief Central manager for all object pools in the NLM system
 * 
 * The PoolManager provides a unified interface for accessing and managing
 * all object pools (neurons, synapses, etc.) used throughout the neural
 * simulation. It ensures proper lifecycle management and provides statistics
 * for performance monitoring.
 * 
 * @author NLM Research Team
 * @version 0.1.0
 */
class PoolManager {
public:
    /**
     * @brief Construct a new PoolManager object
     * 
     * Initializes all system pools with appropriate initial capacities.
     */
    PoolManager()
    {
        // Initialize pools with reasonable initial capacities
        neuronPool_ = std::make_unique<NeuronPool>(1000, 50000);  // Up to 50k neurons
        synapsePool_ = std::make_unique<SynapsePool>(5000, 100000);  // Up to 100k synapses
    }
    
    /**
     * @brief Destroy the PoolManager object
     */
    ~PoolManager() = default;
    
    // Disable copying
    PoolManager(const PoolManager&) = delete;
    PoolManager& operator=(const PoolManager&) = delete;
    
    /**
     * @brief Get the singleton instance of PoolManager
     * 
     * @return PoolManager& Reference to the singleton instance
     */
    static PoolManager& getInstance() {
        static PoolManager instance;
        return instance;
    }
    
    /**
     * @brief Get the neuron pool
     * 
     * @return NeuronPool* Pointer to neuron pool
     */
    NeuronPool* getNeuronPool() { return neuronPool_.get(); }
    
    /**
     * @brief Get the synapse pool
     * 
     * @return SynapsePool* Pointer to synapse pool
     */
    SynapsePool* getSynapsePool() { return synapsePool_.get(); }
    
    /**
     * @brief Initialize all pools with custom capacities
     * 
     * @param neuronInitial Initial neuron pool capacity
     * @param neuronMax Maximum neuron pool capacity
     * @param synapseInitial Initial synapse pool capacity
     * @param synapseMax Maximum synapse pool capacity
     */
    void initializePools(size_t neuronInitial = 1000, size_t neuronMax = 50000,
                        size_t synapseInitial = 5000, size_t synapseMax = 100000)
    {
        neuronPool_ = std::make_unique<NeuronPool>(neuronInitial, neuronMax);
        synapsePool_ = std::make_unique<SynapsePool>(synapseInitial, synapseMax);
    }
    
    /**
     * @brief Get statistics for all pools
     * 
     * @return std::string String representation of pool statistics
     */
    std::string getPoolStatistics() const {
        std::stringstream ss;
        ss << "PoolManager Statistics:\n";
        ss << "  Neurons: " << neuronPool_->getAllocatedCount() << 
              "/" << neuronPool_->getTotalAllocatedCount() << " allocated, " <<
              neuronPool_->getFreeCount() << " free\n";
        ss << "  Synapses: " << synapsePool_->getAllocatedCount() << 
              "/" << synapsePool_->getTotalAllocatedCount() << " allocated, " <<
              synapsePool_->getFreeCount() << " free\n";
        ss << "  Total Objects: " << 
              (neuronPool_->getAllocatedCount() + synapsePool_->getAllocatedCount()) << 
              "/" << (neuronPool_->getTotalAllocatedCount() + synapsePool_->getTotalAllocatedCount());
        return ss.str();
    }
    
    /**
     * @brief Clear all pools and release all objects
     */
    void clearAllPools() {
        neuronPool_->clear();
        synapsePool_->clear();
    }
    
    /**
     * @brief Check if pools are full
     */
    bool arePoolsFull() const {
        return neuronPool_->isFull() || synapsePool_->isFull();
    }
    
    /**
     * @brief Check if pools have available capacity
     */
    bool haveAvailableCapacity() const {
        return neuronPool_->hasAvailable() && synapsePool_->hasAvailable();
    }

private:
    std::unique_ptr<NeuronPool> neuronPool_;    // Neuron object pool
    std::unique_ptr<SynapsePool> synapsePool_;   // Synapse object pool
};

} // namespace nlm
