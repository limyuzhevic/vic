#pragma once

#include "Brain.hpp"
#include "NeuralRegion.hpp"
#include <string>
#include <vector>

namespace nlm {

class BrainRegionManager {
public:
    BrainRegionManager() = default;
    ~BrainRegionManager() = default;

    // Create and manage brain regions
    RegionId addRegion(Brain* brain, const std::string& name = "");
    NeuralRegion* getRegion(Brain* brain, RegionId id);
    const NeuralRegion* getRegion(Brain* brain, RegionId id) const;
    size_t getRegionCount(Brain* brain) const;
    std::vector<RegionId> getRegionIds(Brain* brain) const;
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions(Brain* brain) const;

    // Inter-region connection management
    void addInterRegionConnection(Brain* brain, RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(Brain* brain, RegionId source, RegionId target);

    // Region statistics and utilities
    size_t getTotalNeuronCount(Brain* brain) const;
    size_t getTotalSynapseCount(Brain* brain) const;
    size_t getActiveNeuronCount(Brain* brain) const;
    size_t getFiringNeuronCount(Brain* brain) const;
    float getAverageFiringRate(Brain* brain) const;
    float getExcitationInhibitionRatio(Brain* brain) const;

private:
    // Helper: create region with neurons and populations
    void createRegionWithNeurons(Brain* brain, size_t regionIndex, size_t neuronsPerRegion);

    // Helper: collect input/output neurons for I/O system
    void collectIONeurons(Brain* brain, size_t regionIndex);

    // Helper: initialize random connectivity for a region
    void initializeRegionConnectivity(Brain* brain, size_t regionIndex, float connectionProbability);

    // Helper: build neuron lookup map
    void buildNeuronLookup(Brain* brain);

    // Helper: calculate region statistics
    void updateRegionStatistics(Brain* brain);

    // Region statistics cache
    struct RegionStats {
        size_t neuronCount = 0;
        size_t synapseCount = 0;
        size_t activeNeuronCount = 0;
        size_t firingNeuronCount = 0;
        float averageFiringRate = 0.0f;
    };

    std::unordered_map<RegionId, RegionStats> regionStats;
};
