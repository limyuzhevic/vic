#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <vector>
#include "BrainUtils.hpp"

namespace nlm {

// BrainRegionManager implementation - Region and neuron management
// Extracted from Brain.cpp lines 189-256 and 274-338

class BrainRegionManager {
public:
    // Create brain regions and set up neural populations
    static void setupRegionsAndNeurons(Brain::Impl* impl, size_t neuronCount, size_t regionCount, 
                                      float connectionProbability);
    
    // Initialize all integrated systems
    static void initializeIntegratedSystems(Brain::Impl* impl);
    
    // Register spike handlers for event-driven processing
    static void registerSpikeHandlers(Brain::Impl* impl);
    
private:
    static NeuralRegion* addRegion(Brain::Impl* impl, const std::string& name);
    static NeuralRegion* getRegion(Brain::Impl* impl, RegionId id);
};

} // namespace nlm
