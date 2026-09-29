#pragma once

#include "Brain.hpp"

namespace nlm {

class BrainRegions {
public:
    // Region management
    static RegionId addRegion(Brain& brain, const std::string& name = "");
    static NeuralRegion* getRegion(Brain& brain, RegionId id);
    static const NeuralRegion* getRegion(const Brain& brain, RegionId id);
    static size_t getRegionCount(const Brain& brain);
    static std::vector<RegionId> getRegionIds(const Brain& brain);
    static const std::vector<std::unique_ptr<NeuralRegion>>& getRegions(const Brain& brain);
    
    // Inter-region connection management
    static void addInterRegionConnection(Brain& brain, RegionId source, RegionId target,
                                         float weight = 0.0f, Delay delay = 1);
    static void removeInterRegionConnection(Brain& brain, RegionId source, RegionId target);
    
    // Global statistics
    static size_t getTotalNeuronCount(const Brain& brain);
    static size_t getTotalSynapseCount(const Brain& brain);
    static size_t getActiveNeuronCount(const Brain& brain);
    static float getAverageFiringRate(const Brain& brain);
};

} // namespace nlm