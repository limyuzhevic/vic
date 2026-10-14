#include "SpikeSystem.hpp"
#include "../performance/EventQueue.hpp"
#include <algorithm>
#include <limits>

namespace nlm {

struct SpikeSystem::Impl {
    // Performance: Replace std::queue with SpikeRingBuffer (lock-free SPSC queue)
    SpikeRingBuffer spikeQueue;
    
    // Performance: Replace std::vector<DetailedSpikeEvent> with vector from EventQueue
    std::vector<DetailedSpikeEvent> spikeHistory;
    
    // Performance: Replace std::vector<SpikeHandler> with handlers from EventQueue
    // Note: EventQueue doesn't have a direct handler registry, so we keep handlers
    std::vector<SpikeHandler> handlers;
    
    // Performance: Replace std::vector<DelayedSpikeHandler> with delayedHandlers from EventQueue
    // Note: EventQueue doesn't have a direct delayed handler registry, so we keep delayedHandlers
    std::vector<DelayedSpikeHandler> delayedHandlers;
    
    // Performance: Replace std::unordered_map<SimulationStep, std::vector<DelayedSpikeEvent>> 
    // with DelayedSpikeQueue for efficient time-bucketed processing
    DelayedSpikeQueue delayedSpikeQueue;
    
    // Statistics
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
    // Performance: Convert SpikeEvent to QueuedSpikeEvent for EventQueue processing
    // This is a simplified conversion - in full implementation would optimize data layout
    QueuedSpikeEvent queuedEvent(
        event.source_neuron.value,
        0,  // destinationNeuron (not stored in SpikeEvent)
        0,  // synapseId (not stored in SpikeEvent)
        event.weight,
        event.is_excitatory
    );
    
    // Use lock-free queue from EventQueue
    pImpl->spikeQueue.push(queuedEvent);
}

void SpikeSystem::queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step) {
    // Performance: Use SpikeRingBuffer for lock-free immediate spike processing
    QueuedSpikeEvent queuedEvent(
        neuron.value,
        0,  // destinationNeuron
        0,  // synapseId
        0.0f,  // weight
        1   // isExcitatory (default to 1)
    );
    
    pImpl->spikeQueue.push(queuedEvent);
}

void SpikeSystem::queueDelayedSpike(const DelayedSpikeEvent& event) {
    // Performance: Use DelayedSpikeQueue for time-bucketed spike scheduling
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
    // Performance: Use SpikeRingBuffer's lock-free processing
    std::optional<QueuedSpikeEvent> queuedEvent;
    while ((queuedEvent = pImpl->spikeQueue.pop()).has_value()) {
        // Convert QueuedSpikeEvent back to DetailedSpikeEvent
        DetailedSpikeEvent detailed;
        detailed.source = NeuronId(queuedEvent->sourceNeuron);
        detailed.timestamp = currentStep * pImpl->spikeSystem.getTimestep();  // Need to get timestep
        detailed.step = currentStep;
        
        // Add to history
        pImpl->spikeHistory.push_back(detailed);
        
        // Track spike count per neuron
        pImpl->spikeCountPerNeuron[queuedEvent->sourceNeuron]++;
        
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
    // Performance: Use DelayedSpikeQueue's time-bucketed processing
    std::vector<QueuedSpikeEvent> dueSpikes = pImpl->delayedSpikeQueue.getDueSpikes(currentStep);
    
    // Process all spikes scheduled for this step
    for (const auto& queuedEvent : dueSpikes) {
        // Convert QueuedSpikeEvent back to DelayedSpikeEvent
        DelayedSpikeEvent delayedEvent(
            NeuronId(queuedEvent.sourceNeuron),
            NeuronId(queuedEvent.destNeuron),
            SynapseId(queuedEvent.synapseId),
            queuedEvent.weight,
            queuedEvent.isExcitatory,
            currentTime,
            currentTime,  // deliveryTime (simplified)
            currentStep,
            currentStep  // deliveryStep
        );
        
        // Call delayed spike handlers
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
    pImpl->delayedSpikeQueue.clear();
}

size_t SpikeSystem::getSpikeCount() const {
    return pImpl->spikeHistory.size();
}

size_t SpikeSystem::getPendingSpikeCount() const {
    return pImpl->spikeQueue.size();
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
    pImpl->spikeQueue.clear();
    pImpl->spikeHistory.clear();
    pImpl->spikeCountPerNeuron.clear();
    pImpl->delayedSpikeQueue.clear();
}

} // namespace nlm
