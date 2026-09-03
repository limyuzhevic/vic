#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
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
// PLACEHOLDER - Phase 2 will implement real neural computation

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
    // TODO PHASE 2: Implement real sensory processing
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Produce motor/action output
    // TODO PHASE 2: Implement real action selection
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    // TODO PHASE 2: Implement real neuromodulation
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules
    // TODO PHASE 2: Implement real plasticity
    void updatePlasticity();
    
    // Apply developmental changes
    // TODO PHASE 2: Implement real development
    void develop();
    
    // Reset brain state
    void reset();
    
    // Save brain state to file
    // TODO PHASE 2: Implement checkpointing
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    // TODO PHASE 2: Implement checkpoint loading
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
    
    // Memory systems
    // TODO PHASE 2: Implement real memory systems
    class WorkingMemory* getWorkingMemory();
    class EpisodicMemory* getEpisodicMemory();
    class SemanticMemory* getSemanticMemory();
    class ProceduralMemory* getProceduralMemory();
    
    // Development system
    // TODO PHASE 2: Implement real development
    class DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Neuromodulation system
    // TODO PHASE 2: Implement real neuromodulation
    class Neuromodulator* getNeuromodulator();
    
    // Prediction system
    // TODO PHASE 2: Implement real prediction
    class PredictionSystem* getPredictionSystem();
    
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
