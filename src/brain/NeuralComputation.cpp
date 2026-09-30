// NeuralComputation.cpp implementation
// Contains the complete neural computation logic from Brain.cpp

#include "NeuralComputation.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void NeuralComputationSystem::computeStep(SimulationStep currentStep, Timestamp currentTime) {
    if (!brain_ || !brain_->getSpikeSystem()) return;
    
    // Phase 6: INTEGRATED ARTIFICIAL BRAIN LOOP
    // Step 1: Process pending delayed spike events
    brain_->getSpikeSystem()->processDelayedSpikes(currentStep, currentTime);
    
    // Step 2: Update all neurons (LIF dynamics)
    for (auto& region : brain_->getRegions()) {
        for (auto& population : region->getPopulations()) {
            for (auto* neuron : population->getNeurons()) {
                neuron->stepLIF(currentTime, timestep_);
            }
        }
    }
    
    // Step 3: Detect spikes and schedule spike events
    for (auto& region : brain_->getRegions()) {
        for (auto& population : region->getPopulations()) {
            for (auto* neuron : population->getNeurons()) {
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep_ * 2.0f);
                
                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    brain_->getSpikeSystem()->queueSpike(event);
                    
                    // Record post-synaptic spike for incoming synapses
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }
                    
                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = currentStep + delay;
                        Timestamp deliveryTime = currentTime + delay * timestep_;
                        
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
                        
                        brain_->getSpikeSystem()->queueDelayedSpike(delayedEvent);
                        
                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                }
            }
        }
    }
    
    // Process immediate spikes
    brain_->getSpikeSystem()->processSpikes(currentStep);
}

void NeuralComputationSystem::processSensoryInput(const SensoryInput& input) {
    // Inject current into sensory neurons based on input
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = 0;
    for (auto& region : brain_->getRegions()) {
        for (auto& population : region->getPopulations()) {
            if (population->getNeuronType() == NeuronType::Sensory) {
                for (auto* neuron : population->getNeurons()) {
                    numSensory++;
                }
            }
        }
    }
    
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current (simplified - would need actual access to neurons)
        // This would need to be connected to the brain's sensory neuron vectors
    }
}

} // namespace nlm
