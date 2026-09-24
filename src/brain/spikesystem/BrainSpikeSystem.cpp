#include "spikesystem/BrainSpikeSystem.hpp"
#include "../core/Types/Types.hpp"
#include <functional>

namespace nlm {
    
    // Forward declarations for event types
    struct DetailedSpikeEvent {
        NeuronId neuronId;
        Timestamp time;
        SimulationStep step;
        bool isExcitatory;
        float weight;
    };
    
    struct DelayedSpikeEvent {
        NeuronId sourceNeuron;
        NeuronId destinationNeuron;
        SynapseId synapseId;
        float weight;
        bool isExcitatory;
        Timestamp eventTime;
        Timestamp deliveryTime;
        SimulationStep eventStep;
        SimulationStep deliveryStep;
    };
    
    // Spike event
    struct SpikeEvent {
        NeuronId neuronId;
        Timestamp time;
        SimulationStep step;
    };

    namespace spikesystem {

        struct SpikeSystem::Impl {
            std::vector<std::function<void(const DetailedSpikeEvent&)>> spikeHandlers;
            std::vector<std::function<void(const DelayedSpikeEvent&)>> delayedHandlers;
            std::vector<SpikeEvent> pendingSpikes;
            std::vector<DelayedSpikeEvent> pendingDelayed;
            
            Impl() = default;
            ~Impl() = default;
        };

        SpikeSystem::SpikeSystem() : pImpl(std::make_unique<Impl>()) {}
        SpikeSystem::~SpikeSystem() = default;

        void SpikeSystem::registerHandler(std::function<void(const DetailedSpikeEvent&)>&& handler) {
            pImpl->spikeHandlers.push_back(std::move(handler));
        }

        void SpikeSystem::registerDelayedHandler(std::function<void(const DelayedSpikeEvent&)>&& handler) {
            pImpl->delayedHandlers.push_back(std::move(handler));
        }

        void SpikeSystem::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
            // Remove processed delayed spikes
            pImpl->pendingDelayed.erase(
                std::remove_if(pImpl->pendingDelayed.begin(), pImpl->pendingDelayed.end(),
                              [currentStep](const DelayedSpikeEvent& event) {
                                  return event.deliveryStep <= currentStep;
                              }),
                pImpl->pendingDelayed.end()
            );
            
            // Deliver remaining delayed spikes
            for (const auto& event : pImpl->pendingDelayed) {
                for (auto& handler : pImpl->delayedHandlers) {
                    handler(event);
                }
            }
        }

        void SpikeSystem::processSpikes(SimulationStep currentStep) {
            // Process immediate spikes
            for (const auto& event : pImpl->pendingSpikes) {
                if (event.step == currentStep) {
                    DetailedSpikeEvent detailedEvent;
                    detailedEvent.neuronId = event.neuronId;
                    detailedEvent.time = event.time;
                    detailedEvent.step = event.step;
                    detailedEvent.isExcitatory = true;
                    detailedEvent.weight = 1.0f;
                    
                    for (auto& handler : pImpl->spikeHandlers) {
                        handler(detailedEvent);
                    }
                }
            }
            
            // Clear processed spikes
            pImpl->pendingSpikes.erase(
                std::remove_if(pImpl->pendingSpikes.begin(), pImpl->pendingSpikes.end(),
                              [currentStep](const SpikeEvent& event) {
                                  return event.step == currentStep;
                              }),
                pImpl->pendingSpikes.end()
            );
        }

        void SpikeSystem::queueSpike(const SpikeEvent& event) {
            pImpl->pendingSpikes.push_back(event);
        }

        void SpikeSystem::queueDelayedSpike(const DelayedSpikeEvent& event) {
            pImpl->pendingDelayed.push_back(event);
        }

        size_t SpikeSystem::getPendingSpikeCount() const {
            return pImpl->pendingSpikes.size();
        }

        size_t SpikeSystem::getPendingDelayedCount() const {
            return pImpl->pendingDelayed.size();
        }

        void SpikeSystem::reset() {
            pImpl->pendingSpikes.clear();
            pImpl->pendingDelayed.clear();
        }
    }
}
