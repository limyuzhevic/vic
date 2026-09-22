#pragma once

#include "BrainAccessors.hpp"

namespace nlm {

void BrainAccessors::registerSensoryInputHandler(Brain& brain) {
    brain.pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        ++brain.pImpl->totalSpikesThisStep;
        ++brain.pImpl->totalSpikesTotal;
    });
}

void BrainAccessors::registerDelayedInputHandler(Brain& brain) {
    brain.pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
        for (auto& region : brain.pImpl->regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == event.destination_neuron) {
                    MembranePotential synapticCurrent = event.weight * 10.0f;
                    if (event.is_excitatory) {
                        neuron->receiveExcitatoryInput(synapticCurrent);
                    } else {
                        neuron->receiveInhibitoryInput(-synapticCurrent);
                    }
                    return;
                }
            }
        }
    });
}

void BrainAccessors::processMotorNeuronGroup(Brain& brain, float& firingMotor) {
    firingMotor = 0;
    for (auto* neuron : brain.pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
}

void BrainAccessors::updateAttentionForWorkingMemory(Brain& brain) {
    if (brain.pImpl->workingMemory && !brain.pImpl->workingMemory->getMemoryNeurons().empty()) {
        std::vector<NeuronId> competitors = brain.pImpl->workingMemory->getMemoryNeurons();
        brain.pImpl->attention->processCompetition(competitors);
    }
}

void BrainAccessors::configureDopaminePlasticity(Brain& brain, float plasticityMod) {
    float weight = 1.0f;
    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
    // This would need to be applied to synapses, but we'll keep it simple for now
}

} // namespace nlm