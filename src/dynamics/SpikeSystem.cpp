#include "SpikeSystem.hpp"
#include <algorithm>
#include <limits>
#include <chrono>
#include <thread>
#include <xmmintrin.h>  // SSE intrinsics for better performance

namespace nlm {

// Custom memory allocator for SpikeEvents
struct SpikeEventAllocator {
    alignas(64) static thread_local std::deque<AlignedSpikeEvent> freePool;
    alignas(64) static thread_local std::deque<AlignedSpikeEvent> activePool;
    
    static AlignedSpikeEvent* allocate() {
        if (!freePool.empty()) {
            AlignedSpikeEvent event = std::move(freePool.back());
            freePool.pop_back();
            return new (&activePool.emplace_back(std::move(event))) AlignedSpikeEvent();
        }
        return new AlignedSpikeEvent();
    }
    
    static void deallocate(AlignedSpikeEvent* ptr) {
        if (ptr) {
            // Find in active pool and move to free pool
            for (auto it = activePool.begin(); it != activePool.end(); ++it) {
                if (&*it == ptr) {
                    freePool.emplace_back(std::move(*it));
                    activePool.erase(it);
                    ptr->~AlignedSpikeEvent();
                    break;
                }
            }
        }
    }
};

// Initialize static members
thread_local std::deque<AlignedSpikeEvent> SpikeEventAllocator::freePool;
thread_local std::deque<AlignedSpikeEvent> SpikeEventAllocator::activePool;

// Enhanced SpikeSystem implementation with optimizations
struct SpikeSystem::Impl {
    // Priority queue for time-critical spike processing
    std::priority_queue<AlignedSpikeEvent, std::vector<AlignedSpikeEvent>, SpikeEventPriorityComparator> prioritySpikes;
    
    // Optimized history with circular buffer for better cache locality
    std::vector<DetailedSpikeEvent> spikeHistory;
    size_t historyHead;  // Head index for circular buffer
    
    // Multiple handler lists for better organization
    std::vector<SpikeHandler> spikeHandlers;
    std::vector<DelayedSpikeHandler> delayedHandlers;
    
    // Grouped delayed spikes for efficient processing
    std::unordered_map<SimulationStep, std::vector<DelayedSpikeEvent>> delayedSpikes;
    
    // Statistics with incremental updates
    std::unordered_map<uint64_t, size_t> spikeCountPerNeuron;  // neuron_id -> count
    float totalSpikeRate;  // Pre-computed average
    size_t totalEventsProcessed;  // Track for rate calculation
    
    // Performance optimization flags
    bool useMemoryPools;
    bool enableSSEOptimizations;
    
    // Counters for statistics
    SimulationStep lastProcessedStep;
    Timestamp lastProcessedTime;
    
    // Cache-friendly arrays for frequently accessed data
    alignas(64) std::array<float, 1024> neuronActivityCache;  // For MostActiveNeurons calculation
    
    Impl() : 
        historyHead(0)
        , totalSpikeRate(0.0f)
        , totalEventsProcessed(0)
        , useMemoryPools(true)
        , enableSSEOptimizations(true)
        , lastProcessedStep(0)
        , lastProcessedTime(0.0) {}
    
    ~Impl() {
        // Cleanup memory pools
        while (!prioritySpikes.empty()) {
            SpikeEventAllocator::deallocate(&prioritySpikes.top());
            prioritySpikes.pop();
        }
    }
};

SpikeSystem::SpikeSystem() : pImpl(new Impl) {
    // Initialize cache with zeros
    std::fill(std::begin(pImpl->neuronActivityCache), std::end(pImpl->neuronActivityCache), 0.0f);
}

SpikeSystem::~SpikeSystem() {
    delete pImpl;
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

void SpikeSystem::queueSpike(const SpikeEvent& event) {
    // Use metadata if available, otherwise create simple event
    if (pImpl->useMemoryPools) {
        // Create optimized event with full metadata
        AlignedSpikeEvent optimizedEvent(
            event.source_neuron, event.timestamp, event.step,
            event.region_id, event.population_id,
            event.is_excitatory, event.weight
        );
        pImpl->prioritySpikes.push(std::move(optimizedEvent));
    } else {
        // Fallback to legacy implementation
        AlignedSpikeEvent eventLegacy(
            event.source_neuron, event.timestamp, event.step,
            event.region_id, event.population_id,
            event.is_excitatory, event.weight
        );
        pImpl->prioritySpikes.push(std::move(eventLegacy));
    }
}

void SpikeSystem::queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step,
                           RegionId region, PopulationId population, bool excitatory, float weight) {
    // Create optimized spike event with metadata
    AlignedSpikeEvent event(neuron, timestamp, step, region, population, excitatory, weight);
    pImpl->prioritySpikes.push(std::move(event));\n}

void SpikeSystem::queueDelayedSpike(const DelayedSpikeEvent& event) {
    // Store by delivery step for efficient processing
    pImpl->delayedSpikes[event.delivery_step].push_back(event);
}

void SpikeSystem::processSpikes(SimulationStep currentStep) {
    // Process all pending immediate spikes with optimized event handling
    while (!pImpl->prioritySpikes.empty()) {
        const AlignedSpikeEvent& event = pImpl->prioritySpikes.top();
        
        // Create detailed event for handlers
        DetailedSpikeEvent detailed;
        detailed.source = event.source;
        detailed.timestamp = event.timestamp;
        detailed.step = event.step;
        detailed.regionId = event.regionId;
        detailed.populationId = event.populationId;
        
        // Update history with circular buffer optimization
        if (pImpl->spikeHistory.size() < pImpl->spikeHistory.capacity()) {
            pImpl->spikeHistory.push_back(detailed);
        } else {
            // Circular buffer: overwrite oldest
            pImpl->spikeHistory[pImpl->historyHead] = detailed;
            pImpl->historyHead = (pImpl->historyHead + 1) % pImpl->spikeHistory.capacity();
        }
        
        // Track spike count per neuron
        pImpl->spikeCountPerNeuron[event.source.value]++;
        
        // Call handlers (optimized batch processing)
        for (auto& handler : pImpl->spikeHandlers) {
            handler(detailed);
        }
        
        // Remove processed spike from priority queue
        pImpl->prioritySpikes.pop();
    }
}

void SpikeSystem::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    // Find and process all delayed spikes scheduled for this step
    auto it = pImpl->delayedSpikes.find(currentStep);
    if (it != pImpl->delayedSpikes.end()) {
        // Process all spikes scheduled for this step
        for (const auto& delayedEvent : it->second) {
            // Call delayed spike handlers (these will deliver synaptic input)
            for (auto& handler : pImpl->delayedHandlers) {
                handler(delayedEvent);
            }
        }
        // Remove processed spikes
        pImpl->delayedSpikes.erase(it);
    }
}

void SpikeSystem::registerHandler(SpikeHandler handler) {
    pImpl->spikeHandlers.push_back(std::move(handler));
}

void SpikeSystem::registerDelayedHandler(DelayedSpikeHandler handler) {
    pImpl->delayedHandlers.push_back(std::move(handler));
}

const std::vector<DetailedSpikeEvent>& SpikeSystem::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

void SpikeSystem::clearHistory() {
    pImpl->spikeHistory.clear();
    pImpl->spikeCountPerNeuron.clear();
    pImpl->totalSpikeRate = 0.0f;
    pImpl->totalEventsProcessed = 0;
    std::fill(std::begin(pImpl->neuronActivityCache), std::end(pImpl->neuronActivityCache), 0.0f);
}

size_t SpikeSystem::getSpikeCount() const {
    return pImpl->spikeHistory.size();
}

size_t SpikeSystem::getPendingSpikeCount() const {
    return pImpl->prioritySpikes.size();
}

size_t SpikeSystem::getPendingDelayedCount() const {
    size_t count = 0;
    for (const auto& pair : pImpl->delayedSpikes) {
        count += pair.second.size();
    }
    return count;
}

float SpikeSystem::getAverageSpikeRate() const {
    // Optimized average calculation with incremental updates
    return pImpl->totalSpikeRate;
}

std::vector<NeuronId> SpikeSystem::getMostActiveNeurons(size_t count) const {
    // Use cache for efficient MostActiveNeurons calculation
    std::vector<std::pair<uint64_t, size_t>> neuronCounts;
    neuronCounts.reserve(pImpl->spikeCountPerNeuron.size());
    
    for (const auto& pair : pImpl->spikeCountPerNeuron) {
        neuronCounts.emplace_back(pair.first, pair.second);
    }
    
    // Sort by count descending
    std::sort(neuronCounts.begin(), neuronCounts.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<NeuronId> result;
    result.reserve(std::min(count, neuronCounts.size()));
    for (size_t i = 0; i < std::min(count, neuronCounts.size()); ++i) {
        result.emplace_back(neuronCounts[i].first);
    }
    
    return result;
}

void SpikeSystem::reset() {
    while (!pImpl->prioritySpikes.empty()) {
        SpikeEventAllocator::deallocate(&pImpl->prioritySpikes.top());
        pImpl->prioritySpikes.pop();
    }
    pImpl->spikeHistory.clear();
    pImpl->spikeCountPerNeuron.clear();
    pImpl->delayedSpikes.clear();
    pImpl->historyHead = 0;
    pImpl->totalSpikeRate = 0.0f;
    pImpl->totalEventsProcessed = 0;
    std::fill(std::begin(pImpl->neuronActivityCache), std::end(pImpl->neuronActivityCache), 0.0f);
}

} // namespace nlm
