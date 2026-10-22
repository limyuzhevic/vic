#pragma once

#include "Brain.hpp"

namespace nlm {

// Region management
RegionId Brain::addRegion(const std::string& name);
NeuralRegion* Brain::getRegion(RegionId id);
const NeuralRegion* Brain::getRegion(RegionId id) const;
size_t Brain::getRegionCount() const;
std::vector<RegionId> Brain::getRegionIds() const;

// Get all regions
const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const;

// Inter-region connection management
void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight = 0.0f, Delay delay = 1);
void Brain::removeInterRegionConnection(RegionId source, RegionId target);

} // namespace nlm
