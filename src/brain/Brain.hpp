#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "BrainNeuralCore.hpp"
#include "BrainPlasticityManager.hpp"
#include "BrainMemoryManager.hpp"
#include "BrainCognitiveManager.hpp"
#include "BrainNeuromodulationManager.hpp"
#include "BrainDevelopmentManager.hpp"
#include "BrainSimulationManager.hpp"
#include "../environment/Observation.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <memory>
#include <string>

namespace nlm {

// Forward declarations
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;

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
    void receiveSensoryInput(const SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Access brain subsystems
    BrainNeuralCore* getNeuralCore();
    const BrainNeuralCore* getNeuralCore() const;
    
    BrainPlasticityManager* getPlasticityManager();
    const BrainPlasticityManager* getPlasticityManager() const;
    
    BrainMemoryManager* getMemoryManager();
    const BrainMemoryManager* getMemoryManager() const;
    
    BrainCognitiveManager* getCognitiveManager();
    const BrainCognitiveManager* getCognitiveManager() const;
    
    BrainNeuromodulationManager* getNeuromodulationManager();
    const BrainNeuromodulationManager* getNeuromodulationManager() const;
    
    BrainDevelopmentManager* getDevelopmentManager();
    const BrainDevelopmentManager* getDevelopmentManager() const;
    
    BrainSimulationManager* getSimulationManager();
    const BrainSimulationManager* getSimulationManager() const;
    
    // Spike system access (delegates to neural core)
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access (delegates to plasticity manager)
    STDP* getSTDP();
    const STDP* getSTDP() const;
    
    Hebbian* getHebbian();
    const Hebbian* getHebbian() const;
    
    StructuralPlasticity* getStructuralPlasticity();
    const StructuralPlasticity* getStructuralPlasticity() const;
    
    // Statistics (delegates to neural core)
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    // (delegates to neural core)
    std::unique_ptr<Action> produceAction();
    
    // Apply neuromodulatory signals (delegates to neuromodulation manager)
    void applyNeuromodulation(const Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    void develop();
    
    // Reset brain state (delegates to simulation manager)
    void reset();
    
    // Save brain state to file (checkpointing)
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    bool load(const std::string& filepath);
    
    // Region management (delegates to neural core)
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions (delegates to neural core)
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management (delegates to neural core)
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics (delegates to neural core)
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information (delegates to memory manager)
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage (delegates to memory manager)
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations (delegates to memory manager)
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    // (direct access for backward compatibility)
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning (delegates to cognitive manager)
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery (delegates to cognitive manager)
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus (delegates to cognitive manager)
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    // Development system (delegates to development manager)
    DevelopmentSystem* getDevelopmentSystem();
    
    // Developmental stage (delegates to development manager)
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement (delegates to neuromodulation manager)
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation (delegates to neuromodulation manager)
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection (delegates to neuromodulation manager)
    Novelty* getNovelty();
    
    // Prediction error signal (delegates to neuromodulation manager)
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration (delegates to simulation manager)
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator (delegates to simulation manager)
    RandomGenerator* getRandomGenerator();
    
    // Logging (delegates to simulation manager)
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm