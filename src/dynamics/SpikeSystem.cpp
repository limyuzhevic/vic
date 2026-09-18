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
    
    // Event queues for efficient processing
    nlm::SpikeRingBuffer immediateSpikeQueue;
    nlm::DelayedSpikeQueue delayedSpikeQueue;
    
    // Spike counting for statistics
    std::unordered_map<uint64_t, size_t> spikeCountPerNeuron;  // neuron_id -> count
    
    Impl() : maxHistorySize(10000)
        , immediateSpikeQueue(nlm::SPIKE_QUEUE_CAPACITY)
        , delayedSpikeQueue() {}
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
    // Use EventQueue for O(1) enqueue operation
    nlm::QueuedSpikeEvent queuedEvent;
    queuedEvent.sourceNeuron = event.source_neuron.value;
    queuedEvent.destNeuron = event.destination_neuron.value;
    queuedEvent.synapseId = event.synapse_id.value;
    queuedEvent.weight = event.weight;
    queuedEvent.isExcitatory = 1;  // Default excitatory
    
    if (!pImpl->immediateSpikeQueue.push(queuedEvent)) {
        // Fallback to queue if EventQueue is full
        pImpl->pendingSpikes.push(event);
    }
}

void SpikeSystem::queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step) {
    // Use EventQueue for O(1) enqueue operation
    nlm::QueuedSpikeEvent queuedEvent;
    queuedEvent.sourceNeuron = neuron.value;
    queuedEvent.destNeuron = neuron.value;  // Same neuron for immediate spike
    queuedEvent.synapseId = 0;  // Self-connection for immediate spike
    queuedEvent.weight = 1.0f;
    queuedEvent.isExcitatory = 1;
    
    if (!pImpl->immediateSpikeQueue.push(queuedEvent)) {
        // Fallback to queue if EventQueue is full
        pImpl->pendingSpikes.emplace(neuron, timestamp, step);
    }
}

void SpikeSystem::queueDelayedSpike(const DelayedSpikeEvent& event) {
    // Use EventQueue for O(1) enqueue operation
    pImpl->delayedSpikeQueue.schedule(
        event.source_neuron,
        event.destination_neuron,
        event.synapse_id,
        event.weight,
        event.is_excitatory,
        event.delivery_step
    );
}

void SpikeSystem::processSpikes(SimulationStep currentStep) {
    // Process immediate spikes using EventQueue for O(1) delivery
    std::vector<nlm::QueuedSpikeEvent> processedEvents;
    
    // Drain the ring buffer
    while (!pImpl->immediateSpikeQueue.empty()) {
        auto event = pImpl->immediateSpikeQueue.pop();
        if (event) {
            processedEvents.push_back(*event);
        }
    }
    
    // Also process any remaining spikes in the old queue
    while (!pImpl->pendingSpikes.empty()) {
        SpikeEvent oldEvent = pImpl->pendingSpikes.front();
        pImpl->pendingSpikes.pop();
        
        // Convert to QueuedSpikeEvent for uniform processing
        nlm::QueuedSpikeEvent queuedEvent;
        queuedEvent.sourceNeuron = oldEvent.source_neuron.value;
        queuedEvent.destNeuron = oldEvent.destination_neuron.value;
        queuedEvent.synapseId = oldEvent.synapse_id.value;
        queuedEvent.weight = oldEvent.weight;
        queuedEvent.isExcitatory = 1;
        
        processedEvents.push_back(queuedEvent);
    }
    
    // Process all spikes
    for (const auto& event : processedEvents) {
        // Create detailed event
        DetailedSpikeEvent detailed;
        detailed.source = event.sourceNeuron;
        detailed.timestamp = static_cast<Timestamp>(currentStep) * 0.001f;  // Assuming 1ms timestep
        detailed.step = currentStep;
        
        // Add to history
        pImpl->spikeHistory.push_back(detailed);
        
        // Track spike count per neuron
        pImpl->spikeCountPerNeuron[event.sourceNeuron]++;
        
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
    // Process delayed spikes using time-bucketed EventQueue for O(1) delivery
    auto delayedEvents = pImpl->delayedSpikeQueue.getDueSpikes(currentStep);
    
    // Process all delayed spikes scheduled for this step
    for (const auto& delayedEvent : delayedEvents) {
        // Call delayed spike handlers (these will deliver synaptic input)
        for (auto& handler : pImpl->delayedHandlers) {
            handler(delayedEvent);
        }
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
    pImpl->immediateSpikeQueue.clear();
    pImpl->delayedSpikeQueue.clear();
}

size_t SpikeSystem::getSpikeCount() const {
    return pImpl->spikeHistory.size();
}

size_t SpikeSystem::getPendingSpikeCount() const {
    return pImpl->immediateSpikeQueue.size() + pImpl->pendingSpikes.size();
}

size_t SpikeSystem::getPendingDelayedCount() const {
    return pImpl->delayedSpikeQueue.pendingCount();
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
    pImpl->pendingSpikes = std::queue<SpikeEvent>();
    pImpl->immediateSpikeQueue.clear();
    pImpl->delayedSpikeQueue.clear();
}

} // namespace nlm
