#include "SpikeSystem.hpp"
#include <algorithm>

namespace nlm {

struct SpikeSystem::Impl {
    std::vector<DetailedSpikeEvent> spikeHistory;
    std::queue<DetailedSpikeEvent> pendingDelayedSpikes;
    std::unordered_map<NeuronId, SpikeHandler> spikeHandlers;
    std::unordered_map<NeuronId, DelayedSpikeHandler> delayedHandlers;
    SimulationStep currentStep;
    
    Impl() : currentStep(0) {}
};

SpikeSystem::SpikeSystem() : pImpl(std::make_unique<Impl>()) {}

SpikeSystem::~SpikeSystem() = default;

SpikeSystem::SpikeSystem(SpikeSystem&& other) noexcept : pImpl(std::move(other.pImpl)) {}

SpikeSystem& SpikeSystem::operator=(SpikeSystem&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void SpikeSystem::queueSpike(const SpikeEvent& event) {
    DetailedSpikeEvent detailed;
    detailed.source = event.neuronId;
    detailed.timestamp = event.timestamp;
    detailed.step = event.step;
    detailed.regionId = event.regionId;
    detailed.populationId = event.populationId;
    pImpl->spikeHistory.push_back(detailed);
    pImpl->spikeHandlers[event.neuronId](detailed);
}

void SpikeSystem::queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step) {
    // Create a default SpikeEvent
    SpikeEvent event;
    event.neuronId = neuron;
    event.timestamp = timestamp;
    event.step = step;
    event.regionId = INVALID_REGION_ID;
    event.populationId = INVALID_POPULATION_ID;
    queueSpike(event);
}

void SpikeSystem::queueDelayedSpike(const DelayedSpikeEvent& event) {
    DetailedSpikeEvent detailed;
    detailed.source = event.sourceNeuron;
    detailed.timestamp = event.fireTime;
    detailed.step = event.fireStep;
    detailed.regionId = event.regionId;
    detailed.populationId = event.populationId;
    pImpl->pendingDelayedSpikes.push(detailed);
    pImpl->delayedHandlers[event.destinationNeuron](detailed);
}

void SpikeSystem::processSpikes(SimulationStep currentStep) {
    pImpl->currentStep = currentStep;
    std::vector<DetailedSpikeEvent> currentStepSpikes;
    std::swap(currentStepSpikes, pImpl->spikeHistory);
    
    for (const auto& spike : currentStepSpikes) {
        // Deliver synaptic input
        // This would integrate with synapse system
    }
}

void SpikeSystem::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    std::queue<DetailedSpikeEvent> remaining;
    while (!pImpl->pendingDelayedSpikes.empty()) {
        DetailedSpikeEvent& spike = pImpl->pendingDelayedSpikes.front();
        if (spike.step <= currentStep) {
            // Fire the spike
            queueSpike(static_cast<SpikeEvent>(spike));
            pImpl->pendingDelayedSpikes.pop();
        } else {
            remaining.push(spike);
        }
    }
    pImpl->pendingDelayedSpikes = std::move(remaining);
}

void SpikeSystem::registerHandler(SpikeHandler handler) {
    // Add handler for all neurons
    // In practice, this would be neuron-specific
}

void SpikeSystem::registerDelayedHandler(DelayedSpikeHandler handler) {
    // Add handler for all neurons
    // In practice, this would be neuron-specific
}

const std::vector<DetailedSpikeEvent>& SpikeSystem::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

void SpikeSystem::clearHistory() {
    pImpl->spikeHistory.clear();
}

size_t SpikeSystem::getSpikeCount() const {
    return pImpl->spikeHistory.size();
}

size_t SpikeSystem::getPendingSpikeCount() const {
    return 0; // Simplified - would need spike tracking
}

size_t SpikeSystem::getPendingDelayedCount() const {
    return pImpl->pendingDelayedSpikes.size();
}

float SpikeSystem::getAverageSpikeRate() const {
    if (pImpl->spikeHistory.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& spike : pImpl->spikeHistory) {
        sum += 1.0f; // Each spike counts as 1
    }
    return sum / static_cast<float>(pImpl->spikeHistory.size());
}

std::vector<NeuronId> SpikeSystem::getMostActiveNeurons(size_t count) const {
    std::vector<std::pair<size_t, NeuronId>> neuronCounts;
    for (const auto& spike : pImpl->spikeHistory) {
        // Count spikes per neuron
        auto it = std::find_if(neuronCounts.begin(), neuronCounts.end(),
                              [&spike](const auto& pair) { return pair.second == spike.source; });
        if (it != neuronCounts.end()) {
            it->first++;
        } else {
            neuronCounts.push_back({1, spike.source});
        }
    }
    std::sort(neuronCounts.begin(), neuronCounts.end(), std::greater<>());
    std::vector<NeuronId> result;
    result.reserve(std::min(count, neuronCounts.size()));
    for (size_t i = 0; i < std::min(count, neuronCounts.size()); ++i) {
        result.push_back(neuronCounts[i].second);
    }
    return result;
}

void SpikeSystem::reset() {
    pImpl->spikeHistory.clear();
    std::queue<DetailedSpikeEvent> empty;
    pImpl->pendingDelayedSpikes = std::move(empty);
}

} // namespace nlm
