// Brain interface implementation for better modularity
// Brain facade implementation for improved modularity
#pragma once

#include "IBrainInterface.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "../system/BrainFacade.hpp"
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <cstring>
#include <sstream>

namespace nlm {

class Brain : public IBrainInterface {
public:
    explicit Brain(std::shared_ptr<Config> config);
    ~Brain() override;
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    bool initialize() override;
    
    // Main simulation step
    void step(SimulationStep currentStep) override;
    void step(SimulationStep currentStep, Timestamp currentTime) override;
    
    // Receive sensory input from environment
    void receiveSensoryInput(const class SensoryInput& input) override;
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current) override;
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current) override;
    
    // Spike system access
    SpikeSystem* getSpikeSystem() override;
    const SpikeSystem* getSpikeSystem() const override;
    
    // Plasticity system access
    STDP* getSTDP() override;
    Hebbian* getHebbian() override;
    StructuralPlasticity* getStructuralPlasticity() override;
    
    // Statistics
    float getExcitationInhibitionRatio() const override;
    size_t getTotalSpikeCount() const override;
    size_t getPendingSpikeEventCount() const override;
    
    // Produce motor/action output based on motor neuron activity
    std::unique_ptr<class Action> produceAction() override;
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const class Neuromodulator& signal) override;
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity() override;
    
    // Apply developmental changes (called automatically in step)
    void develop() override;
    
    // Reset brain state
    void reset() override;
    
    // Save brain state to file (checkpointing)
    bool save(const std::string& filepath) const override;
    
    // Load brain state from file
    bool load(const std::string& filepath) override;
    
    // Region management
    RegionId addRegion(const std::string& name = "") override;
    NeuralRegion* getRegion(RegionId id) override;
    const NeuralRegion* getRegion(RegionId id) const override;
    size_t getRegionCount() const override;
    std::vector<RegionId> getRegionIds() const override;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const override;
    
    // Inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1) override;
    void removeInterRegionConnection(RegionId source, RegionId target) override;
    
    // Global statistics
    size_t getTotalNeuronCount() const override;
    size_t getTotalSynapseCount() const override;
    size_t getActiveNeuronCount() const override;
    size_t getFiringNeuronCount() const override;
    float getAverageFiringRate() const override;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    NeuralWorkingMemory* getWorkingMemory() override;
    
    // Episodic memory - experience storage
    NeuralEpisodicMemory* getEpisodicMemory() override;
    
    // Associative memory - pattern associations
    NeuralAssociativeMemory* getAssociativeMemory() override;
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    PredictionSystem* getPredictionSystem() override;
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    NeuralPlanner* getPlanner() override;
    
    // Concept formation for pattern discovery
    ConceptFormation* getConceptFormation() override;
    
    // Attentional selection for focus
    AttentionalSelection* getAttention() override;
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem() override;
    DevelopmentalStage getDevelopmentalStage() const override;
    void setDevelopmentalStage(DevelopmentalStage stage) override;
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    Dopamine* getDopamine() override;
    
    // Curiosity - exploration motivation
    Curiosity* getCuriosity() override;
    
    // Novelty - novelty detection
    Novelty* getNovelty() override;
    
    // Prediction error signal
    PredictionError* getPredictionErrorSignal() override;
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const override;
    
    // Get random generator
    RandomGenerator* getRandomGenerator() override;
    
    // Learned preference trait (Phase 5): a single scalar, -1..1,
    float getLearnedPreference() const override;
    void setLearnedPreference(float preference) override;
    
    // Logging
    void logStatus() const override;
    
    // === IBrainInterface Implementation ===
    
    uint64_t getNeuronCount() const override { return getTotalNeuronCount(); }
    uint64_t getSynapseCount() const override { return getTotalSynapseCount(); }
    uint64_t getRegionCount() const override { return getRegionCount(); }
    SimulationStep getCurrentStep() const override { return pImpl->currentStep; }
    Timestamp getCurrentTime() const override { return pImpl->currentTime; }
    
    void registerForSpikes(std::function<void(const DetailedSpikeEvent&)> handler) override;
    void registerForDelayedSpikes(std::function<void(const DelayedSpikeEvent&)> handler) override;
    
    void advanceStep(SimulationStep step, Timestamp time) override;
    
    void notifyNeuronFiring(const NeuronId& neuron) override;
    void notifySynapseUpdated(const SynapseId& synapse, float oldWeight, float newWeight) override;
    void notifyMemoryUpdated(const MemoryEvent& event) override;
    
    // === Facade Pattern Support ===
    
    // Access to system managers for advanced users
    MemorySystemManager* getMemorySystemManager() { return memorySystemManager_.get(); }
    NeuromodulationSystemManager* getNeuromodulationSystemManager() { return neuromodulationSystemManager_.get(); }
    CognitionSystemManager* getCognitionSystemManager() { return cognitionSystemManager_.get(); }
    PlasticitySystemManager* getPlasticitySystemManager() { return plasticitySystemManager_.get(); }
    DevelopmentSystemManager* getDevelopmentSystemManager() { return developmentSystemManager_.get(); }
    
    // Event system access
    EventBus& getEventBus() { return EventBus::getInstance(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // System managers for advanced access and control
    std::unique_ptr<MemorySystemManager> memorySystemManager_;
    std::unique_ptr<NeuromodulationSystemManager> neuromodulationSystemManager_;
    std::unique_ptr<CognitionSystemManager> cognitionSystemManager_;
    std::unique_ptr<PlasticitySystemManager> plasticitySystemManager_;
    std::unique_ptr<DevelopmentSystemManager> developmentSystemManager_;
};

} // namespace nlm
