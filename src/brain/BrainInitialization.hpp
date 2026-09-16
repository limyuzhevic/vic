#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;

class BrainInitializer {
public:
    explicit BrainInitializer(Brain::Impl* impl) : pImpl(impl) {}
    
    void initialize();
    
private:
    void initializeRandomGenerator();
    void initializePlasticitySystems();
    void configurePlasticity();
    void configureSimulationParameters();
    void configureCheckpointManager();
    void initializeMemorySystems();
    void initializePredictionSystem();
    void initializeCognitionSystems();
    void initializeDevelopmentSystem();
    void initializeNeuromodulationSystems();
    void registerSpikeHandlers();
    void initializeAllIntegratedSystems();
    
    Brain::Impl* pImpl;
};

} // namespace nlm
