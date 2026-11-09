#pragma once

namespace nlm {

// Forward declarations
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;
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
class NeuralRegion;

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

// Core brain components - forward declarations for specialization
class BrainCore {
public:
    virtual ~BrainCore() = default;
    virtual void initializeCore() = 0;
    virtual void processStep() = 0;
    virtual void applyPlasticity() = 0;
};

class BrainMemory {
public:
    virtual ~BrainMemory() = default;
    virtual void initializeMemory() = 0;
    virtual void updateMemory() = 0;
};

class BrainCognition {
public:
    virtual ~BrainCognition() = default;
    virtual void initializeCognition() = 0;
    virtual void updateCognition() = 0;
};

class BrainDevelopment {
public:
    virtual ~BrainDevelopment() = default;
    virtual void initializeDevelopment() = 0;
    virtual void updateDevelopment() = 0;
};

class BrainNeuromodulation {
public:
    virtual ~BrainNeuromodulation() = default;
    virtual void initializeNeuromodulation() = 0;
    virtual void updateNeuromodulation() = 0;
};

} // namespace nlm