#pragma once

#include "../Brain.hpp"

namespace nlm {

class BrainRegions : public Brain {
public:
    // Region management
    RegionId addRegion(const std::string& name = "") override;
    NeuralRegion* getRegion(RegionId id) override;
    const NeuralRegion* getRegion(RegionId id) const override;
    size_t getRegionCount() const override;
    std::vector<RegionId> getRegionIds() const override;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const override;
    
    // Inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target,
                                  float weight = 0.0f, Delay delay = 1) override;
    void removeInterRegionConnection(RegionId source, RegionId target) override;
    
    // Global statistics
    size_t getTotalNeuronCount() const override;
    size_t getTotalSynapseCount() const override;
    size_t getActiveNeuronCount() const override;
    size_t getFiringNeuronCount() const override;
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current) override;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm