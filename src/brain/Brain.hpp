#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>

namespace nlm {

// Forward declarations
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;

// Inter-regional connection (long-range connectivity)
struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    float weight;
    Delay delay;
    PlasticityFlags plasticityFlags;
    
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems

class Brain {
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    
    // Simulation time step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    void develop();
    
    // Reset brain state
    void reset();
    
    // Save brain state to file (checkpointing)
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    bool load(const std::string& filepath);
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // ========== ENHANCED BRAIN STATISTICS ==========
    
    // Synaptic statistics
    float getSynapticDensity() const;
    float getAverageSynapticWeight() const;
    float getConnectionProbability() const;
    
    // Regional statistics
    struct RegionalStats {
        RegionId regionId;
        std::string regionName;
        size_t neuronCount;
        size_t synapseCount;
        size_t activeNeuronCount;
        size_t firingNeuronCount;
        float averageFiringRate;
        float excitationInhibitionRatio;
        float averageSynapticWeight;
        float synapticDensity;
        
        RegionalStats(RegionId id = RegionId(), const std::string& name = "")
            : regionId(id), regionName(name), neuronCount(0), synapseCount(0),
              activeNeuronCount(0), firingNeuronCount(0), averageFiringRate(0.0f),
              excitationInhibitionRatio(1.0f), averageSynapticWeight(0.0f),
              synapticDensity(0.0f) {}
    };
    
    std::vector<RegionalStats> getRegionalStats() const;
    
    // Firing statistics
    struct FiringStats {
        size_t totalSpikeCount;
        size_t firingNeuronCount;
        float averageFiringRate;
        float maximumFiringRate;
        float minimumFiringRate;
        float firingRateStdDev;
        float firingRateSkew;
        float firingRateKurtosis;
        
        FiringStats()
            : totalSpikeCount(0), firingNeuronCount(0), averageFiringRate(0.0f),
              maximumFiringRate(0.0f), minimumFiringRate(0.0f), firingRateStdDev(0.0f),
              firingRateSkew(0.0f), firingRateKurtosis(0.0f) {}
    };
    
    FiringStats getFiringStatistics() const;
    
    // Neural population statistics
    struct PopulationStats {
        PopulationId populationId;
        RegionId regionId;
        size_t neuronCount;
        NeuronType neuronType;
        size_t activeNeuronCount;
        size_t firingNeuronCount;
        float averageFiringRate;
        float excitationInhibitionRatio;
        
        PopulationStats(PopulationId pid = PopulationId(), NeuronType type = NeuronType::Internal)
            : populationId(pid), regionId(), neuronCount(0), neuronType(type),
              activeNeuronCount(0), firingNeuronCount(0), averageFiringRate(0.0f),
              excitationInhibitionRatio(1.0f) {}
    };
    
    std::vector<PopulationStats> getNeuralPopulationStats() const;
    
    // Network metrics
    float getCriticalityIndex() const;
    float getNetworkEfficiency() const;
    float getSynchronizationMetrics() const;
    float getControlFlowMetrics() const;
    float getInformationProcessingRate() const;
    float getEnergyConsumption() const;
    
    // ========== STATISTICS ACCESS ==========
    
    // System performance metrics
    float getSystemUptime() const;  // Simulation time since initialization
    size_t getMemoryUsage() const;   // Approximate memory usage in bytes
    float getMemoryUtilization() const;  // Memory usage as percentage of capacity
    
    // Developmental metrics
    float getDevelopmentalPlasticity() const;
    float getCriticalPeriodProgress() const;
    
    // Network topology metrics
    size_t getInterRegionalConnectionCount() const;
    float getInterRegionalConnectivity() const;
    float getClusteringCoefficient() const;
    
    // Temporal dynamics metrics
    float getTemporalCoherence() const;
    float getBurstingRate() const;
    size_t getSpikeTemporalPrecision() const;
    
    // ========== PLURALISM ACCESS ==========
    
    // Neural regions and populations access
    size_t getRegionPopulationCount(RegionId regionId) const;
    size_t getTotalConnectionsFromNeuron(NeuronId neuronId) const;
    size_t getTotalConnectionsToNeuron(NeuronId neuronId) const;
    
    // Circuit analysis
    float getCircuitExcitability(RegionId regionId) const;
    float getCircuitInhibition(RegionId regionId) const;
    std::vector<NeuronId> getDominantNeurons(RegionId regionId, size_t count) const;
    
    // Network motifs
    size_t getFeedForwardLoopCount() const;
    size_t getFeedbackLoopCount() const;
    size_t getRecurrentLoopCount() const;
    
    // ========== METRICS FOR SIMULATION ANALYSIS ==========
    
    // Learning metrics
    float getLearningProgress() const;
    float getMemoryRetention() const;
    float getPredictionAccuracy() const;
    
    // Stability metrics
    float getSystemStability() const;
    float getOscillationFrequency() const;
    float getAlphaPower() const;
    
    // Performance metrics
    float getTaskPerformanceScore() const;
    float getCognitiveLoad() const;
    float getDecisionConfidence() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
