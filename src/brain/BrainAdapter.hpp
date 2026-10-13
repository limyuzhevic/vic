#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../interfaces/IBrainInterface.hpp"
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

// BrainAdapter: Implementation of IBrainInterface
// Bridges the original Brain implementation to the interface-based architecture
class BrainAdapter : public IBrainInterface {
public:
    // Create brain with configuration
    explicit BrainAdapter(std::shared_ptr<Config> config);
    
    ~BrainAdapter() override;
    
    // Disable copying, enable moving
    BrainAdapter(const BrainAdapter&) = delete;
    BrainAdapter& operator=(const BrainAdapter&) = delete;
    BrainAdapter(BrainAdapter&&) noexcept = default;
    BrainAdapter& operator=(BrainAdapter&&) noexcept = default;
    
    // IBrainInterface implementations
    bool initialize() override;
    void step(SimulationStep currentStep) override;
    void step(SimulationStep currentStep, Timestamp currentTime) override;
    void reset() override;
    
    RegionId addRegion(const std::string& name) override;
    NeuralRegion* getRegion(RegionId id) override;
    const NeuralRegion* getRegion(RegionId id) const override;
    size_t getRegionCount() const override;
    std::vector<RegionId> getRegionIds() const override;
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const override;
    
    void receiveSensoryInput(const SensoryInput& input) override;
    void injectCurrent(NeuronId neuron, MembranePotential current) override;
    void injectCurrentToNeurons(NeuronType type, MembranePotential current) override;
    std::unique_ptr<Action> produceAction() override;
    
    size_t getTotalNeuronCount() const override;
    size_t getTotalSynapseCount() const override;
    size_t getActiveNeuronCount() const override;
    size_t getFiringNeuronCount() const override;
    float getAverageFiringRate() const override;
    float getExcitationInhibitionRatio() const override;
    size_t getTotalSpikeCount() const override;
    size_t getPendingSpikeEventCount() const override;
    
    // Memory system access (via interfaces)
    IMemorySystem* getWorkingMemory() override;
    IMemorySystem* getEpisodicMemory() override;
    IMemorySystem* getAssociativeMemory() override;
    
    // Prediction system
    IPredictionSystem* getPredictionSystem() override;
    
    // Cognition systems
    ICognitionSystem* getPlanner() override;
    ICognitionSystem* getConceptFormation() override;
    ICognitionSystem* getAttention() override;
    
    // Development system
    IDevelopmentSystem* getDevelopmentSystem() override;
    DevelopmentalStage getDevelopmentalStage() const override;
    void setDevelopmentalStage(DevelopmentalStage stage) override;
    
    // Neuromodulation systems
    INeuromodulationSystem* getDopamine() override;
    INeuromodulationSystem* getCuriosity() override;
    INeuromodulationSystem* getNovelty() override;
    INeuromodulationSystem* getPredictionErrorSignal() override;
    
    // Plasticity and dynamics systems
    IDynamicsSystem* getSpikeSystem() override;
    IDynamicsSystem* getSTDP() override;
    IDynamicsSystem* getHebbian() override;
    IPlasticitySystem* getStructuralPlasticity() override;
    
    // Configuration and random number generation
    std::shared_ptr<const Config> getConfig() const override;
    RandomGenerator* getRandomGenerator() override;
    
    // State management
    bool save(const std::string& filepath) const override;
    bool load(const std::string& filepath) override;
    
    // Logging and status
    void logStatus() const override;
    void applyNeuromodulation(const Neuromodulator& signal) override;
    void updatePlasticity() override;
    void develop() override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm