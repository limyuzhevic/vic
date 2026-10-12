# NLM (熙然) API Documentation
# Comprehensive API Reference for Phase 6 - Final Integration

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project that creates neural systems capable of learning, memory, and cognition through interaction with an environment. This document provides comprehensive API documentation for all components of the Phase 6 integrated system.

## Table of Contents

1. [Introduction](#introduction)
2. [Core Architecture](#core-architecture)
3. [Brain System API](#brain-system-api)
4. [Memory Systems API](#memory-systems-api)
5. [Neuromodulation Systems API](#neuromodulation-systems-api)
6. [Prediction System API](#prediction-system-api)
7. [Cognition Systems API](#cognition-systems-api)
8. [Plugin System API](#plugin-system-api)
9. [Configuration System API](#configuration-system-api)
10. [Command Line Interface API](#command-line-interface-api)
11. [Performance Optimization API](#performance-optimization-api)
12. [Extension Points and Customization](#extension-points-and-customization)
13. [Migration Guide](#migration-guide)
14. [Examples and Tutorials](#examples-and-tutorials)

---

## Introduction

### Project Purpose

NLM is designed to investigate computational brain-like systems without claiming to reproduce biological brains. Current limitations include simplified neuron models and lack of detailed biological fidelity, but the system successfully demonstrates coherent brain loop functionality.

### Key Features

- **Integrated Brain Loop**: All systems (memory, neuromodulation, prediction, cognition) work together
- **Experience-Based Learning**: Continual adaptation through environmental interaction
- **Multi-Modal Processing**: Sensory input, working memory, attention, action selection
- **Developmental Stages**: Age-based plasticity and capability modulation
- **Persistence**: Checkpoint save/load and replay capabilities

### System Components

1. **Core Brain**: LIF neurons, spikes, plasticity rules, structural development
2. **Agent Integration**: Simple world, sensory processing, motor output
3. **Memory Systems**: Working memory (persistent activity), episodic memory (experience encoding), associative memory (pattern associations)
4. **Neuromodulation**: Dopamine, curiosity, novelty, prediction error
5. **Prediction**: Forward models, prediction error signals, confidence tracking
6. **Cognition**: Neural planning, concept formation, attention, self-model, social learning
7. **Development**: Age-based plasticity, structural changes, stage transitions

---

## Core Architecture

### File Structure

```
NLM/
├── src/
│   ├── brain/                    # Core brain components
│   │   ├── Brain.cpp, Brain.hpp    # Main brain interface
│   │   ├── NeuralRegion.cpp, NeuralRegion.hpp  # Neural regions
│   │   └── ...                     # Other brain components
│   ├── memory/                   # Memory systems
│   │   ├── NeuralWorkingMemory.cpp/.hpp
│   │   ├── NeuralEpisodicMemory.cpp/.hpp
│   │   └── NeuralAssociativeMemory.cpp/.hpp
│   ├── neuromodulation/           # Neuromodulators
│   │   ├── Dopamine.cpp, Dopamine.hpp
│   │   ├── Curiosity.cpp, Curiosity.hpp
│   │   └── ...                     # Other neuromodulators
│   ├── prediction/               # Prediction systems
│   │   ├── PredictionSystem.cpp, PredictionSystem.hpp
│   │   └── PredictionError.cpp, PredictionError.hpp
│   ├── cognition/                # Cognitive mechanisms
│   │   ├── NeuralPlanner.cpp, NeuralPlanner.hpp
│   │   ├── ConceptFormation.cpp, ConceptFormation.hpp
│   │   ├── SelfModel.cpp, SelfModel.hpp
│   │   └── SocialLearning.cpp, SocialLearning.hpp
│   ├── sensory/                  # Sensory processing
│   │   ├── SensoryInput.cpp, SensoryInput.hpp
│   │   └── SensoryPercept.cpp, SensoryPercept.hpp
│   ├── motor/                    # Motor system
│   │   ├── Action.cpp, Action.hpp
│   │   └── MotorCommand.cpp, MotorCommand.hpp
│   ├── world/                    # Environment interface
│   │   ├── SimpleWorld.cpp, SimpleWorld.hpp
│   │   └── WorldInterface.cpp, WorldInterface.hpp
│   └── utils/                    # Utility systems
│       ├── ConfigValidator.hpp
│       ├── ActionSequenceUtils.hpp
│       ├── WorkingMemoryPerformance.hpp
│       └── PluginSystem.hpp
├── configs/                      # Configuration files
│   └── default.cfg               # Default configuration
├── docs/                         # Documentation
│   └── ...                       # Phase 6 audit and architecture docs
├── examples/                     # Usage examples
│   └── ...                       # Python and C++ examples
└── scripts/                      # Automation scripts
    ├── optimize_performance.sh
    └── ...                       # Build and test scripts
```

### Key Dependencies

- **C++17**: Modern C++ features and standard library
- **CMake 3.16+**: Build system
- **pybind11 ≥ 2.11**: Python bindings
- **scikit-build-core ≥ 0.5**: Python package building

---

## Brain System API

### Core Classes

#### Brain Class (`brain/Brain.hpp`)

**Primary Interface**: The main brain interface for neural simulation and control.

```cpp
class Brain {
public:
    // Construction and Lifecycle
    Brain(std::shared_ptr<Config> config);
    ~Brain();
    
    // Core Operations
    bool initialize();           // Initialize neural networks
    void step(SimulationStep step);  // Process one simulation step
    void reset();                 // Reset to initial state
    
    // Configuration Access
    std::shared_ptr<const Config> getConfig() const;
    void logStatus() const;       // Log current brain status
    
    // Neural Access
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    float getAverageFiringRate() const;
    float getExcitationInhibitionRatio() const;
    
    // Memory System Accessors
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Prediction System Accessors
    PredictionSystem* getPredictionSystem();
    
    // Cognition System Accessors
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    // Neuromodulation System Accessors
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Plasticity System Accessors
    SpikeSystem* getSpikeSystem();
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Development System Accessors
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Agent Integration
    void receiveSensoryInput(const SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    std::unique_ptr<Action> produceAction();
    
    // Checkpoint Operations
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Statistics and Monitoring
    size_t getRegionCount() const;
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    size_t getPendingSpikeEventCount() const;
    size_t getTotalSpikeCount() const;
    Timestamp getCurrentTime() const;
    SimulationStep getCurrentStep() const;
    
    // Specialized Operations
    void applyNeuromodulation(const Neuromodulator& signal);
    void updatePlasticity();
    void develop();
    void reset();
};
```

#### NeuralRegion Class (`brain/NeuralRegion.hpp`)

**Description**: Container for neurons and their connections.

```cpp
class NeuralRegion {
public:
    NeuralRegion(RegionId id, const std::string& name);
    ~NeuralRegion();
    
    // Population Management
    PopulationId addPopulation(size_t neuronCount, NeuronType type);
    NeuralPopulation* getPopulation(PopulationId id);
    const NeuralPopulation* getPopulation(PopulationId id) const;
    
    // Neuron Access
    std::vector<Neuron*> getAllNeurons();
    const std::vector<Neuron*> getAllNeurons() const;
    
    // Statistics
    size_t getPopulationCount() const;
    size_t getTotalNeuronCount() const;
    size_t getSynapseCount() const;
    float getAverageSynapticWeight() const;
    float getAverageFiringRate() const;
    size_t getActiveNeuronCount() const;
    
    // Connectivity
    void initializeRandomConnectivity(RandomGenerator& rng, 
                                       float connectionProbability,
                                       float excitationWeight = 0.2f,
                                       float inhibitionWeight = 0.1f);
    
    // State Management
    void reset();
    
    // Inter-region Connections
    void addInterRegionConnection(RegionId target, float weight, Delay delay);
    void removeInterRegionConnection(RegionId target);
    
    // Identification
    RegionId getId() const;
    std::string getName() const;
};
```

### Core Types and Enumerations

#### Neuron Types

```cpp
enum class NeuronType {
    Sensory,      // Receives external input
    Internal,     // Processes internal signals
    Motor,        // Produces output signals
    BasalGanglia, // Action selection
    Cerebellum,    // Timing and coordination
    Thalamus,     // Sensory relay
    Cortex,       // Higher processing
    Special       // Specialized functions
};
```

#### Action Types

```cpp
enum class ActionType {
    Wait,           // Do nothing
    MoveForward,    // Move in forward direction
    MoveBackward,   // Move in backward direction
    TurnLeft,       // Rotate left
    TurnRight,      // Rotate right
    Interact,       // Manipulate environment
    Attack,         // Aggressive action
    Escape,         // Flee behavior
    Explore,        // Search new areas
    WaitForInput,   // Await external signal
    Custom         // User-defined action
};
```

#### Developmental Stages

```cpp
enum class DevelopmentalStage {
    Initial,        // Primitive capabilities
    CriticalPeriod, // High plasticity, rapid learning
    Maturation,     // Fine-tuning and specialization
    Adult,          // Stable, refined performance
    Decline,        // Age-related changes
};
```

#### Firing States

```cpp
enum class FiringState {
    Resting,        // No activity
    Depolarizing,   // Building potential
    Firing,         // Active spike generation
    Refractory,     // Recovery period after spike
};
```

#### Synapse Types

```cpp
enum class SynapseType {
    Excitatory,     // Positive current injection
    Inhibitory,     // Negative current injection
    Modulatory,     // Neuromodulatory effects
    Gap,            // Electrical coupling
    Neuromod,       // Chemical modulation
};
```

---

## Memory Systems API

### NeuralWorkingMemory Class (`memory/NeuralWorkingMemory.hpp`)

**Description**: Persistent activity memory system with competitive dynamics.

```cpp
class NeuralWorkingMemory {
public:
    NeuralWorkingMemory();
    ~NeuralWorkingMemory();
    
    // Lifecycle
    void initialize(Brain* brain);
    void clear();
    
    // Memory Operations
    void store(const std::vector<float>& pattern, float strength);
    void storeToNeuron(NeuronId neuron, float activation);
    std::vector<float> retrieve() const;
    
    // Neuron Management
    bool contains(NeuronId neuron) const;
    float getNeuronActivation(NeuronId neuron) const;
    std::vector<NeuronId> getMemoryNeurons() const;
    
    // Memory Dynamics
    void update(TimestepDuration dt);
    void strengthenMemory(float factor);
    
    // Competition and Selection
    bool isWinning(NeuronId neuron) const;
    float getMemoryActivity() const;
    std::vector<NeuronId> getWinners() const;
    void runCompetition();
    
    // Capacity Control
    void setCapacity(size_t capacity);
    size_t getActiveTraces() const;
    
    // Performance
    size_t getOperationCount() const;
    float getLookupEfficiency() const;
};
```

### NeuralEpisodicMemory Class (`memory/NeuralEpisodicMemory.hpp`)

**Description**: Experience encoding and replay system for temporal memory.

```cpp
class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory();
    ~NeuralEpisodicMemory();
    
    // Lifecycle
    void initialize(Brain* brain);
    void clear();
    
    // Experience Storage
    void storeEpisode(const EpisodicMemoryItem& episode);
    void replayEpisode(const EpisodicMemoryItem* episode);
    void replaySequence(const std::vector<size_t>& episodeIds);
    
    // Experience Retrieval
    std::vector<const EpisodicMemoryItem*> retrieveSimilar(
        const std::vector<float>& sensoryPattern,
        size_t maxResults = 10) const;
    
    std::vector<const EpisodicMemoryItem*> retrieveTemporal(
        SimulationStep startTime,
        SimulationStep endTime,
        size_t maxResults = 10) const;
    
    std::vector<const EpisodicMemoryItem*> retrieveByLocation(
        float x, float y, float radius,
        size_t maxResults = 10) const;
    
    std::vector<const EpisodicMemoryItem*> retrieveAfterAction(
        ActionType action,
        size_t maxResults = 10) const;
    
    // Management
    const EpisodicMemoryItem* getEpisode(size_t index) const;
    std::vector<const EpisodicMemoryItem*> getRecentEpisodes(size_t count) const;
    size_t getEpisodeCount() const;
    size_t getMaxEpisodes() const;
    void setMaxEpisodes(size_t max);
    
    // Consolidation and Maintenance
    void consolidate(float relevanceThreshold);
    void updateRelevance(SimulationStep episodeId, float relevanceDelta);
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const;
    
    // Performance
    float getAverageReward() const;
};
```

### NeuralAssociativeMemory Class (`memory/NeuralAssociativeMemory.hpp`)

**Description**: Pattern association and spreading activation system.

```cpp
class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Lifecycle
    void initialize(Brain* brain);
    void clear();
    
    // Pattern Association
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength);
    
    void associateFromExperience(const EpisodicMemoryItem& episode);
    
    // Pattern Retrieval
    std::vector<std::vector<float>> retrieve(
        const std::vector<float>& queryPattern,
        size_t maxResults = 10) const;
    
    float getAssociationStrength(const std::vector<float>& patternA,
                                const std::vector<float>& patternB) const;
    
    void updateAssociation(const std::vector<float>& patternA,
                          const std::vector<float>& patternB,
                          float delta);
    
    // Spreading Activation
    std::vector<std::vector<float>> spreadActivation(
        const std::vector<float>& cuePattern,
        size_t steps = 5) const;
    
    // Pattern Management
    void addPatternNeuron(const std::vector<float>& pattern);
    float computeSimilarity(const std::vector<float>& a,
                           const std::vector<float>& b) const;
    
    // Statistics
    size_t getPatternCount() const;
    size_t getAssociationCount() const;
    float getAverageAssociationStrength() const;
};
```

#### EpisodicMemoryItem Structure

```cpp
struct EpisodicMemoryItem {
    SimulationStep timestamp;              // Time of episode
    std::vector<NeuronId> activeNeurons;    // Neurons that fired
    std::vector<float> neuronActivations;   // Activation levels
    float reward;                          // Associated reward
    std::vector<float> sensoryState;        // Sensory input
    std::vector<float> resultingSensoryState; // State after action
    ActionType action;                     // Action taken
    float age;                            // Episode age in steps
    float relevance;                      // Memory relevance score
    float x, y;                           // Spatial location
    
    EpisodicMemoryItem() : timestamp(0), reward(0.0f), age(0), relevance(0), x(0), y(0) {}
};
```

---

## Neuromodulation Systems API

### Neuromodulator Base Class (`neuromodulation/Neuromodulator.hpp`)

**Description**: Base class for all neuromodulation systems.

```cpp
class Neuromodulator {
public:
    Neuromodulator();
    virtual ~Neuromodulator();
    
    // Plugin Identification
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    
    // Plugin Lifecycle
    virtual bool initialize() = 0;
    virtual bool shutdown() = 0;
    virtual bool isInitialized() const = 0;
    
    // Plugin Functionality
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    
    // Configuration
    virtual bool loadConfiguration(const std::string& config) = 0;
    virtual std::string getConfiguration() const = 0;
    
    // Event Handling
    virtual void onEvent(const std::string& eventType, const std::string& eventData) = 0;
    
    // Extension Points
    virtual std::vector<std::string> getExtensionPoints() const = 0;
    virtual bool provideExtension(const std::string& extensionPoint, const std::string& extensionData) = 0;
};
```

### PluginManager Class (`utils/PluginSystem.hpp`)

**Description**: Core plugin management system for loading, initializing, and coordinating plugins.

```cpp
class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    // Plugin Registration
    void registerPluginFactory(std::unique_ptr<PluginFactory> factory);
    
    // Plugin Lifecycle
    bool loadPlugin(const std::string& pluginName);
    bool unloadPlugin(const std::string& pluginName);
    std::shared_ptr<PluginInterface> getPlugin(const std::string& pluginName);
    
    // Plugin Discovery
    std::vector<std::string> getAllPluginNames() const;
    std::vector<std::string> getAvailablePlugins() const;
    PluginMetadata getPluginMetadata(const std::string& pluginName) const;
    
    // System Management
    void updateAllPlugins(float deltaTime);
    void shutdownAllPlugins();
    bool isPluginLoaded(const std::string& pluginName) const;
};
```

---

## Configuration System API

### Config Class (`core/Config/Config.hpp`)

**Description**: Hierarchical configuration management system.

```cpp
class Config {
public:
    Config();
    virtual ~Config();
    
    // Configuration Loading
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;
    bool loadFromString(const std::string& configString);
    
    // Configuration Access
    template<typename T>
    bool set(const std::string& key, const T& value);
    
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    std::optional<T> getOr(const std::string& key, const T& defaultValue) const;
    
    // Configuration Types
    bool set(const std::string& key, const std::string& value);
    bool set(const std::string& key, int value);
    bool set(const std::string& key, float value);
    bool set(const std::string& key, double value);
    bool set(const std::string& key, bool value);
    bool set(const std::string& key, const std::vector<std::string>& values);
    
    // Configuration Validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    std::vector<std::string> getValidationWarnings() const;
    
    // Configuration Management
    bool remove(const std::string& key);
    bool hasKey(const std::string& key) const;
    std::vector<std::string> getAllKeys() const;
    size_t getKeyCount() const;
    
    // Configuration Inspection
    std::string getType(const std::string& key) const;
    std::string getValueString(const std::string& key) const;
    bool isValidType(const std::string& key, const std::string& type) const;
    
    // Configuration Templates
    static std::string getDefaultConfig();
    static bool validateConfig(const std::string& configString);
    static std::vector<std::string> getRequiredKeys();
    static std::vector<std::string> getOptionalKeys();
};
```

---

## Command Line Interface API

### nlm CLI (`bin/nlm`)

**Description**: Advanced command-line interface for NLM system management and administration.

#### Commands:

**nlm status**
```bash
nlm status
```
Show brain system status and diagnostics.

**nlm validate**
```bash
nlm validate
```
Validate configuration files and generate reports.

**nlm save <filepath>**
```bash
nlm save brain_state.nlm
```
Save brain state to file.

**nlm load <filepath>**
```bash
nlm load brain_state.nlm
```
Load brain state from file.

**nlm reset**
```bash
nlm reset
```
Reset brain to initial state.

**nlm config**
```bash
nlm config
```
Show current configuration.

**nlm stats**
```bash
nlm stats
```
Show detailed system statistics.

**nlm generate-config**
```bash
nlm generate-config
```
Generate default configuration file.

**nlm validate-config <file>**
```bash
nlm validate-config configs/my_config.cfg
```
Validate specific configuration file.

**nlm help**
```bash
nlm help
```
Show this help message.

**nlm version**
```bash
nlm version
```
Show version information.

#### Options:

**-v, --verbose**: Enable verbose output

**--log-file <file>**: Set log file (default: nlm_cli.log)

**--config-dir <dir>**: Set configuration directory (default: configs)

**--build-dir <dir>**: Set build directory (default: build)

---

## Performance Optimization API

### WorkingMemoryPerformance Utilities (`utils/WorkingMemoryPerformance.hpp`)

**Description**: Performance optimization utilities for neural working memory.

```cpp
namespace WorkingMemoryPerformance {
    // Optimized memory data structures
    struct OptimizedMemoryData {
        std::unordered_map<NeuronId, size_t> neuronToIndex;
        std::vector<NeuronId> memoryNeurons;
        std::vector<float> memoryActivations;
        std::vector<SimulationStep> memoryTimestamps;
        std::vector<NeuronId> winners;
        
        // Performance tracking
        mutable size_t findOperations;
        mutable size_t linearSearches;
        size_t totalOperations;
    };
    
    // O(1) lookup operations
    bool containsOptimized(const std::unordered_map<NeuronId, size_t>& neuronToIndex,
                          const NeuronId& neuron);
    
    float getNeuronActivationOptimized(const std::unordered_map<NeuronId, size_t>& neuronToIndex,
                                      const std::vector<NeuronId>& memoryNeurons,
                                      const std::vector<float>& memoryActivations,
                                      const NeuronId& neuron);
    
    // Batch operations
    void cleanupWeakTracesOptimized(std::vector<NeuronId>& memoryNeurons,
                                   std::vector<float>& memoryActivations,
                                   std::vector<SimulationStep>& memoryTimestamps,
                                   std::vector<NeuronId>& winners,
                                   float decayRate = 0.01f);
    
    // Performance monitoring
    class PerformanceBenchmark {
    public:
        struct BenchmarkResult {
            std::string operationName;
            uint64_t iterations;
            double totalTimeNs;
            double avgTimeNs;
            double opsPerSecond;
        };
        
        void startBenchmark(const std::string& operation);
        void endBenchmark();
        void incrementIterations();
        std::string generatePerformanceReport() const;
    };
    
    // Performance-aware containers
    template<typename T>
    class PerformanceVector {
        // Optimized memory management with chunks
    };
}
```

---

## Extension Points and Customization

### Plugin Development

#### Basic Plugin Template

```cpp
#include "utils/PluginSystem.hpp"

class CustomPlugin : public nlm::PluginTemplate<CustomPlugin> {
public:
    CustomPlugin(const std::string& name, const std::string& version, const std::string& description)
        : PluginTemplate(name, version, description) {}
    
protected:
    bool onInitialize() override {
        // Plugin initialization
        return true;
    }
    
    bool onShutdown() override {
        // Plugin shutdown
        return true;
    }
    
    void onUpdate(float deltaTime) override {
        // Plugin update logic
    }
    
    void onRender() override {
        // Plugin rendering
    }
    
    bool onLoadConfiguration(const std::string& config) override {
        // Parse configuration
        return true;
    }
    
    std::string onGetConfiguration() const override {
        return "plugin_config";
    }
    
    void onPluginEvent(const std::string& eventType, const std::string& eventData) override {
        // Handle plugin events
    }
    
    std::vector<std::string> onGetExtensionPoints() const override {
        return {"brain_extension", "memory_extension"};
    }
    
    bool onProvideExtension(const std::string& extensionPoint, 
                           const std::string& extensionData) override {
        // Provide extension to other plugins
        return true;
    }
};
```

---

## Migration Guide

### Configuration Updates

**Key Configuration Changes (Version 2.0.0)**:

1. **Required Keys** (now enforced):
   - `random_seed`
   - `simulation_timestep`
   - `neuron_count`
   - `region_count`
   - `connection_probability`

2. **New Optional Keys**:
   - `replay_interval`: Memory replay interval (default: 100)
   - `consolidation_interval`: Memory consolidation interval (default: 1000)
   - `environment_name`: World environment type (default: "GridWorld")
   - `visualization_enabled`: Enable visualization (default: false)

### Code Examples

#### Brain Usage

```cpp
// Initialize brain
auto config = std::make_shared<nlm::Config>("configs/default.cfg");
nlm::Brain brain(config);

if (!brain.initialize()) {
    // Handle initialization failure
}

// Run simulation
for (size_t step = 0; step < 1000; ++step) {
    brain.step(step);
}
```

#### Memory System Access

```cpp
// Access memory systems
auto workingMemory = brain->getWorkingMemory();
auto episodicMemory = brain->getEpisodicMemory();
auto associativeMemory = brain->getAssociativeMemory();

// Use memory systems
workingMemory->store(pattern, strength);
episodicMemory->storeEpisode(episode);
associativeMemory->associate(patternA, patternB, strength);
```

#### Configuration Management

```cpp
// Create and configure
nlm::Config config;
config.set("random_seed", 42);
config.set("simulation_timestep", 0.001f);

// Validate and save
nlm::ConfigValidator validator;
auto result = validator.validateConfig(std::make_shared<nlm::Config>(config));

if (!result.isValid) {
    // Handle validation errors
}

config.saveToFile("custom_config.cfg");
```

---

## References

1. **Phase 6 Final Audit**: `docs/PHASE6_FINAL_AUDIT.md`
2. **Architecture Documentation**: `docs/ARCHITECTURE.md`
3. **Science Documentation**: `docs/SCIENCE.md`
4. **Roadmap**: `docs/ROADMAP.md`
5. **Experiment Framework**: `docs/EXPERIMENTS.md`
6. **Phase 6 Results**: `docs/PHASE6_FINAL_RESULTS.md`

---

## Version Information

**Version**: 2.0.0 (Phase 6 Complete - Final Integration)

**Build Date**: October 12, 2026

**Authors**: NLM Research Team

**License**: MIT

**Status**: Production Ready (Research Use Only)

---

*End of Documentation*

This documentation provides comprehensive API reference for all components of the NLM Phase 6 integrated artificial brain system. The system is designed for research purposes and should not be used for safety-critical applications.*