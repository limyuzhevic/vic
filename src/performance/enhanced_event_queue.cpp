#include "../performance/enhanced_event_queue.hpp"
#include <chrono>
#include <algorithm>
#include <random>

namespace nlm {

// EnhancedSpikeRingBuffer implementation
EnhancedSpikeRingBuffer::EnhancedSpikeRingBuffer(size_t capacity, size_t partitionSize)
    : capacity_(capacity), partitionSize_(partitionSize)
{
    assert((capacity & (capacity - 1)) == 0 && "capacity must be power of 2");
    
    // Calculate number of partitions
    numPartitions_ = capacity_ / partitionSize_;
    
    // Initialize partitions
    partitions_.resize(numPartitions_);
    accessStats_.resize(numPartitions_);
    
    for (size_t i = 0; i < numPartitions_; ++i) {
        partitions_[i].initialize(partitionSize_, 32);  // 32-byte alignment
    }
    
    // Initialize statistics
    partitionStats_.resize(numPartitions_);
}

EnhancedSpikeRingBuffer::~EnhancedSpikeRingBuffer() {
    // Partitions are automatically destroyed
}

bool EnhancedSpikeRingBuffer::push(const EnhancedSpikeEvent& event, uint32_t partitionId) {
    if (partitionId >= numPartitions_) {
        partitionId = partitionId % numPartitions_;
    }
    
    Partition& part = partitions_[partitionId];
    
    if (part.push(event)) {
        part.pushCount.fetch_add(1, std::memory_order_relaxed);
        accessStats_[partitionId].lastAccess.store(
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count(),
            std::memory_order_release
        );
        return true;
    } else {
        part.overflowCount.fetch_add(1, std::memory_order_relaxed);
        return false;
    }
}

std::optional<EnhancedSpikeEvent> EnhancedSpikeRingBuffer::pop(uint32_t partitionId) {
    if (partitionId >= numPartitions_) {
        partitionId = partitionId % numPartitions_;
    }
    
    Partition& part = partitions_[partitionId];
    
    if (part.size() > 0) {
        size_t idx = part.readIndex.load(std::memory_order_acquire);
        EnhancedSpikeEvent event = part.buffer[idx];
        
        part.readIndex.store((idx + 1) & (partSize_ - 1), std::memory_order_release);
        part.popCount.fetch_add(1, std::memory_order_relaxed);
        
        return event;
    } else {
        part.underflowCount.fetch_add(1, std::memory_order_relaxed);
        return std::nullopt;
    }
}

std::vector<EnhancedSpikeEvent> EnhancedSpikeRingBuffer::popBatch(size_t count, uint32_t partitionId) {
    if (partitionId >= numPartitions_) {
        partitionId = partitionId % numPartitions_;
    }
    
    Partition& part = partitions_[partitionId];
    size_t currentSize = part.size();
    
    if (currentSize == 0) {
        return {};
    }
    
    size_t batchSize = std::min(count, currentSize);
    std::vector<EnhancedSpikeEvent> result;
    result.reserve(batchSize);
    
    for (size_t i = 0; i < batchSize; ++i) {
        size_t idx = part.readIndex.load(std::memory_order_acquire);
        EnhancedSpikeEvent event = part.buffer[idx];
        
        part.readIndex.store((idx + 1) & (partSize_ - 1), std::memory_order_release);
        part.popCount.fetch_add(1, std::memory_order_relaxed);
        
        result.push_back(event);
    }
    
    return result;
}

bool EnhancedSpikeRingBuffer::empty(uint32_t partitionId) const {
    if (partitionId >= numPartitions_) {
        partitionId = partitionId % numPartitions_;
    }
    return partitions_[partitionId].size() == 0;
}

bool EnhancedSpikeRingBuffer::full(uint32_t partitionId) const {
    if (partitionId >= numPartitions_) {
        partitionId = partitionId % numPartitions_;
    }
    return partitions_[partitionId].size() >= partitionSize_;
}

size_t EnhancedSpikeRingBuffer::size(uint32_t partitionId) const {
    if (partitionId >= numPartitions_) {
        partitionId = partitionId % numPartitions_;
    }
    return partitions_[partitionId].size();
}

EnhancedSpikeRingBuffer::PartitionStats EnhancedSpikeRingBuffer::getPartitionStats(uint32_t partitionId) const {
    PartitionStats stats;
    if (partitionId < numPartitions_) {
        const Partition& part = partitions_[partitionId];
        stats.pushCount = part.pushCount.load(std::memory_order_relaxed);
        stats.popCount = part.popCount.load(std::memory_order_relaxed);
        stats.overflowCount = part.overflowCount.load(std::memory_order_relaxed);
        stats.underflowCount = part.underflowCount.load(std::memory_order_relaxed);
        stats.currentSize = part.size();
        if (stats.pushCount + stats.popCount > 0) {
            stats.utilizationRate = (double)stats.currentSize / (double)partitionSize_;
        }
    }
    return stats;
}

void EnhancedSpikeRingBuffer::adaptiveBucketResizing() {
    // Implement adaptive bucket resizing based on load patterns
    // This is a simplified version - production systems would have more sophisticated logic
    
    uint64_t currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    for (size_t i = 0; i < numPartitions_; ++i) {
        uint64_t lastAccess = accessStats_[i].lastAccess.load(std::memory_order_acquire);
        uint64_t idleTime = currentTime - lastAccess;
        
        // If partition has been idle for a while, consider reducing its size
        if (idleTime > 1000000000) {  // 1 second
            PartitionStats stats = getPartitionStats(i);
            if (stats.currentSize < partitionSize_ / 4 && stats.currentSize > 0) {
                // Consider downsizing (simplified - would need more complex logic)
                // For now, just log the opportunity
                // In practice, you would actually resize the partition
            }
        }
    }
}

uint32_t EnhancedSpikeRingBuffer::getOptimalPartition() const {
    // Find partition with lowest load for load balancing
    uint32_t optimalPartition = 0;
    double minUtilization = 1.0;
    
    for (size_t i = 0; i < numPartitions_; ++i) {
        PartitionStats stats = getPartitionStats(i);
        if (stats.utilizationRate < minUtilization) {
            minUtilization = stats.utilizationRate;
            optimalPartition = static_cast<uint32_t>(i);
        }
    }
    
    return optimalPartition;
}

// EnhancedDelayedSpikeQueue implementation
EnhancedDelayedSpikeQueue::EnhancedDelayedSpikeQueue(size_t numBuckets, size_t maxDelaySteps, 
                                                   size_t numPartitions)
    : numBuckets_(numBuckets), maxDelaySteps_(maxDelaySteps), numPartitions_(numPartitions),
      currentStep_(0), oldestStep_(0)
{
    assert(numBuckets >= maxDelaySteps && "numBuckets must be >= maxDelaySteps");
    
    // Initialize buckets for each partition
    buckets_.resize(numPartitions_);
    pendingCounts_.resize(numPartitions_);
    partitionPools_.resize(numPartitions_);
    
    for (size_t i = 0; i < numPartitions_; ++i) {
        buckets_[i].resize(numBuckets_);
        pendingCounts_[i].resize(numBuckets_, 0);
        partitionPools_[i] = BucketMemoryPool(numBuckets_, 1);  // Simplified
    }
    
    partitionStats_.resize(numPartitions_);
}

EnhancedDelayedSpikeQueue::~EnhancedDelayedSpikeQueue() {
    // Buckets are automatically destroyed
}

void EnhancedDelayedSpikeQueue::schedule(uint64_t sourceNeuron, uint64_t destNeuron,
                                        uint64_t synapseId, float weight,
                                        uint32_t delaySteps, uint64_t deliveryStep,
                                        uint32_t partitionId) {
    if (partitionId >= numPartitions_) {
        partitionId = partitionId % numPartitions_;
    }
    
    EnhancedSpikeEvent event(sourceNeuron, destNeuron, synapseId, weight, 
                            delaySteps, deliveryStep, partitionId);
    
    size_t bucketIdx = deliveryStep % numBuckets_;
    SpikeBucket& bucket = buckets_[partitionId][bucketIdx];
    
    if (bucket.push(event)) {
        pendingCounts_[partitionId][bucketIdx]++;
        partitionStats_[partitionId].totalScheduled++;
    } else {
        partitionStats_[partitionId].totalOverflow++;
    }
}

void EnhancedDelayedSpikeQueue::scheduleBatch(const std::vector<EnhancedSpikeEvent>& events) {
    for (const auto& event : events) {
        schedule(event.sourceNeuron, event.destNeuron, event.synapseId,
                 event.weight, event.delaySteps, event.eventTimestamp,
                 event.partitionId);
    }
}

std::vector<EnhancedSpikeEvent> EnhancedDelayedSpikeQueue::getDueSpikes(uint64_t currentStep) {
    currentStep_ = currentStep;
    std::vector<EnhancedSpikeEvent> result;
    
    // Process all buckets up to current step for each partition
    for (size_t p = 0; p < numPartitions_; ++p) {
        while (oldestStep_ <= currentStep) {
            size_t bucketIdx = oldestStep_ % numBuckets_;
            
            SpikeBucket& bucket = buckets_[p][bucketIdx];
            std::vector<EnhancedSpikeEvent> dueEvents = bucket.drain();
            
            result.insert(result.end(), dueEvents.begin(), dueEvents.end());
            
            pendingCounts_[p][bucketIdx] = 0;
            oldestStep_++;
        }
    }
    
    partitionStats_[0].totalDelivered += result.size();  // Simplified - aggregate across partitions
    return result;
}

std::vector<std::pair<uint32_t, EnhancedSpikeEvent>> 
EnhancedDelayedSpikeQueue::getDueSpikesWithPartition(uint64_t currentStep) {
    currentStep_ = currentStep;
    std::vector<std::pair<uint32_t, EnhancedSpikeEvent>> result;
    
    // Process all buckets up to current step for each partition
    for (size_t p = 0; p < numPartitions_; ++p) {
        while (oldestStep_ <= currentStep) {
            size_t bucketIdx = oldestStep_ % numBuckets_;
            
            SpikeBucket& bucket = buckets_[p][bucketIdx];
            std::vector<EnhancedSpikeEvent> dueEvents = bucket.drain();
            
            for (auto& event : dueEvents) {
                result.emplace_back(p, event);
            }
            
            pendingCounts_[p][bucketIdx] = 0;
            oldestStep_++;
        }
    }
    
    partitionStats_[0].totalDelivered += result.size();  // Simplified
    return result;
}

size_t EnhancedDelayedSpikeQueue::pendingCount() const {
    size_t total = 0;
    for (size_t p = 0; p < numPartitions_; ++p) {
        for (size_t count : pendingCounts_[p]) {
            total += count;
        }
    }
    return total;
}

size_t EnhancedDelayedSpikeQueue::bucketSize(uint64_t step, uint32_t partitionId) const {
    if (partitionId >= numPartitions_) {
        partitionId = partitionId % numPartitions_;
    }
    
    size_t bucketIdx = step % numBuckets_;
    return buckets_[partitionId][bucketIdx].size();
}

void EnhancedDelayedSpikeQueue::clear() {
    for (size_t p = 0; p < numPartitions_; ++p) {
        for (size_t& count : pendingCounts_[p]) {
            count = 0;
        }
        for (auto& bucket : buckets_[p]) {
            bucket = SpikeBucket();  // Reset bucket
        }
    }
    
    currentStep_ = 0;
    oldestStep_ = 0;
}

void EnhancedDelayedSpikeQueue::advanceTime(uint64_t currentStep) {
    // Advance the oldest step marker
    while (oldestStep_ <= currentStep && oldestStep_ <= currentStep - numBuckets_) {
        size_t bucketIdx = oldestStep_ % numBuckets_;
        
        for (size_t p = 0; p < numPartitions_; ++p) {
            buckets_[p][bucketIdx] = SpikeBucket();  // Reset bucket
            pendingCounts_[p][bucketIdx] = 0;
        }
        
        oldestStep_++;
    }
}

std::vector<EnhancedDelayedSpikeQueue::BucketStats> 
EnhancedDelayedSpikeQueue::getBucketStats(uint64_t startStep, uint64_t endStep) const {
    std::vector<BucketStats> stats;
    
    for (uint64_t step = startStep; step <= endStep; ++step) {
        for (size_t p = 0; p < numPartitions_; ++p) {
            size_t bucketIdx = step % numBuckets_;
            const SpikeBucket& bucket = buckets_[p][bucketIdx];
            
            BucketStats bucketStat;
            bucketStat.step = step;
            bucketStat.eventCount = bucket.size();
            bucketStat.partitionId = static_cast<uint32_t>(p);
            bucketStat.memoryUsage = bucket.size() * sizeof(EnhancedSpikeEvent);
            bucketStat.averageDelay = bucket.totalDeliveryTime.load(std::memory_order_relaxed) / 
                                     (bucket.size() + 1);  // Avoid division by zero
            
            stats.push_back(bucketStat);
        }
    }
    
    return stats;
}

void EnhancedDelayedSpikeQueue::optimizeBucketUsage() {
    // Simple optimization: consolidate empty buckets and compact memory
    for (size_t p = 0; p < numPartitions_; ++p) {
        for (size_t i = 0; i < numBuckets_; ++i) {
            if (buckets_[p][i].size() == 0) {
                // Reset empty bucket
                buckets_[p][i] = SpikeBucket();
            }
        }
    }
}

void EnhancedDelayedSpikeQueue::compressOldBuckets(uint64_t cutoffStep) {
    // Remove buckets older than cutoffStep
    for (size_t p = 0; p < numPartitions_; ++p) {
        for (size_t i = 0; i < numBuckets_; ++i) {
            uint64_t step = (i + oldestStep_) % numBuckets_;
            if (step < cutoffStep) {
                buckets_[p][i] = SpikeBucket();
            }
        }
    }
}

EnhancedDelayedSpikeQueue::MemoryStats EnhancedDelayedSpikeQueue::getMemoryStats() const {
    MemoryStats stats;
    
    for (size_t p = 0; p < numPartitions_; ++p) {
        for (size_t i = 0; i < numBuckets_; ++i) {
            const SpikeBucket& bucket = buckets_[p][i];
            stats.totalMemoryUsage += bucket.size() * sizeof(EnhancedSpikeEvent);
            stats.peakMemoryUsage = std::max(stats.peakMemoryUsage, stats.totalMemoryUsage);
            stats.currentBucketMemory += bucket.size() * sizeof(EnhancedSpikeEvent);
        }
    }
    
    if (stats.totalMemoryUsage > 0) {
        stats.compressionRatio = (double)stats.currentBucketMemory / (double)stats.totalMemoryUsage;
    }
    
    stats.activeBuckets = numBuckets_;  // Simplified
    
    return stats;
}

// PriorityEventQueue implementation
PriorityEventQueue::PriorityEventQueue(size_t numPartitions) 
    : numPartitions_(numPartitions) {
    partitionQueues_.resize(numPartitions_);
    partitionStats_.resize(numPartitions_);
}

PriorityEventQueue::~PriorityEventQueue() {
    // Queues are automatically destroyed
}

void PriorityEventQueue::push(uint64_t timestamp, uint8_t priority, uint16_t type,
                             const uint64_t* data, uint32_t partitionId) {
    if (partitionId >= numPartitions_) {
        partitionId = partitionId % numPartitions_;
    }
    
    PriorityEvent event;
    event.timestamp = timestamp;
    event.priority = priority;
    event.type = type;
    event.partitionId = partitionId;
    
    if (data) {
        for (int i = 0; i < 4; ++i) {
            event.data[i] = data[i];
        }
    }
    
    partitionQueues_[partitionId].push(event);
    partitionStats_[partitionId].pushCount++;
}

void PriorityEventQueue::pushBatch(const std::vector<PriorityEvent>& events) {
    for (const auto& event : events) {
        push(event.timestamp, event.priority, event.type, 
             event.data, event.partitionId);
    }
}

std::optional<PriorityEventQueue::PriorityEvent> PriorityEventQueue::pop() {
    for (size_t i = 0; i < numPartitions_; ++i) {
        if (!partitionQueues_[i].empty()) {
            PriorityEvent event = partitionQueues_[i].top();
            partitionQueues_[i].pop();
            partitionStats_[i].popCount++;
            return event;
        }
    }
    return std::nullopt;
}

std::vector<PriorityEventQueue::PriorityEvent> 
PriorityEventQueue::getDueEvents(uint64_t currentTime) {
    std::vector<PriorityEvent> result;
    
    for (size_t i = 0; i < numPartitions_; ++i) {
        while (!partitionQueues_[i].empty()) {
            PriorityEvent event = partitionQueues_[i].top();
            if (event.timestamp <= currentTime) {
                partitionQueues_[i].pop();
                partitionStats_[i].popCount++;
                result.push_back(event);
            } else {
                break;
            }
        }
    }
    
    return result;
}

std::vector<PriorityEventQueue::PartitionStats> PriorityEventQueue::getPartitionStats() const {
    std::vector<PartitionStats> stats;
    
    for (size_t i = 0; i < numPartitions_; ++i) {
        PartitionStats partStat;
        partStat.pushCount = partitionStats_[i].pushCount;
        partStat.popCount = partitionStats_[i].popCount;
        partStat.overflowCount = partitionStats_[i].overflowCount;
        partStat.currentSize = partitionQueues_[i].size();
        stats.push_back(partStat);
    }
    
    return stats;
}

// EventRouter implementation
EventRouter::EventRouter(size_t numPartitions) 
    : numPartitions_(numPartitions), partitionLoads_(numPartitions, 0.0) {
    sourceToPartitionMap_.resize(numPartitions);
    eventTypeToPartitionMap_.resize(numPartitions);
    accessPatterns_.resize(numPartitions);
    
    for (size_t i = 0; i < numPartitions_; ++i) {
        sourceToPartitionMap_[i].resize(1000000);  // Approximate neuron ID range
        eventTypeToPartitionMap_[i].resize(65536);  // Approximate event type range
    }
}

uint32_t EventRouter::routeEvent(const EnhancedSpikeEvent& event) const {
    // Simple routing based on neuron ID hash
    uint32_t partition = hashNeuronId(event.sourceNeuron, numPartitions_);
    
    // Update routing table
    if (event.sourceNeuron < sourceToPartitionMap_[partition].size()) {
        sourceToPartitionMap_[partition][event.sourceNeuron] = partition;
    }
    
    return partition;
}

uint32_t EventRouter::routePriorityEvent(const PriorityEventQueue::PriorityEvent& event) const {
    // Route based on event type and priority
    uint32_t partition = eventTypeToPartitionMap_[0][event.type % eventTypeToPartitionMap_[0].size()];
    return partition;
}

void EventRouter::learnFromAccess(const EnhancedSpikeEvent& event) {
    std::lock_guard<std::mutex> lock(loadMutex_);
    
    uint32_t partition = routeEvent(event);
    partitionLoads_[partition] += 1.0;
    
    // Update access pattern
    if (event.sourceNeuron < accessPatterns_[partition].size()) {
        accessPatterns_[partition][event.sourceNeuron].accessCount++;
        accessPatterns_[partition][event.sourceNeuron].lastAccess = 
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
}

std::vector<double> EventRouter::getPartitionLoads() const {
    std::lock_guard<std::mutex> lock(loadMutex_);
    return partitionLoads_;
}

void EventRouter::rebalancePartitions() {
    std::lock_guard<std::mutex> lock(loadMutex_);
    
    // Simple load balancing: move load from high-load to low-load partitions
    double avgLoad = 0.0;
    for (double load : partitionLoads_) {
        avgLoad += load;
    }
    avgLoad /= partitionLoads_.size();
    
    for (size_t i = 0; i < numPartitions_; ++i) {
        if (partitionLoads_[i] > avgLoad * 1.5) {
            // Partition is overloaded, consider moving some load
            // This is a simplified version - actual implementation would be more sophisticated
            partitionLoads_[i] *= 0.9;  // Reduce load estimate
        }
    }
}

} // namespace nlm
