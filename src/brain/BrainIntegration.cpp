#pragma once

#include "BrainIntegration.hpp"

namespace nlm {

void BrainIntegration::registerSpikeHandlers(Brain& brain) {
    brain.pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        ++brain.pImpl->totalSpikesThisStep;
        ++brain.pImpl->totalSpikesTotal;
    });

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

void BrainIntegration::integrateMemorySystems(Brain& brain, size_t neuronCount) {
    brain.pImpl->workingMemory->initialize(&brain);
    brain.pImpl->workingMemory->setCapacity(neuronCount / 10);

    brain.pImpl->episodicMemory->initialize(&brain);
    brain.pImpl->episodicMemory->setMaxEpisodes(1000);

    brain.pImpl->associativeMemory->initialize(&brain);
}

void BrainIntegration::integrateCognitionSystems(Brain& brain) {
    brain.pImpl->planner->initialize(&brain);
    brain.pImpl->planner->setPlanningDepth(5);

    brain.pImpl->conceptFormation->initialize(&brain);

    brain.pImpl->attention->initialize(&brain);
    brain.pImpl->attention->setInhibitionStrength(0.5f);
    brain.pImpl->attention->setExcitationStrength(1.5f);
}

void BrainIntegration::integrateNeuromodulationSystems(Brain& brain) {
    brain.pImpl->novelty->initialize(&brain);
    brain.pImpl->curiosity->initialize(&brain);
}

void BrainIntegration::configureCheckpointSystem(Brain& brain) {
    std::string checkpointDir = brain.pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    brain.pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
}

} // namespace nlm