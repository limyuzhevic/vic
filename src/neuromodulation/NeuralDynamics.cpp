#include "NeuralDynamics.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct NeuralDynamics::Impl {
    Brain* brain;
    SpikeSystem* spikeSystem;
    NeuralWorkingMemory* workingMemory;
    const Neuromodulator* neuromodulator;
    
    SimulationStep currentStep;
    Timestamp currentTime;
    TimestepDuration timestep;
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    size_t firingNeuronCount;
    
    Impl(Brain* b, SpikeSystem* ss) 
        : brain(b), spikeSystem(ss), workingMemory(nullptr), neuromodulator(nullptr),
          currentStep(0), currentTime(0.0), timestep(0.001),
          totalSpikesThisStep(0), totalSpikesTotal(0), firingNeuronCount(0) {}
};

NeuralDynamics::NeuralDynamics(Brain* brain, SpikeSystem* spikeSystem) 
    : pImpl(new Impl(brain, spikeSystem)) {
    if (brain) {
        pImpl->timestep = brain->getConfig()->getOr<double>("simulation_timestep", 0.001);
    }
}

NeuralDynamics::~NeuralDynamics() {
    delete pImpl;
}

void NeuralDynamics::updateStep(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    pImpl->firingNeuronCount = 0;
    
    // Step 1: Update all neurons (LIF dynamics)
    for (auto& region : pImpl->brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    
    // Step 2: Detect spikes and schedule spike events
    for (auto& region : pImpl->brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    pImpl->spikeSystem->queueSpike(event);
                    pImpl->totalSpikesThisStep++;
                    pImpl->firingNeuronCount++;
                    
                    // Record post-synaptic spike for incoming synapses (plasticity)
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }

                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        // Create delayed spike event
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = currentStep + delay;
                        Timestamp deliveryTime = currentTime + delay * pImpl->timestep;

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

                        pImpl->spikeSystem->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (pImpl->workingMemory) {
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
    
    // Process immediate spikes
    pImpl->spikeSystem->processSpikes(currentStep);
    pImpl->totalSpikesTotal += pImpl->totalSpikesThisStep;
}

void NeuralDynamics::setWorkingMemory(NeuralWorkingMemory* workingMemory) {
    pImpl->workingMemory = workingMemory;
}

void NeuralDynamics::setNeuromodulator(const Neuromodulator* neuromodulator) {
    pImpl->neuromodulator = neuromodulator;
}

size_t NeuralDynamics::getTotalSpikesThisStep() const {
    return pImpl->totalSpikesThisStep;
}

size_t NeuralDynamics::getTotalSpikesTotal() const {
    return pImpl->totalSpikesTotal;
}

size_t NeuralDynamics::getFiringNeuronCount() const {
    return pImpl->firingNeuronCount;
}

} // namespace nlm
