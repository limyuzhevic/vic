#pragma once

#include "BrainPersistence.hpp"

namespace nlm {

void BrainPersistence::Impl::setupNeuronCheckpointData(Brain& brain, NeuronCheckpointData& data) {
    data.membranePotential.reserve(brain.getTotalNeuronCount());
    data.restingPotential.reserve(brain.getTotalNeuronCount());
    data.threshold.reserve(brain.getTotalNeuronCount());
    data.resetPotential.reserve(brain.getTotalNeuronCount());
    data.leakConductance.reserve(brain.getTotalNeuronCount());
}

void BrainPersistence::Impl::saveNeurons(Brain& brain, CheckpointWriter& writer) {
    NeuronCheckpointData neuronData;
    setupNeuronCheckpointData(brain, neuronData);

    for (const auto& region : brain.pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                neuronData.membranePotential.push_back(state.membranePotential);
                neuronData.restingPotential.push_back(state.restingPotential);
                neuronData.threshold.push_back(state.threshold);
                neuronData.resetPotential.push_back(state.resetPotential);
                neuronData.leakConductance.push_back(state.leakConductance);
                neuronData.firingState.push_back(static_cast<uint8_t>(state.firingState));
                neuronData.refractoryRemaining.push_back(state.refractoryRemaining);
                neuronData.refractoryPeriod.push_back(state.refractoryPeriod);
                neuronData.lastSpikeTime.push_back(state.lastSpikeTime);
            }
        }
    }

    writer.writeNeurons(neuronData);
}

void BrainPersistence::Impl::saveSynapses(Brain& brain, CheckpointWriter& writer) {
    SynapseCheckpointData synapseData;
    for (const auto& region : brain.pImpl->regions) {
        for (const auto* syn : region->getSynapses()) {
            synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
            synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
            synapseData.weight.push_back(syn->getWeight());
            synapseData.delay.push_back(syn->getDelay());
            synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
            synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
        }
    }

    writer.writeSynapses(synapseData);
}

void BrainPersistence::Impl::loadNeurons(Brain& brain, CheckpointReader& reader) {
    NeuronCheckpointData neuronData;
    if (!reader.readNeurons(neuronData)) {
        throw std::runtime_error("Failed to read neurons from checkpoint");
    }

    size_t idx = 0;
    for (auto& region : brain.pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (idx < neuronData.membranePotential.size()) {
                    neuron->setMembranePotential(neuronData.membranePotential[idx]);
                    neuron->setRestingPotential(neuronData.restingPotential[idx]);
                    neuron->setThreshold(neuronData.threshold[idx]);
                    neuron->setResetPotential(neuronData.resetPotential[idx]);
                    neuron->setLeakConductance(neuronData.leakConductance[idx]);
                    if (idx < neuronData.firingState.size()) {
                        neuron->setFiringState(static_cast<FiringState>(neuronData.firingState[idx]));
                    }
                    if (idx < neuronData.refractoryRemaining.size()) {
                        neuron->setRefractoryPeriod(neuronData.refractoryPeriod[idx]);
                    }
                }
                idx++;
            }
        }
    }
}

} // namespace nlm