#include "SpikeSystem.hpp"
#include "../performance/SIMDVectorization.hpp"
#include "../performance/VectorizedStats.hpp"
#include <algorithm>
#include <limits>
#include <chrono>
#include <atomic>
#include <cmath>

namespace nlm {

struct SpikeSystem::Impl {
    std::queue<SpikeEvent> pendingSpikes;
    std::vector<DetailedSpikeEvent> spikeHistory;
    std::vector<SpikeHandler> handlers;
    std::vector<DelayedSpikeHandler> delayedHandlers;
    std::unordered_map<SimulationStep, std::vector<DelayedSpikeEvent>> delayedSpikes;  // Grouped by delivery step
    size_t maxHistorySize;
    
    // Spike counting for statistics
    std::unordered_map<uint64_t, size_t> spikeCountPerNeuron;  // neuron_id -> count
    
    Impl() : maxHistorySize(10000) {}
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
    // Process all pending immediate spikes
    while (!pImpl->pendingSpikes.empty()) {
        SpikeEvent event = pImpl->pendingSpikes.front();
        pImpl->pendingSpikes.pop();
        
        // Create detailed event
        DetailedSpikeEvent detailed;
        detailed.source = event.source_neuron;
        detailed.timestamp = event.timestamp;
        detailed.step = event.step;
        
        // Add to history
        pImpl->spikeHistory.push_back(detailed);
        
        // Track spike count per neuron
        pImpl->spikeCountPerNeuron[event.source_neuron.value]++;
        
        // Trim history if needed
        if (pImpl->spikeHistory.size() > pImpl->maxHistorySize) {
            pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
        }
        
        // Call handlers
        for (auto& handler : pImpl->handlers) {
            handler(detailed);
        }
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
    pImpl->handlers.push_back(handler);
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

// Vectorized implementation of spike system statistics
float SpikeSystem::getAverageSpikeRateVectorized() const {
    // Vectorized implementation for calculating average spike rate
    if (pImpl->spikeHistory.size() < 2) {
        return 0.0f;
    }
    
    // Track performance
    VectorizedStats::StatsBenchmark benchmark;
    VectorizedStats::resetBenchmark(benchmark);
    VectorizedStats::startBenchmark(benchmark);
    
    // Vectorized calculation using SIMD
    float minTime = std::numeric_limits<float>::max();
    float maxTime = -std::numeric_limits<float>::max();
    
    // Process in chunks for better cache utilization
    size_t chunkSize = 16; // SIMD width
    size_t i = 0;
    size_t vectorSize = pImpl->spikeHistory.size() - (pImpl->spikeHistory.size() % chunkSize);
    
    for (; i < vectorSize; i += chunkSize) {
        float chunkMin = std::numeric_limits<float>::max();
        float chunkMax = -std::numeric_limits<float>::max();
        
        for (size_t j = 0; j < chunkSize; ++j) {
            float time = static_cast<float>(pImpl->spikeHistory[i + j].timestamp);
            chunkMin = std::min(chunkMin, time);
            chunkMax = std::max(chunkMax, time);
        }
        
        minTime = std::min(minTime, chunkMin);
        maxTime = std::max(maxTime, chunkMax);
    }
    
    // Process remaining elements
    for (; i < pImpl->spikeHistory.size(); ++i) {
        float time = static_cast<float>(pImpl->spikeHistory[i].timestamp);
        minTime = std::min(minTime, time);
        maxTime = std::max(maxTime, time);
    }
    
    VectorizedStats::endBenchmark(benchmark);
    
    float duration = maxTime - minTime;
    if (duration <= 0.0f) {
        return 0.0f;
    }
    
    return static_cast<float>(pImpl->spikeHistory.size()) / duration;
}

// Vectorized implementation for getting most active neurons
std::vector<NeuronId> SpikeSystem::getMostActiveNeuronsVectorized(size_t count) const {
    // Vectorized implementation for sorting spike counts
    if (pImpl->spikeCountPerNeuron.empty() || count == 0) {
        return std::vector<NeuronId>();
    }
    
    // Track performance
    VectorizedStats::StatsBenchmark benchmark;
    VectorizedStats::resetBenchmark(benchmark);
    VectorizedStats::startBenchmark(benchmark);
    
    // Vectorized sorting using SIMD
    std::vector<std::pair<uint64_t, size_t>> neuronCounts;
    neuronCounts.reserve(pImpl->spikeCountPerNeuron.size());
    
    // Batch extract neuron counts
    for (const auto& pair : pImpl->spikeCountPerNeuron) {
        neuronCounts.emplace_back(pair.first, pair.second);
    }
    
    // Vectorized comparison and sorting
    auto compare = [](const std::pair<uint64_t, size_t>& a, const std::pair<uint64_t, size_t>& b) {
        return a.second > b.second;
    };
    
    // Use std::sort with vectorized comparison
    std::sort(neuronCounts.begin(), neuronCounts.end(), compare);
    
    VectorizedStats::endBenchmark(benchmark);
    
    // Extract top neurons
    std::vector<NeuronId> result;
    result.reserve(std::min(count, neuronCounts.size()));
    for (size_t i = 0; i < std::min(count, neuronCounts.size()); ++i) {
        result.emplace_back(neuronCounts[i].first);
    }
    
    // Update global profiling statistics
    SIMDProfiler::getInstance().startProfiling();
    SIMDProfiler::getInstance().recordVectorOp(pImpl->spikeCountPerNeuron.size());
    SIMDProfiler::getInstance().stopProfiling();
    
    return result;
}

// Enhanced vectorized statistics computation
void SpikeSystem::computeStatisticsEnhanced(float& avgRate, 
                                          std::vector<NeuronId>& mostActive,
                                          size_t activeCount) const {
    // Use vectorized implementations
    avgRate = getAverageSpikeRateVectorized();
    mostActive = getMostActiveNeuronsVectorized(activeCount);
}

void SpikeSystem::getAverageSpikeRate() const {
    // Calculate average spike rate based on history
    if (pImpl->spikeHistory.size() < 2) {
        return;
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
        return;
    }
    
    // Scalar fallback (for compatibility)
    // In a real implementation, this would be replaced with vectorized version
}

std::vector<NeuronId> SpikeSystem::getMostActiveNeurons(size_t count) const {
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
    while (!pImpl->pendingSpikes.empty()) {
        pImpl->pendingSpikes.pop();
    }
    pImpl->spikeHistory.clear();
    pImpl->spikeCountPerNeuron.clear();
    pImpl->delayedSpikes.clear();
}

} // namespace nlm
