#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Forward declarations
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

// BrainCore: Basic brain functionality
// Handles: configuration, region management, neuron/synapse creation,
// basic statistics, and memory allocation
class BrainCore {
public:
    BrainCore(std::shared_ptr<Config> config, std::unique_ptr<RandomGenerator> rng);
    
    // Initialize brain with basic configuration
    bool initialize();
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Global statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    float getExcitationInhibitionRatio() const;
    
    // Configuration access
    std::shared_ptr<const Config> getConfig() const;
    RandomGenerator* getRandomGenerator();
    const RandomGenerator* getRandomGenerator() const;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Reset core state
    void reset();
    
    // Setters for key parameters
    void setNeuronCount(size_t count);
    void setRegionCount(size_t count);
    void setConnectionProbability(float prob);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm