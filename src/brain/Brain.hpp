#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <atomic>
#include <mutex>

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

// Thread-safe pointer wrapper for pImpl
class AtomicImplPtr {
public:
    AtomicImplPtr() : implPtr(nullptr) {}
    explicit AtomicImplPtr(Impl* p) : implPtr(p) {}
    
    Impl* get() const {
        return implPtr.load(std::memory_order_acquire);
    }
    
    void set(Impl* p) {
        implPtr.store(p, std::memory_order_release);
    }
    
    Impl* exchange(Impl* p) {
        return implPtr.exchange(p, std::memory_order_acq_rel);
    }
    
    Impl* operator->() const {
        return get();
    }
    
    explicit operator bool() const {
        return get() != nullptr;
    }
    
    Impl& operator*() const {
        return *get();
    }
    
    ~AtomicImplPtr() {
        delete get();
    }

private:
    std::atomic<Impl*> implPtr;
    friend class Brain;
};

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems

class Brain {
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    // Enhanced destructor with proper resource cleanup
    ~Brain();
    
    // Disable copying to maintain encapsulation
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    
    // Enhanced move constructor with safe pImpl transfer
    Brain(Brain&& other) noexcept;
    
    // Enhanced move assignment operator
    Brain& operator=(Brain&& other) noexcept;
    
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
    
    // ========== RAII RESOURCE MANAGEMENT ==========
    
    // RAII resource wrapper for memory management
    template<typename T>
    class ResourceGuard {
    public:
        explicit ResourceGuard(T* resource) : resource_(resource) {}
        ~ResourceGuard() {
            if (resource_) {
                delete resource_;
            }
        }
        
        T* get() const { return resource_; }
        T* release() {
            T* temp = resource_;
            resource_ = nullptr;
            return temp;
        }
        
        // Disable copying
        ResourceGuard(const ResourceGuard&) = delete;
        ResourceGuard& operator=(const ResourceGuard&) = delete;
        
    private:
        T* resource_;
    };
    
    // Memory management helpers
    void cleanupResources() noexcept;
    bool validateResources() const noexcept;
    void reportMemoryStatus() const;
    
    // Configuration validation and resource preparation
    bool prepareResources();
    void initializeSubsystems();
    
    // Atomic pointer wrapper for pImpl
    class AtomicImplPtr {
    public:
        AtomicImplPtr() : implPtr(nullptr) {}
        explicit AtomicImplPtr(Impl* p) : implPtr(p) {}
        
        Impl* get() const {
            return implPtr.load(std::memory_order_acquire);
        }
        
        void set(Impl* p) {
            implPtr.store(p, std::memory_order_release);
        }
        
        Impl* exchange(Impl* p) {
            return implPtr.exchange(p, std::memory_order_acq_rel);
        }
        
        Impl* operator->() const {
            return get();
        }
        
        explicit operator bool() const {
            return get() != nullptr;
        }
        
        Impl& operator*() const {
            return *get();
        }
        
        ~AtomicImplPtr() {
            delete get();
        }

    private:
        std::atomic<Impl*> implPtr;
        friend class Brain;
    };
    
private:
    // Extract step responsibilities into focused methods to reduce cyclomatic complexity
    void processDelayedSpikes();
    void updateNeuronDynamics();
    void detectAndScheduleSpikes();
    void updateWorkingMemory();
    void applyNeuromodulationEffects();
    void applyPlasticityRules();
    void updateEpisodicMemory();
    void updatePredictionSystem();
    void updateAttentionSystem();
    void updateConceptFormation();
    void applyStructuralPlasticity();
    void replayImportantMemories();
    void applyDevelopmentEffects();
    void consolidateMemories();
    void manageCheckpoints();
    
    // Enhanced pImpl with thread-safe access and memory management
    struct Impl;
    AtomicImplPtr pImpl;
    
    // Memory management helpers
    void cleanupResources() noexcept;
    bool validateResources() const noexcept;
    void reportMemoryStatus() const;
    
    // Configuration validation and resource preparation
    bool prepareResources();
    void initializeSubsystems();
};

} // namespace nlm
