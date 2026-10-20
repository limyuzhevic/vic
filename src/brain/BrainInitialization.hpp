#pragma once

#include "../brain/Brain.hpp"

namespace nlm {

class BrainInitialization {
public:
    BrainInitialization(Brain::Impl* pImpl) : pImpl(pImpl) {}
    
    // Main initialization method
    bool initialize();
    
    // Region management
    void addRegion(const std::string& name);
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target, float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    
    // System configuration
    void configureNeuromodulation();
    void configureSimulationParameters();

private:
    Brain::Impl* pImpl;
};

} // namespace nlm