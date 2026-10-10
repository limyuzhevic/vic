#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <vector>

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

// Forward declarations for new brain architecture
class NeuralHardware;
class WorkingMemorySystem;
class PredictionSystem;
class CognitiveProcessor;
class NeuromodulationSystem;
class DevelopmentSystem;
class BrainOrchestrator;

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
    void applyNeuromodulation(const class Neuromodulator& modulator);
    
    // Development system access
    DevelopmentSystem* getDevelopmentSystem();
    const DevelopmentSystem* getDevelopmentSystem() const;
    
    // Memory system access
    NeuralWorkingMemory* getWorkingMemory();
    const NeuralWorkingMemory* getWorkingMemory() const;
    
    NeuralEpisodicMemory* getEpisodicMemory();
    const NeuralEpisodicMemory* getEpisodicMemory() const;
    
    NeuralAssociativeMemory* getAssociativeMemory();
    const NeuralAssociativeMemory* getAssociativeMemory() const;
    
    // Prediction system access
    PredictionSystem* getPredictionSystem();
    const PredictionSystem* getPredictionSystem() const;
    
    // Cognition system access
    NeuralPlanner* getPlanner();
    const NeuralPlanner* getPlanner() const;
    
    ConceptFormation* getConceptFormation();
    const ConceptFormation* getConceptFormation() const;
    
    AttentionalSelection* getAttention();
    const AttentionalSelection* getAttention() const;
    
    // Neuromodulation system access
    Dopamine* getDopamine();
    const Dopamine* getDopamine() const;
    
    Curiosity* getCuriosity();
    const Curiosity* getCuriosity() const;
    
    PredictionError* getPredictionError();
    const PredictionError* getPredictionError() const;
    
    Novelty* getNovelty();
    const Novelty* getNovelty() const;
    
    // System coordination
    void integrateAllSystems();
    void coordinatePredictionAndLearning();
    void updateDevelopmentBasedOnExperience();
    
    // State management
    void reset();
    void checkpoint(const std::string& filepath);
    bool loadCheckpoint(const std::string& filepath);
    
    // Configuration
    void configureWith(const std::shared_ptr<Config>& config);
    
    // Statistics and monitoring
    void logStatus() const;
    
    // Component access for testing and debugging
    NeuralHardware* getNeuralHardware();
    const NeuralHardware* getNeuralHardware() const;
    
    WorkingMemorySystem* getWorkingMemorySystem();
    const WorkingMemorySystem* getWorkingMemorySystem() const;
    
    PredictionSystem* getPredictionSystem();
    const PredictionSystem* getPredictionSystem() const;
    
    CognitiveProcessor* getCognitiveProcessor();
    const CognitiveProcessor* getCognitiveProcessor() const;
    
    NeuromodulationSystem* getNeuromodulationSystem();
    const NeuromodulationSystem* getNeuromodulationSystem() const;
    
    DevelopmentSystem* getDevelopmentSystem();
    const DevelopmentSystem* getDevelopmentSystem() const;
    
    // Architecture inspection
    static std::string getArchitectureDescription();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm