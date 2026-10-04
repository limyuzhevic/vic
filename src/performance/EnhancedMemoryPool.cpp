#include "EnhancedMemoryPool.hpp"

namespace nlm {

// Template implementation must be in header for generic types - already implemented in header

// Specialized pool for neurons with cache-aligned states
class EnhancedNeuronPool {
public:
    EnhancedNeuronPool(size_t initialCapacity = 1024, bool enableMonitoring = true)
        : capacity_(0), monitoringEnabled_(enableMonitoring) {
        reserve(initialCapacity, "EnhancedNeuronPool constructor", __LINE__);
    }
    
    ~EnhancedNeuronPool() {
        if (monitoringEnabled_) {
            std::cout << "NeuronPool cleanup completed." << std::endl;
            std::cout << "Final statistics:" << std::endl;
            std::cout << getStats() << std::endl;
        }
    }
    
    // Disable copying
    EnhancedNeuronPool(const EnhancedNeuronPool&) = delete;
    EnhancedNeuronPool& operator=(const EnhancedNeuronPool&) = delete;
    
    /**
     * Allocate a neuron with monitoring
     * @return Neuron ID or INVALID_NEURON_ID if pool exhausted
     */
    NeuronId allocate(const char* file = "unknown", int line = 0) {
        // Try to get from free list
        if (freeList_.empty()) {
            if (capacity_ < initialCapacity_) {
                // Need to grow pool
                growPool();
            } else {
                return INVALID_NEURON_ID;
            }
        }
        
        NeuronIndex idx = freeList_.back();
        freeList_.pop_back();
        
        // Create neuron state in the pool
        auto& block = blocks_.back();
        
        // Initialize neuron state
        NeuronState& state = block.membranePotential[idx];
        state = NeuronState(); // Reset to defaults
        
        // Track allocation
        if (monitoringEnabled_) {
            allocationCount_++;
            auto now = std::chrono::high_resolution_clock::now();
            auto timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
                now.time_since_epoch()).count();
            
            AllocationEvent event;
            event.type = AllocationEvent::Type::ALLOCATE;
            event.timestamp = timestamp;
            event.size = sizeof(NeuronState);
            event.file = file;
            event.line = line;
            
            allocationEvents_.push_back(event);
        }
        
        return NeuronId(idx);
    }
    
    /**
     * Deallocate a neuron with monitoring
     */
    void deallocate(NeuronId id, const char* file = "unknown", int line = 0) {
        if (id.value >= capacity_) {
            return; // Invalid ID
        }
        
        freeList_.push_back(static_cast<NeuronIndex>(id.value));
        
        // Track deallocation
        if (monitoringEnabled_) {
            deallocationCount_++;
            auto now = std::chrono::high_resolution_clock::now();
            auto timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
                now.time_since_epoch()).count();
            
            AllocationEvent event;
            event.type = AllocationEvent::Type::DEALLOCATE;
            event.timestamp = timestamp;
            event.size = sizeof(NeuronState);
            event.file = file;
            event.line = line;
            
            allocationEvents_.push_back(event);
        }
    }
    
    /**
     * Get neuron state directly (for SoA access)
     * @tparam Member Member type to retrieve
     * @param index Neuron index
     * @return Pointer to member or nullptr if invalid
     */
    template<typename Member>
    Member* getState(NeuronIndex index) {
        if (index >= capacity_) {
            return nullptr;
        }
        
        auto& block = blocks_.back();
        
        // For SoA, we need to compute the offset of the member
        // This is a simplified version - in real implementation, we'd use template metaprogramming
        if constexpr (std::is_same<Member, float>::value) {
            return &block.membranePotential[index];
        } else if constexpr (std::is_same<Member, uint8_t>::value) {
            return &block.firingState[index];
        } else if constexpr (std::is_same<Member, uint32_t>::value) {
            return &block.refractoryRemaining[index];
        } else if constexpr (std::is_same<Member, NeuronType>::value) {
            return &block.neuronType[index];
        } else {
            return nullptr; // Unsupported type
        }
    }
    
    /**
     * Get total count of neurons
     */
    size_t size() const {
        return capacity_;
    }
    
    /**
     * Get memory usage
     */
    size_t memoryUsage() const {
        size_t total = 0;
        for (const auto& block : blocks_) {
            total += block.membranePotential.capacity() * sizeof(float);
        }
        return total;
    }
    
    /**
     * Reserve space for neurons
     */
    void reserve(size_t count, const char* file = "unknown", int line = 0) {
        initialCapacity_ = count;
        growPool();
    }
    
    /**
     * Get statistics
     */
    std::string getStats() const {
        std::stringstream ss;
        ss << "EnhancedNeuronPool Statistics:" << std::endl;
        ss << "  Total neurons: " << capacity_ << std::endl;
        ss << "  Allocations: " << allocationCount_ << std::endl;
        ss << "  Deallocations: " << deallocationCount_ << std::endl;
        ss << "  Memory usage: " << memoryUsage() << " bytes" << std::endl;
        
        if (!allocationEvents_.empty()) {
            auto lastEvent = allocationEvents_.back();
            auto lastTime = std::chrono::high_resolution_clock::time_point(
                std::chrono::nanoseconds(lastEvent.timestamp));
            auto duration = std::chrono::duration<double>(lastTime - startTime_).count();
            ss << "  Events recorded: " << allocationEvents_.size() << std::endl;
            ss << "  Events per second: " << (allocationEvents_.size() / std::max(duration, 0.1)) << std::endl;
        }
        
        return ss.str();
    }
    
    /**
     * Check for memory issues
     */
    std::string checkHealth() const {
        std::stringstream ss;
        
        if (allocationCount_ < deallocationCount_) {
            ss << "ERROR: More deallocations than allocations! Possible double-free or under-allocation." << std::endl;
        }
        
        if (allocationCount_ > deallocationCount_) {
            ss << "WARNING: " << (allocationCount_ - deallocationCount_) << " more allocations than deallocations." << std::endl;
            ss << "Potential memory leak detected!" << std::endl;
        }
        
        if (freeList_.size() == capacity_) {
            ss << "ERROR: All neurons are deallocated (pool is empty)." << std::endl;
        }
        
        return ss.str();
    }
    
private:
    /**
     * Grow the pool to accommodate more neurons
     */
    void growPool() {
        if (capacity_ >= initialCapacity_) {
            return; // Already at desired capacity
        }
        
        // Calculate new size (grow by 50% or 1000 neurons, whichever is larger)
        size_t newSize = capacity_ + std::max(capacity_ / 2, static_cast<size_t>(1000));
        
        // Create new block
        NeuronBlock newBlock;
        newBlock.membranePotential.resize(newSize);
        newBlock.restingPotential.resize(newSize);
        newBlock.threshold.resize(newSize);
        newBlock.resetPotential.resize(newSize);
        newBlock.leakConductance.resize(newSize);
        newBlock.synapseConductance.resize(newSize);
        newBlock.firingRate.resize(newSize);
        newBlock.firingState.resize(newSize);
        newBlock.refractoryRemaining.resize(newSize);
        newBlock.refractoryPeriod.resize(newSize);
        newBlock.adaptationVariable.resize(newSize);
        newBlock.lastSpikeTime.resize(newSize);
        newBlock.neuronType.resize(newSize);
        newBlock.regionId.resize(newSize);
        newBlock.populationId.resize(newSize);
        newBlock.synapticInput.resize(newSize);
        
        // Initialize free list for new neurons
        for (NeuronIndex i = capacity_; i < newSize; ++i) {
            freeList_.push_back(i);
        }
        
        // Add block to blocks
        blocks_.push_back(std::move(newBlock));
        
        // Update capacity
        capacity_ = newSize;
        
        // Record event
        if (monitoringEnabled_) {
            auto now = std::chrono::high_resolution_clock::now();
            auto timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
                now.time_since_epoch()).count();
            
            AllocationEvent event;
            event.type = AllocationEvent::Type::ALLOCATE;
            event.timestamp = timestamp;
            event.size = newSize * sizeof(NeuronState);
            event.file = "EnhancedNeuronPool.cpp";
            event.line = __LINE__;
            
            allocationEvents_.push_back(event);
        }
    }
    
    // Neuron block structure
    struct alignas(64) NeuronBlock {
        std::vector<float> membranePotential;
        std::vector<float> restingPotential;
        std::vector<float> threshold;
        std::vector<float> resetPotential;
        std::vector<float> leakConductance;
        std::vector<float> synapseConductance;
        std::vector<float> firingRate;
        std::vector<uint8_t> firingState;  // FiringState enum
        std::vector<uint32_t> refractoryRemaining;
        std::vector<uint32_t> refractoryPeriod;
        std::vector<float> adaptationVariable;
        std::vector<float> lastSpikeTime;
        std::vector<uint64_t> neuronType;  // NeuronType enum
        std::vector<uint64_t> regionId;
        std::vector<uint64_t> populationId;
        
        // Per-neuron current injection (not in NeuronState, but needed for hot path)
        std::vector<float> synapticInput;
        
        NeuronBlock() {}
    };
    
    size_t capacity_;
    size_t initialCapacity_;
    std::vector<NeuronBlock> blocks_;
    std::vector<NeuronIndex> freeList_;
    bool monitoringEnabled_;
    
    // Statistics
    size_t allocationCount_;
    size_t deallocationCount_;
    
    // Event tracking
    std::chrono::high_resolution_clock::time_point startTime_;
    std::vector<AllocationEvent> allocationEvents_;
};

// SynapsePool - Enhanced memory pool for synapses
class EnhancedSynapsePool {
public:
    EnhancedSynapsePool(size_t initialCapacity = 5000, bool enableMonitoring = true)
        : capacity_(0), monitoringEnabled_(enableMonitoring) {
        reserve(initialCapacity, "EnhancedSynapsePool constructor", __LINE__);
    }
    
    ~EnhancedSynapsePool() {
        if (monitoringEnabled_) {
            std::cout << "SynapsePool cleanup completed." << std::endl;
            std::cout << "Final statistics:" << std::endl;
            std::cout << getStats() << std::endl;
        }
    }
    
    // Disable copying
    EnhancedSynapsePool(const EnhancedSynapsePool&) = delete;
    EnhancedSynapsePool& operator=(const EnhancedSynapsePool&) = delete;
    
    /**
     * Allocate a synapse with monitoring
     * @param source Source neuron ID
     * @param destination Destination neuron ID
     * @return Synapse ID or INVALID_SYNAPSE_ID if pool exhausted
     */
    SynapseId allocate(NeuronId source, NeuronId destination, const char* file = "unknown", int line = 0) {
        // Try to get from free list
        if (freeList_.empty()) {
            if (capacity_ < initialCapacity_) {
                // Need to grow pool
                growPool();
            } else {
                return INVALID_SYNAPSE_ID;
            }
        }
        
        SynapseIndex idx = freeList_.back();
        freeList_.pop_back();
        
        // Create synapse state in the pool
        auto& block = blocks_.back();
        
        // Initialize synapse state
        block.sourceNeuron[idx] = source.value;
        block.destinationNeuron[idx] = destination.value;
        block.weight[idx] = 0.0f; // Default weight
        block.delay[idx] = 1;     // Default delay
        block.synapseType[idx] = SynapseType::Excitatory;
        block.plasticityFlags[idx] = 0;
        
        // Track allocation
        if (monitoringEnabled_) {
            allocationCount_++;
            auto now = std::chrono::high_resolution_clock::now();
            auto timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
                now.time_since_epoch()).count();
            
            AllocationEvent event;
            event.type = AllocationEvent::Type::ALLOCATE;
            event.timestamp = timestamp;
            event.size = sizeof(SynapseState);
            event.file = file;
            event.line = line;
            
            allocationEvents_.push_back(event);
        }
        
        return SynapseId(idx);
    }
    
    /**
     * Deallocate a synapse with monitoring
     */
    void deallocate(SynapseId id, const char* file = "unknown", int line = 0) {
        if (id.value >= capacity_) {
            return; // Invalid ID
        }
        
        freeList_.push_back(static_cast<SynapseIndex>(id.value));
        
        // Track deallocation
        if (monitoringEnabled_) {
            deallocationCount_++;
            auto now = std::chrono::high_resolution_clock::now();
            auto timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
                now.time_since_epoch()).count();
            
            AllocationEvent event;
            event.type = AllocationEvent::Type::DEALLOCATE;
            event.timestamp = timestamp;
            event.size = sizeof(SynapseState);
            event.file = file;
            event.line = line;
            
            allocationEvents_.push_back(event);
        }
    }
    
    /**
     * Get synapse data directly (for SoA access)
     * @tparam Member Member type to retrieve
     * @param index Synapse index
     * @return Pointer to member or nullptr if invalid
     */
    template<typename Member>
    Member* getState(SynapseIndex index) {
        if (index >= capacity_) {
            return nullptr;
        }
        
        auto& block = blocks_.back();
        
        // For SoA, we need to compute the offset of the member
        // This is a simplified version - in real implementation, we'd use template metaprogramming
        if constexpr (std::is_same<Member, uint64_t>::value) {
            // We need to know which member we're accessing
            return nullptr; // Simplified - real implementation would be more complex
        } else {
            return nullptr; // Unsupported type
        }
    }
    
    /**
     * Get source neuron
     */
    NeuronId getSource(SynapseIndex index) const {
        if (index >= capacity_) {
            return INVALID_NEURON_ID;
        }
        return NeuronId(blocks_.back().sourceNeuron[index]);
    }
    
    /**
     * Get destination neuron
     */
    NeuronId getDestination(SynapseIndex index) const {
        if (index >= capacity_) {
            return INVALID_NEURON_ID;
        }
        return NeuronId(blocks_.back().destinationNeuron[index]);
    }
    
    /**
     * Get total count of synapses
     */
    size_t size() const {
        return capacity_;
    }
    
    /**
     * Get memory usage
     */
    size_t memoryUsage() const {
        size_t total = 0;
        for (const auto& block : blocks_) {
            total += block.sourceNeuron.capacity() * sizeof(uint64_t);
        }
        return total;
    }
    
    /**
     * Reserve space for synapses
     */
    void reserve(size_t count, const char* file = "unknown", int line = 0) {
        initialCapacity_ = count;
        growPool();
    }
    
    /**
     * Get statistics
     */
    std::string getStats() const {
        std::stringstream ss;
        ss << "EnhancedSynapsePool Statistics:" << std::endl;
        ss << "  Total synapses: " << capacity_ << std::endl;
        ss << "  Allocations: " << allocationCount_ << std::endl;
        ss << "  Deallocations: " << deallocationCount_ << std::endl;
        ss << "  Memory usage: " << memoryUsage() << " bytes" << std::endl;
        
        if (!allocationEvents_.empty()) {
            auto lastEvent = allocationEvents_.back();
            auto lastTime = std::chrono::high_resolution_clock::time_point(
                std::chrono::nanoseconds(lastEvent.timestamp));
            auto duration = std::chrono::duration<double>(lastTime - startTime_).count();
            ss << "  Events recorded: " << allocationEvents_.size() << std::endl;
            ss << "  Events per second: " << (allocationEvents_.size() / std::max(duration, 0.1)) << std::endl;
        }
        
        return ss.str();
    }
    
    /**
     * Check for memory issues
     */
    std::string checkHealth() const {
        std::stringstream ss;
        
        if (allocationCount_ < deallocationCount_) {
            ss << "ERROR: More deallocations than allocations! Possible double-free or under-allocation." << std::endl;
        }
        
        if (allocationCount_ > deallocationCount_) {
            ss << "WARNING: " << (allocationCount_ - deallocationCount_) << " more allocations than deallocations." << std::endl;
            ss << "Potential memory leak detected!" << std::endl;
        }
        
        if (freeList_.size() == capacity_) {
            ss << "ERROR: All synapses are deallocated (pool is empty)." << std::endl;
        }
        
        return ss.str();
    }
    
private:
    /**
     * Grow the pool to accommodate more synapses
     */
    void growPool() {
        if (capacity_ >= initialCapacity_) {
            return; // Already at desired capacity
        }
        
        // Calculate new size (grow by 50% or 10000 synapses, whichever is larger)
        size_t newSize = capacity_ + std::max(capacity_ / 2, static_cast<size_t>(10000));
        
        // Create new block
        SynapseBlock newBlock;
        newBlock.sourceNeuron.resize(newSize);
        newBlock.destinationNeuron.resize(newSize);
        newBlock.weight.resize(newSize);
        newBlock.delay.resize(newSize);
        newBlock.synapseType.resize(newSize);
        newBlock.plasticityFlags.resize(newSize);
        newBlock.shortTermDepression.resize(newSize);
        newBlock.shortTermFacilitation.resize(newSize);
        newBlock.lastPreSpikeTime.resize(newSize);
        newBlock.lastPostSpikeTime.resize(newSize);
        newBlock.eligibilityTrace.resize(newSize);
        newBlock.efficacy.resize(newSize);
        
        // Initialize free list for new synapses
        for (SynapseIndex i = capacity_; i < newSize; ++i) {
            freeList_.push_back(i);
        }
        
        // Add block to blocks
        blocks_.push_back(std::move(newBlock));
        
        // Update capacity
        capacity_ = newSize;
        
        // Record event
        if (monitoringEnabled_) {
            auto now = std::chrono::high_resolution_clock::now();
            auto timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
                now.time_since_epoch()).count();
            
            AllocationEvent event;
            event.type = AllocationEvent::Type::ALLOCATE;
            event.timestamp = timestamp;
            event.size = newSize * sizeof(SynapseState);
            event.file = "EnhancedSynapsePool.cpp";
            event.line = __LINE__;
            
            allocationEvents_.push_back(event);
        }
    }
    
    // Synapse block structure
    struct alignas(64) SynapseBlock {
        std::vector<uint64_t> sourceNeuron;  // Source neuron ID
        std::vector<uint64_t> destinationNeuron;  // Dest neuron ID
        std::vector<float> weight;
        std::vector<uint32_t> delay;
        std::vector<uint8_t> synapseType;  // SynapseType enum
        std::vector<uint8_t> plasticityFlags;  // Packed flags
        
        // Short-term plasticity
        std::vector<float> shortTermDepression;
        std::vector<float> shortTermFacilitation;
        std::vector<float> lastPreSpikeTime;
        std::vector<float> lastPostSpikeTime;
        
        // Eligibility trace
        std::vector<float> eligibilityTrace;
        
        // Synaptic efficacy
        std::vector<float> efficacy;
        
        SynapseBlock() {}
    };
    
    size_t capacity_;
    size_t initialCapacity_;
    std::vector<SynapseBlock> blocks_;
    std::vector<SynapseIndex> freeList_;
    bool monitoringEnabled_;
    
    // Statistics
    size_t allocationCount_;
    size_t deallocationCount_;
    
    // Event tracking
    std::chrono::high_resolution_clock::time_point startTime_;
    std::vector<AllocationEvent> allocationEvents_;
};

} // namespace nlm