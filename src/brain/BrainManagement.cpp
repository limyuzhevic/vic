#pragma once

#include "BrainManagement.hpp"

namespace nlm {

void BrainManagement::Impl::createRegions(Brain& brain, size_t regionCount) {
    for (size_t i = 0; i < regionCount; ++i) {
        brain.addRegion("Region_" + std::to_string(i + 1));
    }
}

void BrainManagement::Impl::createNeurons(Brain& brain, size_t neuronCount, size_t regionCount) {
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain.getRegion(RegionId(i + 1));
        if (region) {
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);

            auto* sensoryPop = region->getPopulation(sensoryPopId);
            auto* motorPop = region->getPopulation(motorPopId);
            if (sensoryPop) {
                for (auto* neuron : sensoryPop->getNeurons()) {
                    brain.pImpl->sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    brain.pImpl->motorNeurons.push_back(neuron);
                }
            }
        }
    }
}

void BrainManagement::Impl::initializeConnectivity(Brain& brain, size_t regionCount, float connectionProbability) {
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain.getRegion(RegionId(i + 1));
        if (region) {
            region->initializeRandomConnectivity(*brain.pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
}

void BrainManagement::Impl::updateStatistics(Brain& brain) {
    brain.pImpl->totalSpikesThisStep = 0;
}

void BrainManagement::Impl::collectSensoryMotorNeurons(Brain& brain) {
    pImpl->sensoryNeurons.clear();
    pImpl->motorNeurons.clear();
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Sensory) {
                for (auto* neuron : pop->getNeurons()) {
                    pImpl->sensoryNeurons.push_back(neuron);
                }
            } else if (pop->getNeuronType() == NeuronType::Motor) {
                for (auto* neuron : pop->getNeurons()) {
                    pImpl->motorNeurons.push_back(neuron);
                }
            }
        }
    }
}

} // namespace nlm