#include "SpikeSystem.hpp"
#include <algorithm>
#include <limits>

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
    // Optimized spike queuing with direct push
    pImpl->pendingSpikes.push(event);
}

void SpikeSystem::queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step) {
    // Optimized spike creation and queuing with in-place constructor
    pImpl->pendingSpikes.emplace(neuron, timestamp, step);
}

void SpikeSystem::queueDelayedSpike(const DelayedSpikeEvent& event) {
    // Optimized delayed spike storage - reserve space to avoid reallocations
    auto& delayedList = pImpl->delayedSpikes[event.delivery_step];
    if (delayedList.size() >= delayedList.capacity()) {
        delayedList.reserve(delayedList.size() * 1.5);
    }
    delayedList.push_back(event);
}

void SpikeSystem::processSpikes(SimulationStep currentStep) {
    // Optimized spike processing with batch operations
    while (!pImpl->pendingSpikes.empty()) {
        // Direct access to front element for efficiency
        SpikeEvent event = std::move(pImpl->pendingSpikes.front());
        pImpl->pendingSpikes.pop();
        
        // Create detailed event with direct member initialization
        DetailedSpikeEvent detailed{
            event.source_neuron,
            event.timestamp,
            event.step
        };
        
        // Add to history
        pImpl->spikeHistory.push_back(detailed);
        
        // Track spike count per neuron - optimized access
        pImpl->spikeCountPerNeuron[event.source_neuron.value]++;
        
        // Trim history if needed - check capacity first
        if (pImpl->spikeHistory.size() > pImpl->maxHistorySize) {
            // Use swap and pop for O(1) removal from beginning
            std::rotate(pImpl->spikeHistory.begin(),
                       pImpl->spikeHistory.begin() + 1,
                       pImpl->spikeHistory.end());
            pImpl->spikeHistory.pop_back();
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
        // Batch process all spikes scheduled for this step
        std::vector<DelayedSpikeEvent>& delayedList = it->second;
        
        // Call delayed spike handlers for all events
        for (auto& delayedEvent : delayedList) {
            // Call delayed spike handlers (these will deliver synaptic input)
            for (auto& handler : pImpl->delayedHandlers) {
                handler(delayedEvent);
            }
        }
        
        // Clear all spikes for this step at once
        delayedList.clear();
        delayedList.shrink_to_fit();
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

std::vector<NeuronId> SpikeSystem::getMostActiveNeurons(size_t count) const {
    // Optimized most active neurons calculation
    std::vector<std::pair<uint64_t, size_t>> neuronCounts;
    neuronCounts.reserve(pImpl->spikeCountPerNeuron.size());
    
    // Batch extract and convert to vector of pairs
    for (const auto& pair : pImpl->spikeCountPerNeuron) {
        neuronCounts.emplace_back(pair.first, pair.second);
    }
    
    // Sort by count descending - use std::sort with lambda
    std::sort(neuronCounts.begin(), neuronCounts.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Create result vector
    std::vector<NeuronId> result;
    size_t resultSize = std::min(count, neuronCounts.size());
    result.reserve(resultSize);
    
    for (size_t i = 0; i < resultSize; ++i) {
        result.emplace_back(neuronCounts[i].first);
    }
    
    return result;
}

void SpikeSystem::reset() {
    // Optimized reset with direct access
    while (!pImpl->pendingSpikes.empty()) {
        pImpl->pendingSpikes.pop();
    }
    pImpl->spikeHistory.clear();
    pImpl->spikeHistory.shrink_to_fit();
    
    pImpl->spikeCountPerNeuron.clear();
    
    // Optimized delayed spikes clear
    pImpl->delayedSpikes.clear();
}

} // namespace nlm
