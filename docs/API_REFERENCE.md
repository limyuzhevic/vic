# NLM API Reference

## Overview

This document provides a comprehensive reference for the NLM (熙然) API, including all public classes, functions, and methods available for working with the neural learning system.

## Core Types

### NeuronId, SynapseId, RegionId
Unique identifiers for neural components.

### NeuronType
Enumeration of neuron types:
- `NEURON_TYPE_EXCITATORY`: Excitatory neurons that promote firing
- `NEURON_TYPE_INHIBITORY`: Inhibitory neurons that suppress firing
- `NEURON_TYPE_SENSORY`: Sensory input neurons
- `NEURON_TYPE_MOTOR`: Motor output neurons

### DevelopmentalStage
Stages of brain development:
- `DEVELOPMENTAL_STAGE_INITIAL`: Embryonic stage with high plasticity
- `DEVELOPMENTAL_STAGE_CRITICAL`: Sensitive learning period
- `DEVELOPMENTAL_STAGE_MATURATION`: Development of adult properties
- `DEVELOPMENTAL_STAGE_ADULT`: Stable adult brain

## Brain Class

### Constructors/Destructors
```cpp
explicit Brain(std::shared_ptr<Config> config);
~Brain();
```

### Initialization
```cpp
bool initialize(); // Initializes brain with current configuration
```

### Core Simulation
```cpp
void step(SimulationStep currentStep);
void step(SimulationStep currentStep, Timestamp currentTime);
// Main simulation loop - advances neural dynamics
```

### Input/Output
```cpp
void receiveSensoryInput(const SensoryInput& input);
void injectCurrent(NeuronId neuron, MembranePotential current);
void injectCurrentToNeurons(NeuronType type, MembranePotential current);
std::unique_ptr<Action> produceAction();
```

### Plasticity and Development
```cpp
void updatePlasticity(); // Updates all plasticity rules
void develop();          // Applies developmental changes
void reset();            // Resets brain state
```

### State Management
```cpp
bool save(const std::string& filepath) const;
bool load(const std::string& filepath);
```

### Statistics
```cpp
float getExcitationInhibitionRatio() const;
size_t getTotalSpikeCount() const;
size_t getPendingSpikeEventCount() const;
size_t getTotalNeuronCount() const;
size_t getTotalSynapseCount() const;
size_t getActiveNeuronCount() const;
size_t getFiringNeuronCount() const;
float getAverageFiringRate() const;
```

### Memory Systems
```cpp
NeuralWorkingMemory* getWorkingMemory();           // Working memory for transient information
NeuralEpisodicMemory* getEpisodicMemory();        // Episodic memory for experience storage
NeuralAssociativeMemory* getAssociativeMemory();   // Associative memory for pattern associations
```

### Prediction System
```cpp
PredictionSystem* getPredictionSystem(); // Temporal sequence learning and prediction
```

### Cognition Systems
```cpp
NeuralPlanner* getPlanner();              // Action planning and evaluation
ConceptFormation* getConceptFormation();  // Pattern discovery and concept formation
AttentionalSelection* getAttention();     // Competitive attentional selection
```

### Development System
```cpp
DevelopmentSystem* getDevelopmentSystem();           // Developmental process management
DevelopmentalStage getDevelopmentalStage() const;   // Current developmental stage
developmentalStage getDevelopmentalStage() const;   // Current developmental stage
developmentalStage getDevelopmentalStage() const;   // Current developmental stage
developmentalStage getDevelopmental stage() const; // Current developmental stage
developmentalStage getDevelopment stage() const; // Current developmental stage

void setDevelopmentalStage(DevelopmentalStage stage);
```

### Neuromodulation Systems
```cpp
Dopamine* getDopamine();                     // Reward prediction error signals
Curiosity* getCuriosity();                   // Exploration motivation
Novelty* getNovelty();                        // Novelty detection
PredictionError* getPredictionErrorSignal(); // Prediction error computation
```

### Configuration and Random
```cpp
std::shared_ptr<const Config> getConfig() const;   // Access configuration
RandomGenerator* getRandomGenerator();            // Access random number generator
```

## AgentBrain Class

### Overview
AgentBrain connects the NLM brain to the external world, handling sensory transduction and motor decoding.

### Core Methods
```cpp
AgentBrain(std::shared_ptr<Brain> brain);
~AgentBrain();

void initialize(const SimpleWorld& world);
void processSensoryInput(const SensoryPercept& percept);
MotorCommand decodeMotorCommand();

void applyRewardModulation(float reward, float predictedReward);
void updateDevelopment(double timestep);

void reset();
```

### Configuration
```cpp
void enableRewardModulation(bool enable);
void enableStructuralPlasticity(bool enable);
void enableDevelopment(bool enable);
void enableCuriosity(bool enable);

bool isRewardModulationEnabled() const;
bool isStructuralPlasticityEnabled() const;
bool isDevelopmentEnabled() const;
bool isCuriosityEnabled() const;
```

### State Access
```cpp
float getNeuromodulationLevel() const;
float getCuriosityLevel() const;
float getNoveltyLevel() const;
float getPredictionError() const;
Brain* getBrain(); // Get underlying brain pointer
```

## Memory System Classes

### NeuralWorkingMemory
Persistent activity for maintaining information over time.

### NeuralEpisodicMemory
Storage of experiences as neural patterns with temporal indexing.

### NeuralAssociativeMemory
Experience-based associations between patterns.

## Prediction System

### NeuralPrediction
Temporal sequence learning and prediction of future states.

### ActionConsequencePredictor
Association of actions with their sensory consequences.

## Cognitive System Classes

### NeuralPlanner
Evaluates potential action sequences using learned predictions.

### ConceptFormation
Discovers patterns in experience and forms stable concept prototypes.

### AttentionalSelection
Competitive selection of sensory representations for processing.

## Development System

### DevelopmentSystem
Manages developmental stages and age-dependent plasticity.

### Synaptogenesis
Formation of new synaptic connections.

### Pruning
Removal of weak or unused synapses.

### Maturation
Development of adult-level neural properties.

## Neuromodulation Classes

### Dopamine
Reward prediction error signal for reinforcement learning.

### Curiosity
Exploration motivation based on novelty or prediction error.

### Novelty
Detection of unexpected sensory patterns.

### PredictionError
Computation of prediction error signals for learning.

## Configuration System

### Config
Central configuration management for all brain parameters.

### Available Configuration Options
- `neuron_count`: Number of neurons
- `synapse_density`: Probability of synapse formation
- `plasticity_rate`: Base learning rate
- `working_memory_capacity`: Size of working memory
- `prediction_horizon`: Length of prediction sequences
- `development_enabled`: Whether development occurs
- `reward_modulation_enabled`: Whether reward affects learning

## Utility Classes

### SimulationClock
Manages simulation time steps and timestamps.

### RandomGenerator
Deterministic random number generation for reproducible experiments.

### Logger
Structured logging system for debugging and monitoring.

### ExperimentRunner
Framework for running scientific experiments and collecting metrics.

## Memory Management

### Pool Allocation
All neural components use pre-allocated memory pools for performance:
- NeuronPool: Neuron state storage
- SynapsePool: Synaptic connection storage
- EventQueue: Spike event buffer

### Resource Cleanup
All NLM classes follow RAII principles with proper destructors and move semantics.

## Best Practices

1. **Initialize** before use: Always call `Brain::initialize()` after construction
2. **Use seeds**: Set random seeds for reproducible experiments
3. **Check return values**: Methods return bool for success/failure where applicable
4. **Thread safety**: Brain objects are not thread-safe; use one Brain per thread
5. **Memory management**: Use smart pointers for NLM objects
6. **Configuration**: Set configuration before initialization
7. **Experiment design**: Use explicit random seeds for reproducible scientific experiments

## Example Usage

See examples/beginner_tutorial.py, examples/intermediate_tutorial.py, and examples/advanced_example.py for complete usage examples.

## File Structure

- **Source files**: Located in `src/` directory
- **Headers**: Located in `src/core/`, `src/brain/`, `src/memory/`, etc.
- **Build system**: CMake-based, requires C++17
- **Documentation**: Markdown files in `docs/` directory
- **Tests**: Located in `tests/` directory

## Error Handling

NLM uses structured logging for errors. Check return values and examine logs for detailed error information.

## Threading Model

NLM uses a single-threaded execution model with event-driven spike processing. All updates occur within a single step function call.