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
    
    // Spike system access with null validation
    SpikeSystem* getSpikeSystem() {
        if (!pImpl || !pImpl->spikeSystem) {
            NLM_LOG_WARNING("Brain: Null spike system accessed");
            return nullptr;
        }
        return pImpl->spikeSystem.get();
    }
    
    const SpikeSystem* getSpikeSystem() const {
        if (!pImpl || !pImpl->spikeSystem) {
            NLM_LOG_WARNING("Brain: Null spike system accessed (const)");
            return nullptr;
        }
        return pImpl->spikeSystem.get();
    }
    
    // Plasticity system access with null validation
    STDP* getSTDP() {
        if (!pImpl || !pImpl->stdp) {
            NLM_LOG_WARNING("Brain: Null STDP accessed");
            return nullptr;
        }
        return pImpl->stdp.get();
    }
    
    Hebbian* getHebbian() {
        if (!pImpl || !pImpl->hebbian) {
            NLM_LOG_WARNING("Brain: Null Hebbian accessed");
            return nullptr;
        }
        return pImpl->hebbian.get();
    }
    
    StructuralPlasticity* getStructuralPlasticity() {
        if (!pImpl || !pImpl->structuralPlasticity) {
            NLM_LOG_WARNING("Brain: Null structural plasticity accessed");
            return nullptr;
        }
        return pImpl->structuralPlasticity.get();
    }
    
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
    
    // Region management with null validation
    RegionId addRegion(const std::string& name = "");
    
    NeuralRegion* getRegion(RegionId id) {
        if (!pImpl) {
            NLM_LOG_WARNING("Brain: Null brain implementation in getRegion");
            return nullptr;
        }
        for (auto& region : pImpl->regions) {
            if (region->getId() == id) {
                return region.get();
            }
        }
        NLM_LOG_WARNING("Brain: Null region for ID " + std::to_string(id.index()));
        return nullptr;
    }
    
    const NeuralRegion* getRegion(RegionId id) const {
        if (!pImpl) {
            NLM_LOG_WARNING("Brain: Null brain implementation in getRegion (const)");
            return nullptr;
        }
        for (const auto& region : pImpl->regions) {
            if (region->getId() == id) {
                return region.get();
            }
        }
        NLM_LOG_WARNING("Brain: Null region for ID " + std::to_string(id.index()) + " (const)");
        return nullptr;
    }
    
    size_t getRegionCount() const {
        if (!pImpl) {
            NLM_LOG_WARNING("Brain: Null brain implementation in getRegionCount");
            return 0;
        }
        return pImpl->regions.size();
    }
    
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const {
        if (!pImpl) {
            NLM_LOG_WARNING("Brain: Null brain implementation in getRegions");
            static const std::vector<std::unique_ptr<NeuralRegion>> empty;
            return empty;
        }
        return pImpl->regions;
    }
    
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
    NeuralWorkingMemory* getWorkingMemory() {
        if (!pImpl || !pImpl->workingMemory) {
            NLM_LOG_WARNING("Brain: Null working memory accessed");
            return nullptr;
        }
        return pImpl->workingMemory.get();
    }
    
    // Episodic memory - experience storage
    NeuralEpisodicMemory* getEpisodicMemory() {
        if (!pImpl || !pImpl->episodicMemory) {
            NLM_LOG_WARNING("Brain: Null episodic memory accessed");
            return nullptr;
        }
        return pImpl->episodicMemory.get();
    }
    
    // Associative memory - pattern associations
    NeuralAssociativeMemory* getAssociativeMemory() {
        if (!pImpl || !pImpl->associativeMemory) {
            NLM_LOG_WARNING("Brain: Null associative memory accessed");
            return nullptr;
        }
        return pImpl->associativeMemory.get();
    }
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    PredictionSystem* getPredictionSystem() {
        if (!pImpl || !pImpl->predictionSystem) {
            NLM_LOG_WARNING("Brain: Null prediction system accessed");
            return nullptr;
        }
        return pImpl->predictionSystem.get();
    }
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    NeuralPlanner* getPlanner() {
        if (!pImpl || !pImpl->planner) {
            NLM_LOG_WARNING("Brain: Null planner accessed");
            return nullptr;
        }
        return pImpl->planner.get();
    }
    
    // Concept formation for pattern discovery
    ConceptFormation* getConceptFormation() {
        if (!pImpl || !pImpl->conceptFormation) {
            NLM_LOG_WARNING("Brain: Null concept formation accessed");
            return nullptr;
        }
        return pImpl->conceptFormation.get();
    }
    
    // Attentional selection for focus
    AttentionalSelection* getAttention() {
        if (!pImpl || !pImpl->attention) {
            NLM_LOG_WARNING("Brain: Null attention system accessed");
            return nullptr;
        }
        return pImpl->attention.get();
    }
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    // Development system
    DevelopmentSystem* getDevelopmentSystem() {
        if (!pImpl || !pImpl->developmentSystem) {
            NLM_LOG_WARNING("Brain: Null development system accessed");
            return nullptr;
        }
        return pImpl->developmentSystem.get();
    }
    
    DevelopmentalStage getDevelopmentalStage() const {
        if (!pImpl) {
            NLM_LOG_WARNING("Brain: Null brain implementation in getDevelopmentalStage");
            return DevelopmentalStage::Initial;
        }
        return pImpl->developmentalStage;
    }
    
    void setDevelopmentalStage(DevelopmentalStage stage) {
        if (!pImpl) {
            NLM_LOG_WARNING("Brain: Null brain implementation in setDevelopmentalStage");
            return;
        }
        pImpl->developmentalStage = stage;
    }
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    Dopamine* getDopamine() {
        if (!pImpl || !pImpl->dopamine) {
            NLM_LOG_WARNING("Brain: Null dopamine accessed");
            return nullptr;
        }
        return pImpl->dopamine.get();
    }
    
    // Curiosity - exploration motivation
    Curiosity* getCuriosity() {
        if (!pImpl || !pImpl->curiosity) {
            NLM_LOG_WARNING("Brain: Null curiosity accessed");
            return nullptr;
        }
        return pImpl->curiosity.get();
    }
    
    // Novelty - novelty detection
    Novelty* getNovelty() {
        if (!pImpl || !pImpl->novelty) {
            NLM_LOG_WARNING("Brain: Null novelty accessed");
            return nullptr;
        }
        return pImpl->novelty.get();
    }
    
    // Prediction error signal
    PredictionError* getPredictionErrorSignal() {
        if (!pImpl || !pImpl->predictionError) {
            NLM_LOG_WARNING("Brain: Null prediction error accessed");
            return nullptr;
        }
        return pImpl->predictionError.get();
    }
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const {
        if (!pImpl || !pImpl->config) {
            NLM_LOG_WARNING("Brain: Null config accessed");
            return nullptr;
        }
        return pImpl->config;
    }
    
    // Get random generator
    RandomGenerator* getRandomGenerator() {
        if (!pImpl || !pImpl->rng) {
            NLM_LOG_WARNING("Brain: Null random generator accessed");
            return nullptr;
        }
        return pImpl->rng.get();
    }
    
    // Logging
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
