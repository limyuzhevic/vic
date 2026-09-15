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
class Acetylcholine;
class Norepinephrine;
class Serotonin;
class Curiosity;
class Novelty;
class PredictionError;
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
class CheckpointManager;

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
class Acetylcholine;
class Norepinephrine;
class Serotonin;
class Curiosity;
class Novelty;
class PredictionError;
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
class CheckpointManager;

// Memory pools for neural objects
class NeuronPool;
class SynapsePool;

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

// Acetylcholine - attention and memory consolidation
Acetylcholine* getAcetylcholine();

// Norepinephrine - arousal and vigilance
Norepinephrine* getNorepinephrine();

// Serotonin - mood, impulsivity, and social behavior
Serotonin* getSerotonin();

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
}

} // namespace nlm

// Brain memory pool system (Phase 2 enhancements)
namespace nlm {
    // NeuronPool: Pre-allocated memory for neurons
    class NeuronPool {
    public:
        explicit NeuronPool(size_t capacity);
        ~NeuronPool();
        
        // Allocate a neuron from the pool
        NeuronId allocate();
        
        // Return neuron to the pool
        void deallocate(NeuronId id);
        
        // Get neuron state for direct access (Structure of Arrays layout)
        // This enables SIMD vectorization and cache-friendly access patterns
        template<typename Member>
        Member* getState(NeuronIndex index);
        
        // Get total count of allocated neurons
        size_t size() const { return size_; }
        
        // Memory usage in bytes
        size_t memoryUsage() const;
    
    private:
        // SoA storage for neuron states - improves cache locality
        struct NeuronBlock {
            std::vector<float> membranePotential;    // V_m for LIF dynamics
            std::vector<float> restingPotential;    // V_rest baseline
            std::vector<float> threshold;           // V_threshold for firing
            std::vector<float> resetPotential;       // V_reset post-spike
            std::vector<float> leakConductance;      // g_L for membrane leak
            std::vector<float> synapseConductance;    // g_syn for synaptic input
            std::vector<float> firingRate;          // f for rate-based computation
            std::vector<uint8_t> firingState;       // FiringState enum (uint8)
            std::vector<uint32_t> refractoryRemaining; // Steps remaining refractory
            std::vector<uint32_t> refractoryPeriod;   // Total refractory period
            std::vector<float> adaptationVariable;   // A for spike-frequency adaptation
            std::vector<float> lastSpikeTime;        // t_last for STDP
            std::vector<uint64_t> neuronType;       // NeuronType enum (uint64)
            std::vector<uint64_t> regionId;         // Region ID for routing
            std::vector<uint64_t> populationId;     // Population ID for organization
            std::vector<float> synapticInput;        // Total I_syn input this step
        };
        
        std::vector<std::unique_ptr<NeuronBlock>> blocks_;  // Memory blocks
        size_t size_;                                    // Current allocated neurons
        size_t capacity_;                                // Maximum capacity
        std::vector<NeuronIndex> freeList_;             // Free list for O(1) deallocation
        std::atomic<size_t> freeListTop_;              // Atomic index for thread-safe free list
    };
    
    // SynapsePool: Pre-allocated memory for synapses
    class SynapsePool {
    public:
        SynapsePool();
        ~SynapsePool();
        
        // Allocate a synapse from the pool
        SynapseId allocate(NeuronId source, NeuronId destination);
        
        // Return synapse to the pool
        void deallocate(SynapseId id);
        
        // Get synapse state for direct access (Structure of Arrays layout)
        template<typename Member>
        Member* getState(SynapseIndex index);
        
        // Get source/destination neurons (convenience methods)
        NeuronId getSource(SynapseIndex index) const;
        NeuronId getDestination(SynapseIndex index) const;
        
        // Total count of allocated synapses
        size_t size() const { return size_; }
        
        // Memory usage in bytes
        size_t memoryUsage() const;
    
    private:
        // SoA storage for synapse states
        struct SynapseBlock {
            std::vector<uint64_t> sourceNeuron;          // Source neuron ID
            std::vector<uint64_t> destinationNeuron;      // Dest neuron ID
            std::vector<float> weight;                    // Synaptic weight W
            std::vector<uint32_t> delay;                  // Transmission delay (steps)
            std::vector<uint8_t> synapseType;              // SynapseType enum (uint8)
            std::vector<uint8_t> plasticityFlags;          // Packed flags
            
            // Short-term plasticity dynamics
            std::vector<float> shortTermDepression;        // u for Tsodyks-Markram model
            std::vector<float> shortTermFacilitation;       // U for Tsodyks-Markram model
            std::vector<float> lastPreSpikeTime;           // t_pre for STP
            std::vector<float> lastPostSpikeTime;          // t_post for STP
            
            // Reward-modulated learning
            std::vector<float> eligibilityTrace;           // e for RBSTS
            std::vector<float> efficacy;                  // g for efficacy
        };
        
        std::vector<std::unique_ptr<SynapseBlock>> blocks_;
        size_t size_;
        size_t capacity_;
        std::vector<SynapseIndex> freeList_;
        std::atomic<size_t> freeListTop_;
    };
}

} // namespace nlm
