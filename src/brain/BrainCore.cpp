#include "BrainCore.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// ===========================================
// NEURAL DYNAMICS AND SPIKE SYSTEM CORE
// ===========================================

void BrainCore::initializeNeuronDynamics() {
    // Initialize neuron dynamics parameters
    // This is a placeholder for the real implementation
    NLM_LOG_INFO("Initializing neural dynamics (LIF model)");
}

void BrainCore::updateNeuronDynamics(const std::vector<Neuron*>& neurons, 
                                     Timestamp currentTime, TimestepDuration dt) {
    // Update all neurons using LIF dynamics
    for (auto* neuron : neurons) {
        neuron->stepLIF(currentTime, dt);
    }
}

void BrainCore::processSpikes(std::vector<Neuron*>& neurons, SimulationStep step,
                              SpikeSystem* spikeSystem) {
    // Detect and process spikes
    for (auto* neuron : neurons) {
        const auto& state = neuron->getState();
        bool justFired = (state.firingState == FiringState::Refractory &&
                         state.lastSpikeTime >= 0.0f &&
                         std::abs(static_cast<float>(step) - state.lastSpikeTime) < 0.002f);

        if (justFired) {
            // Queue spike event
            SpikeEvent event(neuron->getId(), step, step);
            spikeSystem->queueSpike(event);

            // Record spike for plasticity
            neuron->recordPostSpike(step);

            // Schedule delayed spike events for outgoing synapses
            for (auto* syn : neuron->getOutgoingSynapses()) {
                Delay delay = syn->getDelay();
                SimulationStep deliveryStep = step + delay;
                Timestamp deliveryTime = step * 0.001 + delay * 0.001;

                DelayedSpikeEvent delayedEvent(
                    neuron->getId(),
                    syn->getDestinationNeuron(),
                    syn->getId(),
                    syn->getWeight(),
                    syn->getType(),
                    step * 0.001,
                    deliveryTime,
                    step,
                    deliveryStep
                );

                spikeSystem->queueDelayedSpike(delayedEvent);

                // Record pre-synaptic spike
                syn->recordPreSpike(step * 0.001);
            }
        }
    }
}

void BrainCore::deliverSynapticInput(std::vector<Neuron*>& neurons,
                                     const DelayedSpikeEvent& event,
                                     SpikeSystem* spikeSystem) {
    // Deliver delayed synaptic input to destination neuron
    for (auto* neuron : neurons) {
        if (neuron->getId() == event.destination_neuron) {
            // Apply synaptic weight as current
            MembranePotential synapticCurrent = event.weight * 10.0f;
            if (event.is_excitatory) {
                neuron->receiveExcitatoryInput(synapticCurrent);
            } else {
                neuron->receiveInhibitoryInput(-synapticCurrent);
            }
            break;
        }
    }
}

// ===========================================
// PLASTICITY RULES (STDP AND HEBBIAN)
// ===========================================

void BrainCore::applySTDP(std::vector<Synapse*>& synapses, SpikeSystem* spikeSystem,
                          float plasticityMod) {
    for (auto* syn : synapses) {
        if (syn->getPlasticityFlags().stdp) {
            const auto& preSpikes = syn->getPreSpikeHistory();
            const auto& postSpikes = syn->getPostSpikeHistory();

            if (!preSpikes.empty() && !postSpikes.empty()) {
                // Update STDP weights
                spikeSystem->getSTDP()->update(syn, preSpikes, postSpikes, 0.001);
                float weight = syn->getWeight();
                weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                syn->setWeight(weight);
            }
        }
    }
}

void BrainCore::applyHebbianLearning(std::vector<Synapse*>& synapses, SpikeSystem* spikeSystem) {
    for (auto* syn : synapses) {
        if (syn->getPlasticityFlags().hebbian) {
            const auto& preSpikes = syn->getPreSpikeHistory();
            const auto& postSpikes = syn->getPostSpikeHistory();

            if (!preSpikes.empty() && !postSpikes.empty()) {
                spikeSystem->getHebbian()->update(syn, preSpikes, postSpikes, 0.001);
            }
        }
    }
}

void BrainCore::updateSynapticState(std::vector<Synapse*>& synapses, Timestamp currentTime) {
    for (auto* syn : synapses) {
        syn->step(currentTime);
    }
}

// ===========================================
// STRUCTURAL PLASTICITY
// ===========================================

void BrainCore::applyStructuralPlasticity(Brain* brain, RandomGenerator* rng) {
    if (brain) {
        brain->getStructuralPlasticity()->update(brain, *rng);
    }
}

// ===========================================
// NEUROMODULATION EFFECTS ON NEURONS
// ===========================================

void BrainCore::applyDopamineEffects(std::vector<Neuron*>& neurons, Dopamine* dopamine) {
    if (!dopamine) return;

    float dopamineLevel = dopamine->getLevel();
    float excitabilityMod = dopamineLevel * 0.5f;
    if (excitabilityMod > 0.0f) {
        for (auto* neuron : neurons) {
            neuron->injectCurrent(excitabilityMod);
        }
    }
}

// ===========================================
// UTILITY FUNCTIONS
// ===========================================

void BrainCore::storeActiveNeuronsToWorkingMemory(NeuralWorkingMemory* workingMemory,
                                                  const std::vector<Neuron*>& neurons) {
    if (!workingMemory) return;

    for (auto* neuron : neurons) {
        const auto& state = neuron->getState();
        float activation = std::abs(state.membranePotential - state.restingPotential) / 10.0f;
        workingMemory->storeToNeuron(neuron->getId(), activation);
    }
}

float BrainCore::getExcitationInhibitionRatio(const std::vector<Synapse*>& synapses) {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;

    for (const auto* syn : synapses) {
        float weight = syn->getWeight();
        if (weight > 0) {
            totalExcitatory += weight;
        } else {
            totalInhibitory += std::abs(weight);
        }
    }

    if (totalInhibitory > 0.0f) {
        return totalExcitatory / totalInhibitory;
    }
    return totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
}

} // namespace nlm
