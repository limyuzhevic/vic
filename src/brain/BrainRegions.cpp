#include "BrainRegions.hpp"

namespace nlm {

// Region management
RegionId BrainRegions::addRegion(Brain& brain, const std::string& name) {
    return brain.addRegion(name);
}

NeuralRegion* BrainRegions::getRegion(Brain& brain, RegionId id) {
    return brain.getRegion(id);
}

const NeuralRegion* BrainRegions::getRegion(const Brain& brain, RegionId id) {
    return brain.getRegion(id);
}

size_t BrainRegions::getRegionCount(const Brain& brain) {
    return brain.getRegionCount();
}

std::vector<RegionId> BrainRegions::getRegionIds(const Brain& brain) {
    return brain.getRegionIds();
}

const std::vector<std::unique_ptr<NeuralRegion>>& BrainRegions::getRegions(const Brain& brain) {
    return brain.getRegions();
}

// Inter-region connection management
void BrainRegions::addInterRegionConnection(Brain& brain, RegionId source, RegionId target,
                                            float weight, Delay delay) {
    brain.addInterRegionConnection(source, target, weight, delay);
}

void BrainRegions::removeInterRegionConnection(Brain& brain, RegionId source, RegionId target) {
    brain.removeInterRegionConnection(source, target);
}

// Global statistics
size_t BrainRegions::getTotalNeuronCount(const Brain& brain) {
    return brain.getTotalNeuronCount();
}

size_t BrainRegions::getTotalSynapseCount(const Brain& brain) {
    return brain.getTotalSynapseCount();
}

size_t BrainRegions::getActiveNeuronCount(const Brain& brain) {
    return brain.getActiveNeuronCount();
}

float BrainRegions::getAverageFiringRate(const Brain& brain) {
    return brain.getAverageFiringRate();
}

} // namespace nlm