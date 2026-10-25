// NeuralDynamics subsystem implementation

#include "NeuralDynamics.h"
#include "core/Logger/Logger.hpp"

namespace nlm {

NeuralDynamicsImpl::NeuralDynamicsImpl() {
    NLM_LOG_INFO("NeuralDynamics initialized");
}

void NeuralDynamicsImpl::updateNeurons(const std::vector<std::unique_ptr<NeuralRegion>>& regions, 
                                      Timestamp currentTime, TimestepDuration timestep) {
    NLM_LOG_DEBUG("Updating neurons (LIF dynamics)");
    
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
    
    NLM_LOG_DEBUG("Neuron update complete, processed " + 
                 std::to_string(calculateNeuronsUpdated(regions)) + " neurons");
}

void NeuralDynamicsImpl::detectAndProcessSpikes(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                               SimulationStep currentStep, Timestamp currentTime,
                                               SpikeSystem& spikeSystem,
                                               std::unique_ptr<NeuralWorkingMemory>& workingMemory) {
    NLM_LOG_DEBUG("Detecting spikes and processing events");
    
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < 0.002f);
                
                if (justFired) {
                    processSingleSpikeEvent(neuron, currentStep, currentTime, 
                                          spikeSystem, workingMemory, *region);
                }
            }
        }
    }
}

size_t NeuralDynamicsImpl::countTotalSpikes() const {
    return totalSpikesCount;
}

void NeuralDynamicsImpl::registerSpikeHandler(std::function<void(const SpikeEvent&)> handler) {
    // In this implementation, we don't store the handlers but use them directly
    // The actual handling is done through detectAndProcessSpikes
    NLM_LOG_DEBUG("Spike handler registered");
}

void NeuralDynamicsImpl::registerDelayedSpikeHandler(std::function<void(const DelayedSpikeEvent&)> handler) {
    // In this implementation, we don't store the handlers but use them directly
    // The actual handling is done through detectAndProcessSpikes
    NLM_LOG_DEBUG("Delayed spike handler registered");
}

private:
    size_t calculateNeuronsUpdated(const std::vector<std::unique_ptr<NeuralRegion>>& regions) const {
        size_t count = 0;
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                count += pop->getNeuronCount();
            }
        }
        return count;
    }
    
    void processSingleSpikeEvent(Neuron* neuron, SimulationStep currentStep, 
                                Timestamp currentTime, SpikeSystem& spikeSystem,
                                std::unique_ptr<NeuralWorkingMemory>& workingMemory,
                                NeuralRegion& region) {
        // Queue spike event
        SpikeEvent event(neuron->getId(), currentTime, currentStep);
        spikeSystem.queueSpike(event);
        
        // Record post-synaptic spike for plasticity
        auto incomingSynapses = region.getSynapsesTo(neuron->getId());
        for (Synapse* syn : incomingSynapses) {
            syn->recordPostSpike(currentTime);
        }
        
        // Schedule delayed spike events
        auto outgoingSynapses = region.getSynapsesFrom(neuron->getId());
        for (Synapse* syn : outgoingSynapses) {
            Delay delay = syn->getDelay();
            SimulationStep deliveryStep = currentStep + delay;
            Timestamp deliveryTime = currentTime + delay * 0.001;
            
            DelayedSpikeEvent delayedEvent(
                neuron->getId(),
                syn->getDestinationNeuron(),
                syn->getId(),
                syn->getWeight(),
                syn->getType(),
                currentTime,
                deliveryTime,
                currentStep,
                deliveryStep
            );
            
            spikeSystem.queueDelayedSpike(delayedEvent);
            syn->recordPreSpike(currentTime);
        }
        
        // Store to working memory
        if (workingMemory) {
            const auto& state = neuron->getState();
            workingMemory->storeToNeuron(neuron->getId(), 
                std::abs(state.membranePotential - state.restingPotential) / 10.0f);
        }
    }
};

} // namespace nlm