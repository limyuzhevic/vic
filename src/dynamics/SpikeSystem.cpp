#include "SpikeSystem.hpp"
#include <algorithm>
#include <limits>
#include <unordered_set>
#include <chrono>

namespace nlm {

struct SpikeSystem::Impl {
    std::queue<SpikeEvent> pendingSpikes;
    std::vector<DetailedSpikeEvent> spikeHistory;
    std::vector<SpikeHandler> handlers;
    std::vector<DelayedSpikeHandler> delayedHandlers;
    std::unordered_map<SimulationStep, std::vector<DelayedSpikeEvent>> delayedSpikes;  // Grouped by delivery step
    size_t maxHistorySize;
    
    // Track spike delivery statistics
    struct DeliveryStats {
        uint64_t totalDelivered = 0;
        uint64_t totalDropped = 0;
        float deliveryEfficiency = 1.0f;
        float averageDeliveryTime = 0.0f;
        std::chrono::steady_clock::time_point lastDelivery;
        uint64_t spikesProcessed = 0;
        uint64_t spikesDropped = 0;
        
        // Reset function to reinitialize stats
        void reset() {
            totalDelivered = 0;
            totalDropped = 0;
            deliveryEfficiency = 1.0f;
            averageDeliveryTime = 0.0f;
            lastDelivery = std::chrono::steady_clock::time_point();
            spikesProcessed = 0;
            spikesDropped = 0;
        }
    } deliveryStats;
    
    // Maximum number of delayed spikes to store
    size_t maxDelayedSpikes;
    
    // Function to lookup neurons by ID for spike delivery
    NeuronLookupFunc neuronLookupFunc;
    
    // Spike counting for statistics
    std::unordered_map<uint64_t, size_t> spikeCountPerNeuron;  // neuron_id -> count
    
    Impl() : maxHistorySize(10000), maxDelayedSpikes(100000) {
        deliveryStats.reset();
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
            pImpl->spikeHistory.erase(pImpl->spikeHistory.begin() + pImpl->maxHistorySize, pImpl->spikeHistory.end());
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
            
            // Update delivery statistics
            pImpl->deliveryStats.totalDelivered++;
            pImpl->deliveryStats.spikesProcessed++;
            
            // Record delivery time
            auto deliveryTime = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(deliveryTime - pImpl->deliveryStats.lastDelivery);
            float elapsedSeconds = static_cast<float>(elapsed.count()) / 1000000.0f;
            pImpl->deliveryStats.averageDeliveryTime = (pImpl->deliveryStats.averageDeliveryTime + elapsedSeconds) / 2.0f;
            pImpl->deliveryStats.lastDelivery = deliveryTime;
        }
        
        // Remove processed spikes
        pImpl->delayedSpikes.erase(it);
    }
    
    // Cleanup old delayed spikes to prevent memory leak
    // Remove entries older than the current step (approximate)
    auto cleanupThreshold = currentStep - pImpl->maxDelayedSpikes / 10;  // Keep last 10% of steps
    std::vector<SimulationStep> toRemove;
    for (const auto& pair : pImpl->delayedSpikes) {
        if (pair.first < cleanupThreshold) {
            toRemove.push_back(pair.first);
            pImpl->deliveryStats.totalDropped += pair.second.size();
            pImpl->deliveryStats.spikesDropped += pair.second.size();
        }
    }
    
    for (const auto& step : toRemove) {
        pImpl->delayedSpikes.erase(step);
    }
    
    // Recalculate delivery efficiency
    if (pImpl->deliveryStats.spikesProcessed > 0) {
        pImpl->deliveryStats.deliveryEfficiency = 
            static_cast<float>(pImpl->deliveryStats.totalDelivered) / 
            static_cast<float>(pImpl->deliveryStats.spikesProcessed);
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
    
    // Reset delivery statistics
    pImpl->deliveryStats = DeliveryStats{};
}

// Implementation of new methods
void SpikeSystem::setNeuronLookupFunc(NeuronLookupFunc lookupFunc) {
    pImpl->neuronLookupFunc = lookupFunc;
}

void SpikeSystem::setMaxDelayedSpikes(size_t maxDelayedSpikes) {
    pImpl->maxDelayedSpikes = maxDelayedSpikes;
}

SpikeSystem::DeliveryStats SpikeSystem::getDeliveryStats() const {
    return pImpl->deliveryStats;
}

float SpikeSystem::getDeliveryEfficiency() const {
    return pImpl->deliveryStats.deliveryEfficiency;
}

float SpikeSystem::getAverageDeliveryTime() const {
    return pImpl->deliveryStats.averageDeliveryTime;
}
