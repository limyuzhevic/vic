#include "SpikeSystem.hpp"
#include <algorithm>
#include <limits>
#include <cstring>
#include <numeric>

namespace nlm;

// Simple memory pool for SpikeSystem event management
class SpikeSystemMemoryPool {
public:
    static constexpr size_t MAX_PENDING_SPIKES = 10000;
    static constexpr size_t MAX_SPIKE_HISTORY = 10000;
    static constexpr size_t MAX_HANDLERS = 50;
    
    // Ring buffer for pending spikes
    class SpikeRingBuffer {
    private:
        SpikeEvent events[MAX_PENDING_SPIKES];
        size_t head;
        size_t tail;
        size_t count;
        
    public:
        SpikeRingBuffer() : head(0), tail(0), count(0) {}
        
        bool isEmpty() const { return count == 0; }
        bool isFull() const { return count == MAX_PENDING_SPIKES; }
        
        void push(const SpikeEvent& event) {
            if (isFull()) return;
            events[tail] = event;
            tail = (tail + 1) % MAX_PENDING_SPIKES;
            count++;
        }
        
        SpikeEvent front() const {
            return events[head];
        }
        
        void pop() {
            head = (head + 1) % MAX_PENDING_SPIKES;
            count--;
        }
        
        size_t size() const { return count; }
    };
    
    // Ring buffer for spike history
    class SpikeHistoryRingBuffer {
    private:
        DetailedSpikeEvent events[MAX_SPIKE_HISTORY];
        size_t head;
        size_t tail;
        size_t count;
        
    public:
        SpikeHistoryRingBuffer() : head(0), tail(0), count(0) {}
        
        void push(const DetailedSpikeEvent& event) {
            if (count == MAX_SPIKE_HISTORY) {
                head = (head + 1) % MAX_SPIKE_HISTORY;
            } else {
                count++;
            }
            events[tail] = event;
            tail = (tail + 1) % MAX_SPIKE_HISTORY;
        }
        
        size_t size() const { return count; }
    };
    
    // Fixed-size arrays for handlers
    SpikeHandler handlers[MAX_HANDLERS];
    DelayedSpikeHandler delayedHandlers[MAX_HANDLERS];
    size_t handlersCount;
    size_t delayedHandlersCount;
    
    // Fixed-size array for spike counts per neuron
    size_t spikeCountPerNeuron[MAX_PENDING_SPIKES];
    
    // Array of vectors for delayed spikes per delivery step
    std::vector<DelayedSpikeEvent> delayedSpikes[MAX_PENDING_SPIKES];
    
    // Ring buffers
    SpikeRingBuffer spikeRingBuffer;
    SpikeHistoryRingBuffer spikeHistoryRingBuffer;
    
    SpikeSystemMemoryPool() : handlersCount(0), delayedHandlersCount(0) {
        memset(spikeCountPerNeuron, 0, sizeof(spikeCountPerNeuron));
    }
};

struct SpikeSystem::Impl {
    SpikeSystemMemoryPool memoryPool;
    size_t maxHistorySize;
    
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
    pImpl->memoryPool.spikeRingBuffer.push(event);
}

void SpikeSystem::queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step) {
    SpikeEvent event(neuron, timestamp, step);
    queueSpike(event);
}

void SpikeSystem::queueDelayedSpike(const DelayedSpikeEvent& event) {
    if (event.delivery_step < SpikeSystemMemoryPool::MAX_PENDING_SPIKES) {
        pImpl->memoryPool.delayedSpikes[event.delivery_step].push_back(event);
    }
}

void SpikeSystem::processSpikes(SimulationStep currentStep) {
    while (!pImpl->memoryPool.spikeRingBuffer.isEmpty()) {
        SpikeEvent event = pImpl->memoryPool.spikeRingBuffer.front();
        pImpl->memoryPool.spikeRingBuffer.pop();
        
        // Create detailed event
        DetailedSpikeEvent detailed;
        detailed.source = event.source_neuron;
        detailed.timestamp = event.timestamp;
        detailed.step = event.step;
        
        // Add to history using ring buffer
        pImpl->memoryPool.spikeHistoryRingBuffer.push(detailed);
        
        // Track spike count per neuron
        if (event.source_neuron.value < SpikeSystemMemoryPool::MAX_PENDING_SPIKES) {
            pImpl->memoryPool.spikeCountPerNeuron[event.source_neuron.value]++;
        }
        
        // Call handlers
        for (size_t i = 0; i < pImpl->memoryPool.handlersCount; ++i) {
            pImpl->memoryPool.handlers[i](detailed);
        }
    }
}

void SpikeSystem::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    if (currentStep < SpikeSystemMemoryPool::MAX_PENDING_SPIKES) {
        auto& eventsForStep = pImpl->memoryPool.delayedSpikes[currentStep];
        
        // Process all spikes scheduled for this step
        for (const auto& delayedEvent : eventsForStep) {
            // Call delayed spike handlers
            for (size_t i = 0; i < pImpl->memoryPool.delayedHandlersCount; ++i) {
                pImpl->memoryPool.delayedHandlers[i](delayedEvent);
            }
        }
        
        // Remove processed spikes
        eventsForStep.clear();
    }
}

void SpikeSystem::registerHandler(SpikeHandler handler) {
    if (pImpl->memoryPool.handlersCount < SpikeSystemMemoryPool::MAX_HANDLERS) {
        pImpl->memoryPool.handlers[pImpl->memoryPool.handlersCount++] = std::move(handler);
    }
}

void SpikeSystem::registerDelayedHandler(DelayedSpikeHandler handler) {
    if (pImpl->memoryPool.delayedHandlersCount < SpikeSystemMemoryPool::MAX_HANDLERS) {
        pImpl->memoryPool.delayedHandlers[pImpl->memoryPool.delayedHandlersCount++] = std::move(handler);
    }
}

const std::vector<DetailedSpikeEvent>& SpikeSystem::getSpikeHistory() const {
    static std::vector<DetailedSpikeEvent> tempHistory;
    tempHistory.clear();
    return tempHistory;
}

void SpikeSystem::clearHistory() {
    // Reset spike history ring buffer
    pImpl->memoryPool.spikeHistoryRingBuffer.head = 0;
    pImpl->memoryPool.spikeHistoryRingBuffer.tail = 0;
    pImpl->memoryPool.spikeHistoryRingBuffer.count = 0;
    
    // Clear spike counts
    memset(pImpl->memoryPool.spikeCountPerNeuron, 0, sizeof(pImpl->memoryPool.spikeCountPerNeuron));
}

size_t SpikeSystem::getSpikeCount() const {
    return pImpl->memoryPool.spikeHistoryRingBuffer.size();
}

size_t SpikeSystem::getPendingSpikeCount() const {
    return pImpl->memoryPool.spikeRingBuffer.size();
}

size_t SpikeSystem::getPendingDelayedCount() const {
    size_t count = 0;
    for (size_t i = 0; i < SpikeSystemMemoryPool::MAX_PENDING_SPIKES; ++i) {
        count += pImpl->memoryPool.delayedSpikes[i].size();
    }
    return count;
}

float SpikeSystem::getAverageSpikeRate() const {
    if (pImpl->memoryPool.spikeHistoryRingBuffer.size() < 2) {
        return 0.0f;
    }
    return static_cast<float>(pImpl->memoryPool.spikeHistoryRingBuffer.size()) / 1000.0f;
}

std::vector<NeuronId> SpikeSystem::getMostActiveNeurons(size_t count) const {
    std::vector<std::pair<uint64_t, size_t>> neuronCounts;
    neuronCounts.reserve(SpikeSystemMemoryPool::MAX_PENDING_SPIKES);
    
    // Collect data from spike count array
    for (size_t i = 0; i < SpikeSystemMemoryPool::MAX_PENDING_SPIKES; ++i) {
        if (pImpl->memoryPool.spikeCountPerNeuron[i] > 0) {
            neuronCounts.emplace_back(i, pImpl->memoryPool.spikeCountPerNeuron[i]);
        }
    }
    
    // Sort by count descending
    std::sort(neuronCounts.begin(), neuronCounts.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<NeuronId> result;
    result.reserve(std::min(count, neuronCounts.size()));
    for (size_t i = 0; i < std::min(count, neuronCounts.size()); ++i) {
        result.emplace_back(NeuronId(neuronCounts[i].first));
    }
    
    return result;
}

void SpikeSystem::reset() {
    // Reset all ring buffers
    pImpl->memoryPool.spikeRingBuffer.head = 0;
    pImpl->memoryPool.spikeRingBuffer.tail = 0;
    pImpl->memoryPool.spikeRingBuffer.count = 0;
    
    clearHistory();
    
    // Clear delayed spikes
    for (size_t i = 0; i < SpikeSystemMemoryPool::MAX_PENDING_SPIKES; ++i) {
        pImpl->memoryPool.delayedSpikes[i].clear();
    }
}

} // namespace nlm