# NLM Architecture

## Overview

NLM (熙然) is designed as a modular computational brain with the following major components:

```
┌─────────────────────────────────────────────────────────────────┐
│                        NLM Brain                                 │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │   Sensory   │───▶│    Brain    │───▶│    Motor    │          │
│  │   Input    │    │   Core      │    │    Output   │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│         │                  │                  │                 │
│         ▼                  ▼                  ▼                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │  Sensory    │    │  Memory     │    │   Action    │          │
│  │  Processing │    │  Systems   │    │  Selection  │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │  Prediction │                              │
│                    │   System    │                              │
│                    └─────────────┘                              │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │Neuromodul. │                              │
│                    │  Signals    │                              │
│                    └─────────────┘                              │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │ Development │                              │
│                    │   System    │                              │
│                    └─────────────┘                              │
│                                                                 │
│  ┌─────────────────────────────────────────────────────────────┐│
│  │              PHASE 4: EMERGING COGNITION                    ││
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐       ││
│  │  │Attention│  │Planning │  │Self-Model│ │ Social  │       ││
│  │  │         │  │         │  │         │  │Learning │       ││
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘       ││
│  └─────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────┘
```

## Overview

NLM (熙然) is designed as a modular computational brain with the following major components:

```
┌─────────────────────────────────────────────────────────────────┐
│                        NLM Brain                                 │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │   Sensory   │───▶│    Brain    │───▶│    Motor    │          │
│  │   Input    │    │   Core      │    │    Output   │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│         │                  │                  │                 │
│         ▼                  ▼                  ▼                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │  Sensory    │    │  Memory     │    │   Action    │          │
│  │  Processing │    │  Systems   │    │  Selection  │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │  Prediction │                              │
│                    │   System    │                              │
│                    └─────────────┘                              │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │Neuromodul. │                              │
│                    │  Signals    │                              │
│                    └─────────────┘                              │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │ Development │                              │
│                    │   System    │                              │
│                    └─────────────┘                              │
│                                                                 │
│  ┌─────────────────────────────────────────────────────────────┐│
│  │              PHASE 4: EMERGING COGNITION                    ││
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐       ││
│  │  │Attention│  │Planning │  │Self-Model│ │ Social  │       ││
│  │  │         │  │         │  │         │  │Learning │       ││
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘       ││
│  └─────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────┘
```

## Core Components

### 1. Core Layer
- **Types**: Strongly-typed identifiers (NeuronId, SynapseId, etc.)
- **Config**: Configuration management system
- **Random**: Deterministic random number generator
- **Logger**: Structured logging system
- **SimulationClock**: Time management for simulation

### 2. Brain Layer
- **Neuron**: Individual neuron representation (LIF dynamics)
- **Synapse**: Synaptic connection representation
- **NeuralPopulation**: Group of neurons with shared properties
- **NeuralRegion**: Brain region with multiple populations
- **Brain**: Central coordinator

### 3. Dynamics Layer
- **NeuralDynamics**: Abstract base for neuron dynamics
- **SpikeSystem**: Event-driven spike processing

### 4. Plasticity Layer
- **PlasticityRule**: Abstract base for plasticity rules
- **Hebbian**: Hebbian learning rule
- **STDP**: Spike-Timing-Dependent Plasticity
- **StructuralPlasticity**: Synapse formation/removal

### 5. Development Layer
- **DevelopmentSystem**: Orchestrates development
- **Synaptogenesis**: New synapse formation
- **Pruning**: Weak synapse removal
- **Maturation**: Adult property development

### 6. Neuromodulation Layer
- **Neuromodulator**: Abstract base for neuromodulators
- **Dopamine**: Reward and reinforcement
- **Novelty**: Novelty detection
- **Curiosity**: Exploration motivation
- **PredictionError**: Error signals

### 7. Memory Layer (Phase 4)
- **NeuralWorkingMemory**: Persistent activity for temporary storage
- **NeuralEpisodicMemory**: Experience encoding in neural patterns
- **NeuralAssociativeMemory**: Experience-based associations
- **ConceptFormation**: Pattern discovery without labels
- **SpatialRepresentation**: Learned spatial relationships
- **TemporalRelation**: Before/after/sequence encoding

### 8. Prediction Layer (Phase 4)
- **NeuralPrediction**: Temporal sequence learning
- **ActionConsequencePredictor**: Action effect prediction
- **PredictionErrorSignal**: Error computation for learning

### 9. Cognitive Layer (Phase 4)
- **AttentionalSelection**: Competitive selection (NOT Transformer QKV)
- **NeuralPlanner**: Predictive action selection
- **SelfModel**: Sensorimotor self-awareness
- **SocialLearning**: Observation and imitation

### 10. Sensory Layer
- **SensoryInput**: Base class for sensory data
- **Vision**: Visual input processing
- **Audio**: Audio input processing
- **InternalSignals**: Internal state signals

### 11. Motor Layer
- **Action**: Action representation
- **MotorSystem**: Neural-to-action conversion

### 12. Environment Layer
- **Environment**: Abstract environment interface
- **Observation**: Sensory observation wrapper
- **Action**: Action wrapper

### 13. Experiments Layer
- **Experiment**: Single experiment recording
- **ExperimentRunner**: Experiment execution
- **Metrics**: Data analysis
- **Phase4Experiment**: Phase 4 cognitive experiments

## Data Flow

### Phase 2: Neural Computation
```
Sensory Input (test signals)
      ↓
Brain.receiveSensoryInput()
      ↓
Neural Computation (LIF dynamics)
      ↓
Spike Processing (event-driven)
      ↓
Plasticity (STDP, Hebbian)
      ↓
Structural Plasticity (synaptogenesis/pruning)
```

### Phase 3: World Interaction
```
WORLD (SimpleWorld)
      ↓
SENSORY PERCEPT (Vision, Touch, Internal, Proprioception)
      ↓
AgentBrain.processSensoryInput()
      ↓
Brain.receiveSensoryInput() → Sensory Neurons
      ↓
Neural Computation (LIF dynamics)
      ↓
Spike Processing (event-driven)
      ↓
Motor Decoding → MotorCommand
      ↓
AgentBrain.decodeMotorCommand()
      ↓
WORLD.applyMotorCommand()
      ↓
ACTION RESULT (reward, success)
      ↓
Reward Prediction Error
      ↓
Neuromodulation (dopamine-like signal)
      ↓
Eligibility Trace Update
      ↓
Plasticity (STDP, Hebbian, Reward-Modulated)
      ↓
Structural Plasticity (synaptogenesis/pruning)
      ↓
Development System (stage progression)
```

### Phase 4: Emerging Cognition
```
WORLD (SimpleWorld)
      ↓
SENSORY PERCEPT
      ↓
ATTENTION (competitive selection)
      ↓
NEURAL PROCESSING
      ↓
PREDICTION (temporal sequence learning)
      ↓
WORKING MEMORY (persistent activity)
      ↓
EPISODIC MEMORY (experience storage)
      ↓
CONCEPT FORMATION (pattern discovery)
      ↓
PLANNING (multi-step prediction)
      ↓
SELF-MODEL (sensorimotor learning)
      ↓
MOTOR ACTION
      ↓
WORLD.applyMotorCommand()
      ↓
ACTION RESULT
      ↓
REWARD / ERROR
      ↓
NEUROMODULATION
      ↓
PLASTICITY
      ↓
SOCIAL LEARNING (observation/imitation)
```

## Agent-Brain Interface

Phase 3/4 introduces the AgentBrain class that connects the NLM brain to the world:

- **SensoryTransduction**: Converts world percepts to neural input
- **MotorDecoding**: Converts neural activity to motor commands
- **Neuromodulation**: Applies reward signals to plasticity
- **Development**: Updates developmental stage
- **Attention**: Selective processing through competition
- **Planning**: Uses predictions to select actions

## Key Design Principles

1. **No ML Frameworks**: NLM implements its own neural computation
2. **No Transformer Architecture**: Uses spiking dynamics and competitive attention
3. **Biological Inspiration**: Designed to be incrementally more biologically accurate
4. **Modularity**: Clear interfaces between components
5. **Determinism**: Reproducible experiments via explicit random seeds
6. **Performance**: Data-oriented design for large-scale simulation
7. **Extensibility**: Clear patterns for adding new components
8. **No Predefined Knowledge**: All learning from experience

## Phase 4 Key Innovations

### Prediction System
- Learns temporal patterns from experience
- Predicts future states and action consequences
- Multi-step prediction capability
- Error signals modulate learning

### Working Memory
- Persistent neural activity
- Recurrent connections for maintenance
- Competition for selective retention
- Neural-grounded (not a vector store)

### Episodic Memory
- Experience encoded as neural patterns
- Pattern completion retrieval
- Temporal and spatial indexing
- Replay during consolidation

### Concept Formation
- Discovers patterns without labels
- Prototype formation through averaging
- Stability measurement
- Category hints from properties

### Attention
- Lateral inhibition competition
- Winner-take-more dynamics
- Top-down and bottom-up bias
- NOT Transformer QKV attention

### Planning
- Uses learned predictions
- Evaluates potential futures
- Selects actions leading to goals
- Adapts based on outcomes

### Self-Model
- Learns body schema
- Predicts action consequences
- Self vs external distinction
- Capability representation

### Social Learning
- Observes other agents
- Learns action effects
- Imitation capability
- Simple communication signals

## Python API Enhancements

### Version and Metadata
The Python bindings now include metadata attributes for better introspection:

```python
import pynlm

print(pynlm.__version__)        # "0.1.0"
print(pynlm.__author__)         # "NLM Project"
print(pynlm.__description__)    # "Neural Learning Machine - Python Bindings"
```

### Enhanced Configuration Helpers

#### Default Configuration
```python
import pynlm

# Create brain with sensible defaults
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create with specific parameters
brain = pynlm.createBrain(1000, region_count=2)  # Convenience constructor
```

#### Configuration Validation
```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)

# Validate configuration and get errors
errors = config.validate()
if errors:
    for error in errors:
        print(f"Error: {error}")
else:
    print("Configuration is valid!")
```

#### File Format Support
```python
import pynlm

# Create and save configuration in different formats
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)

# Save in JSON format
config.saveToJSON("brain_config.json")

# Save in YAML format  
config.saveToYAML("brain_config.yaml")

# Load from any supported format
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("brain_config.json")  # Auto-detects JSON
```

### Enhanced Brain Creation

#### Convenience Constructors
```python
import pynlm

# Simple: create brain with defaults
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Advanced: specify parameters directly
brain = pynlm.createBrain(2000, region_count=3)  # 2000 neurons, 3 regions

# Load configuration from file
config = pynlm.createDefaultConfig()
config.loadFromFile("my_config.json")
brain = pynlm.createBrain(config)
```

#### Sensory Input Creation
```python
import pynlm

# Create vision input for 64x64 images with 3 channels
vision = pynlm.createVisionInput(64, 64, 3)

# Create audio input
audio = pynlm.createAudioInput(sampleRate=44100, numSamples=1024)

# Create vision with default 3 channels
vision = pynlm.createVisionInput(32, 32)
```

#### Action Creation
```python
import pynlm

# Create action with parameters
action = pynlm.createAction(pynlm.ActionType.MoveForward, [1.5, 2.0])

# Create movement action with direction and distance
action = pynlm.createMoveAction("forward", 2.0)
action = pynlm.createMoveAction("left", 1.5)

# Wait action (no movement)
wait_action = pynlm.createAction(pynlm.ActionType.Wait)
```

### Error Handling and Safety

#### Safe Brain Stepping
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Safely perform simulation steps with error handling
success = pynlm.safeStep(brain, step=100, time=0.1)
if success:
    print("Step completed successfully")
else:
    print("Step failed - check brain state")
```

#### Statistics and Monitoring
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get comprehensive brain statistics as dictionary
stats = pynlm.getBrainStats(brain)
print(f"Neurons: {stats['total_neurons']}")
print(f"Spikes: {stats['total_spikes']}")
print(f"Firing rate: {stats['avg_firing_rate']:.3f}")
print(f"Memory traces: {stats.get('working_memory_traces', 'N/A')}")
```

### Integration Helpers

#### Complete Agent Episode
```python
import pynlm

# Setup a complete agent simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Run a complete episode
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Episode completed!")
```

### Neuron Type Configuration
```python
import pynlm

# Create configurations for different neuron types
excitatory_config = pynlm.createExcitatoryNeuronConfig()
-inhibitory_config = pynlm.createInhibitoryNeuronConfig()
-sensory_config = pynlm.createSensoryNeuronConfig()
-motor_config = pynlm.createMotorNeuronConfig()

# These can be used to create specific neuron populations
```

## Configuration File Formats

### JSON Format
```json
{
  "neuron_count": 1000,
  "region_count": 1,
  "connection_probability": 0.1,
  "simulation_timestep": 0.001,
  "random_seed": 42,
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.01,
      "tau_plus": 20.0,
      "tau_minus": 20.0
    }
  }
}
```

### YAML Format
```yaml
# NLM Configuration
# Generated: Mon Jan 15 14:30:00 2026

neuron_count: 1000
region_count: 1
connection_probability: 0.1
simulation_timestep: 0.001
random_seed: 42

plasticity:
  stdp:
    enable: true
    learning_rate: 0.01
    tau_plus: 20.0
    tau_minus: 20.0
  hebbian:
    enable: true
  structural:
    enable: true

neuromodulation:
  dopamine:
    scale: 1.0
  curiosity:
    enable: true
  novelty:
    enable: true
```

### Simple Format (Legacy)
```
# Sample config for legacy format
neuron_count = 500
region_count = 1
connection_probability = 0.15
simulation_timestep = 0.001
random_seed = 42
```

## Advanced Features

### Configuration Validation

The system now validates configuration files automatically:

- **Range validation**: Ensures values are within acceptable ranges
- **Required keys**: Checks for essential configuration parameters
- **Type consistency**: Validates parameter types
- **Value validation**: Tests that numeric parameters are reasonable

Validation errors are returned as a list of descriptive messages:

```python
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)  # Too low
errors = config.validate()
print(errors)  # ["neuron_count should be at least 10"]
```

### File Format Auto-Detection

The system automatically detects file formats by extension:

- `.json` → JSON format
- `.yaml`, `.yml` → YAML format
- `.cfg`, `.txt`, no extension → Simple key=value format

```python
# Any file load function works automatically
config.loadFromFile("config.json")    # Detects JSON
config.loadFromFile("config.yaml")    # Detects YAML  
config.loadFromFile("config.txt")     # Detects simple format
```

### Performance Monitoring

The enhanced statistics API provides comprehensive monitoring:

```python
stats = pynlm.getBrainStats(brain)

# Core metrics
stats["total_neurons"]      # Total neuron count
stats["total_synapses"]     # Total synapse count
stats["firing_neurons"]     # Currently firing
stats["total_spikes"]       # Spike count
stats["avg_firing_rate"]    # Average firing rate

# Memory system metrics
stats["working_memory_traces"]  # Active working memory traces
stats["episodes_stored"]       # Stored episodic memories

# Neuromodulation metrics
stats["dopamine_level"]    # Current dopamine level
stats["curiosity_level"]   # Current curiosity level

# System metrics
stats["pending_events"]    # Pending spike events
stats["e_i_ratio"]         # Excitation/inhibition ratio
```

## Error Handling Improvements

### Safe Operations

Many core operations now include built-in error handling:

```python
# Safe brain stepping with error reporting
success = pynlm.safeStep(brain, step=100, time=0.1)

# Safe configuration validation
errors = config.validate()  # Always returns list (empty if valid)

# Safe file operations (returns bool)
if config.saveToJSON("config.json"):
    print("Saved successfully")
else:
    print("Save failed")
```

### Graceful Degradation

When operations fail, the system provides meaningful error messages and fallback behavior:

- **Missing files**: Clear error messages with file path suggestions
- **Invalid formats**: Detailed error messages for malformed files
- **Invalid values**: Specific guidance on correction needed
- **Resource limits**: Clear feedback about system limits

## Migration Guide

### From Old API to New API

#### Old Way
```python
import pynlm

# Old simple API
config = pynlm.createDefaultConfig()
config.loadFromFile("config.txt")
brain = pynlm.createBrain(config)
```

#### New Way (Enhanced)
```python
import pynlm

# New API with auto-detection and validation
config = pynlm.createDefaultConfig()

# Load any format - automatically detects JSON, YAML, or simple
config.loadFromFile("config.json")   # Auto-detects format
config.loadFromFile("config.yaml")   # Auto-detects format
config.loadFromFile("config.txt")    # Auto-detects format

# Validate configuration
if config.validate():
    print("Configuration valid!")

# Create brain with convenience constructor
brain = pynlm.createBrain(1000, region_count=2)  # Even simpler!
```

### Best Practices

1. **Use convenience constructors** for common use cases
2. **Always validate** your configuration before use
3. **Use JSON/YAML formats** for complex configurations
4. **Use the enhanced stats API** for monitoring and debugging
5. **Implement error handling** for production applications
6. **Use file format auto-detection** to handle multiple formats
7. **Leverage the extended sensory input helpers** for common use cases
8. **Use the enhanced action creation helpers** for movement actions

This enhanced Python API provides both greater flexibility and simpler usage for developers, from beginners setting up their first brain simulation to advanced users implementing complex experiments.