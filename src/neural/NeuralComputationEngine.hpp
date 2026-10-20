#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../brain/Brain.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

class NeuralComputationEngine {
public:
    NeuralComputationEngine(std::shared_ptr<Config> config);
    ~NeuralComputationEngine();
    
    // Initialize neural computation components
    bool initialize(Brain* brain);
    
    // Main computation phase
    void update(Timestamp time, TimestepDuration timestep);
    
    // Reset all neural components
    void reset();
    
    // Get component access
    SpikeSystem* getSpikeSystem() { return spikeSystem.get(); }
    STDP* getSTDP() { return stdp.get(); }
    Hebbian* getHebbian() { return hebbian.get(); }
    StructuralPlasticity* getStructuralPlasticity() { return structuralPlasticity.get(); }
    
    // Statistics and monitoring
    size_t getTotalSpikeCount() const { return totalSpikesThisStep; }
    size_t getPendingSpikeCount() const;
    float getExcitatoryInhibitionRatio() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm