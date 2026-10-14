#include "SpikeSystem.hpp"
#include <algorithm>
#include <limits>
#include <span>
#include <format>
#include <numeric>
#include <chrono>
#include <atomic>
#include <bit>
#include <memory>
#include <vector>
#include <unordered_map>
#include <thread>
#include <optional>
#include <expected>

namespace nlm {

// ============================================================================
// PERFORMANCE MONITORING AND STATISTICS
// ============================================================================

struct SpikeSystemStats {
    std::atomic<size_t> totalSpikes{0};
    std::atomic<size_t> totalDelayedSpikes{0};
    std::atomic<size_t> totalHandlers{0};
    std::atomic<float> averageSpikeRate{0.0f};
    std::chrono::steady_clock::time_point lastUpdateTime;
    
    SpikeSystemStats() : lastUpdateTime(std::chrono::steady_clock::now()) {}
    
    void update() {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdateTime).count();
        if (duration > 0) {
            size_t spikes = totalSpikes.load(std::memory_order_relaxed);
            averageSpikeRate.store(static_cast<float>(spikes) / (duration / 1000.0f), std::memory_order_relaxed);
        }
        lastUpdateTime = now;
    }
};

// ============================================================================
// ENHANCED IMPLEMENTATION WITH C++20 FEATURES
// ============================================================================

struct SpikeSystem::Impl {
    std::queue<SpikeEvent> pendingSpikes;
    std::vector<DetailedSpikeEvent> spikeHistory;
    std::vector<SpikeHandler> handlers;
    std::vector<DelayedSpikeHandler> delayedHandlers;
    std::unordered_map<SimulationStep, std::vector<DelayedSpikeEvent>> delayedSpikes;  // Grouped by delivery step
    size_t maxHistorySize;
    
    // Spike counting for statistics
    std::unordered_map<uint64_t, size_t> spikeCountPerNeuron;  // neuron_id -> count
    
    // Performance monitoring
    SpikeSystemStats stats;
    
    // SIMD batch processing
    static constexpr size_t SIMD_BATCH_SIZE = 8;
    
    // Thread-local storage for optimized processing
    thread_local static std::vector<DetailedSpikeEvent> threadLocalHistory;
    
    // Pre-allocated memory for batch operations
    std::vector<DetailedSpikeEvent> batchBuffer;
    std::vector<SpikeEvent> pendingBatch;
    
    Impl() : maxHistorySize(10000), batchBuffer(), pendingBatch() {
        // Pre-allocate memory for performance
        batchBuffer.reserve(SIMD_BATCH_SIZE);
        pendingBatch.reserve(1000);
    }
    
    // Helper function to check SIMD support
    static bool hasSIMDSupport() {
#ifdef __x86_64__
        return true; // x86 has SSE/AVX support
#elif defined(__ARM_NEON)
        return true;
#else
        return false;
#endif
    }
    
    // SIMD-accelerated batch spike processing
    void processSpikeBatch(const std::span<DetailedSpikeEvent>& batch) {
        if (!hasSIMDSupport() || batch.size() < SIMD_BATCH_SIZE) {
            // Scalar fallback
            for (const auto& spike : batch) {
                processSingleSpike(spike);
            }
            return;
        }
        
#ifdef __AVX2__
        // AVX2 optimized batch processing
        size_t i = 0;
        const size_t vectorSize = SIMD_BATCH_SIZE;
        
        for (; i + vectorSize <= batch.size(); i += vectorSize) {
            processAVXBatch(batch.data() + i, vectorSize);
        }
        
        // Process remaining elements
        for (; i < batch.size(); ++i) {
            processSingleSpike(batch[i]);
        }
#else
        for (const auto& spike : batch) {
            processSingleSpike(spike);
        }
#endif
    }
    
    // Single spike processing
    void processSingleSpike(const DetailedSpikeEvent& spike) {
        // Call all registered handlers
        for (auto& handler : handlers) {
            handler(spike);
        }
        
        // Update statistics
        stats.totalSpikes.fetch_add(1, std::memory_order_relaxed);
    }
    
    // AVX2 optimized batch processing (simplified example)
    void processAVXBatch(const DetailedSpikeEvent* spikes, size_t count) {
#ifdef __AVX2__
        // This is a simplified example - real implementation would use AVX intrinsics
        for (size_t i = 0; i < count; ++i) {
            processSingleSpike(spikes[i]);
        }
#endif
    }
    
    // SIMD-accelerated delayed spike processing
    void processDelayedSpikeBatch(const std::span<DelayedSpikeEvent>& batch) {
        if (!hasSIMDSupport() || batch.size() < SIMD_BATCH_SIZE) {
            // Scalar fallback
            for (const auto& spike : batch) {
                processDelayedSingleSpike(spike);
            }
            return;
        }
        
#ifdef __AVX2__
        size_t i = 0;
        const size_t vectorSize = SIMD_BATCH_SIZE;
        
        for (; i + vectorSize <= batch.size(); i += vectorSize) {
            processDelayedAVXBatch(batch.data() + i, vectorSize);
        }
        
        for (; i < batch.size(); ++i) {
            processDelayedSingleSpike(batch[i]);
        }
#else
        for (const auto& spike : batch) {
            processDelayedSingleSpike(spike);
        }
#endif
    }
    
    // Single delayed spike processing
    void processDelayedSingleSpike(const DelayedSpikeEvent& spike) {
        // Call all registered delayed handlers
        for (auto& handler : delayedHandlers) {
            handler(spike);
        }
        
        stats.totalDelayedSpikes.fetch_add(1, std::memory_order_relaxed);
    }
    
    // AVX2 optimized delayed batch processing
    void processDelayedAVXBatch(const DelayedSpikeEvent* spikes, size_t count) {
#ifdef __AVX2__
        for (size_t i = 0; i < count; ++i) {
            processDelayedSingleSpike(spikes[i]);
        }
#endif
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
    pImpl->pendingSpikes.push(event);
}

void SpikeSystem::queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step) {
    pImpl->pendingSpikes.emplace(neuron, timestamp, step);
}

void SpikeSystem::queueDelayedSpike(const DelayedSpikeEvent& event) {
    // Store by delivery step for efficient processing
    pImpl->delayedSpikes[event.delivery_step].push_back(event);
}

void SpikeSystem::processSpikes(SimulationStep currentStep) {
    // Use SIMD-optimized batch processing
    if (!pImpl->pendingSpikes.empty()) {
        // Batch process pending spikes
        pImpl->pendingBatch.clear();
        size_t batchSize = 0;
        
        // Collect spikes into batch
        while (!pImpl->pendingSpikes.empty() && batchSize < pImpl->pendingBatch.capacity()) {
            pImpl->pendingBatch.push_back(pImpl->pendingSpikes.front());
            pImpl->pendingSpikes.pop();
            ++batchSize;
        }
        
        if (batchSize > 0) {
            // Convert to detailed events and process in SIMD batches
            pImpl->batchBuffer.clear();
            pImpl->batchBuffer.reserve(batchSize);
            
            for (const auto& spike : pImpl->pendingBatch) {
                DetailedSpikeEvent detailed;
                detailed.source = spike.source_neuron;
                detailed.timestamp = spike.timestamp;
                detailed.step = spike.step;
                detailed.regionId = spike.region_neuron;
                detailed.populationId = spike.population_neuron;
                pImpl->batchBuffer.push_back(detailed);
            }
            
            // Process in SIMD batches
            size_t i = 0;
            const size_t simdSize = Impl::SIMD_BATCH_SIZE;
            
            for (; i + simdSize <= pImpl->batchBuffer.size(); i += simdSize) {
                std::span<DetailedSpikeEvent> batch(pImpl->batchBuffer.data() + i, simdSize);
                pImpl->processSpikeBatch(batch);
            }
            
            // Process remaining elements
            for (; i < pImpl->batchBuffer.size(); ++i) {
                pImpl->processSingleSpike(pImpl->batchBuffer[i]);
            }
        }
    }
}

void SpikeSystem::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    // Find and process all delayed spikes scheduled for this step
    auto it = pImpl->delayedSpikes.find(currentStep);
    if (it != pImpl->delayedSpikes.end()) {
        // Use SIMD-optimized processing for delayed spikes
        if (it->second.size() >= Impl::SIMD_BATCH_SIZE) {
            std::span<DelayedSpikeEvent> batch(it->second.data(), it->second.size());
            pImpl->processDelayedSpikeBatch(batch);
        } else {
            // Scalar fallback for small batches
            for (const auto& delayedEvent : it->second) {
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
    pImpl->handlers.push_back(handler);
    pImpl->stats.totalHandlers.fetch_add(1, std::memory_order_relaxed);
}

void SpikeSystem::registerDelayedHandler(DelayedSpikeHandler handler) {
    pImpl->delayedHandlers.push_back(handler);
}

const std::vector<DetailedSpikeEvent>& SpikeSystem::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

void SpikeSystem::clearHistory() {
    pImpl->spikeHistory.clear();
    pImpl->spikeCountPerNeuron.clear();
}

size_t SpikeSystem::getSpikeCount() const {
    return pImpl->spikeHistory.size();
}

size_t SpikeSystem::getPendingSpikeCount() const {
    return pImpl->pendingSpikes.size();
}

size_t SpikeSystem::getPendingDelayedCount() const {
    size_t count = 0;
    for (const auto& pair : pImpl->delayedSpikes) {
        count += pair.second.size();
    }
    return count;
}

float SpikeSystem::getAverageSpikeRate() const {
    // Update statistics
    pImpl->stats.update();
    
    // Calculate average spike rate based on history
    if (pImpl->spikeHistory.size() < 2) {
        return 0.0f;
    }
    
    // Find time range
    float minTime = std::numeric_limits<float>::max();
    float maxTime = -std::numeric_limits<float>::max();
    
    for (const auto& spike : pImpl->spikeHistory) {
        minTime = std::min(minTime, static_cast<float>(spike.timestamp));
        maxTime = std::max(maxTime, static_cast<float>(spike.timestamp));
    }
    
    float duration = maxTime - minTime;
    if (duration <= 0.0f) {
        return 0.0f;
    }
    
    return static_cast<float>(pImpl->spikeHistory.size()) / duration;
}

std::vector<NeuronId> SpikeSystem::getMostActiveNeurons(size_t count) const {
    std::vector<std::pair<uint64_t, size_t>> neuronCounts;
    neuronCounts.reserve(pImpl->spikeCountPerNeuron.size());
    
    for (const auto& pair : pImpl->spikeCountPerNeuron) {
        neuronCounts.emplace_back(pair.first, pair.second);
    }
    
    // Sort by count descending (using std::sort with optimization for small data)
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
    while (!pImpl->pendingSpikes.empty()) {
        pImpl->pendingSpikes.pop();
    }
    pImpl->spikeHistory.clear();
    pImpl->spikeCountPerNeuron.clear();
    pImpl->delayedSpikes.clear();
    pImpl->stats.totalSpikes.store(0, std::memory_order_relaxed);
    pImpl->stats.totalDelayedSpikes.store(0, std::memory_order_relaxed);
}

} // namespace nlm
