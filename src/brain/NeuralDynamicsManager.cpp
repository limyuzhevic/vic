#include "NeuralDynamicsManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

NeuralDynamicsManager::NeuralDynamicsManager(Brain* brain)
    : brain_(brain), spikeSystem(std::make_unique<SpikeSystem>())
    , spikeHandlersRegistered(false) {
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto* neuron : region->getAllNeurons()) {
                neuronById[neuron->getId().index()] = neuron;
                if (neuron->getType() == NeuronType::Sensory) {
                    sensoryNeurons.push_back(neuron);
                } else if (neuron->getType() == NeuronType::Motor) {
                    motorNeurons.push_back(neuron);
                }
            }
        }
    }
}

NeuralDynamicsManager::~NeuralDynamicsManager() = default;

bool NeuralDynamicsManager::initialize() {
    NLM_LOG_INFO("Initializing Neural Dynamics Manager...");
    
    if (!brain_) return false;
    
    // Register spike handlers once
    if (!spikeHandlersRegistered) {
        spikeHandlersRegistered = true;
        
        // Register spike handlers for event-driven processing
        spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
            // Count spikes for statistics
            ++brain_->pImpl->totalSpikesThisStep;
            ++brain_->pImpl->totalSpikesTotal;
        });
        
        // Register delayed spike handler to deliver synaptic input
        spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
            auto it = neuronById.find(event.destination_neuron.index());
            if (it != neuronById.end()) {
                Neuron* neuron = it->second;
                MembranePotential synapticCurrent = event.weight * 10.0f;
                if (event.is_excitatory) {
                    neuron->receiveExcitatoryInput(synapticCurrent);
                } else {
                    neuron->receiveInhibitoryInput(-synapticCurrent);
                }
            }
        });
    }
    
    NLM_LOG_INFO("Neural Dynamics Manager initialized successfully");
    return true;
}

void NeuralDynamicsManager::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    spikeSystem->processDelayedSpikes(currentStep, currentTime);
}

void NeuralDynamicsManager::detectAndScheduleSpikes(SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : brain_->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < 
                                 brain_->pImpl->timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    spikeSystem->queueSpike(event);

                    // Record post-synaptic spike for incoming synapses (plasticity)
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }

                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = currentStep + delay;
                        Timestamp deliveryTime = currentTime + delay * brain_->pImpl->timestep;

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

                        spikeSystem->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory
                    if (brain_->pImpl->workingMemory) {
                        brain_->pImpl->workingMemory->storeToNeuron(neuron->getId(), 1.0f);
                    }
                }
            }
        }
    }
}

void NeuralDynamicsManager::processSpikes(SimulationStep currentStep) {
    spikeSystem->processSpikes(currentStep);
}

void NeuralDynamicsManager::updateAllNeurons(SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : brain_->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, brain_->pImpl->timestep);
            }
        }
    }
}

void NeuralDynamicsManager::reset() {
    spikeSystem->reset();
    neuronById.clear();
    sensoryNeurons.clear();
    motorNeurons.clear();
    spikeHandlersRegistered = false;
}

void NeuralDynamicsManager::injectCurrent(NeuronId neuron, MembranePotential current) {
    auto it = neuronById.find(neuron.index());
    if (it != neuronById.end()) {
        it->second->injectCurrent(current);
    }
}

void NeuralDynamicsManager::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    for (auto& region : brain_->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

} // namespace nlm
