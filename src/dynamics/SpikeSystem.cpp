#include "SpikeSystem.hpp"
#include <algorithm>
#include <limits>
#include <mutex>
#include <chrono>
#include <stdexcept>

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
    
    // Thread safety
    mutable std::mutex mutex;
    bool initialized;
    
    Impl() : maxHistorySize(10000), initialized(true) {
        if (maxHistorySize == 0) {
            throw std::invalid_argument("maxHistorySize cannot be zero");
        }
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
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->pendingSpikes.push(event);
    }

void SpikeSystem::queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->pendingSpikes.emplace(neuron, timestamp, step);
    }

void SpikeSystem::queueDelayedSpike(const DelayedSpikeEvent& event) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        // Store by delivery step for efficient processing
        pImpl->delayedSpikes[event.delivery_step].push_back(event);
    }

void SpikeSystem::processSpikes(SimulationStep currentStep) {
        if (!pImpl->initialized) {
            throw std::runtime_error("SpikeSystem not properly initialized");
        }
        
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        // Create a local copy of spikes to process to avoid holding lock during handler calls
        std::vector<SpikeEvent> spikesToProcess;
        while (!pImpl->pendingSpikes.empty()) {
            spikesToProcess.push_back(pImpl->pendingSpikes.front());
            pImpl->pendingSpikes.pop();
        }
        
        // Process spikes outside the lock to avoid blocking other operations
        for (const auto& event : spikesToProcess) {
            // Create detailed event
            DetailedSpikeEvent detailed;
            detailed.source = event.source_neuron;
            detailed.timestamp = event.timestamp;
            detailed.step = event.step;
            
            // Add to history with efficient trimming
            {
                std::lock_guard<std::mutex> historyLock(pImpl->mutex);
                pImpl->spikeHistory.push_back(detailed);
                
                // Track spike count per neuron
                pImpl->spikeCountPerNeuron[event.source_neuron.value]++;
                
                // Trim history if needed (use more efficient approach for large history)
                if (pImpl->spikeHistory.size() > pImpl->maxHistorySize) {
                    // Remove from beginning in batches if history is very large
                    size_t excess = pImpl->spikeHistory.size() - pImpl->maxHistorySize;
                    if (excess > 100) {
                        // For large excess, remove in chunks
                        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin(), 
                                                 pImpl->spikeHistory.begin() + excess);
                    } else {
                        // For small excess, remove normally
                        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
                    }
                }
            }
            
            // Call handlers outside the main lock to avoid blocking
            // Make a local copy of handlers to avoid potential issues with modification
            std::vector<SpikeHandler> handlersCopy;
            {
                std::lock_guard<std::mutex> handlersLock(pImpl->mutex);
                handlersCopy = pImpl->handlers;
            }
            
            // Call handlers with the detailed event
            for (auto& handler : handlersCopy) {
                try {
                    handler(detailed);
                } catch (const std::exception& e) {
                    // Log error but continue processing
                    // In production, this should use proper logging
                    std::cerr << "Error in spike handler: " << e.what() << std::endl;
                }
            }
        }
    }

void SpikeSystem::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
        if (!pImpl->initialized) {
            throw std::runtime_error("SpikeSystem not properly initialized");
        }
        
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        // Find and process all delayed spikes scheduled for this step
        auto it = pImpl->delayedSpikes.find(currentStep);
        if (it != pImpl->delayedSpikes.end()) {
            // Create a local copy of delayed events to process
            std::vector<DelayedSpikeEvent> delayedEvents = std::move(it->second);
            
            // Remove from map first to ensure we don't process again
            pImpl->delayedSpikes.erase(it);
            
            // Make a local copy of handlers
            std::vector<DelayedSpikeHandler> delayedHandlersCopy;
            {
                std::lock_guard<std::mutex> handlersLock(pImpl->mutex);
                delayedHandlersCopy = pImpl->delayedHandlers;
            }
            
            // Process events outside of the main lock
            for (const auto& delayedEvent : delayedEvents) {
                // Call delayed spike handlers (these will deliver synaptic input)
                for (auto& handler : delayedHandlersCopy) {
                    try {
                        handler(delayedEvent);
                    } catch (const std::exception& e) {
                        std::cerr << "Error in delayed spike handler: " << e.what() << std::endl;
                    }
                }
            }
        }
    }

void SpikeSystem::registerHandler(SpikeHandler handler) {
        if (!handler) {
            throw std::invalid_argument("Handler cannot be null");
        }
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->handlers.push_back(handler);
    }

void SpikeSystem::registerDelayedHandler(DelayedSpikeHandler handler) {
        if (!handler) {
            throw std::invalid_argument("Delayed handler cannot be null");
        }
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->delayedHandlers.push_back(handler);
    }

const std::vector<DetailedSpikeEvent>& SpikeSystem::getSpikeHistory() const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        return pImpl->spikeHistory;
    }

void SpikeSystem::clearHistory() {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->spikeHistory.clear();
        pImpl->spikeCountPerNeuron.clear();
    }

    size_t SpikeSystem::getSpikeCount() const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        return pImpl->spikeHistory.size();
    }

    size_t SpikeSystem::getPendingSpikeCount() const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        return pImpl->pendingSpikes.size();
    }

    size_t SpikeSystem::getPendingDelayedCount() const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        size_t count = 0;
        for (const auto& pair : pImpl->delayedSpikes) {
            count += pair.second.size();
        }
        return count;
    }

    float SpikeSystem::getAverageSpikeRate() const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        // Calculate average spike rate based on history
        if (pImpl->spikeHistory.size() < 2) {
            return 0.0f;
        }
        
        // Find time range
        float minTime = std::numeric_limits<float>::max();
        float maxTime = -std::numeric_limits<float>::max();
        
        for (const auto& spike : pImpl->spikeHistory) {
            float time = static_cast<float>(spike.timestamp);
            minTime = std::min(minTime, time);
            maxTime = std::max(maxTime, time);
        }
        
        float duration = maxTime - minTime;
        if (duration <= std::numeric_limits<float>::epsilon()) {
            // Handle near-zero duration to avoid division by zero
            return 0.0f;
        }
        
        return static_cast<float>(pImpl->spikeHistory.size()) / duration;
    }

    std::vector<NeuronId> SpikeSystem::getMostActiveNeurons(size_t count) const {
        if (count == 0) {
            return std::vector<NeuronId>();
        }
        
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
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
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->pendingSpikes = std::queue<SpikeEvent>();
        pImpl->spikeHistory.clear();
        pImpl->spikeCountPerNeuron.clear();
        pImpl->delayedSpikes.clear();
    }

} // namespace nlm
