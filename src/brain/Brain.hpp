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
    
    // Logging
    void logStatus() const;
    
    // ========== ADVANCED BRAIN CONTROLS ==========
    
    // Direct neuron population access
    std::vector<Neuron*> getNeuronsByType(NeuronType type) const;
    std::vector<Neuron*> getNeuronsByPopulation(PopulationId popId) const;
    Neuron* getNeuron(NeuronId id) const;
    
    // Simulation control
    bool isRunning() const { return !pImpl->isResting; }
    void pause() { pImpl->isResting = true; }
    void resume() { pImpl->isResting = false; }
    void togglePause() { pImpl->isResting = !pImpl->isResting; }
    
    // Time control and monitoring
    Timestamp getCurrentTime() const { return pImpl->currentTime; }
    SimulationStep getCurrentStep() const { return pImpl->currentStep; }
    double getTimeStep() const { return pImpl->timestep; }
    
    // Enhanced statistics
    size_t getSpikesThisStep() const { return pImpl->totalSpikesThisStep; }
    size_t getPendingSpikeEvents() const { return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount(); }
    float getEnergyConsumption() const;
    float getSynapticActivity() const;
    
    // Spike system monitoring and control
    void addSpikeHandler(const std::function<void(const SpikeEvent&)>& handler);
    void removeSpikeHandler(const std::function<void(const SpikeEvent&)>& handler);
    std::vector<SpikeEvent> getRecentSpikes(size_t count) const;
    void clearSpikeHistory();
    
    // Region and population control
    NeuralRegion* getRegionByName(const std::string& name) const;
    PopulationId addPopulation(RegionId regionId, size_t neuronCount, NeuronType type);
    bool removePopulation(RegionId regionId, PopulationId popId);
    
    // Development system controls
    void advanceDevelopmentStage();
    void setDevelopmentAge(double age);
    double getDevelopmentAge() const;
    
    // Neuromodulation controls
    void setDopamineLevel(float level);
    void setCuriosityLevel(float level);
    void setNoveltyLevel(float level);
    float getDopamineLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    
    // Memory system controls
    size_t getWorkingMemorySize() const;
    size_t getEpisodicMemoryCount() const;
    size_t getAssociativeMemorySize() const;
    void clearWorkingMemory();
    void clearEpisodicMemory();
    void clearAssociativeMemory();
    
    // Pattern recording and analysis
    std::vector<NeuronId> getActiveNeurons() const;
    std::vector<float> getNeuralActivityPattern() const;
    float getNeuralActivityForNeuron(NeuronId id) const;
    std::vector<NeuronId> getMostActiveNeurons(size_t count) const;
    
    // Event queue management
    bool hasPendingEvents() const;
    void flushPendingEvents();
    
    // Event-driven processing control
    void setEventDrivenMode(bool enabled);
    bool isEventDrivenMode() const;
    void setProcessingLatency(float latency);
    float getProcessingLatency() const;
    
    // Configuration overrides
    void setNeuronParameters(const std::string& paramName, float value);
    void restoreDefaultNeuronParameters();
    
    // Performance and debugging
    void enableDebugLogging(bool enabled);
    bool isDebugLoggingEnabled() const;
    void setPerformanceMode(bool highPerformance);
    bool isPerformanceMode() const;
    
    // State monitoring
    bool isInReplayMode() const { return pImpl->stepsSinceLastEpisode > 0; }
    size_t getReplayProgress() const;
    double getConsolidationProgress() const;
    
    // Reward and reinforcement controls
    void setReward(float reward, float predictedReward = 0.0f);
    void setPunishment(float punishment);
    
    // Curiosity and exploration controls
    void setExplorationThreshold(float threshold);
    void setExploitationWeight(float weight);
    float getExplorationDrive() const;
    float getExploitationDrive() const;
    
    // Integration time management
    void setIntegrationInterval(size_t replayInterval, size_t consolidationInterval);
    void getIntegrationInterval(size_t& replayInterval, size_t& consolidationInterval) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
