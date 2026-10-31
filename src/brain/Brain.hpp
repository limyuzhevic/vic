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
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    Novelty* getNovelty();
    
    // Prediction error signal
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    
    // Get neuromodulation levels for advanced control
    float getDopamineLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    float getNeuromodulationLevel() const;

    // Set neuromodulation parameters for advanced control
    void setDopamineLevel(float level);
    void setCuriosityLevel(float level);
    void setNoveltyLevel(float level);
    void setPredictionErrorLevel(float level);

    // Get development system information
    std::string getDevelopmentStageName() const;
    double getDevelopmentalProgress() const;

    // Get brain performance metrics for monitoring
    double getAverageFiringRate() const;
    double getExcitatoryInhibitionRatio() const;
    size_t getMemoryTraceCount() const;
    size_t getEpisodeCount() const;
    size_t getConceptCount() const;

    // Advanced configuration methods
    void configure(float v_thresh = -55.0f, float v_rest = -70.0f, float v_reset = -75.0f,
                   float tau_mem = 20.0f, float tau_ref = 2.0f, size_t neuron_count = 1000,
                   float connection_probability = 0.1f);
    
    // Brain state management for experiments
    bool saveState(const std::string& filepath) const;
    bool loadState(const std::string& filepath);
    std::string getBrainHash() const;
    bool validateState() const;

    // Getters for all memory system pointers (for advanced access)
    std::unique_ptr<class NeuralWorkingMemory> getWorkingMemoryPtr() const;
    std::unique_ptr<class NeuralEpisodicMemory> getEpisodicMemoryPtr() const;
    std::unique_ptr<class NeuralAssociativeMemory> getAssociativeMemoryPtr() const;
    std::unique_ptr<class PredictionSystem> getPredictionSystemPtr() const;
    std::unique_ptr<class NeuralPlanner> getPlannerPtr() const;
    std::unique_ptr<class ConceptFormation> getConceptFormationPtr() const;
    std::unique_ptr<class AttentionalSelection> getAttentionPtr() const;
    std::unique_ptr<class DevelopmentSystem> getDevelopmentSystemPtr() const;
    std::unique_ptr<class Dopamine> getDopaminePtr() const;
    std::unique_ptr<class Curiosity> getCuriosityPtr() const;
    std::unique_ptr<class Novelty> getNoveltyPtr() const;
    std::unique_ptr<class PredictionError> getPredictionErrorPtr() const;

    // Getters for all system pointers (for advanced access)
    std::unique_ptr<class SpikeSystem> getSpikeSystemPtr() const;
    std::unique_ptr<class STDP> getSTDPPtr() const;
    std::unique_ptr<class Hebbian> getHebbianPtr() const;
    std::unique_ptr<class StructuralPlasticity> getStructuralPlasticityPtr() const;
    std::unique_ptr<class RandomGenerator> getRandomGeneratorPtr() const;
    std::unique_ptr<class SimulationClock> getSimulationClockPtr() const;
    std::unique_ptr<class Logger> getLoggerPtr() const;

    // Get detailed statistics for performance analysis
    struct BrainStats {
        size_t totalNeurons;
        size_t totalSynapses;
        size_t activeNeurons;
        size_t firingNeurons;
        float avgFiringRate;
        float excitationInhibitionRatio;
        size_t totalSpikes;
        size_t workingMemoryTraces;
        size_t episodicMemoryEpisodes;
        size_t conceptCount;
        float curiosityLevel;
        float noveltyLevel;
        float dopamineLevel;
        float predictionError;
        std::string developmentalStage;
        double developmentalProgress;
        std::chrono::steady_clock::time_point lastUpdate;
    };
    
    BrainStats getStatistics() const;
    std::string getStatisticsString() const;

    // Logging
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
