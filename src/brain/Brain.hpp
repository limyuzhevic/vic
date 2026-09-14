#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iostream>
#include <vector>
#include <memory>
#include <string>

// Brain: The central artificial brain with integrated cognitive systems
// Implements neural dynamics, memory, plasticity, and cognition
class Brain {
public:
    Brain();
    ~Brain();
    
    // Initialize brain with configuration
    void initialize(Config config);
    
    // Main simulation step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Reset brain to initial state
    void reset();
    
    // Save/load brain state
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Getters for integrated systems
    DevelopmentSystem* getDevelopmentSystem();
    Neuromodulator* getNeuromodulator();
    Curiosity* getCuriosity();
    PredictionError* getPredictionError();
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    SemanticMemory* getSemanticMemory();
    ProceduralMemory* getProceduralMemory();
    AssociativeMemory* getAssociativeMemory();
    PredictionSystem* getPredictionSystem();
    NeuralPlanner* getNeuralPlanner();
    ConceptFormation* getConceptFormation();
    
    // Configuration and state access
    void setConfig(const Config& config);
    const Config& getConfig() const;
    
    SimulationStep getCurrentStep() const;
    Timestamp getCurrentTime() const;
    DevelopmentalStage getDevelopmentalStage() const;
    double getDevelopmentalAge() const;
    
    // Neural substrate access
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    std::vector<std::unique_ptr<NeuralRegion>>& getRegions();
    
    // Statistics and debugging
    void collectStatistics();
    void generateReport();
    std::string exportState() const;
    void importState(const std::string& stateData);
    
    // Cleanup resources
    void cleanup();
    
    // Memory replay integration methods
    void replayMemory(const std::vector<size_t>& episodeIds, ReplayMode mode);
    void replayForConsolidation(const std::vector<size_t>& episodeIds);
    void replayDuringSleep(float startTime, float duration, SleepPhase phase);
    void injectCurrentToNeurons(NeuronType neuronType, float current);
    void strengthenConnections(NeuronId source, const std::vector<float>& pattern, float delta);
    void activatePattern(const std::vector<float>& pattern, float strength);
    void updateWorkingMemoryIntegration();
    std::vector<const EpisodicMemoryItem*> getRecentEpisodesForReplay(size_t count) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Memory replay type definitions for brain integration
enum class ReplayMode {
    LTP_CONSOLIDATION,     // Long-term potentiation during deep sleep
    PATTERN_COMPLETION,   // Pattern completion during wakefulness
    SLEEP_REPLAY,         // Coordinated replay during rest (hippocampal-neocortical dialogue)
    WORKING_MEMORY_REPLAY // Integration with working memory
};

enum class ReplaySelectionStrategy {
    RECENCY_PRIORITY,     // Prioritize recent memories
    IMPORTANCE_PRIORITY,  // Prioritize important/rewarded memories
    NOVELTY_DRIVEN,       // Prioritize novel experiences
    BALANCED             // Balance between all factors
};

enum class SleepPhase {
    DEEP_SLEEP,    // Slow wave sleep with high-amplitude replay
    LIGHT_SLEEP,   // Intermediate sleep with moderate replay
    REM_SLEEP      // Paradoxical sleep with emotional memory processing
};

} // namespace nlm

class DevelopmentSystem;
class Neuromodulator;
class Curiosity;
class PredictionError;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class SemanticMemory;
class ProceduralMemory;
class AssociativeMemory;
class NeuralPlanner;
class ConceptFormation;
class PredictionSystem;
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;

struct Brain::Impl;