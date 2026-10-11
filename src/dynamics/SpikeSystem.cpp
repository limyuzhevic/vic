#include "SpikeSystem.hpp"
#include "../performance/EventQueue.hpp"
#include <algorithm>
#include <limits>

namespace nlm {

struct SpikeSystem::Impl {
    // Use EventQueue for immediate spike events
    nlm::SpikeRingBuffer pendingSpikes;
    
    // Use DelayedSpikeQueue from EventQueue
    nlm::DelayedSpikeQueue delayedSpikes;
    
    // History maintained separately for statistics
    std::vector<DetailedSpikeEvent> spikeHistory;
    std::vector<SpikeHandler> handlers;
    std::vector<DelayedSpikeHandler> delayedHandlers;
    size_t maxHistorySize;
    
    // Spike counting for statistics
    std::unordered_map<uint64_t, size_t> spikeCountPerNeuron;
    
    Impl() : maxHistorySize(10000) {
        // Initialize event queues with appropriate capacities
        // pendingSpikes uses default capacity from EventQueue.hpp
        // delayedSpikes uses default buckets and max delay
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
    // Convert to QueuedSpikeEvent for EventQueue processing
    nlm::QueuedSpikeEvent queuedEvent(
        event.source_neuron.value,  // Convert to uint64_t for EventQueue
        0, // destNeuron - placeholder, will be set during processing
        0, // synapseId - placeholder, will be set during processing
        1.0f, // weight - placeholder, will be set during processing
        false // isExcitatory - placeholder, will be set during processing
    );
    
    // Use EventQueue for spike processing
    if (!pImpl->pendingSpikes.push(queuedEvent)) {
        // EventQueue full - handle overflow
        NLM_LOG_WARNING("Spike event queue full - dropping event");
    }
}

void SpikeSystem::queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step) {
    // Create simple spike event
    SpikeEvent event(neuron, timestamp, step);
    queueSpike(event);
}

void SpikeSystem::queueDelayedSpike(const DelayedSpikeEvent& event) {
    // Use DelayedSpikeQueue from EventQueue
    pImpl->delayedSpikes.schedule(
        event.source_neuron.value,
        event.destination_neuron.value,
        event.synapse_id.value,
        event.weight,
        event.is_excitatory,
        event.delivery_step
    );
}

void SpikeSystem::processSpikes(SimulationStep currentStep) {
    // Process all queued spikes from EventQueue
    std::vector<nlm::QueuedSpikeEvent> dueSpikes;
    
    // Extract all events from the ring buffer
    while (!pImpl->pendingSpikes.empty()) {
        auto eventOpt = pImpl->pendingSpikes.pop();
        if (eventOpt) {
            dueSpikes.push_back(*eventOpt);
        }
    }
    
    // Process each spike
    for (const auto& queuedEvent : dueSpikes) {
        // Create DetailedSpikeEvent for handlers
        DetailedSpikeEvent detailed;
        detailed.source = NeuronId(queuedEvent.sourceNeuron);
        detailed.timestamp = static_cast<Timestamp>(queuedEvent.sourceNeuron); // Use timestamp from event
        detailed.step = currentStep;
        detailed.regionId = INVALID_REGION_ID;
        detailed.populationId = INVALID_POPULATION_ID;
        
        // Add to history
        pImpl->spikeHistory.push_back(detailed);
        
        // Track spike count
        pImpl->spikeCountPerNeuron[queuedEvent.sourceNeuron]++;
        
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
    // Get all due delayed spikes from DelayedSpikeQueue
    auto dueSpikes = pImpl->delayedSpikes.getDueSpikes(currentStep);
    
    // Process each delayed spike
    for (const auto& queuedEvent : dueSpikes) {
        // Create DelayedSpikeEvent for handlers
        DelayedSpikeEvent delayedEvent;
        delayedEvent.source_neuron = NeuronId(queuedEvent.sourceNeuron);
        delayedEvent.destination_neuron = NeuronId(queuedEvent.destNeuron);
        delayedEvent.synapse_id = SynapseId(queuedEvent.synapseId);
        delayedEvent.weight = queuedEvent.weight;
        delayedEvent.synapse_type = queuedEvent.isExcitatory ? SynapseType::Excitatory : SynapseType::Inhibitory;
        delayedEvent.timestamp = currentTime;
        delayedEvent.delivery_time = currentTime;
        delayedEvent.step = currentStep;
        delayedEvent.delivery_step = currentStep;
        delayedEvent.is_excitatory = queuedEvent.isExcitatory;
        
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
}

size_t SpikeSystem::getSpikeCount() const {
    return pImpl->spikeHistory.size();
}

size_t SpikeSystem::getPendingSpikeCount() const {
    return pImpl->pendingSpikes.size();
}

size_t SpikeSystem::getPendingDelayedCount() const {
    return pImpl->delayedSpikes.pendingCount();
}

float SpikeSystem::getAverageSpikeRate() const {
    if (pImpl->spikeHistory.size() < 2) {
        return 0.0f;
    }
    
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

const std::vector<SpikeSystem::SpikeHandler>& SpikeSystem::getHandlers() const {
    return pImpl->handlers;
}

const std::vector<SpikeSystem::DelayedSpikeHandler>& SpikeSystem::getDelayedHandlers() const {
    return pImpl->delayedHandlers;
}

} // namespace nlm