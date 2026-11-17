// Brain Initializer
// Phase 6: Brain initialization and setup

#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

class BrainInitializer {
public:
    BrainInitializer();
    ~BrainInitializer();
    
    // Initialize brain with configuration
    bool initialize(Brain* brain);
    
    // Setup neural populations
    void setupNeuralPopulations(Brain* brain);
    
    // Setup inter-region connections
    void setupInterRegionConnections(Brain* brain);
    
    // Setup plasticity systems
    void setupPlasticitySystems(Brain* brain);
    
    // Setup memory systems
    void setupMemorySystems(Brain* brain);
    
    // Setup cognition systems
    void setupCognitionSystems(Brain* brain);
    
    // Setup neuromodulation systems
    void setupNeuromodulationSystems(Brain* brain);
    
    // Setup development system
    void setupDevelopmentSystem(Brain* brain);
    
    // Setup prediction system
    void setupPredictionSystem(Brain* brain);
    
    // Setup checkpoint system
    void setupCheckpointSystem(Brain* brain);
    
    // Validate brain state after initialization
    bool validateBrainState(Brain* brain) const;
    
    // Log initialization statistics
    void logInitializationStats(Brain* brain) const;
    
private:
    // Helper functions for population setup
    void setupSensoryPopulations(Brain* brain);
    void setupMotorPopulations(Brain* brain);
    void setupInternalPopulations(Brain* brain);
    
    // Helper functions for connectivity
    void setupRandomConnectivity(Brain* brain, float connectionProbability);
    void setupStructuredConnections(Brain* brain);
    
    // Configuration validation
    bool validateConfiguration(const Config* config) const;
    
    // Resource management
    void cleanup(Brain* brain);
    
    // Statistics tracking
    struct InitializationStats {
        size_t neuronsCreated = 0;
        size_t synapsesCreated = 0;
        size_t regionsCreated = 0;
        size_t populationsCreated = 0;
        double initializationTime = 0.0;
        bool success = false;
    };
    
    InitializationStats stats_;
};

} // namespace nlm