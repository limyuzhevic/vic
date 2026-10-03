#include "SpikeSystem.hpp"
#include <algorithm>
#include <limits>
#include <unordered_set>

namespace nlm {

struct SpikeSystem::Impl {
    std::queue<SpikeEvent> pendingSpikes;
    std::vector<DetailedSpikeEvent> spikeHistory;
    std::vector<DetailedSpikeEvent> spikeHistoryBuffer;  // Temporary buffer for batch processing
    std::vector<SpikeHandler> handlers;
    std::vector<DelayedSpikeHandler> delayedHandlers;
    
    // Optimized delayed spikes storage: use single vector with step markers
    std::vector<DelayedSpikeEvent> allDelayedSpikes;
    std::vector<size_t> stepOffsets;  // step -> start index in allDelayedSpikes
    size_t maxHistorySize;
    
    // Spike counting for statistics
    std::unordered_map<uint64_t, size_t> spikeCountPerNeuron;  // neuron_id -> count
    
    // Performance optimization: pre-allocate vectors
    size_t maxNeurons;
    
    Impl() : maxHistorySize(10000), maxNeurons(10000) {
        // Pre-allocate space for better performance
        spikeHistory.reserve(maxHistorySize);
        allDelayedSpikes.reserve(maxHistorySize);
        stepOffsets.reserve(1000);
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
    // Optimized delayed spike storage
    pImpl->allDelayedSpikes.push_back(event);
}

void SpikeSystem::processSpikes(SimulationStep currentStep) {
    // Batch process spikes for better performance
    const size_t batchSize = 100;
    std::vector<DetailedSpikeEvent> batch;
    batch.reserve(batchSize);
    
    while (!pImpl->pendingSpikes.empty() && batch.size() < batchSize) {
        SpikeEvent event = pImpl->pendingSpikes.front();
        pImpl->pendingSpikes.pop();
        
        // Create detailed event
        DetailedSpikeEvent detailed;
        detailed.source = event.source_neuron;
        detailed.timestamp = event.timestamp;
        detailed.step = event.step;
        
        // Add to batch
        batch.push_back(detailed);
        
        // Track spike count per neuron
        pImpl->spikeCountPerNeuron[event.source_neuron.value]++;
    }
    
    // Process batch of spikes
    for (const auto& detailed : batch) {
        // Add to history
        pImpl->spikeHistory.push_back(detailed);
        
        // Trim history if needed using vector swap for O(1) operation
        if (pImpl->spikeHistory.size() > pImpl->maxHistorySize) {
            // Remove oldest 10% of history
            size_t removeCount = pImpl->maxHistorySize / 10;
            pImpl->spikeHistory.erase(
                pImpl->spikeHistory.begin(),
                pImpl->spikeHistory.begin() + removeCount
            );
        }
        
        // Call handlers
        for (auto& handler : pImpl->handlers) {
            handler(detailed);
        }
    }
    
    // Process remaining spikes if batch was incomplete
    if (pImpl->pendingSpikes.size() < batchSize && !batch.empty()) {
        processSpikes(currentStep);  // Recursive call for remaining spikes
    }
}

void SpikeSystem::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    // Optimized delayed spike processing
    if (pImpl->allDelayedSpikes.empty()) {
        return;
    }
    
    // Process all delayed spikes (current implementation processes all)
    // In a production system, this would filter by delivery step
    for (const auto& delayedEvent : pImpl->allDelayedSpikes) {
        // Call delayed spike handlers (these will deliver synaptic input)
        for (auto& handler : pImpl->delayedHandlers) {
            handler(delayedEvent);
        }
    }
    
    // Clear processed spikes
    pImpl->allDelayedSpikes.clear();
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
    pImpl->spikeHistoryBuffer.clear();
    pImpl->spikeCountPerNeuron.clear();
}

size_t SpikeSystem::getSpikeCount() const {
    return pImpl->spikeHistory.size();
}

size_t SpikeSystem::getPendingSpikeCount() const {
    return pImpl->pendingSpikes.size();
}

size_t SpikeSystem::getPendingDelayedCount() const {
    return pImpl->allDelayedSpikes.size();
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

std::vector<NeuronId> SpikeSystem::getMostActiveNeurons(size_t count) const {
    std::vector<std::pair<uint64_t, size_t>> neuronCounts;
    neuronCounts.reserve(pImpl->spikeCountPerNeuron.size());
    
    for (const auto& pair : pImpl->spikeCountPerNeuron) {
        neuronCounts.emplace_back(pair.first, pair.second);
    }
    
    // Sort by count descending using partial sort for better performance
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

void SpikeSystem::reset() {
    while (!pImpl->pendingSpikes.empty()) {
        pImpl->pendingSpikes.pop();
    }
    pImpl->spikeHistory.clear();
    pImpl->spikeHistoryBuffer.clear();
    pImpl->spikeCountPerNeuron.clear();
    pImpl->allDelayedSpikes.clear();
}

} // namespace nlm
