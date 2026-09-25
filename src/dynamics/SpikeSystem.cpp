#include "SpikeSystem.hpp"
#include <algorithm>
#include <limits>
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
    mutable std::mutex handlersMutex;
    mutable std::mutex historyMutex;
    mutable std::mutex delayedSpikesMutex;
    
    // Callback statistics
    mutable size_t spikeHandlerCalls;
    mutable size_t delayedHandlerCalls;
    
    Impl() : maxHistorySize(10000), spikeHandlerCalls(0), delayedHandlerCalls(0) {}
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
    std::lock_guard<std::mutex> lock(pImpl->delayedSpikesMutex);
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
        {
            std::lock_guard<std::mutex> lock(pImpl->historyMutex);
            pImpl->spikeHistory.push_back(detailed);
        }
        
        // Track spike count per neuron
        pImpl->spikeCountPerNeuron[event.source_neuron.value]++;
        
        // Trim history if needed
        {
            std::lock_guard<std::mutex> lock(pImpl->historyMutex);
            if (pImpl->spikeHistory.size() > pImpl->maxHistorySize) {
                pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
            }
        }
        
        // Call handlers safely
        {
            std::lock_guard<std::mutex> lock(pImpl->handlersMutex);
            pImpl->spikeHandlerCalls++;
            for (auto& handler : pImpl->handlers) {
                try {
                    handler(detailed);
                } catch (const std::exception& e) {
                    // Log error but continue processing
                }
            }
        }
    }
}

void SpikeSystem::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    // Find and process all delayed spikes scheduled for this step
    std::vector<DelayedSpikeEvent> spikesToProcess;
    {
        std::lock_guard<std::mutex> lock(pImpl->delayedSpikesMutex);
        auto it = pImpl->delayedSpikes.find(currentStep);
        if (it != pImpl->delayedSpikes.end()) {
            spikesToProcess = std::move(it->second);
            pImpl->delayedSpikes.erase(it);
        }
    }
    
    // Call delayed spike handlers safely
    if (!spikesToProcess.empty()) {
        std::lock_guard<std::mutex> lock(pImpl->handlersMutex);
        pImpl->delayedHandlerCalls++;
        for (const auto& delayedEvent : spikesToProcess) {
            for (auto& handler : pImpl->delayedHandlers) {
                try {
                    handler(delayedEvent);
                } catch (const std::exception& e) {
                    // Log error but continue processing
                }
            }
        }
    }
}

void SpikeSystem::registerHandler(SpikeHandler handler) {
    // Register a new spike handler callback with safety validation
    // Prevents null handler registration which would cause undefined behavior
    if (!handler) {
        throw std::invalid_argument("Handler cannot be null");
    }
    std::lock_guard<std::mutex> lock(pImpl->handlersMutex);
    pImpl->handlers.push_back(std::move(handler));
}

void SpikeSystem::registerDelayedHandler(DelayedSpikeHandler handler) {
    // Register a new delayed spike handler callback with safety validation
    // Prevents null handler registration which would cause undefined behavior
    if (!handler) {
        throw std::invalid_argument("Delayed handler cannot be null");
    }
    std::lock_guard<std::mutex> lock(pImpl->handlersMutex);
    pImpl->delayedHandlers.push_back(std::move(handler));
}

bool SpikeSystem::removeHandler(size_t index) {
    // Remove a spike handler by index (0-based)
    // Performs bounds checking and returns success/failure status
    std::lock_guard<std::mutex> lock(pImpl->handlersMutex);
    if (index >= pImpl->handlers.size()) {
        return false;
    }
    pImpl->handlers.erase(pImpl->handlers.begin() + index);
    return true;
}

bool SpikeSystem::removeDelayedHandler(size_t index) {
    // Remove a delayed spike handler by index (0-based)
    // Performs bounds checking and returns success/failure status
    std::lock_guard<std::mutex> lock(pImpl->handlersMutex);
    if (index >= pImpl->delayedHandlers.size()) {
        return false;
    }
    pImpl->delayedHandlers.erase(pImpl->delayedHandlers.begin() + index);
    return true;
}

size_t SpikeSystem::getSpikeHandlerCount() const {
    // Return the current number of registered spike handlers
    // Thread-safe access to handler count for monitoring and statistics
    std::lock_guard<std::mutex> lock(pImpl->handlersMutex);
    return pImpl->handlers.size();
}

size_t SpikeSystem::getDelayedSpikeHandlerCount() const {
    // Return the current number of registered delayed spike handlers
    // Thread-safe access to delayed handler count for monitoring and statistics
    std::lock_guard<std::mutex> lock(pImpl->handlersMutex);
    return pImpl->delayedHandlers.size();
}

const std::vector<DetailedSpikeEvent>& SpikeSystem::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

void SpikeSystem::clearHistory() {
    std::lock_guard<std::mutex> lock1(pImpl->historyMutex);
    std::lock_guard<std::mutex> lock2(pImpl->handlersMutex);
    pImpl->spikeHistory.clear();
    pImpl->spikeCountPerNeuron.clear();
}

size_t SpikeSystem::getSpikeCount() const {
    std::lock_guard<std::mutex> lock(pImpl->historyMutex);
    return pImpl->spikeHistory.size();
}

size_t SpikeSystem::getPendingSpikeCount() const {
    return pImpl->pendingSpikes.size();
}

size_t SpikeSystem::getPendingDelayedCount() const {
    std::lock_guard<std::mutex> lock(pImpl->delayedSpikesMutex);
    size_t count = 0;
    for (const auto& pair : pImpl->delayedSpikes) {
        count += pair.second.size();
    }
    return count;
}

float SpikeSystem::getAverageSpikeRate() const {
    // Calculate average spike rate based on history
    std::lock_guard<std::mutex> lock(pImpl->historyMutex);
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
    {
        std::lock_guard<std::mutex> lock(pImpl->handlersMutex);
        neuronCounts.reserve(pImpl->spikeCountPerNeuron.size());
        
        for (const auto& pair : pImpl->spikeCountPerNeuron) {
            neuronCounts.emplace_back(pair.first, pair.second);
        }
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
    std::lock_guard<std::mutex> lock1(pImpl->pendingSpikesMutex);
    std::lock_guard<std::mutex> lock2(pImpl->historyMutex);
    std::lock_guard<std::mutex> lock3(pImpl->delayedSpikesMutex);
    std::lock_guard<std::mutex> lock4(pImpl->handlersMutex);
    
    while (!pImpl->pendingSpikes.empty()) {
        pImpl->pendingSpikes.pop();
    }
    pImpl->spikeHistory.clear();
    pImpl->spikeCountPerNeuron.clear();
    pImpl->delayedSpikes.clear();
}

} // namespace nlm
