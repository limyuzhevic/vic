#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../plasticity/PlasticityRule.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointManager.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <atomic>

namespace nlm {

class Brain {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Add private helper methods for better encapsulation
    class BrainInitializer;
    class BrainStepProcessor;
    class BrainSystemManager;
    class BrainCheckpointHandler;
    
    // Private constructor for internal use
    explicit Brain(std::unique_ptr<Impl> impl) : pImpl(std::move(impl)) {}
    
    friend class BrainInitializer;
    friend class BrainStepProcessor;
    friend class BrainSystemManager;
    friend class BrainCheckpointHandler;
    
public:
    // Public constructor
    explicit Brain(std::shared_ptr<Config> config);
    
    // Disable copying
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    
    // Enable moving
    Brain(Brain&&) noexcept = default;
    Brain& operator=(Brain&&) noexcept = default;
    
    ~Brain();
    
    // Main interface
    bool initialize();
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    void receiveSensoryInput(const SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    std::unique_ptr<Action> produceAction();
    void reset();
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // System access methods
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics and information
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    float getTotalSpikeCount() const;  // Overloaded for compatibility
    
    // Memory systems
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Prediction system
    PredictionSystem* getPredictionSystem();
    
    // Cognition systems
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    // Development system
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Neuromodulation systems
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connections
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Configuration and utilities
    std::shared_ptr<const Config> getConfig() const;
    RandomGenerator* getRandomGenerator();
    SimulationClock* getClock();
    Logger* getLogger();
    
    // Subsystem control
    void enableSystem(const std::string& systemName, bool enable);
    bool isSystemEnabled(const std::string& systemName) const;
    
    // Validation and health checks
    bool validateSystemState() const;
    std::string getSystemStatus() const;
    
    // Performance monitoring
    double getPerformanceMetrics() const;
    void logPerformanceStats() const;
    
    // Development and debugging
    void debugSystem() const;
    void setDebugLevel(int level);
    int getDebugLevel() const;
};

} // namespace nlm
