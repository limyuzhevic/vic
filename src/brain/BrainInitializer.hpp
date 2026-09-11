#pragma once

#include "BrainInitializer.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/NeuralRegion.hpp"

namespace nlm {

class BrainInitializer {
public:
    explicit BrainInitializer(const Config& config) : config_(config) {}
    
    void initializeRegions(Brain& brain, size_t neuronCount, size_t regionCount, 
                          float connectionProbability);
    
    void collectSensoryMotorNeurons(Brain& brain);
    
    void initializeConnectivity(Brain& brain, float connectionProbability);
    
    void initializeMemorySystems(Brain& brain);
    
    void initializePredictionSystem(Brain& brain);
    
    void initializeCognitionSystems(Brain& brain);
    
    void initializeNeuromodulationSystems(Brain& brain);
    
    void initializePlasticitySystems(Brain& brain);
    
    void configureCheckpointSystem(Brain& brain);
    
    void registerEventDrivenProcessors(Brain& brain);
    
    void logInitializationStatus(const Brain& brain) const;
    
private:
    const Config& config_;
    
    void configureSTDPParameters(Brain& brain) const;
    void configureStructuralPlasticity(Brain& brain) const;
    
    void setupRegionNeurons(Brain& brain, size_t regionIndex, size_t neuronsPerRegion);
    
    static float getCurrentTimestep(const Config& config);
    static void getIntegrationIntervals(size_t& replayInterval, size_t& consolidationInterval, 
                                        const Config& config);
};
