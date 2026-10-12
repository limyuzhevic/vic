#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

// Brain region management methods

RegionId Brain::addRegion(const std::string& name) {
    // Create and add a new region
    RegionId newId = ++pImpl->nextRegionId;
    
    auto region = std::make_unique<NeuralRegion>(newId, name);
    NeuralRegion* regionPtr = region.get();
    pImpl->regions.push_back(std::move(region));
    
    NLM_LOG_INFO("Added new region " + std::to_string(newId) + ": " + name);
    return newId;
}

NeuralRegion* Brain::getRegion(RegionId id) {
    // Find region by ID
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    return const_cast<Brain*>(this)->getRegion(id);
}

size_t Brain::getRegionCount() const {
    return pImpl->regions.size();
}

std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(pImpl->regions.size());
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                    float weight, Delay delay) {
    // Add long-range connection between regions
    InterRegionConnection conn(source, target, weight, delay);
    pImpl->interRegionConnections.push_back(conn);
    
    NLM_LOG_INFO("Added inter-region connection: " + std::to_string(source) + 
                 " -> " + std::to_string(target) + " (weight=" + 
                 std::to_string(weight) + ", delay=" + std::to_string(delay) + ")");
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    // Remove inter-region connection
    auto it = std::remove_if(pImpl->interRegionConnections.begin(), 
                             pImpl->interRegionConnections.end(),
        [source, target](const InterRegionConnection& conn) {
            return conn.sourceRegion == source && conn.targetRegion == target;
        });
    
    if (it != pImpl->interRegionConnections.end()) {
        pImpl->interRegionConnections.erase(it, pImpl->interRegionConnections.end());
        NLM_LOG_INFO("Removed inter-region connection: " + std::to_string(source) + 
                     " -> " + std::to_string(target));
    }
}

size_t Brain::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getTotalSynapseCount();
    }
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    size_t active = 0;
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (neuron->isFiring() || neuron->getState().membranePotential > neuron->getState().restingPotential) {
                    active++;
                }
            }
        }
    }
    return active;
}

size_t Brain::getFiringNeuronCount() const {
    size_t firing = 0;
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (neuron->isFiring()) {
                    firing++;
                }
            }
        }
    }
    return firing;
}

float Brain::getAverageFiringRate() const {
    if (getTotalNeuronCount() == 0) return 0.0f;
    
    size_t totalFiring = 0;
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (neuron->isFiring()) {
                    totalFiring++;
                }
            }
        }
    }
    
    return static_cast<float>(totalFiring) / getTotalNeuronCount();
}

} // namespace nlm