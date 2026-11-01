#include "BrainRegionManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

RegionId BrainRegionManager::addRegion(Brain* brain, const std::string& name) {
    // Create new region ID
    RegionId regionId(++brain->pImpl->nextRegionId);
    
    // Create region with the ID and name
    auto region = std::make_unique<NeuralRegion>(regionId, name);
    NeuralRegion* regionPtr = region.get();
    
    // Add to brain's region collection
    brain->pImpl->regions.push_back(std::move(region));
    brain->pImpl->nextRegionId = regionId;
    
    // Initialize statistics
    regionStats[regionId] = RegionStats();
    
    NLM_LOG_INFO("Created region " + std::to_string(regionId.index()) + ": " + (name.empty() ? "(unnamed)" : name));
    
    return regionId;
}

NeuralRegion* BrainRegionManager::getRegion(Brain* brain, RegionId id) {
    for (auto& region : brain->pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* BrainRegionManager::getRegion(Brain* brain, RegionId id) const {
    return const_cast<BrainRegionManager*>(this)->getRegion(const_cast<Brain*>(brain), id);
}

size_t BrainRegionManager::getRegionCount(Brain* brain) const {
    return brain->pImpl->regions.size();
}

std::vector<RegionId> BrainRegionManager::getRegionIds(Brain* brain) const {
    std::vector<RegionId> ids;
    ids.reserve(brain->pImpl->regions.size());
    for (const auto& region : brain->pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& BrainRegionManager::getRegions(Brain* brain) const {
    return const_cast<Brain*>(brain)->pImpl->regions;
}

void BrainRegionManager::addInterRegionConnection(Brain* brain, RegionId source, RegionId target, 
                                                 float weight, Delay delay) {
    // This would create an inter-region connection if the data structures support it
    // For now, this is a placeholder - the actual implementation would need
    // to be added to Brain.hpp based on the existing interRegionConnections field
    
    // For now, just log that this would be called
    NLM_LOG_INFO("Inter-region connection: " + std::to_string(source.index()) + 
                 " -> " + std::to_string(target.index()) + " (weight: " + 
                 std::to_string(weight) + ", delay: " + std::to_string(delay) + ")");
}

void BrainRegionManager::removeInterRegionConnection(Brain* brain, RegionId source, RegionId target) {
    // Similar to addInterRegionConnection, this would need implementation
    // Based on the existing interRegionConnections field in Brain::Impl
    NLM_LOG_INFO("Removing inter-region connection: " + std::to_string(source.index()) + 
                 " -> " + std::to_string(target.index()));
}

size_t BrainRegionManager::getTotalNeuronCount(Brain* brain) const {
    size_t total = 0;
    for (const auto& region : brain->pImpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t BrainRegionManager::getTotalSynapseCount(Brain* brain) const {
    size_t total = 0;
    for (const auto& region : brain->pImpl->regions) {
        total += region->getSynapseCount();
    }
    return total;
}

size_t BrainRegionManager::getActiveNeuronCount(Brain* brain) const {
    size_t total = 0;
    for (const auto& region : brain->pImpl->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t BrainRegionManager::getFiringNeuronCount(Brain* brain) const {
    size_t total = 0;
    for (const auto& region : brain->pImpl->regions) {
        total += region->getFiringNeuronCount();
    }
    return total;
}

float BrainRegionManager::getAverageFiringRate(Brain* brain) const {
    size_t totalNeurons = getTotalNeuronCount(brain);
    if (totalNeurons == 0) return 0.0f;
    
    size_t totalFiring = 0;
    for (const auto& region : brain->pImpl->regions) {
        totalFiring += region->getFiringNeuronCount();
    }
    
    return static_cast<float>(totalFiring) / static_cast<float>(totalNeurons);
}

float BrainRegionManager::getExcitationInhibitionRatio(Brain* brain) const {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : brain->pImpl->regions) {
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            if (weight > 0) {
                totalExcitatory += weight;
            } else {
                totalInhibitory += std::abs(weight);
            }
        }
    }
    
    if (totalInhibitory > 0.0f) {
        return totalExcitatory / totalInhibitory;
    }
    return totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
}

void BrainRegionManager::createRegionWithNeurons(Brain* brain, size_t regionIndex, size_t neuronsPerRegion) {
    auto* region = getRegion(brain, RegionId(regionIndex + 1));
    if (!region) return;

    // Add populations with neuron type distribution
    auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
    size_t internalCount = neuronsPerRegion / 2;
    size_t internalExcitatory = internalCount * 4 / 5;
    auto internalPopId = region->addPopulation(internalExcitatory, NeuronType::Internal);
    auto inhibitoryPopId = region->addPopulation(internalCount - internalExcitatory, NeuronType::Inhibitory);
    auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);

    (void)internalPopId; (void)inhibitoryPopId; (void)sensoryPopId; (void)motorPopId;
    
    collectIONeurons(brain, regionIndex);
}

void BrainRegionManager::collectIONeurons(Brain* brain, size_t regionIndex) {
    auto* region = getRegion(brain, RegionId(regionIndex + 1));
    if (!region) return;

    auto* sensoryPop = region->getPopulation(PopulationId(1));  // First population is sensory
    auto* motorPop = region->getPopulation(PopulationId(4));    // Fourth population is motor

    if (sensoryPop) {
        for (auto* neuron : sensoryPop->getNeurons()) {
            brain->pImpl->sensoryNeurons.push_back(neuron);
        }
    }
    if (motorPop) {
        for (auto* neuron : motorPop->getNeurons()) {
            brain->pImpl->motorNeurons.push_back(neuron);
        }
    }
}

void BrainRegionManager::initializeRegionConnectivity(Brain* brain, size_t regionIndex, float connectionProbability) {
    auto* region = getRegion(brain, RegionId(regionIndex + 1));
    if (!region) return;

    if (brain->pImpl->rng) {
        region->initializeRandomConnectivity(*brain->pImpl->rng, connectionProbability, 0.2f, 0.1f);
    }
}

void BrainRegionManager::buildNeuronLookup(Brain* brain) {
    for (auto& region : brain->pImpl->regions) {
        for (auto* neuron : region->getAllNeurons()) {
            brain->pImpl->neuronById[neuron->getId().index()] = neuron;
        }
    }
}

void BrainRegionManager::updateRegionStatistics(Brain* brain) {
    for (const auto& region : brain->pImpl->regions) {
        RegionId id = region->getId();
        regionStats[id].neuronCount = region->getTotalNeuronCount();
        regionStats[id].synapseCount = region->getSynapseCount();
        regionStats[id].activeNeuronCount = region->getActiveNeuronCount();
        regionStats[id].firingNeuronCount = region->getFiringNeuronCount();
        regionStats[id].averageFiringRate = region->getAverageFiringRate();
    }
}
