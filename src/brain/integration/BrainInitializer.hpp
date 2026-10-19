#pragma once

#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

class BrainInitializer {
public:
    BrainInitializer();
    ~BrainInitializer();
    
    // Initialize brain with configuration
    bool initialize(Brain* brain);
    
    // Create regions and neurons
    void setupRegions(Brain* brain, size_t neuronCount, size_t regionCount, 
                     float connectionProbability);
    
    // Create neurons across regions
    void createNeurons(Brain* brain, size_t neuronCount, size_t regionCount);
    
    // Initialize connectivity
    void initializeConnectivity(Brain* brain, float connectionProbability);
    
    // Initialize all integrated systems
    void initializeIntegratedSystems(Brain* brain, size_t neuronCount);
    
    // Configure plasticity systems
    void configurePlasticitySystems(Brain* brain);
    
    // Configure integration intervals
    void configureIntegrationIntervals(Brain* brain);
    
    // Configure checkpoint manager
    void configureCheckpointManager(Brain* brain);
    
    // Display initialization summary
    void logInitializationStatus(Brain* brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm