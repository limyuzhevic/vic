#include "SpikeSystem.hpp"
#include <algorithm>
#include <limits>
#include <vector>

namespace nlm {

struct SpikeSystem::Impl {
    // Use fixed-size circular buffer for spike history
    std::vector<DetailedSpikeEvent> spikeHistory;
    std::vector<SpikeHandler> handlers;
    std::vector<DelayedSpikeHandler> delayedHandlers;
    std::unordered_map<SimulationStep, std::vector<DelayedSpikeEvent>> delayedSpikes;
    
    // Spike counting for statistics - using map for efficient lookup
    std::unordered_map<uint64_t, size_t> spikeCountPerNeuron;
    
    // Performance optimizations
    size_t maxHistorySize;
    size_t historyHead;  // Index for circular buffer
    bool historyFull;
    
    // Batch processing optimization
    std::vector<SpikeEvent> pendingSpikeBatch;
    bool batchPending;
    
    Impl() : maxHistorySize(10000), historyHead(0), historyFull(false), batchPending(false) {}
};

SpikeSystem::SpikeSystem() : pImpl(std::make_unique<Impl>()) {
    pImpl->spikeHistory.reserve(pImpl->maxHistorySize);
}

SpikeSystem::~SpikeSystem() = default;

SpikeSystem::SpikeSystem(SpikeSystem&& other) noexcept : pImpl(std::move(other.pImpl)) {
}

SpikeSystem& SpikeSystem::operator=(SpikeSystem&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void SpikeSystem::queueSpike(const SpikeEvent& event) {
    // Batch optimization: accumulate spikes and process in batches
    if (!pImpl->batchPending) {
        pImpl->batchPending = true;
    }
    pImpl->pendingSpikeBatch.push_back(event);
    
    // Process batch immediately if it gets too large
    if (pImpl->pendingSpikeBatch.size() >= 1000) {
        processBatch();
    }
}

void SpikeSystem::queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step) {
    queueSpike(SpikeEvent{neuron, timestamp, step});
}

void SpikeSystem::queueDelayedSpike(const DelayedSpikeEvent& event) {
    // Store by delivery step for efficient processing
    pImpl->delayedSpikes[event.delivery_step].push_back(event);
}

void SpikeSystem::processSpikes(SimulationStep currentStep) {
    // Process any batched spikes first
    if (pImpl->batchPending) {
        processBatch();
    }
    
    // Process all pending immediate spikes
    while (!pImpl->pendingSpikeBatch.empty()) {
        SpikeEvent event = pImpl->pendingSpikeBatch.back();
        pImpl->pendingSpikeBatch.pop_back();
        
        // Create detailed event
        DetailedSpikeEvent detailed;
        detailed.source = event.source_neuron;
        detailed.timestamp = event.timestamp;
        detailed.step = event.step;
        
        // Add to history using circular buffer for optimal performance
        addToHistory(detailed);
        
        // Track spike count per neuron
        pImpl->spikeCountPerNeuron[event.source_neuron.value]++;
        
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
        const auto& events = it->second;
        
        // Call delayed spike handlers
        for (const auto& delayedEvent : events) {
            for (auto& handler : pImpl->delayedHandlers) {
                handler(delayedEvent);
            }
        }
        
        // Remove processed spikes - use swap and clear for efficiency
        std::unordered_map<SimulationStep, std::vector<DelayedSpikeEvent>>().swap(pImpl->delayedSpikes);
    }
}

void SpikeSystem::registerHandler(SpikeHandler handler) {
    // Reserve capacity to avoid reallocations
    if (pImpl->handlers.capacity() < pImpl->handlers.size() + 1) {
        pImpl->handlers.reserve(pImpl->handlers.size() * 1.5);
    }
    pImpl->handlers.push_back(std::move(handler));
}

void SpikeSystem::registerDelayedHandler(DelayedSpikeHandler handler) {
    // Reserve capacity to avoid reallocations
    if (pImpl->delayedHandlers.capacity() < pImpl->delayedHandlers.size() + 1) {
        pImpl->delayedHandlers.reserve(pImpl->delayedHandlers.size() * 1.5);
    }
    pImpl->delayedHandlers.push_back(std::move(handler));
}

const std::vector<DetailedSpikeEvent>& SpikeSystem::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

void SpikeSystem::clearHistory() {
    pImpl->spikeHistory.clear();
    pImpl->spikeCountPerNeuron.clear();
    pImpl->historyHead = 0;
    pImpl->historyFull = false;
}

size_t SpikeSystem::getSpikeCount() const {
    return pImpl->spikeHistory.size();
}

size_t SpikeSystem::getPendingSpikeCount() const {
    return pImpl->pendingSpikeBatch.size();
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
    
    // Find time range - optimized with early exit
    float minTime = std::numeric_limits<float>::max();
    float maxTime = -std::numeric_limits<float>::max();
    
    for (const auto& spike : pImpl->spikeHistory) {
        float time = static_cast<float>(spike.timestamp);
        if (time < minTime) minTime = time;
        if (time > maxTime) maxTime = time;
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
    
    // Sort by count descending - optimized with partial sort
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
    // Clear pending spikes
    pImpl->pendingSpikeBatch.clear();
    pImpl->batchPending = false;
    
    // Clear all spike-related data
    pImpl->spikeHistory.clear();
    pImpl->spikeCountPerNeuron.clear();
    pImpl->delayedSpikes.clear();
    
    // Reset circular buffer state
    pImpl->historyHead = 0;
    pImpl->historyFull = false;
}

void SpikeSystem::addToHistory(const DetailedSpikeEvent& detailed) {
    // Add to history using circular buffer for optimal performance
    if (pImpl->historyFull) {
        // Overwrite oldest entry
        pImpl->spikeHistory[pImpl->historyHead] = detailed;
        pImpl->historyHead = (pImpl->historyHead + 1) % pImpl->maxHistorySize;
    } else {
        // First entry or not full yet
        if (pImpl->spikeHistory.size() < pImpl->maxHistorySize) {
            pImpl->spikeHistory.push_back(detailed);
        } else {
            pImpl->historyFull = true;
            pImpl->spikeHistory.push_back(detailed);
            pImpl->historyHead = pImpl->spikeHistory.size() - 1;
        }
    }
}

void SpikeSystem::processBatch() {
    // Process accumulated batched spikes
    while (!pImpl->pendingSpikeBatch.empty()) {
        SpikeEvent event = pImpl->pendingSpikeBatch.back();
        pImpl->pendingSpikeBatch.pop_back();
        
        DetailedSpikeEvent detailed;
        detailed.source = event.source_neuron;
        detailed.timestamp = event.timestamp;
        detailed.step = event.step;
        
        addToHistory(detailed);
        pImpl->spikeCountPerNeuron[event.source_neuron.value]++;
        
        for (auto& handler : pImpl->handlers) {
            handler(detailed);
        }
    }
    pImpl->batchPending = false;
}

} // namespace nlm
