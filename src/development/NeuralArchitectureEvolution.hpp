#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <vector>
#include <memory>
#include <cmath>

namespace nlm {

class NeuralArchitectureEvolution {
public:
    NeuralArchitectureEvolution();
    ~NeuralArchitectureEvolution();
    
    // Update neural architecture based on development
    void update(Brain* brain, RandomGenerator& rng, double age, double dt);
    
    // Create new regions
    void createRegions(Brain* brain, RandomGenerator& rng, size_t count);
    
    // Specialize existing regions
    void specializeRegions(Brain* brain, double age);
    
    // Add myelinization to improve signal transmission
    void myelinateConnections(Brain* brain, RandomGenerator& rng, double age);
    
    // Evolve connection topology
    void evolveTopology(Brain* brain, RandomGenerator& rng, double age);
    
    // Get current specialization profile
    std::vector<NeuralRegionSpecialization> getSpecializationProfile() const;
    
    // Get regional specialization score for a region
    float getRegionalSpecialization(RegionId regionId) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm