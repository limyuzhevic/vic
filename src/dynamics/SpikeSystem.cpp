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
    // Batch process all pending spikes - minimize queue operations
    if (pImpl->pendingSpikes.empty()) {
        return;
    }
    
    // Pre-allocate temporary buffers for better memory locality
    std::vector<DetailedSpikeEvent> detailedEvents;
    std::vector<std::pair<uint64_t, size_t>> countsToProcess;
    detailedEvents.reserve(std::min(pImpl->pendingSpikes.size(), 
                                    static_cast<size_t>(pImpl->maxHistorySize)));
    countsToProcess.reserve(pImpl->pendingSpikes.size());
    
    // Extract all events at once to minimize queue pops
    size_t maxEventsToProcess = std::min(pImpl->pendingSpikes.size(), 
                                        static_cast<size_t>(pImpl->maxHistorySize - pImpl->spikeHistory.size()));
    
    // Process events in batches
    for (size_t i = 0; i < maxEventsToProcess; ++i) {
        SpikeEvent event = pImpl->pendingSpikes.front();
        pImpl->pendingSpikes.pop();
        
        // Create detailed event
        DetailedSpikeEvent detailed;
        detailed.source = event.source_neuron;
        detailed.timestamp = event.timestamp;
        detailed.step = event.step;
        
        detailedEvents.push_back(detailed);
        countsToProcess.push_back(std::make_pair(event.source_neuron.value, 1));
    }
    
    // Update statistics and history in batch - use vector insert for contiguous memory
    pImpl->spikeHistory.insert(pImpl->spikeHistory.end(), 
                              detailedEvents.begin(), detailedEvents.end());
    
    // Merge spike counts efficiently - use optimized merging for small datasets
    if (countsToProcess.size() <= 32) {
        // For small numbers, use simple loop for better performance
        for (const auto& pair : countsToProcess) {
            pImpl->spikeCountPerNeuron[pair.first] += pair.second;
        }
    } else {
        // For larger numbers, sort and merge duplicates if any
        std::sort(countsToProcess.begin(), countsToProcess.end());
        uint64_t currentNeuron = UINT64_MAX;
        size_t countSum = 0;
        
        for (const auto& pair : countsToProcess) {
            if (pair.first != currentNeuron) {
                if (currentNeuron != UINT64_MAX) {
                    pImpl->spikeCountPerNeuron[currentNeuron] += countSum;
                }
                currentNeuron = pair.first;
                countSum = pair.second;
            } else {
                countSum += pair.second;
            }
        }
        if (currentNeuron != UINT64_MAX) {
            pImpl->spikeCountPerNeuron[currentNeuron] += countSum;
        }
    }
    
    // Trim history if needed - use efficient erase with move operations
    if (pImpl->spikeHistory.size() > pImpl->maxHistorySize) {
        size_t excess = pImpl->spikeHistory.size() - pImpl->maxHistorySize;
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin(), 
                                 pImpl->spikeHistory.begin() + excess);
    }
    
    // Call handlers - batch processing for better cache locality
    for (auto& handler : pImpl->handlers) {
        for (const auto& event : detailedEvents) {
            handler(event);
        }
    }
}

void SpikeSystem::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    // Batch process delayed spikes for better cache performance
    auto it = pImpl->delayedSpikes.find(currentStep);
    if (it != pImpl->delayedSpikes.end()) {
        // Pre-load handler lists for better locality
        const std::vector<DelayedSpikeHandler>& delayedHandlers = pImpl->delayedHandlers;
        
        // Process all spikes scheduled for this step in batch
        if (it->second.size() == 1) {
            // Single spike optimization - direct access
            const DelayedSpikeEvent& delayedEvent = it->second[0];
            for (const auto& handler : delayedHandlers) {
                handler(delayedEvent);
            }
        } else {
            // Multiple spikes - process in batch
            for (const auto& delayedEvent : it->second) {
                for (const auto& handler : delayedHandlers) {
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
}

} // namespace nlm
