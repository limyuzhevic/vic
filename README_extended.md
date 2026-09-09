# NLM - Neural Learning Machine Documentation

## Overview

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project. This document provides comprehensive documentation for the NLM system, covering all components from neural dynamics to cognitive functions.

## Table of Contents

1. [Introduction](## Introduction)
2. [Core Architecture](## Core Architecture)
3. [Neural Components](## Neural Components)
4. [Memory Systems](## Memory Systems)
5. [Plasticity and Learning](## Plasticity and Learning)
6. [Development](## Development)
7. [Neuromodulation](## Neuromodulation)
8. [Cognitive Systems](## Cognitive Systems)
9. [Prediction and Planning](## Prediction and Planning)
10. [Environment Interface](## Environment Interface)
11. [Phase 6 Integration](## Phase 6 Integration)
12. [Configuration](## Configuration)
13. [Building and Running](## Building and Running)
14. [API Reference](## API Reference)

---

## Introduction

### What is NLM?

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

### Key Features

- **Real Neural Computation**: LIF neuron dynamics with spike propagation
- **Integrated Memory**: Working memory, episodic memory, and associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error signals
- **Cognitive Functions**: Attention, planning, concept formation, and self-model
- **Development**: Stage-dependent plasticity and structural changes
- **Closed-Loop Processing**: Complete brain loop from sensory input to motor output

### Design Philosophy

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

---

## Core Architecture

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

### Core Layers

#### 1. Core Layer
- **Types**: Strongly-typed identifiers (NeuronId, SynapseId, etc.)
- **Config**: Configuration management system
- **Random**: Deterministic random number generator
- **Logger**: Structured logging system
- **SimulationClock**: Time management for simulation

#### 2. Neural Layer
- **Neuron**: Individual neuron representation (LIF dynamics)
- **Synapse**: Synaptic connection representation
- **NeuralPopulation**: Group of neurons with shared properties
- **NeuralRegion**: Brain region with multiple populations
- **Brain**: Central coordinator

#### 3. Dynamics Layer
- **NeuralDynamics**: Abstract base for neuron dynamics
- **SpikeSystem**: Event-driven spike processing

#### 4. Plasticity Layer
- **PlasticityRule**: Abstract base for plasticity rules
- **Hebbian**: Hebbian learning rule
- **STDP**: Spike-Timing-Dependent Plasticity
- **StructuralPlasticity**: Synapse formation/removal

#### 5. Development Layer
- **DevelopmentSystem**: Orchestrates development
- **Synaptogenesis**: New synapse formation
- **Pruning**: Weak synapse removal
- **Maturation**: Adult property development

#### 6. Neuromodulation Layer
- **Neuromodulator**: Abstract base for neuromodulators
- **Dopamine**: Reward and reinforcement
- **Curiosity**: Exploration motivation
- **Novelty**: Novelty detection
- **PredictionError**: Error signals

#### 7. Memory Layer
- **NeuralWorkingMemory**: Persistent activity for temporary storage
- **NeuralEpisodicMemory**: Experience encoding in neural patterns
- **NeuralAssociativeMemory**: Experience-based associations

#### 8. Prediction Layer
- **PredictionSystem**: Temporal sequence learning
- **NeuralPrediction**: Action consequence prediction

#### 9. Cognitive Layer
- **AttentionalSelection**: Competition-based attention
- **NeuralPlanner**: Predictive action selection
- **ConceptFormation**: Pattern discovery without labels

#### 10. Sensory/Motor Layers
- **SensoryInput**: Base class for sensory data
- **Vision**: Visual input processing
- **Audio**: Audio input processing
- **MotorSystem**: Neural-to-action conversion

### Data Flow

#### Phase 2: Neural Computation
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

#### Phase 3: World Interaction
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

---

## Neural Components

### Neuron Class

The fundamental building block of the NLM system, implementing Leaky Integrate-and-Fire (LIF) dynamics.

#### LIF Dynamics
- **Membrane Potential**: Integrates incoming currents
- **Threshold**: When reached, neuron fires spike
- **Refractory Period**: Prevents immediate re-firing
- **Reset Potential**: Returns after spike

#### Key Methods
- `stepLIF()`: Update membrane potential
- `injectCurrent()`: Add current to neuron
- `receiveExcitatoryInput()`: Apply positive current
- `receiveInhibitoryInput()`: Apply negative current
- `isFiring()`: Check if neuron is currently firing

### Synapse Class

Connection between neurons with plasticity and transmission properties.

#### Properties
- **Type**: Excitatory, inhibitory, or modulatory
- **Weight**: Synaptic strength
- **Delay**: Transmission delay
- **PlasticityFlags**: STDP and Hebbian enabled

#### Key Methods
- `getWeight()`: Get current weight
- `setWeight()`: Set weight
- `recordPreSpike()`: Record pre-synaptic spike for STDP
- `recordPostSpike()`: Record post-synaptic spike for STDP

### SpikeSystem Class

Event-driven spike processing system with delayed events.

#### Features
- **Immediate Spike Processing**: Handle spikes instantly
- **Delayed Spike Events**: Schedule synaptic transmission
- **Spike Handlers**: Callback for spike events
- **Delayed Handlers**: Callback for delayed spike events

#### Key Methods
- `queueSpike()`: Queue immediate spike event
- `queueDelayedSpike()`: Schedule delayed spike
- `processSpikes()`: Process immediate spikes
- `processDelayedSpikes()`: Deliver delayed synaptic input

---

## Memory Systems

### NeuralWorkingMemory

Implements working memory using neural dynamics rather than simple storage. Key mechanisms:

- **Persistent Activity**: Reverberating neural activity
- **Recurrent Connections**: Self-sustaining connections
- **Activity-Dependent Plasticity**: Short-term changes based on activity
- **Neural Competition**: Winner-take-all dynamics

#### Operations
- **Store()**: Encode pattern as neural activity
- **Retrieve()**: Read out current memory content
- **Update()**: Maintain and decay memory
- **RunCompetition()**: Select winning memories

### NeuralEpisodicMemory

Stores experiences as neural patterns with temporal indexing:

- **Experience Encoding**: Capture current brain state as episode
- **Pattern Completion**: Retrieve episodes from partial cues
- **Replay**: Reactivate episodes during consolidation
- **Temporal Indexing**: Organize by simulation time

### NeuralAssociativeMemory

Stores relationships between neural patterns:

- **Pattern Associations**: Link related neural patterns
- **Strength-Based Storage**: Association strength reflects importance
- **Pattern Completion**: Retrieve associated patterns
- **Reinforcement Learning**: Strengthen important associations

---

## Plasticity and Learning

### STDP (Spike-Timing-Dependent Plasticity)

Classic Hebbian learning rule where spike timing matters:

- **LTP (Long-Term Potentiation)**: Pre-before-post strengthens synapse
- **LTD (Long-Term Depression)**: Post-before-pre weakens synapse
- **Asymmetry**: Potentiation > Depression
- **Time Window**: ~20-50ms window

### Hebbian Learning

Standard association rule:

- **Correlation-Based**: Synapses strengthen with correlated activity
- **Symmetric**: Both directions can learn
- **Rate-Based**: Average firing rates determine learning

### Structural Plasticity

Dynamic synapse formation and removal:

- **Synaptogenesis**: Create new connections
- **Pruning**: Remove weak or redundant connections
- **Homeostatic Regulation**: Maintain network stability
- **Developmental Regulation**: Stage-dependent rates

---

## Development

### Developmental Stages

1. **Initial**: High plasticity, rapid learning
2. **Critical Period**: Sensitive window for specific learning
3. **Maturation**: Gradual stabilization
4. **Adult**: Stable, refined function
5. **Aging**: Gradual decline in plasticity

### Development Mechanisms

- **Stage Transitions**: Progressive maturation
- **Plasticity Modulation**: Stage-dependent learning rates
- **Structural Changes**: Synaptogenesis and pruning
- **Functional Specialization**: Region-specific development

---

## Neuromodulation

### Dopamine

Reward prediction error signal:

- **Reward Signaling**: Respond to unexpected rewards
- **Prediction Error**: Compare expected vs actual reward
- **Plasticity Modulation**: Gate learning based on dopamine level
- **Excitability Modulation**: Adjust neural responsiveness

### Other Neuromodulators

- **Curiosity**: Drive for exploration and novel information
- **Novelty**: Detection of unexpected sensory input
- **PredictionError**: Compute prediction violations

---

## Cognitive Systems

### AttentionalSelection

Neural attention through competition (NOT Transformer attention):

- **Lateral Inhibition**: Suppress competing neurons
- **Salience Detection**: Prioritize important stimuli
- **Top-Down Bias**: Goal-directed attention
- **Winner-Take-More**: Amplify selected representations

### NeuralPlanner

Predictive action selection:

- **Multi-Step Prediction**: Simulate future outcomes
- **Action Evaluation**: Compare potential actions
- **Adaptation**: Learn from prediction errors
- **Goal Integration**: Align actions with objectives

### ConceptFormation

Pattern discovery without labels:

- **Prototype Formation**: Abstract representations
- **Stability Measurement**: Pattern regularity
- **Category Learning**: Cluster similar patterns
- **Generalization**: Apply learned patterns to new situations

---

## Prediction and Planning

### NeuralPrediction

Temporal sequence learning:

- **Sequence Detection**: Identify recurring patterns
- **Action Consequences**: Learn cause-effect relationships
- **Multi-Step Prediction**: Forecast future states
- **Error Correction**: Refine predictions based on feedback

### Planning Integration

- **Backward Induction**: Work from goals to actions
- **Monte Carlo Methods**: Sample possible futures
- **Heuristic Search**: Efficient action selection
- **Adaptive Planning**: Update based on new information

---

## Environment Interface

### SimpleWorld

2D simulation environment for agent development:

- **Grid-based World**: Discrete positions
- **Visual Sensing**: Limited field of view
- **Agent Body**: Position, orientation, velocity
- **Actions**: Move, turn, interact
- **Objects**: Resources, hazards, walls

### AgentBrain Interface

Connects NLM brain to world:

- **SensoryTransduction**: World percepts → neural input
- **MotorDecoding**: Neural activity → motor commands
- **Neuromodulation**: Reward signals → plasticity
- **Development**: World feedback → stage progression

---

## Phase 6 Integration

### Phase 6: Final Integration

Phase 6 focuses on integrating all existing systems into a coherent artificial brain:

#### Integration Achievements

**Memory Systems**
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

**Neuromodulation Integration**
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

**Prediction System**
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

**Cognition Systems**
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

**Development Integration**
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

**Persistence**
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

**Replay and Consolidation**
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

### Phase 6 Integration Test

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

---

## Configuration

### Configuration System

NLM uses a flexible configuration system:

#### Configuration Types
- **Integer Values**: `config->set("brain.neuron_count", 1000)`
- **Float Values**: `config->set("brain.synapse_density", 0.1f)`
- **String Values**: `config->set("checkpoint_dir", "./checkpoints")`

#### Configuration Files
- **JSON Format**: Human-readable configuration
- **Command Line**: Override configuration parameters
- **Default Values**: Sensible defaults for most parameters

#### Key Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection |
| `random_seed` | int64_t | 42 | Random number generator seed |
| `simulation_timestep` | double | 0.001 | Simulation time step |
| `stdp_ltp_weight` | float | 0.01 | STDP LTP learning rate |
| `stdp_ltd_weight` | float | 0.012 | STDP LTD learning rate |
| `synaptogenesis_rate` | float | 0.0001f | New synapse formation rate |
| `pruning_rate` | float | 0.00001f | Synapse removal rate |

### Python API Configuration

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Modify configuration
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.5)

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

---

## Building and Running

### Building from Source

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j4
```

### Building Python Package

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install Python bindings
pip install .

# Or install in development mode
pip install -e .
```

### Running NLM

#### Basic Simulation

```bash
./nlm
```

This runs a basic neural computation demonstration.

#### Phase 3 Demo

```bash
./nlm_phase3_demo
```

This runs the Phase 3 world interaction demo.

#### Phase 4 Demo

```bash
./nlm_phase4_demo
```

This runs the Phase 4 cognitive capabilities demo.

#### Phase 6 Integration Test

```bash
./nlm_phase6_demo
```

This runs the complete integration test.

### Running Tests

```bash
cd build
ctest --output-on-failure
```

### Python Usage

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()}")
```

---

## API Reference

### Core Classes

#### Config
```python
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")
config.saveToFile("config.json")
config.set("brain.neuron_count", 1000)
config.getOr("brain.synapse_density", 0.1f)
config.has("brain.neuron_count")
```

#### Brain
```python
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(step_number)
brain.receiveSensoryInput(percept)
action = brain.produceAction()
brain.save("checkpoint.bin")
brain.load("checkpoint.bin")
```

#### AgentBrain
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.processSensoryInput(percept)
action = agent.decodeMotorCommand()
agent.applyRewardModulation(reward, predicted_reward)
agent.updateDevelopment(timestep)
```

#### SimpleWorld
```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.update(timestep=0.1)
percept = world.getSensoryPercept()
world.applyMotorCommand(action, world.getSimulationTime())
```

### Enumerations

#### NeuronType
- `Excitatory`: Positive current injection
- `Inhibitory`: Negative current injection
- `Modulatory`: Neuromodulatory influence
- `Sensory`: External input reception
- `Motor`: Output action generation
- `Internal`: Internal state regulation

#### ActionType
- `MoveForward`: Advance in current direction
- `MoveBackward`: Move opposite direction
- `MoveLeft`: Turn left
- `MoveRight`: Turn right
- `TurnLeft`: Rotate left
- `TurnRight`: Rotate right
- `Look`: Change gaze direction
- `Interact`: Interact with nearby object
- `Eat`: Consume resource
- `Drink`: Consume liquid
- `Rest`: Enter resting state
- `Wait`: No action
- `Custom`: User-defined action

#### DevelopmentalStage
- `Initial`: High plasticity, rapid learning
- `CriticalPeriod`: Sensitive learning window
- `Maturation`: Gradual stabilization
- `Adult`: Stable function
- `Aging`: Declining plasticity

### Memory System Classes

#### NeuralWorkingMemory
```python
brain = pynlm.createBrain(config)
brain.initialize()

# Store pattern
pattern = [0.1, 0.5, 1.0, 0.3]
brain.getWorkingMemory().store(pattern)

# Retrieve pattern
retrieved = brain.getWorkingMemory().retrieve()
print(f"Retrieved pattern: {retrieved}")
```

#### NeuralEpisodicMemory
```python
# Create episode
episode = EpisodicMemoryItem()
episode.timestamp = 100
episode.neurons = [NeuronId(1), NeuronId(2)]
episode.activations = [0.5, 0.8]

# Store episode
brain.getEpisodicMemory().storeEpisode(episode)

# Get recent episodes
recent = brain.getEpisodicMemory().getRecentEpisodes(10)
```

#### NeuralAssociativeMemory
```python
# Create association
brain.getAssociativeMemory().associate(NeuronId(1), NeuronId(2), 0.8)

# Get associations
associated = brain.getAssociativeMemory().getAssociations(NeuronId(1))
print(f"Associated neurons: {associated}")
```

### Neuromodulation Classes

#### Dopamine
```python
dopamine = brain.getDopamine()
dopamine.setLevel(0.5)
level = dopamine.getLevel()
plasticity_factor = dopamine.getPlasticityFactor()
dopamine.signalReward(1.0)
dopamine.signalRewardPredictionError(0.5)
```

### Development System

```python
brain = pynlm.createBrain(config)
brain.initialize()

# Get developmental stage
stage = brain.getDevelopmentalStage()
print(f"Current stage: {stage}")

# Advance stage
brain.getDevelopmentSystem().advanceStage()

# Get plasticity modifier
mod = brain.getDevelopmentSystem().getPlasticityModifier()
```

### Prediction System

```python
brain = pynlm.createBrain(config)
brain.initialize()

prediction = brain.getPredictionSystem()
# Use prediction for planning and action selection
```

### Cognition Systems

#### AttentionalSelection
```python
brain = pynlm.createBrain(config)
brain.initialize()

attention = brain.getAttention()
winners = attention.processCompetition(competitors, inhibition=0.5)
```

#### NeuralPlanner
```python
brain = pynlm.createBrain(config)
brain.initialize()

planner = brain.getPlanner()
planner.setPlanningDepth(5)
# Use planner for action selection
```

---

## Troubleshooting

### Common Issues

#### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check configuration parameters (neuron count, etc.)

#### "The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify motor neurons are being activated

#### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check configuration parameters

#### "ImportError: No module named 'pynlm'"
```bash
pip install --force-reinstall .
```

#### "Segmentation faults"
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Check for null pointer dereference

### Debug Commands

#### Run with Logging
```bash
./nlm --config_level=debug
```

#### Visualize Brain State
```python
import pynlm

brain = pynlm.createBrain(config)
brain.initialize()
brain.logStatus()  # Print detailed brain status
```

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - Detailed system architecture
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 integration audit

---

## License

MIT License

Copyright (c) 2024 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

---

## Authors

Research project — See docs for scientific background.

## Version

Version 0.1.0 (Phase 6: Final Integration)

Built with C++20, pybind11, and scikit-build-core.
