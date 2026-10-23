#include "BrainRegionManagement.hpp"
#include "Brain.hpp"
#include "Neuron.hpp"
#include "Synapse.hpp"

namespace nlm {

RegionId BrainRegionManagement::addRegion(Brain::Impl* pImpl, const std::string& name) {
    RegionId id(pImpl->nextRegionId++);
    auto region = std::make_unique<NeuralRegion>(id, name);
    pImpl->regions.push_back(std::move(region));
    return id;
}

NeuralRegion* BrainRegionManagement::getRegion(Brain::Impl* pImpl, RegionId id) {
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* BrainRegionManagement::getRegion(const Brain::Impl* pImpl, RegionId id) const {
    for (const auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t BrainRegionManagement::getRegionCount(const Brain::Impl* pImpl) const {
    return pImpl->regions.size();
}

std::vector<RegionId> BrainRegionManagement::getRegionIds(const Brain::Impl* pImpl) const {
    std::vector<RegionId> ids;
    ids.reserve(pImpl->regions.size());
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& BrainRegionManagement::getRegions(const Brain::Impl* pImpl) const {
    return pImpl->regions;
}

void BrainRegionManagement::addInterRegionConnection(Brain::Impl* pImpl, RegionId source, RegionId target, 
                                                    float weight, Delay delay) {
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

void BrainRegionManagement::removeInterRegionConnection(Brain::Impl* pImpl, RegionId source, RegionId target) {
    pImpl->interRegionConnections.erase(
        std::remove_if(pImpl->interRegionConnections.begin(),
                      pImpl->interRegionConnections.end(),
                      [source, target](const InterRegionConnection& conn) {
                          return conn.sourceRegion == source && conn.targetRegion == target;
                      }),
        pImpl->interRegionConnections.end()
    );
}

size_t BrainRegionManagement::getTotalNeuronCount(const Brain::Impl* pImpl) const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t BrainRegionManagement::getTotalSynapseCount(const Brain::Impl* pImpl) const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getSynapseCount();
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

size_t BrainRegionManagement::getActiveNeuronCount(const Brain::Impl* pImpl) const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t BrainRegionManagement::getFiringNeuronCount(const Brain::Impl* pImpl) const {
    return pImpl->totalSpikesThisStep;
}

float BrainRegionManagement::getAverageFiringRate(const Brain::Impl* pImpl) const {
    if (pImpl->regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : pImpl->regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(pImpl->regions.size());
}

float BrainRegionManagement::getExcitationInhibitionRatio(const Brain::Impl* pImpl) const {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : pImpl->regions) {
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

size_t BrainRegionManagement::getTotalSpikeCount(const Brain::Impl* pImpl) const {
    return pImpl->totalSpikesTotal;
}

size_t BrainRegionManagement::getPendingSpikeEventCount(const Brain::Impl* pImpl) const {
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
}
