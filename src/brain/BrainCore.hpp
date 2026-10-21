#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

class BrainCore {
public:
    BrainCore(std::shared_ptr<Config> config);
    
    // Configure systems from config
    void configurePlasticitySystems();
    void configureSimulationParameters();
    
    // Initialize brain components
    void initializeNeurons();
    void collectSensoryNeurons();
    
    // Create synaptic connections
    void createSynapticConnections(float connectionProbability);
    
    // Region management
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    void addInterRegionConnection(RegionId source, RegionId target, 
                                 float weight = 0.0f, Delay delay = 1);
    
    // Process simulation step
    void processSimulationStep();
    void processInterRegionConnections();
    void updatePlasticitySystems();
    void develop();
    void updateMemorySystems();
    void updatePredictionSystem();
    void checkAndTriggerReplayConsolidation();
    
    // Statistics and logging
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    void logStatus() const;
    
    // Configuration access
    const std::shared_ptr<Config>& getConfig() const { return config_; }
    
private:
    // Core components
    std::shared_ptr<Config> config_;
    std::unique_ptr<RandomGenerator> rng_;
    std::vector<std::unique_ptr<NeuralRegion>> regions_;
    std::vector<InterRegionConnection> interRegionConnections_;
    
    // Phase 2: Real neural computation components
    std::unique_ptr<SpikeSystem> spikeSystem_;
    std::unique_ptr<STDP> stdp_;
    std::unique_ptr<Hebbian> hebbian_;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity_;
    
    // Simulation parameters
    TimestepDuration timestep_;
    SimulationStep currentStep_;
    Timestamp currentTime_;
    
    // Statistics
    size_t totalSpikesThisStep_;
    size_t totalSpikesTotal_;
    
    // Sensory neurons for input injection
    std::vector<Neuron*> sensoryNeurons_;
    std::vector<Neuron*> motorNeurons_;
    
    // Integration state
    bool isResting_;  // For sleep/rest cycle
    size_t stepsSinceLastEpisode_;
    size_t replayInterval_;
    size_t consolidationInterval_;
    
    // Development state
    DevelopmentalStage developmentalStage_;
    RegionId nextRegionId_;
};

}} // namespace nlm