// SpikeSystem implementation with parallel processing optimizations

#include "SpikeSystem.hpp"
#include <algorithm>
#include <limits>
#include <thread>
#include <mutex>
#include <atomic>
#include <tbb/concurrent_queue.h>
#include <tbb/parallel_for.h>
#include <tbb/parallel_reduce.h>

namespace nlm {

struct SpikeSystem::Impl {
    // Thread-safe concurrent queue for pending spikes
    tbb::concurrent_queue<SpikeEvent> pendingSpikes;
    std::vector<DetailedSpikeEvent> spikeHistory;
    std::vector<SpikeHandler> handlers;
    std::vector<DelayedSpikeHandler> delayedHandlers;
    std::unordered_map<SimulationStep, std::vector<DelayedSpikeEvent>> delayedSpikes;
    size_t maxHistorySize;
    
    // Atomic counter for thread-safe spike counting
    std::atomic<size_t> totalSpikeCount{0};
    
    // Spike counting for statistics - use concurrent unordered_map
    tbb::concurrent_hash_map<uint64_t, size_t> spikeCountPerNeuron;
    
    // Thread-local storage for performance
    alignas(64) thread_local static std::vector<DetailedSpikeEvent> threadLocalSpikeHistory;
    
    Impl() : maxHistorySize(10000) {}
};

SpikeSystem::SpikeSystem() : pImpl(new Impl) {}

SpikeSystem::~SpikeSystem() = default;

SpikeSystem::SpikeSystem(SpikeSystem&& other) noexcept : pImpl(std::move(other.pImpl)) {}

SpikeSystem& SpikeSystem::operator=(SpikeSystem&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

void SpikeSystem::queueSpike(const SpikeEvent& event) {
    pImpl->pendingSpikes.push(event);
}

void SpikeSystem::queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step) {
    pImpl->pendingSpikes.push(SpikeEvent{neuron, timestamp, step});
}

void SpikeSystem::queueDelayedSpike(const DelayedSpikeEvent& event) {
    // Store by delivery step for efficient processing
    std::lock_guard<std::mutex> lock(delayedSpikesMutex);
    pImpl->delayedSpikes[event.delivery_step].push_back(event);
}

void SpikeSystem::processSpikes(SimulationStep currentStep) {
    // Process pending spikes in parallel using thread pool
    processSpikesParallel(currentStep);
}

void SpikeSystem::processSpikesParallel(SimulationStep currentStep) {
    // Batch processing for better cache locality and parallelism
    std::vector<DetailedSpikeEvent> batch;
    batch.reserve(1000); // Process in batches for cache efficiency
    
    // Collect spikes into batch
    while (!pImpl->pendingSpikes.empty() && batch.size() < batch.size()) {
        SpikeEvent event;
        bool success = pImpl->pendingSpikes.try_pop(event);
        if (success) {
            DetailedSpikeEvent detailed;
            detailed.source = event.source_neuron;
            detailed.timestamp = event.timestamp;
            detailed.step = event.step;
            batch.push_back(detailed);
        }
    }
    
    // Process batch in parallel
    processBatchInParallel(batch);
}

void SpikeSystem::processBatchInParallel(const std::vector<DetailedSpikeEvent>& batch) {
    // Use TBB parallel for to process batch items
    tbb::parallel_for(size_t(0), batch.size(), [&](size_t i) {
        const auto& detailed = batch[i];
        
        // Update spike count atomically
        {
            tbb::concurrent_hash_map<uint64_t, size_t>::accessor accessor;
            pImpl->spikeCountPerNeuron.find_or_insert(
                detailed.source.value, 
                [&](size_t& count) { ++count; },
                [&]() { return 1; }
            );
        }
        
        // Add to history (with thread-local optimization)
        pImpl->spikeHistory.push_back(detailed);
        
        // Update total spike count
        ++pImpl->totalSpikeCount;
        
        // Call handlers in parallel
        for (auto& handler : pImpl->handlers) {
            handler(detailed);
        }
    });
    
    // Trim history if needed
    trimHistoryIfNeeded();
}

void SpikeSystem::trimHistoryIfNeeded() {
    if (pImpl->spikeHistory.size() > pImpl->maxHistorySize) {
        std::lock_guard<std::mutex> lock(historyMutex);
        // Use deque for efficient removal from front
        size_t removeCount = pImpl->spikeHistory.size() - pImpl->maxHistorySize;
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin(), 
                                 pImpl->spikeHistory.begin() + removeCount);
    }
}

void SpikeSystem::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    // Find and process all delayed spikes scheduled for this step
    std::lock_guard<std::mutex> lock(delayedSpikesMutex);
    
    auto it = pImpl->delayedSpikes.find(currentStep);
    if (it != pImpl->delayedSpikes.end()) {
        // Process all spikes scheduled for this step in parallel
        const auto& events = it->second;
        if (events.size() > 1) {
            // Parallel processing for multiple events
            tbb::parallel_for(size_t(0), events.size(), [&](size_t i) {
                const auto& delayedEvent = events[i];
                for (auto& handler : pImpl->delayedHandlers) {
                    handler(delayedEvent);
                }
            });
        } else {
            // Single event, process sequentially
            for (const auto& delayedEvent : events) {
                for (auto& handler : pImpl->delayedHandlers) {
                    handler(delayedEvent);
                }
            }
        }
        
        // Remove processed spikes
        pImpl->delayedSpikes.erase(it);
    }
}

void SpikeSystem::registerHandler(SpikeHandler handler) {
    std::lock_guard<std::mutex> lock(handlersMutex);
    pImpl->handlers.push_back(handler);
}

void SpikeSystem::registerDelayedHandler(DelayedSpikeHandler handler) {
    std::lock_guard<std::mutex> lock(delayedHandlersMutex);
    pImpl->delayedHandlers.push_back(handler);
}

const std::vector<DetailedSpikeEvent>& SpikeSystem::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

void SpikeSystem::clearHistory() {
    std::lock_guard<std::mutex> lock(historyMutex);
    pImpl->spikeHistory.clear();
    pImpl->spikeCountPerNeuron.clear();
    pImpl->totalSpikeCount = 0;
}

size_t SpikeSystem::getSpikeCount() const {
    return pImpl->totalSpikeCount.load();
}

size_t SpikeSystem::getPendingSpikeCount() const {
    return pImpl->pendingSpikes.size();
}

size_t SpikeSystem::getPendingDelayedCount() const {
    std::lock_guard<std::mutex> lock(delayedSpikesMutex);
    size_t count = 0;
    for (const auto& pair : pImpl->delayedSpikes) {
        count += pair.second.size();
    }
    return count;
}

float SpikeSystem::getAverageSpikeRate() const {
    // Optimized calculation using parallel reduction
    if (pImpl->spikeHistory.size() < 2) {
        return 0.0f;
    }
    
    // Use parallel reduction for min/max calculation
    float minTime = std::numeric_limits<float>::max();
    float maxTime = -std::numeric_limits<float>::max();
    
    tbb::parallel_reduce(
        tbb::blocked_range<size_t>(0, pImpl->spikeHistory.size()),
        std::make_pair(minTime, maxTime),
        [](const tbb::blocked_range<size_t>& range, std::pair<float, float> acc) {
            float localMin = std::numeric_limits<float>::max();
            float localMax = -std::numeric_limits<float>::max();
            
            for (size_t i = range.begin(); i < range.end(); ++i) {
                const auto& spike = pImpl->spikeHistory[i];
                localMin = std::min(localMin, static_cast<float>(spike.timestamp));
                localMax = std::max(localMax, static_cast<float>(spike.timestamp));
            }
            
            return std::make_pair(
                std::min(acc.first, localMin),
                std::max(acc.second, localMax)
            );
        },
        [](std::pair<float, float> a, std::pair<float, float> b) {
            return std::make_pair(
                std::min(a.first, b.first),
                std::max(a.second, b.second)
            );
        }
    );
    
    float duration = maxTime - minTime;
    if (duration <= 0.0f) {
        return 0.0f;
    }
    
    return static_cast<float>(pImpl->totalSpikeCount.load()) / duration;
}

std::vector<NeuronId> SpikeSystem::getMostActiveNeurons(size_t count) const {
    // Use parallel sort for better performance
    std::vector<std::pair<uint64_t, size_t>> neuronCounts;
    neuronCounts.reserve(pImpl->spikeCountPerNeuron.size());
    
    // Copy data from concurrent hash map
    {
        tbb::concurrent_hash_map<uint64_t, size_t>::const_iterator it = pImpl->spikeCountPerNeuron.begin();
        while (it != pImpl->spikeCountPerNeuron.end()) {
            neuronCounts.push_back(std::make_pair(it->first, it->second));
            ++it;
        }
    }
    
    // Parallel sort using TBB
    tbb::parallel_sort(neuronCounts.begin(), neuronCounts.end(),
                      [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<NeuronId> result;
    result.reserve(std::min(count, neuronCounts.size()));
    for (size_t i = 0; i < std::min(count, neuronCounts.size()); ++i) {
        result.emplace_back(neuronCounts[i].first);
    }
    
    return result;
}

void SpikeSystem::reset() {
    // Clear pending spikes
    while (!pImpl->pendingSpikes.empty()) {
        SpikeEvent event;
        pImpl->pendingSpikes.try_pop(event);
    }
    
    std::lock_guard<std::mutex> lock(clearMutex);
    pImpl->spikeHistory.clear();
    pImpl->spikeCountPerNeuron.clear();
    pImpl->totalSpikeCount = 0;
    pImpl->delayedSpikes.clear();
}

} // namespace nlm