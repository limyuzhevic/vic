// SIMDVectorization.cpp implementation

#include "performance/SIMDVectorization.hpp"
#include <cstring>
#include <cmath>
#include <algorithm>

namespace nlm {

// SIMD implementation for neuron state operations

void SIMDVectorization::addNeuronStates(NeuronState* dest, const NeuronState* src1, 
                                       const NeuronState* src2, size_t count) {
#ifdef NLM_USE_SIMD
    if constexpr (NLM_SIMD_WIDTH > 1) {
        // Use SIMD for processing multiple neurons
        size_t i = 0;
        
        // Process SIMD-width chunks
        for (; i + NLM_SIMD_WIDTH <= count; i += NLM_SIMD_WIDTH) {
            // Process membranePotential
            for (size_t j = 0; j < NLM_SIMD_WIDTH; ++j) {
                dest[i + j].membranePotential += src1[i + j].membranePotential + src2[i + j].membranePotential;
            }
            
            // Process other fields similarly...
            // Note: In practice, NeuronState would use SoA layout for true SIMD
        }
        
        // Process remaining elements
        for (; i < count; ++i) {
            dest[i].membranePotential = src1[i].membranePotential + src2[i].membranePotential;
        }
    } else {
        // Scalar fallback
        for (size_t i = 0; i < count; ++i) {
            dest[i].membranePotential = src1[i].membranePotential + src2[i].membranePotential;
        }
    }
#else
    // Scalar implementation
    for (size_t i = 0; i < count; ++i) {
        dest[i].membranePotential = src1[i].membranePotential + src2[i].membranePotential;
    }
#endif
}

void SIMDVectorization::neuronStateCopy(NeuronState* dest, const NeuronState* src, size_t count) {
#ifdef NLM_USE_SOA
    // Structure-of-Arrays implementation for cache efficiency
    // This would separate the state into contiguous arrays
    // For now, use simple copy
#endif
    
    for (size_t i = 0; i < count; ++i) {
        dest[i] = src[i];
    }
}

void SIMDVectorization::neuronStateZero(NeuronState* dest, size_t count) {
#ifdef NLM_USE_SIMD
    // SIMD zero operation
#endif
    
    for (size_t i = 0; i < count; ++i) {
        dest[i] = NeuronState{};  // Use default constructor
    }
}

float SIMDVectorization::computeTotalFiringRate(const NeuronState* states, size_t count) {
#ifdef NLM_USE_SIMD
    // SIMD sum of firing rates
    float total = 0.0f;
    size_t i = 0;
    
    for (; i + NLM_SIMD_WIDTH <= count; i += NLM_SIMD_WIDTH) {
        for (size_t j = 0; j < NLM_SIMD_WIDTH; ++j) {
            total += states[i + j].firingRate;
        }
    }
    
    for (; i < count; ++i) {
        total += states[i].firingRate;
    }
    
    return total;
#else
    float total = 0.0f;
    for (size_t i = 0; i < count; ++i) {
        total += states[i].firingRate;
    }
    return total;
#endif
}

float SIMDVectorization::computeExcitatoryInhibitionRatio(const NeuronState* states, size_t count) {
    float excitatorySum = 0.0f;
    float inhibitorySum = 0.0f;
    
    for (size_t i = 0; i < count; ++i) {
        if (states[i].neuronType == NeuronType::Excitatory) {
            excitatorySum += states[i].firingRate;
        } else if (states[i].neuronType == NeuronType::Inhibitory) {
            inhibitorySum += states[i].firingRate;
        }
    }
    
    if (excitatorySum + inhibitorySum == 0.0f) {
        return 1.0f;
    }
    
    return excitatorySum / (excitatorySum + inhibitorySum);
}

bool SIMDVectorization::neuronStateEquals(const NeuronState& a, const NeuronState& b) {
    return a.membranePotential == b.membranePotential &&
           a.restingPotential == b.restingPotential &&
           a.threshold == b.threshold &&
           a.resetPotential == b.resetPotential &&
           a.firingState == b.firingState;
}

// SIMD implementation for spike propagation

void SIMDVectorization::processSpikeTargets(Neuron* source, const std::vector<Synapse*>& synapses,
                                           float currentTime, size_t maxDelay) {
    // Process spike delivery in batches
    size_t processed = 0;
    size_t batchSize = SIMD_ENABLE ? SIMD_VECTOR_WIDTH : 1;
    
    while (processed < synapses.size()) {
        size_t end = std::min(processed + batchSize, synapses.size());
        
        // Deliver spikes to postsynaptic neurons
        for (size_t i = processed; i < end; ++i) {
            Synapse* syn = synapses[i];
            Neuron* target = syn->getTarget();
            
            if (target && syn->isActive()) {
                float delay = syn->getDelay();
                float weight = syn->getWeight();
                
                if (delay <= maxDelay) {
                    target->receiveSpike(source->getId(), syn->getId(), weight, delay, currentTime);
                }
            }
        }
        
        processed = end;
    }
}

void SIMDVectorization::applyPlasticityUpdates(const std::vector<Synapse*>& synapses, 
                                               float currentTime, float learningRate) {
    size_t processed = 0;
    size_t batchSize = SIMD_ENABLE ? SIMD_VECTOR_WIDTH : 1;
    
    while (processed < synapses.size()) {
        size_t end = std::min(processed + batchSize, synapses.size());
        
        // Apply STDP, Hebbian, and other plasticity rules
        for (size_t i = processed; i < end; ++i) {
            Synapse* syn = synapses[i];
            if (syn->isPlastic()) {
                syn->updatePlasticity(currentTime, learningRate);
            }
        }
        
        processed = end;
    }
}

void SIMDVectorization::runParallelNeurons(size_t neuronStart, size_t neuronEnd,
                                          std::function<void(size_t, NeuronState&)> neuronFunc) {
    // Execute neuron processing in parallel with SIMD optimization
    size_t count = neuronEnd - neuronStart;
    
    if (SIMD_ENABLE) {
        // Process in SIMD chunks
        size_t i = 0;
        for (; i + SIMD_VECTOR_WIDTH <= count; i += SIMD_VECTOR_WIDTH) {
            for (size_t j = 0; j < SIMD_VECTOR_WIDTH; ++j) {
                size_t idx = neuronStart + i + j;
                neuronFunc(idx, neuronStates_[idx]);
            }
        }
        
        // Process remaining elements
        for (; i < count; ++i) {
            size_t idx = neuronStart + i;
            neuronFunc(idx, neuronStates_[idx]);
        }
    } else {
        // Scalar processing
        for (size_t i = neuronStart; i < neuronEnd; ++i) {
            neuronFunc(i, neuronStates_[i]);
        }
    }
}

// Memory pool implementations

template<typename T>
MemoryPool<T>::MemoryPool(size_t blockSize, size_t initialBlocks)
    : slabSize_(sizeof(T))
    , blockSize_(blockSize)
    , freeList_(nullptr)
    , currentBlock_(nullptr)
    , currentIndex_(0)
    , allocatedCount_(0)
    , availableCount_(0) {
    // Align slab size to cache line (64 bytes) for false sharing prevention
    constexpr size_t cacheLineSize = 64;
    if (slabSize_ % cacheLineSize != 0) {
        slabSize_ = ((slabSize_ / cacheLineSize) + 1) * cacheLineSize;
    }
    
    // Pre-allocate initial blocks
    for (size_t i = 0; i < initialBlocks; ++i) {
        addBlock();
    }
}

template<typename T>
MemoryPool<T>::~MemoryPool() {
    // Blocks are automatically freed via unique_ptr
}

template<typename T>
void MemoryPool<T>::addBlock() {
    auto block = std::make_unique<Block>(slabSize_, blockSize_);
    
    // Chain existing blocks
    Block* expected = nullptr;
    if (!currentBlock_.compare_exchange_strong(expected, block.get())) {
        // Another thread already added a block, add to chain
        block->nextBlock = expected;
    } else {
        block.release();
    }
    
    // Initialize free list for this block
    Block* b = block.get();
    for (size_t i = 0; i < blockSize_; ++i) {
        T* obj = b->getObject(i, slabSize_);
        MemPoolNode* node = reinterpret_cast<MemPoolNode*>(obj);
        node->next.store(freeList_.load(std::memory_order_relaxed), std::memory_order_relaxed);
        freeList_.store(node, std::memory_order_relaxed);
        availableCount_.fetch_add(1, std::memory_order_relaxed);
    }
    
    blocks_.push_back(std::move(block));
}

template<typename T>
T* MemoryPool<T>::allocate() {
    // Try to get from free list first (lock-free pop)
    MemPoolNode* node = freeList_.load(std::memory_order_acquire);
    
    while (node != nullptr) {
        MemPoolNode* next = node->next.load(std::memory_order_relaxed);
        if (freeList_.compare_exchange_weak(node, next, 
                                             std::memory_order_release, 
                                             std::memory_order_relaxed)) {
            allocatedCount_.fetch_add(1, std::memory_order_relaxed);
            availableCount_.fetch_sub(1, std::memory_order_relaxed);
            return reinterpret_cast<T*>(node);
        }
        // node was updated by CAS, reload next
    }
    
    // Free list empty, add a new block
    addBlock();
    
    // Try again
    return allocate();
}

template<typename T>
void MemoryPool<T>::deallocate(T* obj) {
    if (obj == nullptr) return;
    
    MemPoolNode* node = reinterpret_cast<MemPoolNode*>(obj);
    
    // Lock-free push to free list
    MemPoolNode* current = freeList_.load(std::memory_order_relaxed);
    do {
        node->next.store(current, std::memory_order_relaxed);
    } while (!freeList_.compare_exchange_weak(current, node,
                                             std::memory_order_release,
                                             std::memory_order_relaxed));
    
    allocatedCount_.fetch_sub(1, std::memory_order_relaxed);
    availableCount_.fetch_add(1, std::memory_order_relaxed);
}

template<typename T>
size_t MemoryPool<T>::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        total += block->data.capacity() * sizeof(uint8_t);
    }
    return total;
}

template<typename T>
void MemoryPool<T>::reserve(size_t count) {
    while (availableCount_.load(std::memory_order_relaxed) < count) {
        addBlock();
    }
}

// NeuronPool implementation

NeuronPool::NeuronPool()
    : size_(0)
    , capacity_(0) {
    // Start with one block for initialization
    addBlock();
}

NeuronPool::~NeuronPool() = default;

NeuronId NeuronPool::allocate() {
    if (freeList_.empty()) {
        addBlock();
    }
    
    size_t idx = freeList_[freeListTop_.fetch_add(1, std::memory_order_relaxed)];
    
    // Initialize the neuron state
    NeuronBlock* block = blocks_[idx / blockSize_].get();
    size_t blockIdx = idx % blockSize_;
    
    // Mark as allocated (remove from free list)
    // Note: This is simplified - real implementation would use atomic flags
    
    size_++;
    return NeuronId(idx);
}

void NeuronPool::deallocate(NeuronId id) {
    size_t idx = id.getId();
    
    if (idx < freeList_.size()) {
        // Add to free list
        freeList_[freeListTop_.fetch_sub(1, std::memory_order_relaxed)] = idx;
        size_--;
    }
}

template<typename Member>
Member* NeuronPool::getState(NeuronIndex index) {
    size_t blockIdx = index / blockSize_;
    size_t offset = index % blockSize_;
    
    if (blockIdx >= blocks_.size()) {
        return nullptr;
    }
    
    return reinterpret_cast<Member*>(
        blocks_[blockIdx]->getObject(offset, slabSize_)
    );
}

size_t NeuronPool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        total += block->membranePotential.capacity() * sizeof(float);
        total += block->restingPotential.capacity() * sizeof(float);
        total += block->threshold.capacity() * sizeof(float);
        // Add other fields as needed
    }
    return total;
}

// SynapsePool implementation

SynapsePool::SynapsePool() : size_(0), capacity_(0) {
    addBlock();
}

SynapsePool::~SynapsePool() = default;

SynapseId SynapsePool::allocate(NeuronId source, NeuronId destination) {
    if (freeList_.empty()) {
        addBlock();
    }
    
    size_t idx = freeList_[freeListTop_.fetch_add(1, std::memory_order_relaxed)];
    
    // Initialize the synapse data
    SynapseBlock* block = blocks_[idx / blockSize_].get();
    size_t blockIdx = idx % blockSize_;
    
    // Store source and destination
    block->sourceNeuron[blockIdx] = source;
    block->destinationNeuron[blockIdx] = destination;
    
    size_++;
    return SynapseId(idx);
}

void SynapsePool::deallocate(SynapseId id) {
    size_t idx = id.getId();
    
    if (idx < freeList_.size()) {
        freeList_[freeListTop_.fetch_sub(1, std::memory_order_relaxed)] = idx;
        size_--;
    }
}

template<typename Member>
Member* SynapsePool::getState(SynapseIndex index) {
    size_t blockIdx = index / blockSize_;
    size_t offset = index % blockSize_;
    
    if (blockIdx >= blocks_.size()) {
        return nullptr;
    }
    
    return reinterpret_cast<Member*>(
        blocks_[blockIdx]->getObject(offset, slabSize_)
    );
}

NeuronId SynapsePool::getSource(SynapseIndex index) const {
    size_t blockIdx = index / blockSize_;
    size_t offset = index % blockSize_;
    
    if (blockIdx < blocks_.size()) {
        return blocks_[blockIdx]->sourceNeuron[offset];
    }
    return NeuronId();
}

NeuronId SynapsePool::getDestination(SynapseIndex index) const {
    size_t blockIdx = index / blockSize_;
    size_t offset = index % blockSize_;
    
    if (blockIdx < blocks_.size()) {
        return blocks_[blockIdx]->destinationNeuron[offset];
    }
    return NeuronId();
}

size_t SynapsePool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        total += block->sourceNeuron.capacity() * sizeof(uint64_t);
        total += block->destinationNeuron.capacity() * sizeof(uint64_t);
        total += block->weight.capacity() * sizeof(float);
        // Add other fields as needed
    }
    return total;
}

// OptimizedBrain implementations

OptimizedBrain::OptimizedBrain(std::shared_ptr<Config> config)
    : Brain(config) {
    enablePerformanceFeatures();
}

OptimizedBrain::~OptimizedBrain() {
    disablePerformanceFeatures();
}

void OptimizedBrain::enablePerformanceFeatures() {
#ifdef NLM_PERFORMANCE_ENABLED
    // Allocate memory pools for neurons and synapses
    neuronPool_ = std::make_unique<NeuronPool>("neuron", neuronPoolSize_);
    synapsePool_ = std::make_unique<SynapsePool>("synapse", synapsePoolSize_);
    
    // Initialize event queues
    spikeQueue_ = std::make_unique<SpikeRingBuffer>(queueSize_);
    delayedSpikeQueue_ = std::make_unique<DelayedSpikeQueue>(delayBuckets_, maxDelaySteps_);
    
    // Enable SIMD if available
    if constexpr (NLM_USE_SIMD) {
        SIMDState_ = std::make_unique<SIMDVectorization>();
    }
    
    // Configure parallel processing
    if constexpr (NLM_USE_MULTITHREADING) {
        parallelProcessor_ = std::make_unique<ParallelNeuralProcessor>(threadCount_);
    }
#endif
}

void OptimizedBrain::disablePerformanceFeatures() {
    // Clean up performance structures
    neuronPool_.reset();
    synapsePool_.reset();
    spikeQueue_.reset();
    delayedSpikeQueue_.reset();
    SIMDState_.reset();
    parallelProcessor_.reset();
}

NeuronId OptimizedBrain::addNeuron() {
#ifdef NLM_PERFORMANCE_ENABLED
    return neuronPool_->allocate();
#else
    return Brain::addNeuron();
#endif
}

void OptimizedBrain::removeNeuron(NeuronId id) {
#ifdef NLM_PERFORMANCE_ENABLED
    neuronPool_->deallocate(id);
#else
    Brain::removeNeuron(id);
#endif
}

SynapseId OptimizedBrain::addSynapse(NeuronId source, NeuronId destination, float weight) {
#ifdef NLM_PERFORMANCE_ENABLED
    return synapsePool_->allocate(source, destination);
#else
    return Brain::addSynapse(source, destination, weight);
#endif
}

void OptimizedBrain::removeSynapse(SynapseId id) {
#ifdef NLM_PERFORMANCE_ENABLED
    synapsePool_->deallocate(id);
#else
    Brain::removeSynapse(id);
#endif
}

// Event handling with performance optimizations

void OptimizedBrain::processSpikeEvents() {
#ifdef NLM_PERFORMANCE_ENABLED
    // Use event queue for spike processing
    auto spikes = spikeQueue_->popBatch();
    
    if (spikes.empty()) {
        return;
    }
    
    // Process spikes in parallel
    size_t processed = 0;
    size_t batchSize = SIMD_ENABLE ? SIMD_VECTOR_WIDTH : 1;
    
    while (processed < spikes.size()) {
        size_t end = std::min(processed + batchSize, spikes.size());
        
        // Deliver spikes to targets
        for (size_t i = processed; i < end; ++i) {
            const SpikeEvent& spike = spikes[i];
            processSpikeDelivery(spike.source, spike.target, spike.weight, spike.delay);
        }
        
        processed = end;
    }
#else
    Brain::processSpikeEvents();
#endif
}

void OptimizedBrain::advanceTimeStep() {
    Brain::advanceTimeStep();
    
#ifdef NLM_PERFORMANCE_ENABLED
    // Advance delayed spike queue
    delayedSpikeQueue_->advanceTime(currentStep_);
    
    // Process any scheduled events
    auto events = priorityQueue_->popDueEvents(currentStep_);
    for (const auto& event : events) {
        processPriorityEvent(event);
    }
#endif
}

// Performance monitoring and metrics

void OptimizedBrain::enableProfiling() {
#ifdef NLM_PERFORMANCE_ENABLED
    profilingEnabled_ = true;
    startTime_ = std::chrono::high_resolution_clock::now();
    stepCount_ = 0;
    
    // Reset performance counters
    totalAllocations_ = 0;
    totalDeallocations_ = 0;
    totalSpikeEvents_ = 0;
    totalPlasticityUpdates_ = 0;
#endif
}

BrainPerformanceMetrics OptimizedBrain::getPerformanceMetrics() const {
    BrainPerformanceMetrics metrics;
    
#ifdef NLM_PERFORMANCE_ENABLED
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime_);
    
    metrics.totalTime = duration.count();
    metrics.stepsPerSecond = (stepCount_ > 0) ? stepCount_ / duration.count() : 0.0;
    metrics.averageSpikeRate = (stepCount_ > 0) ? totalSpikeEvents_ / stepCount_ : 0.0;
    metrics.averagePlasticityUpdates = (stepCount_ > 0) ? totalPlasticityUpdates_ / stepCount_ : 0.0;
    metrics.memoryUsage = getMemoryUsage();
    metrics.activeThreads = getActiveThreads();
    metrics.cacheEfficiency = getCacheEfficiency();
    
    // Memory pool statistics
    metrics.neuronPoolStats.allocations = neuronPool_->getAllocations();
    metrics.neuronPoolStats.deallocations = neuronPool_->getDeallocations();
    metrics.neuronPoolStats.currentUsage = neuronPool_->getSize();
    metrics.neuronPoolStats.capacity = neuronPool_->getCapacity();
    
    metrics.synapsePoolStats.allocations = synapsePool_->getAllocations();
    metrics.synapsePoolStats.deallocations = synapsePool_->getDeallocations();
    metrics.synapsePoolStats.currentUsage = synapsePool_->getSize();
    metrics.synapsePoolStats.capacity = synapsePool_->getCapacity();
#endif
    
    return metrics;
}

// Performance optimization methods

void OptimizedBrain::enableSIMD(bool enable) {
#ifdef NLM_USE_SIMD
    simdEnabled_ = enable;
#endif
}

void OptimizedBrain::enableMultithreading(bool enable) {
#ifdef NLM_USE_MULTITHREADING
    multithreadingEnabled_ = enable;
#endif
}

void OptimizedBrain::setCacheLineSize(size_t size) {
    cacheLineSize_ = size;
}

void OptimizedBrain::enableEventDriven(bool enable) {
#ifdef NLM_USE_EVENT_QUEUE
    eventDrivenEnabled_ = enable;
#endif
}

// EventQueue implementations

SpikeRingBuffer::SpikeRingBuffer(size_t capacity)
    : capacity_(capacity)
    , mask_(capacity - 1)  // capacity MUST be power of 2
    , buffer_(capacity_)
    , head_(0)
    , tail_(0) {
    assert((capacity & (capacity - 1)) == 0 && "SpikeRingBuffer capacity must be power of 2");
}

SpikeRingBuffer::~SpikeRingBuffer() = default;

bool SpikeRingBuffer::push(const QueuedSpikeEvent& event) {
    size_t tail = tail_.load(std::memory_order_relaxed);
    size_t nextTail = (tail + 1) & mask_;
    
    if (nextTail == head_.load(std::memory_order_acquire)) {
        return false;  // Queue full
    }
    
    buffer_[tail] = event;
    tail_.store(nextTail, std::memory_order_release);
    return true;
}

std::vector<QueuedSpikeEvent> SpikeRingBuffer::popBatch() {
    std::vector<QueuedSpikeEvent> result;
    result.reserve(std::min<size_t>(100, capacity_));  // Limit batch size
    
    while (true) {
        std::optional<QueuedSpikeEvent> event = pop();
        if (!event) break;
        
        result.push_back(std::move(*event));
        
        // Limit batch size to prevent long delays
        if (result.size() >= 100) break;
    }
    
    return result;
}

bool SpikeRingBuffer::empty() const {
    return head_.load(std::memory_order_acquire) == tail_.load(std::memory_order_acquire);
}

bool SpikeRingBuffer::full() const {
    size_t nextTail = (tail_.load(std::memory_order_acquire) + 1) & mask_;
    return nextTail == head_.load(std::memory_order_acquire);
}

size_t SpikeRingBuffer::size() const {
    size_t head = head_.load(std::memory_order_acquire);
    size_t tail = tail_.load(std::memory_order_acquire);
    return (tail - head) & mask_;
}

void SpikeRingBuffer::clear() {
    head_.store(0, std::memory_order_release);
    tail_.store(0, std::memory_order_release);
}

// EventQueue implementations (continued)

DelayedSpikeQueue::DelayedSpikeQueue(size_t numBuckets, size_t maxDelaySteps)
    : numBuckets_(numBuckets)
    , maxDelaySteps_(maxDelaySteps)
    , currentStep_(0)
    , oldestStep_(0)
    , buckets_(numBuckets)
    , pendingCounts_(numBuckets, 0) {
    assert(numBuckets >= maxDelaySteps && "numBuckets must be >= maxDelaySteps");
}

DelayedSpikeQueue::~DelayedSpikeQueue() = default;

void DelayedSpikeQueue::schedule(uint64_t sourceNeuron, uint64_t destNeuron,
                                 uint64_t synapseId, float weight,
                                 bool isExcitatory, uint64_t deliveryStep) {
    if (deliveryStep < currentStep_) return;  // Already due, skip
    
    size_t bucketIdx = deliveryStep % numBuckets_;
    QueuedSpikeEvent event(sourceNeuron, destNeuron, synapseId, weight, isExcitatory);
    
    if (buckets_[bucketIdx].push(event)) {
        pendingCounts_[bucketIdx].fetch_add(1, std::memory_order_relaxed);
    }
}

std::vector<QueuedSpikeEvent> DelayedSpikeQueue::getDueSpikes(uint64_t currentStep) {
    currentStep_ = currentStep;
    std::vector<QueuedSpikeEvent> result;
    
    // Process all buckets up to current step
    while (oldestStep_ <= currentStep) {
        size_t bucketIdx = oldestStep_ % numBuckets_;
        auto spikes = buckets_[bucketIdx].drain();
        result.insert(result.end(), spikes.begin(), spikes.end());
        pendingCounts_[bucketIdx].store(0, std::memory_order_relaxed);
        ++oldestStep_;
    }
    
    return result;
}

size_t DelayedSpikeQueue::pendingCount() const {
    size_t total = 0;
    for (size_t c : pendingCounts_) {
        total += c;
    }
    return total;
}

size_t DelayedSpikeQueue::bucketSize(uint64_t step) const {
    return buckets_[step % numBuckets_].size();
}

void DelayedSpikeQueue::clear() {
    for (auto& bucket : buckets_) {
        bucket = SpikeBucket();  // Reset bucket to default state
    }
    std::fill(pendingCounts_.begin(), pendingCounts_.end(), 0);
    oldestStep_ = currentStep_;
}

void DelayedSpikeQueue::advanceTime(uint64_t currentStep) {
    // Advance the oldest step marker
    while (oldestStep_ <= currentStep && oldestStep_ <= currentStep - numBuckets_) {
        size_t bucketIdx = oldestStep_ % numBuckets_;
        buckets_[bucketIdx] = SpikeBucket();  // Reset bucket
        pendingCounts_[bucketIdx].store(0, std::memory_order_relaxed);
        ++oldestStep_;
    }
}

// PriorityEventQueue implementations

PriorityEventQueue::PriorityEventQueue() = default;

void PriorityEventQueue::push(uint64_t timestamp, uint8_t priority, uint16_t type, const uint64_t* data) {
    if (events_.size() >= MAX_PRIORITY_EVENTS) {
        // Remove lowest priority event to make room
        size_t minPriority = std::numeric_limits<uint8_t>::max();
        size_t minIndex = 0;
        for (size_t i = 0; i < events_.size(); ++i) {
            if (events_[i].priority < minPriority) {
                minPriority = events_[i].priority;
                minIndex = i;
            }
        }
        events_.erase(events_.begin() + minIndex);
    }
    
    PriorityEvent event;
    event.timestamp = timestamp;
    event.priority = priority;
    event.type = type;
    if (data) {
        std::copy(data, data + 4, std::begin(event.data));
    }
    
    events_.push_back(event);
    std::push_heap(events_.begin(), events_.end());
}

std::optional<PriorityEventQueue::PriorityEvent> PriorityEventQueue::pop() {
    if (events_.empty()) {
        return std::nullopt;
    }
    
    std::pop_heap(events_.begin(), events_.end());
    PriorityEvent event = events_.back();
    events_.pop_back();
    
    return event;
}

std::vector<PriorityEventQueue::PriorityEvent> PriorityEventQueue::getDueEvents(uint64_t currentTime) {
    std::vector<PriorityEvent> result;
    
    for (auto it = events_.begin(); it != events_.end();) {
        if (it->timestamp > currentTime) {
            // Events are ordered by priority, then time, so we're done
            break;
        }
        
        result.push_back(*it);
        it = events_.erase(it);
    }
    
    // Restore heap property for remaining events
    std::make_heap(events_.begin(), events_.end());
    
    return result;
}

} // namespace nlm
