#pragma once

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>

namespace nlm {

/**
 * BrainInitializer - Handles brain initialization and setup
 * Separated from main Brain class for better organization
 */
class BrainInitializer {
public:
    BrainInitializer(std::shared_ptr<Config> config, std::unique_ptr<RandomGenerator> rng);
    ~BrainInitializer() = default;
    
    // Initialize brain regions and neurons
    bool initializeRegions(Brain* brain, size_t neuronCount, size_t regionCount);
    
    // Initialize memory systems
    bool initializeMemorySystems(Brain* brain);
    
    // Initialize plasticity systems  
    bool initializePlasticitySystems(Brain* brain);
    
    // Initialize integration points
    bool initializeIntegrationPoints(Brain* brain);
    
    // Initialize checkpoint system
    bool initializeCheckpointSystem(Brain* brain);
    
private:
    std::shared_ptr<Config> config_;
    std::unique_ptr<RandomGenerator> rng_;
    size_t nextRegionId_;
};

/**
 * NeuralProcessor - Handles neural computation and dynamics
 * Separated for better modularity and testing
 */
class NeuralProcessor {
public:
    NeuralProcessor(Brain* brain);
    ~NeuralProcessor() = default;
    
    // Process neuron membrane potentials (LIF dynamics)
    void processNeuronDynamics(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Detect and queue spike events
    void detectSpikes(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Process delayed spike events
    void processDelayedSpikes(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Apply plasticity rules (STDP, Hebbian)
    void applyPlasticity(Brain* brain, float plasticityMod, float dopamineLevel);
    
    // Update structural plasticity
    void updateStructuralPlasticity(Brain* brain, size_t currentStep);
    
private:
    Brain* brain_;
    std::vector<Neuron*> sensoryNeurons_;
    std::vector<Neuron*> motorNeurons_;
};

/**
 * SystemIntegrator - Coordinates all integrated systems
 * Separated for better system coordination
 */
class SystemIntegrator {
public:
    SystemIntegrator(Brain* brain);
    ~SystemIntegrator() = default;
    
    // Coordinate memory systems
    void coordinateMemorySystems(Brain* brain);
    
    // Coordinate neuromodulation effects
    void coordinateNeuromodulation(Brain* brain, float timestep);
    
    // Coordinate cognition systems
    void coordinateCognitionSystems(Brain* brain, float timestep);
    
    // Coordinate development effects
    void coordinateDevelopmentEffects(Brain* brain, size_t currentStep);
    
    // Coordinate replay and consolidation
    void coordinateReplayAndConsolidation(Brain* brain, size_t currentStep);
    
    // Collect and log statistics
    void collectStatistics(Brain* brain) const;
    
private:
    Brain* brain_;
};

/**
 * BrainStatusCollector - Collects and formats brain status information
 * Extracted from Brain::logStatus() for better separation
 */
class BrainStatusCollector {
public:
    BrainStatusCollector(Brain* brain);
    ~BrainStatusCollector() = default;
    
    // Collect all statistics
    void collect(Brain* brain) const;
    
    // Get formatted status string
    std::string getStatusString() const;
    
    // Get specific metric values
    size_t getTotalNeurons() const { return totalNeurons_; }
    size_t getTotalSynapses() const { return totalSynapses_; }
    size_t getActiveNeurons() const { return activeNeurons_; }
    size_t getTotalSpikes() const { return totalSpikes_; }
    float getAverageFiringRate() const { return avgFiringRate_; }
    float getEIRatio() const { return eiRatio_; }
    
private:
    Brain* brain_;
    
    // Collected metrics
    size_t totalNeurons_;
    size_t totalSynapses_;
    size_t activeNeurons_;
    size_t totalSpikes_;
    float avgFiringRate_;
    float eiRatio_;
    
    // Memory system metrics
    size_t workingMemoryTraces_;
    size_t episodicMemoryEpisodes_;
    
    // Neuromodulation metrics
    float dopamineLevel_;
    
    // Development metrics
    int developmentalStage_;
    
    // Region-specific metrics
    struct RegionMetrics {
        size_t regionId;
        std::string regionName;
        size_t neuronCount;
        size_t synapseCount;
        float avgWeight;
    };
    std::vector<RegionMetrics> regionMetrics_;
};

} // namespace nlm
