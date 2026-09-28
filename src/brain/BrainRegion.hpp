#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Region Management
// Handles brain region creation, management, and connectivity

class BrainRegion {
public:
    // Add a new brain region
    RegionId addRegion(Brain::Impl& pImpl, const std::string& name = "");
    
    // Get brain region
    NeuralRegion* getRegion(Brain::Impl& pImpl, RegionId id);
    const NeuralRegion* getRegion(const Brain::Impl& pImpl, RegionId id) const;
    
    // Get region count
    size_t getRegionCount(const Brain::Impl& pImpl) const;
    
    // Get region IDs
    std::vector<RegionId> getRegionIds(const Brain::Impl& pImpl) const;
    
    // Add inter-region connection
    void addInterRegionConnection(Brain::Impl& pImpl, RegionId source, RegionId target,
                                 float weight = 0.0f, Delay delay = 1);
    
    // Remove inter-region connection
    void removeInterRegionConnection(Brain::Impl& pImpl, RegionId source, RegionId target);
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions(const Brain::Impl& pImpl) const;
    
    // Get global statistics
    size_t getTotalNeuronCount(const Brain::Impl& pImpl) const;
    size_t getTotalSynapseCount(const Brain::Impl& pImpl) const;
    size_t getActiveNeuronCount(const Brain::Impl& pImpl) const;
    size_t getFiringNeuronCount(const Brain::Impl& pImpl) const;
    float getAverageFiringRate(const Brain::Impl& pImpl) const;
    
private:
    // Helper functions for region management
    void initializeNeuronsInRegions(Brain::Impl& pImpl);
    void initializeConnectivityInRegions(Brain::Impl& pImpl, float connectionProbability);
    void collectNeuronsForIO(Brain::Impl& pImpl);
    void logRegionInitializationStats(const Brain::Impl& pImpl) const;
};

} // namespace nlm
