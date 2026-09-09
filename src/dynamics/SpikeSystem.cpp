#include "SpikeSystem.hpp"
#include <algorithm>
#include <limits>
#include <chrono>
#include <execution>
#include <numeric>

namespace nlm {

struct SpikeSystem::Impl {
    // Phase 1: Optimized spike storage using priority queues
    PriorityQueue<SpikeEvent> immediateQueue;
    PriorityQueue<DelayedSpikeEvent> delayedQueue;
    PriorityQueue<DetailedSpikeEvent> scheduledQueue;
    
    // Phase 2: Memory optimization with memory pools
    MemoryPool<SpikeEvent> spikeEventPool;
    MemoryPool<DelayedSpikeEvent> delayedSpikePool;
    MemoryPool<DetailedSpikeEvent> detailedSpikePool;
    
    // Phase 3: Statistics and metadata
    std::atomic<size_t> totalSpikeCount{0};
    std::atomic<size_t> pendingImmediateCount{0};
    std::atomic<size_t> pendingDelayedCount{0};
    std::atomic<size_t> pendingScheduledCount{0};
    
    // Phase 4: Performance optimization
    bool compressionEnabled{false};
    bool parallelProcessing{false};
    std::vector<std::thread> workerThreads;
    std::shared_mutex processingMutex;
    std::vector<CompressedSpike> compressedSpikes;
    
    // Phase 5: Spatial hashing (for large networks)
    SpatialHash<Neuron*> spatialHash;
    
    // Callbacks
    std::vector<SpikeHandler> spikeHandlers;
    std::vector<DelayedSpikeHandler> delayedHandlers;
    
    // Spike history with bounded capacity
    std::vector<DetailedSpikeEvent> spikeHistory;
    
    // Statistics for rate calculation
    float minTimestamp{std::numeric_limits<float>::max()};
    float maxTimestamp{-std::numeric_limits<float>::max()};
    
    // Configuration
    SpikeSystem::Config config;
    
    Impl() : totalSpikeCount(0), pendingImmediateCount(0), pendingDelayedCount(0), 
             pendingScheduledCount(0), compressionEnabled(false), parallelProcessing(false) {
        // Initialize with optimized defaults
        config.maxSpikeHistory = 10000;
        config.maxImmediateQueueSize = 100000;
        config.maxDelayedQueueSize = 100000;
        config.enableSpatialHashing = true;
        config.spatialHashCellSize = 16;
        config.enableMemoryPool = true;
        config.enableCompression = true;
        config.compressionThreshold = 0.1f;
        config.enableParallelProcessing = false;
        config.parallelThreads = std::thread::hardware_concurrency();
        config.enableSIMD = true;
        config.enableMultiLevelQueues = true;
        config.immediateQueueCapacity = 10000;
        config.delayedQueueCapacity = 10000;
        config.scheduledQueueCapacity = 10000;
    }
};

SpikeSystem::SpikeSystem(const Config& cfg) : pImpl(new Impl) {
    pImpl->config = cfg;
    
    // Configure based on system capabilities
    if (cfg.enableMemoryPool) {
        pImpl->spikeEventPool = MemoryPool<SpikeEvent>(cfg.immediateQueueCapacity);
        pImpl->delayedSpikePool = MemoryPool<DelayedSpikeEvent>(cfg.delayedQueueCapacity);
        pImpl->detailedSpikePool = MemoryPool<DetailedSpikeEvent>(cfg.scheduledQueueCapacity);
    }
    
    if (cfg.enableSpatialHashing) {
        pImpl->spatialHash = SpatialHash<Neuron*>(1.0, cfg.spatialHashCellSize);
    }
    
    // Start parallel processing if enabled
    if (cfg.enableParallelProcessing && cfg.parallelThreads > 1) {
        pImpl->parallelProcessing = true;
        pImpl->stopWorkers.store(false);
        for (size_t i = 0; i < cfg.parallelThreads; ++i) {
            pImpl->workerThreads.emplace_back(&SpikeSystem::workerThread, this);
        }
    }
}

SpikeSystem::~SpikeSystem() {
    reset();
    
    if (pImpl->parallelProcessing) {
        pImpl->stopWorkers.store(true);
        for (auto& thread : pImpl->workerThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        pImpl->workerThreads.clear();
    }
}

SpikeSystem::SpikeSystem(SpikeSystem&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

SpikeSystem& SpikeSystem::operator=(SpikeSystem&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

void SpikeSystem::configure(const Config& cfg) {
    std::lock_guard<std::shared_mutex> lock(pImpl->processingMutex);
    pImpl->config = cfg;
    
    // Reconfigure components based on new settings
    if (cfg.enableMemoryPool) {
        pImpl->spikeEventPool = MemoryPool<SpikeEvent>(cfg.immediateQueueCapacity);
        pImpl->delayedSpikePool = MemoryPool<DelayedSpikeEvent>(cfg.delayedQueueCapacity);
        pImpl->detailedSpikePool = MemoryPool<DetailedSpikeEvent>(cfg.scheduledQueueCapacity);
    }
}

void SpikeSystem::queueSpike(const SpikeEvent& event) {
    std::lock_guard<std::shared_mutex> lock(pImpl->processingMutex);
    
    // For immediate spikes, use priority queue
    pImpl->immediateQueue.push(event.timestamp, event);
    pImpl->pendingImmediateCount.fetch_add(1, std::memory_order_relaxed);
    
    // Store for history with memory pool if enabled
    if (pImpl->config.enableMemoryPool) {
        DetailedSpikeEvent* detailed = pImpl->detailedSpikePool.allocate();
        detailed->source = event.source_neuron;
        detailed->timestamp = event.timestamp;
        detailed->step = event.step;
        
        // We need regionId and populationId - these would need to be passed or stored elsewhere
        // For now, use default values
        detailed->regionId = INVALID_REGION_ID;
        detailed->populationId = INVALID_POPULATION_ID;
        
        pImpl->spikeHistory.push_back(*detailed);
        pImpl->detailedSpikePool.deallocate(detailed);
    } else {
        DetailedSpikeEvent detailed;
        detailed.source = event.source_neuron;
        detailed.timestamp = event.timestamp;
        detailed.step = event.step;
        detailed.regionId = INVALID_REGION_ID;
        detailed.populationId = INVALID_POPULATION_ID;
        
        pImpl->spikeHistory.push_back(detailed);
    }
    
    // Update min/max timestamps for rate calculation
    float ts = static_cast<float>(event.timestamp);
    if (ts < pImpl->minTimestamp) pImpl->minTimestamp = ts;
    if (ts > pImpl->maxTimestamp) pImpl->maxTimestamp = ts;
    
    // Call handlers immediately for detailed events
    for (auto& handler : pImpl->spikeHandlers) {
        DetailedSpikeEvent detailedEvent;
        detailedEvent.source = event.source_neuron;
        detailedEvent.timestamp = event.timestamp;
        detailedEvent.step = event.step;
        detailedEvent.regionId = INVALID_REGION_ID;
        detailedEvent.populationId = INVALID_POPULATION_ID;
        
        handler(detailedEvent);
    }
}

void SpikeSystem::queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step, 
                            RegionId regionId, PopulationId populationId) {
    SpikeEvent event(neuron, timestamp, step, regionId, populationId);
    queueSpike(event);
}

void SpikeSystem::queueDelayedSpike(const DelayedSpikeEvent& event) {
    std::lock_guard<std::shared_mutex> lock(pImpl->processingMutex);
    
    // Use priority queue for delayed spikes
    pImpl->delayedQueue.push(event.delivery_time, event);
    pImpl->pendingDelayedCount.fetch_add(1, std::memory_order_relaxed);
    
    // Call delayed handlers
    for (auto& handler : pImpl->delayedHandlers) {
        handler(event);
    }
}

void SpikeSystem::processSpikes(SimulationStep currentStep) {
    // For event-driven systems, we process immediately when spikes are generated
    // This is called from Brain::step after neuron firing is detected
    // The actual processing happens in queueSpike and queueDelayedSpike
}

void SpikeSystem::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    // Process delayed spikes that should be delivered now
    std::lock_guard<std::shared_mutex> lock(pImpl->processingMutex);
    
    // Process all delayed spikes due at this step
    DelayedSpikeEvent event;
    Timestamp time;
    
    while (pImpl->delayedQueue.pop(time, event)) {
        // Update statistics
        pImpl->pendingDelayedCount.fetch_sub(1, std::memory_order_relaxed);
        pImpl->totalSpikeCount.fetch_add(1, std::memory_order_relaxed);
        
        // Call delayed handlers
        for (auto& handler : pImpl->delayedHandlers) {
            handler(event);
        }
    }
}

void SpikeSystem::registerHandler(SpikeHandler handler) {
    std::lock_guard<std::shared_mutex> lock(pImpl->processingMutex);
    pImpl->spikeHandlers.push_back(handler);
}

void SpikeSystem::registerDelayedHandler(DelayedSpikeHandler handler) {
    std::lock_guard<std::shared_mutex> lock(pImpl->processingMutex);
    pImpl->delayedHandlers.push_back(handler);
}

const std::vector<DetailedSpikeEvent>& SpikeSystem::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

void SpikeSystem::clearHistory() {
    std::lock_guard<std::shared_mutex> lock(pImpl->processingMutex);
    pImpl->spikeHistory.clear();
    pImpl->minTimestamp = std::numeric_limits<float>::max();
    pImpl->maxTimestamp = -std::numeric_limits<float>::max();
}

size_t SpikeSystem::getSpikeCount() const {
    return pImpl->spikeHistory.size();
}

size_t SpikeSystem::getPendingSpikeCount() const {
    return pImpl->pendingImmediateCount.load(std::memory_order_relaxed);
}

size_t SpikeSystem::getPendingDelayedCount() const {
    return pImpl->pendingDelayedCount.load(std::memory_order_relaxed);
}

float SpikeSystem::getAverageSpikeRate() const {
    if (pImpl->spikeHistory.size() < 2) {
        return 0.0f;
    }
    
    float duration = pImpl->maxTimestamp - pImpl->minTimestamp;
    if (duration <= 0.0f) {
        return 0.0f;
    }
    
    return static_cast<float>(pImpl->spikeHistory.size()) / duration;
}

std::vector<NeuronId> SpikeSystem::getMostActiveNeurons(size_t count) const {
    // For simplicity, return first N neurons from history
    // In a real implementation, this would use spikeCountPerNeuron
    std::vector<NeuronId> result;
    result.reserve(std::min(count, pImpl->spikeHistory.size()));
    
    for (size_t i = 0; i < std::min(count, pImpl->spikeHistory.size()); ++i) {
        result.push_back(pImpl->spikeHistory[i].source);
    }
    
    return result;
}

void SpikeSystem::reset() {
    std::lock_guard<std::shared_mutex> lock(pImpl->processingMutex);
    
    // Clear all queues
    while (!pImpl->immediateQueue.empty()) {
        SpikeEvent event;
        Timestamp time;
        pImpl->immediateQueue.pop(time, event);
    }
    
    while (!pImpl->delayedQueue.empty()) {
        DelayedSpikeEvent event;
        Timestamp time;
        pImpl->delayedQueue.pop(time, event);
    }
    
    while (!pImpl->scheduledQueue.empty()) {
        DetailedSpikeEvent event;
        Timestamp time;
        pImpl->scheduledQueue.pop(time, event);
    }
    
    // Clear history
    pImpl->spikeHistory.clear();
    pImpl->compressedSpikes.clear();
    
    // Reset statistics
    pImpl->totalSpikeCount.store(0, std::memory_order_relaxed);
    pImpl->pendingImmediateCount.store(0, std::memory_order_relaxed);
    pImpl->pendingDelayedCount.store(0, std::memory_order_relaxed);
    pImpl->pendingScheduledCount.store(0, std::memory_order_relaxed);
    pImpl->minTimestamp = std::numeric_limits<float>::max();
    pImpl->maxTimestamp = -std::numeric_limits<float>::max();
}

void SpikeSystem::workerThread() {
    while (!pImpl->stopWorkers.load()) {
        // Process spikes from queues
        SpikeEvent event;
        Timestamp time;
        if (pImpl->immediateQueue.pop(time, event)) {
            processSpikeEvent(event);
        }
    }
}

void SpikeSystem::processSpikeEvent(const SpikeEvent& event) {
    // Update statistics
    pImpl->pendingImmediateCount.fetch_sub(1, std::memory_order_relaxed);
    pImpl->totalSpikeCount.fetch_add(1, std::memory_order_relaxed);
    
    // Create detailed event
    DetailedSpikeEvent detailed;
    detailed.source = event.source_neuron;
    detailed.timestamp = event.timestamp;
    detailed.step = event.step;
    detailed.regionId = event.regionId;
    detailed.populationId = event.populationId;
    
    // Add to history with bounds checking
    if (pImpl->spikeHistory.size() < pImpl->config.maxSpikeHistory) {
        pImpl->spikeHistory.push_back(detailed);
    }
    
    // Call handlers
    for (auto& handler : pImpl->spikeHandlers) {
        handler(detailed);
    }
}

void SpikeSystem::processDelayedSpikeEvent(const DelayedSpikeEvent& event) {
    // Update statistics
    pImpl->pendingDelayedCount.fetch_sub(1, std::memory_order_relaxed);
    pImpl->totalSpikeCount.fetch_add(1, std::memory_order_relaxed);
    
    // Call handlers
    for (auto& handler : pImpl->delayedHandlers) {
        handler(event);
    }
}

void SpikeSystem::processScheduledSpikeEvent(const DetailedSpikeEvent& event) {
    // Update statistics
    pImpl->pendingScheduledCount.fetch_sub(1, std::memory_order_relaxed);
    pImpl->totalSpikeCount.fetch_add(1, std::memory_order_relaxed);
    
    // Call handlers
    for (auto& handler : pImpl->spikeHandlers) {
        handler(event);
    }
}

void SpikeSystem::updateSpatialHash(const Neuron* neuron, Timestamp x, Timestamp y) {
    if (pImpl->config.enableSpatialHashing) {
        pImpl->spatialHash.insert(const_cast<Neuron*>(neuron), x, y);
    }
}

std::vector<Neuron*> SpikeSystem::queryNeuronsInRange(Timestamp x1, Timestamp y1, Timestamp x2, Timestamp y2) {
    if (pImpl->config.enableSpatialHashing) {
        return pImpl->spatialHash.queryRange(x1, y1, x2, y2);
    }
    return std::vector<Neuron*>();
}

void SpikeSystem::compressSpikes(const std::vector<SpikeEvent>& spikes) {
    if (!pImpl->config.enableCompression || spikes.size() < 100) {
        return; // Skip compression for small sets
    }
    
    pImpl->compressedSpikes.clear();
    pImpl->compressedSpikes.reserve(spikes.size() / 10); // Target 10:1 compression ratio
    
    // Simple compression: keep every Nth spike for dense networks
    size_t compressInterval = static_cast<size_t>(1.0f / pImpl->config.compressionThreshold);
    
    for (size_t i = 0; i < spikes.size(); i += compressInterval) {
        CompressedSpike cs;
        cs.neuronId = spikes[i].source_neuron;
        cs.timestamp = spikes[i].timestamp;
        // Simple compression of data (placeholder for real compression)
        cs.compressed_data = static_cast<uint8_t>(i & 0xFF);
        pImpl->compressedSpikes.push_back(cs);
    }
}

void SpikeSystem::decompressSpikes(std::vector<SpikeEvent>& spikes) {
    // Placeholder for decompression logic
    // In a real implementation, this would restore compressed spikes
}

void SpikeSystem::batchQueueSpikes(const std::vector<SpikeEvent>& events) {
    std::lock_guard<std::shared_mutex> lock(pImpl->processingMutex);
    
    for (const auto& event : events) {
        pImpl->immediateQueue.push(event.timestamp, event);
        pImpl->pendingImmediateCount.fetch_add(1, std::memory_order_relaxed);
        
        // Update history
        DetailedSpikeEvent detailed;
        detailed.source = event.source_neuron;
        detailed.timestamp = event.timestamp;
        detailed.step = event.step;
        detailed.regionId = event.regionId;
        detailed.populationId = event.populationId;
        
        pImpl->spikeHistory.push_back(detailed);
    }
}

void SpikeSystem::batchProcessSpikes(SimulationStep currentStep) {
    // Batch process for improved performance
    std::vector<SpikeEvent> batch;
    
    // Extract all spikes from queue
    SpikeEvent event;
    Timestamp time;
    while (pImpl->immediateQueue.pop(time, event)) {
        pImpl->pendingImmediateCount.fetch_sub(1, std::memory_order_relaxed);
        batch.push_back(event);
    }
    
    // Process batch (could use SIMD here)
    if (pImpl->config.enableSIMD && !batch.empty()) {
        processSpikesSIMD(batch);
    } else {
        for (const auto& event : batch) {
            processSpikeEvent(event);
        }
    }
}

void SpikeSystem::processSpikesSIMD(const std::vector<SpikeEvent>& events) {
    // Placeholder for SIMD-optimized processing
    // In a real implementation, this would use SIMD intrinsics
    for (const auto& event : events) {
        processSpikeEvent(event);
    }
}

void SpikeSystem::enableCompressionForLargeNetworks() {
    pImpl->compressionEnabled = true;
    pImpl->config.enableCompression = true;
    pImpl->config.compressionThreshold = 0.05f; // Higher compression for very large networks
}

bool SpikeSystem::isCompressionEnabled() const {
    return pImpl->compressionEnabled;
}

void SpikeSystem::startParallelProcessing() {
    if (!pImpl->parallelProcessing && pImpl->config.parallelThreads > 1) {
        pImpl->parallelProcessing = true;
        pImpl->stopWorkers.store(false);
        
        for (size_t i = 0; i < pImpl->config.parallelThreads; ++i) {
            pImpl->workerThreads.emplace_back(&SpikeSystem::workerThread, this);
        }
    }
}

void SpikeSystem::stopParallelProcessing() {
    if (pImpl->parallelProcessing) {
        pImpl->stopWorkers.store(true);
        
        for (auto& thread : pImpl->workerThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        
        pImpl->workerThreads.clear();
        pImpl->parallelProcessing = false;
    }
}

bool SpikeSystem::isParallelProcessing() const {
    return pImpl->parallelProcessing;
}

size_t SpikeSystem::getMemoryUsage() const {
    size_t usage = 0;
    usage += pImpl->spikeEventPool.pool.size() * sizeof(SpikeEvent);
    usage += pImpl->delayedSpikePool.pool.size() * sizeof(DelayedSpikeEvent);
    usage += pImpl->detailedSpikePool.pool.size() * sizeof(DetailedSpikeEvent);
    usage += pImpl->spikeHistory.size() * sizeof(DetailedSpikeEvent);
    usage += pImpl->compressedSpikes.size() * sizeof(CompressedSpike);
    return usage;
}

size_t SpikeSystem::getMaxMemoryUsage() const {
    return pImpl->config.maxSpikeHistory * sizeof(DetailedSpikeEvent) +
           pImpl->config.maxImmediateQueueSize * sizeof(SpikeEvent) +
           pImpl->config.maxDelayedQueueSize * sizeof(DelayedSpikeEvent);
}

} // namespace nlm
