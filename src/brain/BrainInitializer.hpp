#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include "../brain/Brain.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// BrainInitializer manages the initialization of all brain systems
class BrainInitializer {
public:
    BrainInitializer() = default;
    BrainInitializer(Brain* brain);
    ~BrainInitializer() = default;
    
    // Initialize all brain systems
    bool initialize();
    
    // Get configuration values
    static size_t getNeuronCount(const Config* config);
    static size_t getRegionCount(const Config* config);
    static float getConnectionProbability(const Config* config);
    
    // Create regions with proper organization
    void createRegions(Brain* brain);
    
    // Create populations within regions
    void createPopulations(Brain* brain);
    
    // Initialize connectivity
    void initializeConnectivity(Brain* brain, RandomGenerator& rng, float connectionProbability);
    
    // Initialize memory systems
    void initializeMemorySystems(Brain* brain);
    
    // Initialize prediction system
    void initializePredictionSystem(Brain* brain);
    
    // Initialize cognition systems
    void initializeCognitionSystems(Brain* brain);
    
    // Initialize neuromodulation systems
    void initializeNeuromodulationSystems(Brain* brain);
    
    // Configure STDP parameters
    void configureSTDP(Brain* brain);
    
    // Configure structural plasticity
    void configureStructuralPlasticity(Brain* brain);
    
    // Configure spike system handlers
    void configureSpikeSystem(Brain* brain);
    
    // Configure checkpoint manager
    void configureCheckpointManager(Brain* brain);
    
    // Log initialization status
    void logInitializationStatus(Brain* brain) const;

private:
    Brain* brain_ = nullptr;
};

} // namespace nlm
