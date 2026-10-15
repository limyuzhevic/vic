// Spike processor implementation
// Handles spike processing logic for better separation of concerns

#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

class SpikeProcessor {
public:
    SpikeProcessor(Brain* brain) : brain(brain) {}
    
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
        if (!brain) return;
        
        // Process delayed spikes through spike system
        brain->pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    }
    
    void processSpikes(SimulationStep currentStep) {
        if (!brain) return;
        
        // Process immediate spikes through spike system
        brain->pImpl->spikeSystem->processSpikes(currentStep);
    }
    
    void queueSpike(const SpikeEvent& event) {
        if (!brain) return;
        brain->pImpl->spikeSystem->queueSpike(event);
    }
    
    void queueDelayedSpike(const DelayedSpikeEvent& event) {
        if (!brain) return;
        brain->pImpl->spikeSystem->queueDelayedSpike(event);
    }
    
    size_t getPendingSpikeCount() const {
        if (!brain) return 0;
        return brain->pImpl->spikeSystem->getPendingSpikeCount();
    }
    
    size_t getPendingDelayedCount() const {
        if (!brain) return 0;
        return brain->pImpl->spikeSystem->getPendingDelayedCount();
    }
    
    void reset() {
        if (brain && brain->pImpl->spikeSystem) {
            brain->pImpl->spikeSystem->reset();
        }
    }
    
    // Register spike handlers
    void registerSpikeHandler(std::function<void(const DetailedSpikeEvent&)> handler) {
        if (brain && brain->pImpl->spikeSystem) {
            brain->pImpl->spikeSystem->registerHandler(handler);
        }
    }
    
    void registerDelayedSpikeHandler(std::function<void(const DelayedSpikeEvent&)> handler) {
        if (brain && brain->pImpl->spikeSystem) {
            brain->pImpl->spikeSystem->registerDelayedHandler(handler);
        }
    }
    
    // Process neuron spikes and create spike events
    void detectAndProcessSpikes(SimulationStep currentStep, Timestamp currentTime) {
        if (!brain) return;
        
        for (auto& region : brain->pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    bool justFired = (state.firingState == FiringState::Refractory &&
                                     state.lastSpikeTime >= 0.0f &&
                                     std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < brain->pImpl->timestep * 2.0f);
                    
                    if (justFired) {
                        // Create and queue spike event
                        SpikeEvent event(neuron->getId(), currentTime, currentStep);
                        queueSpike(event);
                        
                        // Process post-synaptic and pre-synaptic spikes for plasticity
                        processPlasticitySpikes(neuron, region.get(), currentTime);
                    }
                }
            }
        }
    }
    
    // Deliver delayed spike input to target neurons
    void deliverDelayedSpikeInput(const DelayedSpikeEvent& event) {
        if (!brain) return;
        
        for (auto& region : brain->pImpl->regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == event.destination_neuron) {
                    // Apply synaptic weight as current
                    MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor
                    if (event.is_excitatory) {
                        neuron->receiveExcitatoryInput(synapticCurrent);
                    } else {
                        neuron->receiveInhibitoryInput(-synapticCurrent);
                    }
                    return;
                }
            }
        }
    }
    
    // Process plasticity-related spikes
    void processPlasticitySpikes(Neuron* neuron, NeuralRegion* region, Timestamp currentTime) {
        if (!brain || !region) return;
        
        // Record post-synaptic spike for incoming synapses
        auto incomingSynapses = region->getSynapsesTo(neuron->getId());
        for (Synapse* syn : incomingSynapses) {
            syn->recordPostSpike(currentTime);
        }
        
        // Record pre-synaptic spike for outgoing synapses
        auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
        for (Synapse* syn : outgoingSynapses) {
            syn->recordPreSpike(currentTime);
        }
    }
    
private:
    Brain* brain;
};

} // namespace nlm
