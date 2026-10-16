#include "BrainRegions.hpp"
#include "BrainCore.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

struct BrainRegions::Impl {
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    RegionId nextRegionId;
    
    Impl() : nextRegionId(1) {}
};

BrainRegions::BrainRegions(std::shared_ptr<Config> config) : Brain(config), pImpl(new Impl()) {}

BrainRegions::~BrainRegions() = default;

RegionId BrainRegions::addRegion(const std::string& name) {
    RegionId id(pImpl->nextRegionId++);
    auto region = std::make_unique<NeuralRegion>(id, name);
    pImpl->regions.push_back(std::move(region));
    return id;
}

NeuralRegion* BrainRegions::getRegion(RegionId id) {
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* BrainRegions::getRegion(RegionId id) const {
    for (const auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

// Continue implementations for other methods...

// Additional implementations for other methods...
size_t BrainRegions::getRegionCount() const {
    return pImpl->regions.size();
}

std::vector<RegionId> BrainRegions::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(pImpl->regions.size());
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& BrainRegions::getRegions() const {
    return pImpl->regions;
}

void BrainRegions::addInterRegionConnection(RegionId source, RegionId target,
                                          float weight, Delay delay) {
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

void BrainRegions::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->interRegionConnections.erase(
        std::remove_if(pImpl->interRegionConnections.begin(),
                      pImpl->interRegionConnections.end(),
                      [source, target](const InterRegionConnection& conn) {
                          return conn.sourceRegion == source && conn.targetRegion == target;
                      }),
        pImpl->interRegionConnections.end()
    );
}

size_t BrainRegions::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t BrainRegions::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getSynapseCount();
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

size_t BrainRegions::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t BrainRegions::getFiringNeuronCount() const {
    // This would need to access the brain's spike system
    // For now, return 0 (to be implemented properly)
    return 0;
}

float BrainRegions::getAverageFiringRate() const {
    if (pImpl->regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : pImpl->regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(pImpl->regions.size());
}

// Inject current into all neurons of a specific type
void BrainRegions::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    // This should delegate to the actual brain regions
    // For now, return a placeholder implementation
    // In a real implementation, this would iterate through all regions
    // and inject current into neurons of the specified type
    // This method should be called from the step() method in BrainStep.cpp
}