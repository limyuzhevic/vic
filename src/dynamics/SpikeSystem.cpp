#include "SpikeSystem.hpp"
#include "../core/MemoryPool.hpp"
#include <algorithm>
#include <limits>
#include <unordered_map>

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
    
    // Cache for most active neurons to avoid recalculation
    bool neuronCountsDirty;
    std::vector<std::pair<uint64_t, size_t>> cachedNeuronCounts;
    
    // Memory pools for efficient allocation/deallocation
    SpikeEventPool spikeEventPool;
    DelayedSpikeEventPool delayedSpikeEventPool;
    
    Impl() : maxHistorySize(10000), neuronCountsDirty(true) {}
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
    // Use memory pool for delayed spike events
    DelayedSpikeEvent* poolEvent = pImpl->delayedSpikeEventPool.allocate();
    *poolEvent = event;
    
    // Store by delivery step for efficient processing
    pImpl->delayedSpikes[event.delivery_step].push_back(*poolEvent);
}

void SpikeSystem::processSpikes(SimulationStep currentStep) {
    // Process all pending immediate spikes
    while (!pImpl->pendingSpikes.empty()) {
        SpikeEvent event = pImpl->pendingSpikes.front();
        pImpl->pendingSpikes.pop();
        
        // Create detailed event using memory pool
        DetailedSpikeEvent* detailed = pImpl->spikeEventPool.allocate();
        detailed->source = event.source_neuron;
        detailed->timestamp = event.timestamp;
        detailed->step = event.step;
        
        // Add to history
        pImpl->spikeHistory.push_back(*detailed);
        
        // Track spike count per neuron
        pImpl->spikeCountPerNeuron[event.source_neuron.value]++;
        
        // Trim history if needed - FIXED: use erase with iterator, not begin()
        if (pImpl->spikeHistory.size() > pImpl->maxHistorySize) {
            // Remove from the beginning more efficiently
            size_t removeCount = pImpl->spikeHistory.size() - pImpl->maxHistorySize;
            pImpl->spikeHistory.erase(pImpl->spikeHistory.begin(), 
                                     pImpl->spikeHistory.begin() + removeCount);
            
            // Clear spike count for neurons that might have had spikes removed
            // (This is a simplification - in a real implementation, we'd track
            // spike history per neuron)
            if (removeCount > 0) {
                // For efficiency, we could maintain a rolling count per neuron
                // For now, just clear and let it rebuild
                pImpl->spikeCountPerNeuron.clear();
                
                // Rebuild spike count from remaining history
                for (const auto& spike : pImpl->spikeHistory) {
                    pImpl->spikeCountPerNeuron[spike.source.value]++;
                }
            }
        }
        
        // Optimization: Mark that most active neurons needs recalculation
        pImpl->neuronCountsDirty = true;
        
        // Call handlers
        for (auto& handler : pImpl->handlers) {
            handler(*detailed);
        }
    }
}

void SpikeSystem::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    // Find and process all delayed spikes scheduled for this step
    auto it = pImpl->delayedSpikes.find(currentStep);
    if (it != pImpl->delayedSpikes.end()) {
        // Batch process all delayed spikes for this step
        const auto& eventsToProcess = it->second;
        
        // Process all events for this step in a single loop
        for (const auto& delayedEvent : eventsToProcess) {
            // Call delayed spike handlers
            for (auto& handler : pImpl->delayedHandlers) {
                handler(delayedEvent);
            }
        }
        
        // Remove all processed spikes at once
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
    
    // Clear memory pools
    pImpl->spikeEventPool.clear();
    pImpl->delayedSpikeEventPool.clear();
    
    // Mark cache dirty
    pImpl->neuronCountsDirty = true;
    pImpl->cachedNeuronCounts.clear();
}

void SpikeSystem::registerGlobalMemoryPool() {
    // Register memory pools for use by other systems
    // This allows other parts of the codebase to access these memory pools
    // for consistent memory management
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
    // Use cached version if available and not dirty
    if (!pImpl->neuronCountsDirty || pImpl->cachedNeuronCounts.empty()) {
        // Rebuild cache
        pImpl->cachedNeuronCounts.clear();
        pImpl->cachedNeuronCounts.reserve(pImpl->spikeCountPerNeuron.size());
        
        for (const auto& pair : pImpl->spikeCountPerNeuron) {
            pImpl->cachedNeuronCounts.emplace_back(pair.first, pair.second);
        }
        
        // Sort by count descending
        std::sort(pImpl->cachedNeuronCounts.begin(), pImpl->cachedNeuronCounts.end(),
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        pImpl->neuronCountsDirty = false;
    }
    
    // Use cached data
    std::vector<NeuronId> result;
    result.reserve(std::min(count, pImpl->cachedNeuronCounts.size()));
    for (size_t i = 0; i < std::min(count, pImpl->cachedNeuronCounts.size()); ++i) {
        result.emplace_back(pImpl->cachedNeuronCounts[i].first);
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
    
    // Clear cache
    pImpl->neuronCountsDirty = true;
    pImpl->cachedNeuronCounts.clear();
    
    // Clear memory pools
    pImpl->spikeEventPool.clear();
    pImpl->delayedSpikeEventPool.clear();
}

} // namespace nlm