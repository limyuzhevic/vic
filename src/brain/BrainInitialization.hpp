#pragma once

#include "../src/core/Config/Config.hpp"
#include "../src/core/Random/Random.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/core/SimulationClock/SimulationClock.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/performance/CheckpointSystem.hpp"
#include <memory>
#include <string>

namespace nlm {

class BrainInitialization {
public:
    explicit BrainInitialization(std::shared_ptr<Config> config);
    ~BrainInitialization();
    
    // Disable copying
    BrainInitialization(const BrainInitialization&) = delete;
    BrainInitialization& operator=(const BrainInitialization&) = delete;
    
    // Initialize the brain with configuration
    bool initialize(Brain* brain);
    
    // Get configuration values
    size_t getNeuronCount() const;
    size_t getRegionCount() const;
    float getConnectionProbability() const;
    
    // Get timestep
    TimestepDuration getTimestep() const;
    
    // Get integration intervals
    size_t getReplayInterval() const;
    size_t getConsolidationInterval() const;
    
    // Configure plasticity systems
    void configurePlasticitySystems(Brain* brain);
    
    // Initialize memory systems
    void initializeMemorySystems(Brain* brain);
    
    // Initialize prediction system
    void initializePredictionSystem(Brain* brain);
    
    // Initialize cognition systems
    void initializeCognitionSystems(Brain* brain);
    
    // Initialize neuromodulation systems
    void initializeNeuromodulationSystems(Brain* brain);
    
    // Configure checkpoint manager
    void configureCheckpointManager(Brain* brain);
    
    // Create regions with neurons
    void createRegions(Brain* brain);
    
    // Setup random connectivity
    void setupRandomConnectivity(Brain* brain);
    
    // Register spike handlers
    void registerSpikeHandlers(Brain* brain);
    
    // Get sensory and motor neurons
    const std::vector<Neuron*>& getSensoryNeurons() const;
    const std::vector<Neuron*>& getMotorNeurons() const;
    
    // Get statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getTotalSpikeCount() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
