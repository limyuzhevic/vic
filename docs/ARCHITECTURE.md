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

## Core Components

### 1. Core Layer
- **Types**: Strongly-typed identifiers (NeuronId, SynapseId, etc.)
- **Config**: Configuration management system with JSON schema validation
- **Random**: Deterministic random number generator
- **Logger**: Structured logging system
- **SimulationClock**: Time management for simulation

### 2. Brain Layer
- **Neuron**: Individual neuron representation (LIF dynamics)
- **Synapse**: Synaptic connection representation
- **NeuralPopulation**: Group of neurons with shared properties
- **NeuralRegion**: Brain region with multiple populations
- **Brain**: Central coordinator with advanced statistics and metrics

## Enhanced Python Bindings

NLM provides comprehensive Python bindings for advanced neuroscience research and complex simulations:

### 1. Enhanced Brain Statistics
- **Detailed brain state information**: Synaptic density, regional statistics, firing statistics
- **Network metrics**: Criticality index, network efficiency, synchronization metrics
- **Performance monitoring**: Energy consumption, information processing rate, system stability
- **Developmental metrics**: Plasticity modifier, critical period progress, developmental age

### 2. Memory System Access
- **Working memory**: Transient active information storage with neural dynamics
- **Episodic memory**: Experience encoding and retrieval with neural patterns
- **Associative memory**: Pattern associations with spreading activation
- **Memory operations**: Store, retrieve, replay, consolidate, update relevance

### 3. Prediction System Access
- **Prediction system**: Next-state prediction with temporal sequence learning
- **Prediction error**: Error signals for curiosity and learning
- **Prediction metrics**: Confidence tracking, error history, training capabilities

### 4. Development System Access
- **Developmental stages**: Initial → CriticalPeriod → Maturation → Adult → Aging
- **Plasticity modulation**: Stage-dependent plasticity rates and modifiers
- **Development tracking**: Age progression, stage advancement, critical period metrics

### 5. Neuromodulation System
- **Dopamine**: Reward prediction error and reinforcement signaling
- **Curiosity**: Exploration drive combining novelty and prediction error
- **Novelty**: Novelty detection from sensory patterns
- **Prediction error**: Error signals for curiosity and learning
- **Neuromodulator interface**: Base class for all neuromodulators

### 6. Advanced Configuration
- **Configuration validation**: JSON schema validation and rule-based validation
- **Configuration import/export**: Support for JSON, YAML, and INI formats
- **Configuration merging**: Merge configurations from different sources
- **Configuration statistics**: Type information, usage patterns, completeness checks
- **Configuration comparison**: Diff functionality between configuration versions

### 7. Performance Monitoring
- **Real-time metrics**: Active neuron counts, firing rates, synaptic activity
- **Network analysis**: Criticality index, efficiency, clustering coefficient
- **System health**: Energy consumption, memory usage, stability metrics
- **Learning metrics**: Progress tracking, retention, accuracy measurements

### 8. Region and Population Access
- **Regional access**: Individual region statistics and properties
- **Population analysis**: Neural population statistics by type and region
- **Circuit analysis**: Connectivity patterns, excitation/inhibition balance
- **Network motifs**: Detection of feed-forward, feedback, and recurrent loops

## Python API Examples

### Basic Brain Usage
```python
from pynlm import Brain, Config, NeuralWorkingMemory, NeuralEpisodicMemory

# Create brain with default configuration
config = Config.createDefaultConfig()
brain = Brain(config)
brain.initialize()

# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Access development system
development = brain.getDevelopmentSystem()
print(f"Developmental stage: {development.getStage()}")
print(f"Critical period: {development.isCriticalPeriod()}")

# Access neuromodulation
pred_error = brain.getPredictionErrorSignal()
curiosity = brain.getCuriosity()
dopamine = brain.getDopamine()

# Get advanced statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()}")
print(f"Criticality index: {brain.getCriticalityIndex()}")
print(f"Network efficiency: {brain.getNetworkEfficiency()}")

# Get regional statistics
regional_stats = brain.getRegionalStats()
for stats in regional_stats:
    print(f"Region {stats.regionName}: {stats.neuronCount} neurons")
```

### Memory Operations
```python
# Working memory operations
working_memory.store([0.1, 0.2, 0.3, 0.4, 0.5])
retrieved = working_memory.retrieve()
print(f"Working memory activity: {working_memory.getMemoryActivity()}")

# Episodic memory operations
episode = {
    'timestamp': brain.getSimulationTime(),
    'sensoryState': [0.1, 0.2, 0.3],
    'action': ActionType.Look,
    'reward': 1.0,
    'novelty': 0.8
}
episodic_memory.storeEpisode(episode)
recent_episodes = episodic_memory.getRecentEpisodes(5)

# Associative memory operations
associative_memory.associate([0.1, 0.2], [0.3, 0.4], strength=0.9)
retrieved_patterns = associative_memory.retrieve([0.1, 0.2], max_results=3)
```

### Prediction and Development
```python
# Prediction system
prediction_system = brain.getPredictionSystem()
predicted = prediction_system.predictNextState(current_state)
error = prediction_system.getPredictionError()

# Development system
development.advanceStage()
plasticity_modifier = development.getPlasticityModifier()
critical_period_progress = development.getCriticalPeriodProgress()

# Neuromodulation
curiosity.update(novelty=0.7, prediction_error=0.3, dt=0.1)
exploration_drive = curiosity.getExplorationDrive()
```

### Advanced Configuration
```python
# Configuration validation and management
config = Brain.createDefaultConfig()

# JSON schema validation
schema = config.getConfigSchema()
validated = config.validateConfig(schema)

# Configuration merging
other_config_json = '{"learning_rate": 0.01, "hidden_size": 256}'
config.mergeConfig(other_config_json)

# Configuration export
config_data = config.exportConfig("json")
loaded_config = Config()
loaded_config.importConfig(config_data, "json")
```

## Key Design Principles

1. **No ML Frameworks**: NLM implements its own neural computation
2. **No Transformer Architecture**: Uses spiking dynamics and competitive attention
3. **Biological Inspiration**: Designed to be incrementally more biologically accurate
4. **Modularity**: Clear interfaces between components
5. **Determinism**: Reproducible experiments via explicit random seeds
6. **Performance**: Data-oriented design for large-scale simulation
7. **Extensibility**: Clear patterns for adding new components
8. **No Predefined Knowledge**: All learning from experience

## Phase 4 Key Innovations (Enhanced in Python Bindings)

### Prediction System
- Learns temporal sequences from experience
- Predicts action consequences
- Multi-step prediction capability
- Error signals modulate learning

### Working Memory
- Persistent neural activity
- Recurrent connections for maintenance
- Competition for selective retention
- Neural-grounded (not a vector store)

### Episodic Memory
- Experiences stored as neural patterns
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
- Learns temporal sequences from experience
- Predicts action consequences
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