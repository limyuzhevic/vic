#include "SpikeSystem.hpp"
#include <algorithm>
#include <limits>
#include <memory>
#include <mutex>
#include <array>
#include <atomic>
#include <numeric>
#include <cmath>

namespace nlm {

// Memory allocator for spike events
class SpikeAllocator {
public:
    explicit SpikeAllocator(size_t poolSize = 65536) {
        pool_.reserve(poolSize);
        pool_.resize(poolSize);
    }
    
    DetailedSpikeEvent* allocate() {
        if (currentIndex_ >= pool_.size()) {
            return nullptr;
        }
        return &pool_[currentIndex_++];
    }
    
    void reset() { currentIndex_ = 0; }
    
    DetailedSpikeEvent* data() { return pool_.data(); }
    const DetailedSpikeEvent* data() const { return pool_.data(); }
    size_t capacity() const { return pool_.size(); }
    
    void setCapacity(size_t newSize) {
        pool_.clear();
        pool_.reserve(newSize);
        pool_.resize(newSize);
    }

private:
    std::vector<DetailedSpikeEvent> pool_;
    size_t currentIndex_ = 0;
};

// Circular buffer for efficient history management
class CircularBuffer {
public:
    explicit CircularBuffer(size_t capacity) 
        : capacity_(capacity), size_(0), head_(0), tail_(0), overflowCount_(0) {
        buffer_ = std::make_unique<DetailedSpikeEvent[]>(capacity);
    }
    
    void push(const DetailedSpikeEvent& event) {
        buffer_[tail_] = event;
        tail_ = (tail_ + 1) % capacity_;
        if (size_ == capacity_) {
            head_ = tail_;
            overflowCount_++;
        } else {
            size_++;
        }
    }
    
    void pop() {
        if (size_ > 0) {
            head_ = (head_ + 1) % capacity_;
            size_--;
        }
    }
    
    const DetailedSpikeEvent& operator[](size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        size_t pos = (head_ + index) % capacity_;
        return buffer_[pos];
    }
    
    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    size_t overflowCount() const { return overflowCount_; }
    bool empty() const { return size_ == 0; }
    bool full() const { return size_ == capacity_; }
    
    const DetailedSpikeEvent* data() const { return buffer_.get(); }
    const DetailedSpikeEvent* begin() const { return buffer_.get() + head_; }
    const DetailedSpikeEvent* end() const { return buffer_.get() + ((head_ + size_) % capacity_); }
    
    // Get all elements in a vector (for backward compatibility)
    std::vector<DetailedSpikeEvent> toVector() const {
        std::vector<DetailedSpikeEvent> result;
        result.reserve(size_);
        for (size_t i = 0; i < size_; ++i) {
            result.push_back((*this)[i]);
        }
        return result;
    }
    
    // Clear the buffer
    void clear() {
        size_ = 0;
        head_ = 0;
        tail_ = 0;
    }

private:
    std::unique_ptr<DetailedSpikeEvent[]> buffer_;
    size_t capacity_;
    size_t size_;
    size_t head_;
    size_t tail_;
    size_t overflowCount_;
};

// Optimized statistics calculator with incremental updates
class StatisticsCalculator {
public:
    struct Stats {
        size_t totalSpikes = 0;
        float minTime = std::numeric_limits<float>::max();
        float maxTime = -std::numeric_limits<float>::max();
        std::unordered_map<uint64_t, size_t> neuronCounts;
        size_t mostActiveNeuronCount = 0;
    };
    
    StatisticsCalculator(size_t maxNeurons) 
        : maxNeurons_(maxNeurons) {
        neuronCounts_.reserve(maxNeurons_);
        mostActive_.reserve(maxNeurons_);
    }
    
    void update(const DetailedSpikeEvent& event, Timestamp time) {
        stats_.totalSpikes++;
        stats_.minTime = std::min(stats_.minTime, static_cast<float>(time));
        stats_.maxTime = std::max(stats_.maxTime, static_cast<float>(time));
        
        auto it = stats_.neuronCounts.find(event.source.value);
        if (it != stats_.neuronCounts.end()) {
            it->second++;
        } else {
            stats_.neuronCounts[event.source.value] = 1;
        }
        
        // Update most active neurons
        updateMostActive(event.source.value);
    }
    
    float getAverageRate() const {
        if (stats_.totalSpikes < 2 || stats_.maxTime <= stats_.minTime) {
            return 0.0f;
        }
        float duration = stats_.maxTime - stats_.minTime;
        return static_cast<float>(stats_.totalSpikes) / duration;
    }
    
    std::vector<NeuronId> getMostActiveNeurons(size_t count) const {
        std::vector<std::pair<uint64_t, size_t>> neuronCounts;
        neuronCounts.reserve(stats_.neuronCounts.size());
        
        for (const auto& pair : stats_.neuronCounts) {
            neuronCounts.emplace_back(pair.first, pair.second);
        }
        
        std::partial_sort(neuronCounts.begin(),
                         neuronCounts.begin() + std::min(count, neuronCounts.size()),
                         neuronCounts.end(),
                         [](const auto& a, const auto& b) { return a.second > b.second; });
        
        std::vector<NeuronId> result;
        result.reserve(std::min(count, neuronCounts.size()));
        for (size_t i = 0; i < std::min(count, neuronCounts.size()); ++i) {
            result.emplace_back(neuronCounts[i].first);
        }
        
        return result;
    }
    
    size_t getTotalSpikes() const { return stats_.totalSpikes; }
    size_t getUniqueNeurons() const { return stats_.neuronCounts.size(); }
    size_t getMostActiveCount() const { return stats_.mostActiveNeuronCount; }
    
    void clear() {
        stats_.totalSpikes = 0;
        stats_.minTime = std::numeric_limits<float>::max();
        stats_.maxTime = -std::numeric_limits<float>::max();
        stats_.neuronCounts.clear();
        stats_.mostActiveNeuronCount = 0;
        mostActive_.clear();
        neuronCounts_.clear();
    }

private:
    void updateMostActive(uint64_t neuronId) {
        auto it = std::find(mostActive_.begin(), mostActive_.end(), neuronId);
        if (it != mostActive_.end()) {
            return; // Already in most active list
        }
        
        if (mostActive_.size() < 10) {
            mostActive_.push_back(neuronId);
        } else if (stats_.neuronCounts[neuronId] > stats_.neuronCounts[mostActive_.back()]) {
            mostActive_.pop_back();
            mostActive_.push_back(neuronId);
        }
        
        stats_.mostActiveNeuronCount = mostActive_.size();
    }
    
    Stats stats_;
    std::vector<uint64_t> mostActive_;
    size_t maxNeurons_;
    std::unordered_map<uint64_t, size_t> neuronCounts_;
};

// Spike compressor for efficient storage
struct CompressedSpike {
    uint64_t neuronId;
    uint32_t step;
    Timestamp time;
    bool isExcitatory;
    uint32_t compressedRegionId;
    uint32_t compressedPopulationId;
    
    CompressedSpike() : neuronId(0), step(0), time(0.0), isExcitatory(false),
                      compressedRegionId(0), compressedPopulationId(0) {}
    
    CompressedSpike(const DetailedSpikeEvent& spike) {
        neuronId = spike.source.value;
        step = spike.step;
        time = spike.timestamp;
        isExcitatory = (spike.regionId.value % 2 == 0); // Simple heuristic for demonstration
        compressedRegionId = static_cast<uint32_t>(spike.regionId.value % 65536);
        compressedPopulationId = static_cast<uint32_t>(spike.populationId.value % 65536);
    }
};

class SpikeCompressor {
public:
    std::vector<CompressedSpike> compress(const std::vector<DetailedSpikeEvent>& spikes) {
        std::vector<CompressedSpike> compressed;
        compressed.reserve(spikes.size());
        
        for (const auto& spike : spikes) {
            compressed.emplace_back(spike);
        }
        
        return compressed;
    }
    
    std::vector<DetailedSpikeEvent> decompress(const std::vector<CompressedSpike>& compressed) {
        std::vector<DetailedSpikeEvent> spikes;
        spikes.reserve(compressed.size());
        
        for (const auto& comp : compressed) {
            DetailedSpikeEvent spike;
            spike.source = NeuronId(comp.neuronId);
            spike.timestamp = comp.time;
            spike.step = comp.step;
            spike.regionId = RegionId(comp.compressedRegionId);
            spike.populationId = PopulationId(comp.compressedPopulationId);
            spikes.push_back(spike);
        }
        
        return spikes;
    }
};

// Optimized delayed spike queue with multiple priority levels
class DelayedSpikeManager {
public:
    DelayedSpikeManager() {
        queues_.resize(3); // High, Medium, Low priority
    }
    
    void queueDelayedSpike(const DelayedSpikeEvent& event) {
        // Determine priority based on synapse type and weight
        int priority = 0;
        if (event.synapse_type == SynapseType::Excitatory && event.weight > 1.0f) {
            priority = 0; // High priority
        } else if (event.synapse_type == SynapseType::Excitatory || event.is_excitatory) {
            priority = 1; // Medium priority
        } else {
            priority = 2; // Low priority
        }
        
        // Push to appropriate priority queue
        queues_[priority].push(event);
    }
    
    bool popDelayedSpike(SimulationStep currentStep, DelayedSpikeEvent& event) {
        // Process all queues from highest to lowest priority
        for (auto& queue : queues_) {
            if (!queue.empty()) {
                event = queue.top();
                queue.pop();
                return true;
            }
        }
        return false;
    }
    
    bool hasPendingSpikes() const {
        for (const auto& queue : queues_) {
            if (!queue.empty()) {
                return true;
            }
        }
        return false;
    }
    
    size_t getTotalCount() const {
        size_t count = 0;
        for (const auto& queue : queues_) {
            count += queue.size();
        }
        return count;
    }
    
    void clear() {
        for (auto& queue : queues_) {
            while (!queue.empty()) {
                queue.pop();
            }
        }
    }
    
    bool isEmpty() const {
        for (const auto& queue : queues_) {
            if (!queue.empty()) {
                return false;
            }
        }
        return true;
    }

private:
    std::array<std::priority_queue<DelayedSpikeEvent>, 3> queues_;
};

struct SpikeSystem::Impl {
    // Circular buffer for spike history
    std::unique_ptr<CircularBuffer> spikeHistory;
    
    // Multiple priority queues for pending spikes
    std::priority_queue<SpikeEvent> highPriorityPending;
    std::priority_queue<SpikeEvent> normalPriorityPending;
    std::priority_queue<SpikeEvent> lowPriorityPending;
    
    // Optimized delayed spike manager
    DelayedSpikeManager delayedSpikes;
    
    // Handler registries
    std::vector<SpikeHandler> handlers;
    std::vector<DelayedSpikeHandler> delayedHandlers;
    
    // Memory pools
    SpikeAllocator spikeAllocator;
    
    // Statistics calculator
    StatisticsCalculator statistics;
    
    // Compressed spike storage
    SpikeCompressor compressor;
    std::vector<CompressedSpike> compressedHistory;
    
    // Indexing for efficient queries
    std::unordered_map<uint64_t, std::vector<size_t>> neuronHistoryIndices;
    std::unordered_map<uint64_t, std::vector<size_t>> regionHistoryIndices;
    
    // Thread safety
    mutable std::mutex mutex_;
    std::atomic<bool> processing_ {false};
    
    // Configuration
    size_t maxHistorySize;
    size_t historyCompressionThreshold;
    bool compressionEnabled;
    float statisticsSamplingInterval;
    
    // Performance optimization flags
    bool batchProcessingEnabled;
    size_t batchSize;
    
    Impl() 
        : spikeHistory(std::make_unique<CircularBuffer>(10000))
        , spikeAllocator(65536)
        , statistics(100000) // Estimate of max neurons
        , maxHistorySize(10000)
        , historyCompressionThreshold(5000)
        , compressionEnabled(true)
        , statisticsSamplingInterval(1.0f)
        , batchProcessingEnabled(true)
        , batchSize(100) {}
        
    // Helper methods
    void addToIndices(const DetailedSpikeEvent& event, size_t index) {
        neuronHistoryIndices[event.source.value].push_back(index);
        regionHistoryIndices[event.regionId.value].push_back(index);
        
        // Maintain indices size limits
        if (neuronHistoryIndices[event.source.value].size() > 1000) {
            neuronHistoryIndices[event.source.value].erase(
                neuronHistoryIndices[event.source.value].begin());
        }
        if (regionHistoryIndices[event.regionId.value].size() > 1000) {
            regionHistoryIndices[event.regionId.value].erase(
                regionHistoryIndices[event.regionId.value].begin());
        }
    }
    
    void removeFromIndices(const DetailedSpikeEvent& event, size_t index) {
        auto& neuronIndices = neuronHistoryIndices[event.source.value];
        auto it = std::find(neuronIndices.begin(), neuronIndices.end(), index);
        if (it != neuronIndices.end()) {
            neuronIndices.erase(it);
        }
        
        auto& regionIndices = regionHistoryIndices[event.regionId.value];
        auto rit = std::find(regionIndices.begin(), regionIndices.end(), index);
        if (rit != regionIndices.end()) {
            regionIndices.erase(rit);
        }
    }
    
    void processBatch(std::vector<SpikeEvent>& batch) {
        // Process a batch of spikes efficiently
        for (auto& event : batch) {
            // Create detailed event
            DetailedSpikeEvent detailed;
            detailed.source = event.source_neuron;
            detailed.timestamp = event.timestamp;
            detailed.step = event.step;
            detailed.regionId = event.regionId.value % 10000; // Simplified for demo
            detailed.populationId = event.populationId.value % 10000; // Simplified for demo
            
            // Add to circular buffer
            spikeHistory->push(detailed);
            
            // Update statistics
            statistics.update(detailed, event.timestamp);
            
            // Add to indices
            addToIndices(detailed, spikeHistory->size() - 1);
            
            // Call handlers
            for (auto& handler : handlers) {
                handler(detailed);
            }
        }
    }
    
    void compressHistoryIfNeeded() {
        if (compressionEnabled && spikeHistory->size() > historyCompressionThreshold) {
            // Compress current history
            auto historyVec = spikeHistory->toVector();
            compressedHistory = compressor.compress(historyVec);
            
            // Clear circular buffer to free memory
            spikeHistory->clear();
            
            // Re-fill from compressed data (selective)
            size_t sampleSize = maxHistorySize / 10; // Keep only 10% for performance
            size_t step = compressedHistory.size() / sampleSize;
            if (step < 1) step = 1;
            
            for (size_t i = 0; i < compressedHistory.size(); i += step) {
                if (spikeHistory->size() >= maxHistorySize) {
                    break;
                }
                
                auto decompressed = compressor.decompress({compressedHistory[i]});
                if (!decompressed.empty()) {
                    spikeHistory->push(decompressed[0]);
                }
            }
        }
    }
};

SpikeSystem::SpikeSystem() : pImpl(new Impl) {}

SpikeSystem::~SpikeSystem() = default;

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

void SpikeSystem::queueSpike(const SpikeEvent& event) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    // Determine priority based on neuron type and timestamp
    if (event.step > 0) { // Heuristic for priority
        pImpl->highPriorityPending.push(event);
    } else {
        pImpl->normalPriorityPending.push(event);
    }
}

void SpikeSystem::queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step) {
    SpikeEvent event{neuron, timestamp, step, INVALID_REGION_ID, INVALID_POPULATION_ID};
    queueSpike(event);
}

void SpikeSystem::queueDelayedSpike(const DelayedSpikeEvent& event) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->delayedSpikes.queueDelayedSpike(event);
}

void SpikeSystem::processSpikes(SimulationStep currentStep) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    // Collect spikes from all priority levels
    std::vector<SpikeEvent> batch;
    batch.reserve(pImpl->batchSize);
    
    // Process high priority
    while (!pImpl->highPriorityPending.empty()) {
        batch.push_back(pImpl->highPriorityPending.top());
        pImpl->highPriorityPending.pop();
        
        if (batch.size() >= pImpl->batchSize) {
            pImpl->processBatch(batch);
            batch.clear();
        }
    }
    
    // Process normal priority
    while (!pImpl->normalPriorityPending.empty()) {
        batch.push_back(pImpl->normalPriorityPending.top());
        pImpl->normalPriorityPending.pop();
        
        if (batch.size() >= pImpl->batchSize) {
            pImpl->processBatch(batch);
            batch.clear();
        }
    }
    
    // Process low priority
    while (!pImpl->lowPriorityPending.empty()) {
        batch.push_back(pImpl->lowPriorityPending.top());
        pImpl->lowPriorityPending.pop();
        
        if (batch.size() >= pImpl->batchSize) {
            pImpl->processBatch(batch);
            batch.clear();
        }
    }
    
    // Process remaining batch
    if (!batch.empty()) {
        pImpl->processBatch(batch);
    }
    
    // Compress history if needed
    pImpl->compressHistoryIfNeeded();
}

void SpikeSystem::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    // Process all delayed spikes
    DelayedSpikeEvent event;
    while (pImpl->delayedSpikes.popDelayedSpike(currentStep, event)) {
        // Call delayed spike handlers (these will deliver synaptic input)
        for (auto& handler : pImpl->delayedHandlers) {
            handler(event);
        }
    }
}

void SpikeSystem::registerHandler(SpikeHandler handler) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->handlers.push_back(std::move(handler));
}

void SpikeSystem::registerDelayedHandler(DelayedSpikeHandler handler) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->delayedHandlers.push_back(std::move(handler));
}

const std::vector<DetailedSpikeEvent>& SpikeSystem::getSpikeHistory() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    return pImpl->spikeHistory->toVector();
}

void SpikeSystem::clearHistory() {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->spikeHistory->clear();
    pImpl->statistics.clear();
    pImpl->neuronHistoryIndices.clear();
    pImpl->regionHistoryIndices.clear();
    pImpl->compressedHistory.clear();
}

size_t SpikeSystem::getSpikeCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    return pImpl->spikeHistory->size();
}

size_t SpikeSystem::getPendingSpikeCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    size_t count = 0;
    count += pImpl->highPriorityPending.size();
    count += pImpl->normalPriorityPending.size();
    count += pImpl->lowPriorityPending.size();
    return count;
}

size_t SpikeSystem::getPendingDelayedCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    return pImpl->delayedSpikes.getTotalCount();
}

float SpikeSystem::getAverageSpikeRate() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    return pImpl->statistics.getAverageRate();
}

std::vector<NeuronId> SpikeSystem::getMostActiveNeurons(size_t count) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    return pImpl->statistics.getMostActiveNeurons(count);
}

void SpikeSystem::reset() {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    // Clear all priority queues
    while (!pImpl->highPriorityPending.empty()) {
        pImpl->highPriorityPending.pop();
    }
    while (!pImpl->normalPriorityPending.empty()) {
        pImpl->normalPriorityPending.pop();
    }
    while (!pImpl->lowPriorityPending.empty()) {
        pImpl->lowPriorityPending.pop();
    }
    
    // Reset spike history
    pImpl->spikeHistory->clear();
    pImpl->statistics.clear();
    pImpl->neuronHistoryIndices.clear();
    pImpl->regionHistoryIndices.clear();
    
    // Reset delayed spikes
    pImpl->delayedSpikes.clear();
    
    // Reset allocator
    pImpl->spikeAllocator.reset();
}

} // namespace nlm
