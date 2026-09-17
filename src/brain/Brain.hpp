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
    
    // ========== UNIFIED MEMORY INTERFACE ==========
    
    // --- Working Memory Unified Interface ---
    
    // Store current neural activity pattern in working memory
    void storeWorkingMemory(const std::vector<float>& pattern, float strength = 1.0f);
    
    // Store activation level for specific neuron in working memory
    void storeWorkingMemoryToNeuron(NeuronId neuron, float activation);
    
    // Retrieve current working memory content
    std::vector<float> retrieveWorkingMemory() const;
    
    // Get number of active memory traces in working memory
    size_t getWorkingMemoryActiveTraces() const;
    
    // Get working memory capacity
    size_t getWorkingMemoryCapacity() const;
    
    // Set working memory capacity
    void setWorkingMemoryCapacity(size_t capacity);
    
    // Get working memory decay rate
    float getWorkingMemoryDecayRate() const;
    
    // Set working memory decay rate
    void setWorkingMemoryDecayRate(float rate);
    
    // Get current neurons stored in working memory
    const std::vector<NeuronId>& getWorkingMemoryNeurons() const;
    
    // Strengthen current working memory traces
    void strengthenWorkingMemory(float factor);
    
    // Clear working memory completely
    void clearWorkingMemory();
    
    // --- Episodic Memory Unified Interface ---
    
    // Store current brain state as an episode
    void storeCurrentEpisode();
    
    // Store a specific episodic memory item
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    // Retrieve recent episodes
    std::vector<const EpisodicMemoryItem*> getRecentEpisodes(size_t count) const;
    
    // Get total number of stored episodes
    size_t getEpisodeCount() const;
    
    // Get episodes for replay (selection based on relevance and recency)
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const;
    
    // Replay a specific episode
    void replayEpisode(const EpisodicMemoryItem* episode);
    
    // Clear all episodes
    void clearEpisodicMemory();
    
    // --- Associative Memory Unified Interface ---
    
    // Create association between two neural patterns
    void associateMemory(const std::vector<float>& patternA, const std::vector<float>& patternB, float strength = 1.0f);
    
    // Retrieve patterns associated with a query pattern
    std::vector<std::vector<float>> retrieveAssociative(const std::vector<float>& queryPattern, size_t maxResults = 5) const;
    
    // Get association strength between two patterns
    float getAssociationStrength(const std::vector<float>& patternA, const std::vector<float>& patternB) const;
    
    // Update association based on outcome
    void updateAssociation(const std::vector<float>& patternA, const std::vector<float>& patternB, float delta);
    
    // Spread activation from cue pattern
    std::vector<std::vector<float>> spreadActivation(const std::vector<float>& cuePattern, size_t steps = 2) const;
    
    // Clear all associations
    void clearAssociativeMemory();
    
    // Get number of associations
    size_t getAssociationCount() const;
    
    // --- Helper Methods ---
    
    // Store current brain state in all memory systems
    void storeCurrentState();
    
    // Replay recent important memories
    void replayRecentMemories(size_t count = 3);
    
    // Clear all memory systems
    void clearAllMemory();
    
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
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
