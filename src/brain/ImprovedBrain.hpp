// ImprovedBrain.hpp - Refactored brain implementation with better organization
#pragma once

#include "Brain.hpp"
#include "src/core/Types/Types.hpp"
#include "src/core/Config/Config.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Forward declarations
class NeuralRegion;
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
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
class PredictionError;
class Novelty;
class CheckpointManager;

class ImprovedBrain {
public:
    ImprovedBrain(std::shared_ptr<Config> config);
    ~ImprovedBrain();
    
    // Disable copying, enable moving
    ImprovedBrain(const ImprovedBrain&) = delete;
    ImprovedBrain& operator=(const ImprovedBrain&) = delete;
    ImprovedBrain(ImprovedBrain&&) noexcept;
    ImprovedBrain& operator=(ImprovedBrain&&) noexcept;
    
    // Core initialization and setup
    bool initialize() noexcept;
    void logStatus() const noexcept;
    
    // Simulation step with better error handling
    void step(SimulationStep currentStep) noexcept;
    void step(SimulationStep currentStep, Timestamp currentTime) noexcept;
    
    // I/O interfaces
    void receiveSensoryInput(const class SensoryInput& input) noexcept;
    void injectCurrent(NeuronId neuron, MembranePotential current) noexcept;
    void injectCurrentToNeurons(NeuronType type, MembranePotential current) noexcept;
    
    // Systems access
    SpikeSystem* getSpikeSystem() noexcept;
    const SpikeSystem* getSpikeSystem() const noexcept;
    STDP* getSTDP() noexcept;
    Hebbian* getHebbian() noexcept;
    StructuralPlasticity* getStructuralPlasticity() noexcept;
    
    // Memory systems
    NeuralWorkingMemory* getWorkingMemory() noexcept;
    NeuralEpisodicMemory* getEpisodicMemory() noexcept;
    NeuralAssociativeMemory* getAssociativeMemory() noexcept;
    
    // Prediction system
    PredictionSystem* getPredictionSystem() noexcept;
    
    // Cognition systems
    NeuralPlanner* getPlanner() noexcept;
    ConceptFormation* getConceptFormation() noexcept;
    AttentionalSelection* getAttention() noexcept;
    
    // Development and neuromodulation
    DevelopmentSystem* getDevelopmentSystem() noexcept;
    Dopamine* getDopamine() noexcept;
    Curiosity* getCuriosity() noexcept;
    PredictionError* getPredictionErrorSignal() noexcept;
    Novelty* getNovelty() noexcept;
    
    // Brain state management
    void reset() noexcept;
    bool save(const std::string& filepath) const noexcept;
    bool load(const std::string& filepath) noexcept;
    
    // Statistics and metrics
    size_t getTotalNeuronCount() const noexcept;
    size_t getTotalSynapseCount() const noexcept;
    size_t getActiveNeuronCount() const noexcept;
    size_t getFiringNeuronCount() const noexcept;
    size_t getTotalSpikeCount() const noexcept;
    size_t getPendingSpikeEventCount() const noexcept;
    float getAverageFiringRate() const noexcept;
    float getExcitationInhibitionRatio() const noexcept;
    
    // Region management
    RegionId addRegion(const std::string& name) noexcept;
    NeuralRegion* getRegion(RegionId id) noexcept;
    const NeuralRegion* getRegion(RegionId id) const noexcept;
    size_t getRegionCount() const noexcept;
    std::vector<RegionId> getRegionIds() const noexcept;
    
    // Action production
    std::unique_ptr<class Action> produceAction() noexcept;
    
    // Plasticity updates
    void applyNeuromodulation(const class Neuromodulator& signal) noexcept;
    void updatePlasticity() noexcept;
    void develop() noexcept;
    
    // Configuration access
    std::shared_ptr<const Config> getConfig() const noexcept;
    RandomGenerator* getRandomGenerator() noexcept;
    DevelopmentalStage getDevelopmentalStage() const noexcept;
    void setDevelopmentalStage(DevelopmentalStage stage) noexcept;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods for internal operations
    void initializeMemorySystems() noexcept;
    void initializeCognitionSystems() noexcept;
    void initializeNeuromodulationSystems() noexcept;
    void registerEventHandlers() noexcept;
    void configurePlasticitySystems() noexcept;
    bool validateInitializationState() const noexcept;
    void cleanupSystems() noexcept;
    
    // Legacy Brain interface compatibility
    friend class nlm::Brain;
    static ImprovedBrain& getCurrentBrain();
    ImprovedBrain& getLegacyBrain() { return *this; }
};