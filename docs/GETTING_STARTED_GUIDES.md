# Getting Started Guides

This section provides comprehensive guides for getting started with the NLM framework, from installation to running your first simulation.

## 1. Installation and Setup

### Prerequisites

Before installing NLM, ensure you have:

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **Build Tools**: Make or Ninja
- **Memory**: At least 8GB RAM (more for large simulations)

### Building from Source

#### Step 1: Clone the Repository

```bash
# Clone NLM from GitHub
git clone https://github.com/nlm-project/nlm.git
 cd nlm

# Navigate to the session directory (if inside a workspace)
cd /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_f9525b1a-bfa4-449e-a128-4af0af9d3e79
```

#### Step 2: Build the C++ Core

```bash
# Create build directory
mkdir build
 cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets (use multiple cores for faster build)
make -j$(nproc)

# Optional: Build tests (if you want to run them)
make nlm_test
```

#### Step 3: Install Python Bindings

```bash
# Install Python dependencies first
pip install pybind11 scikit-build-core pytest numpy

# Install NLM in development mode (recommended for development)
pip install -e .

# Or build and install from source
pip install .

# Verify installation
python -c "import pynlm; print(f'NLM version: {pynlm.__version__}')"
```

### Building with Different Options

#### Debug Build

```bash
mkdir debug_build
 cd debug_build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

#### Custom Compiler

```bash
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc
```

#### Installation to Custom Prefix

```bash
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

### Troubleshooting Common Build Issues

| Error | Solution |
|-------|----------|
| CMake cannot find Python | ```bash pip install scikit-build-core pybind11 cmake .. -DPython_EXECUTABLE=$(which python)``` |
| Compilation errors about C++20 | Ensure your compiler supports C++20 (GCC 10+, Clang 12+) |
| Missing pybind11 headers | ```bash pip install pybind11``` |
| Linker errors about undefined symbols | Build with `-DCMAKE_BUILD_TYPE=Release` |

## 2. First Neural Simulation

This guide walks you through creating and running your first neural simulation with NLM.

### The Concept

A neural simulation in NLM consists of:

1. **Brain**: A collection of neurons that process information
2. **World**: An environment that provides input and receives output
3. **Agent**: The interface between brain and world

The basic loop is:
```
World → Agent → Brain → Agent → World → ...
```

### Step-by-Step Tutorial

#### Step 1: Create a Simple Python Script

Create a file called `first_simulation.py`:

```python
#!/usr/bin/env python3
"""
Your first NLM neural simulation
"""

import pynlm

def main():
    print("=== First NLM Simulation ===")
    
    # 1. Create configuration
    print("\n1. Creating brain configuration...")
    config = pynlm.createDefaultConfig()
    
    # Optional: Customize configuration
    config.set("brain.neuron_count", 100)
    config.set("plasticity.stdp.enable", True)
    
    # 2. Create and initialize brain
    print("2. Creating and initializing brain...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"   - Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"   - {brain.getTotalSynapseCount()} synapses connected")
    
    # 3. Run simulation
    print("\n3. Running simulation (100 steps)...")
    for step in range(100):
        brain.step(step)
        
        if step % 20 == 0:
            print(f"   - Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    print("\n4. Simulation complete!")
    print(f"   - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   - Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    # 5. Save brain state (optional)
    brain.save("first_simulation_checkpoint.bin")
    print("   - Brain state saved")
    
    print("\n=== Simulation Complete ===")

if __name__ == "__main__":
    main()
```

#### Step 2: Run the Simulation

```bash
# Make the script executable
chmod +x first_simulation.py

# Run the simulation
python3 first_simulation.py
```

#### Step 3: Expected Output

You should see output similar to:

```
=== First NLM Simulation ===

1. Creating brain configuration...

2. Creating and initializing brain...
   - Brain created with 100 neurons
   - 1234 synapses connected

3. Running simulation (100 steps)...
   - Step 0: 0 neurons firing
   - Step 20: 3 neurons firing
   - Step 40: 5 neurons firing
   - Step 60: 4 neurons firing
   - Step 80: 2 neurons firing
   - Step 100: 1 neurons firing

4. Simulation complete!
   - Total spikes: 128
   - Average firing rate: 1.28 Hz

   - Brain state saved

=== Simulation Complete ===
```

### Understanding the Output

- **Neurons firing**: The number of neurons currently generating action potentials
- **Total spikes**: The cumulative number of action potentials across all neurons
- **Average firing rate**: Average spikes per second across the entire network

### Tips for First-Time Users

1. **Start small**: Begin with 100 neurons, then increase
2. **Observe learning**: Enable plasticity to see synaptic changes
3. **Check parameters**: Verify neuron threshold and membrane potential settings
4. **Save checkpoints**: Always save brain state for later analysis

### Extending Your First Simulation

#### Add Input Stimulation

```python
# Inject current into specific neurons
brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 20.0)
```

#### Enable Different Plasticity Types

```python
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)
```

#### Monitor Specific Neuron Types

```python
print(f"Excitatory neurons: {brain.getNeuronCount(pynlm.NeuronType.Excitatory)}")
print(f"Inhibitory neurons: {brain.getNeuronCount(pynlm.NeuronType.Inhibitory)}")
```

## 3. Exploring the Python API

This section provides a comprehensive guide to the NLM Python API.

### Core Classes

#### Brain Class

The `Brain` class is the central component of any NLM simulation.

**Key Methods:**

```python
# Creation and Initialization
brain = pynlm.createBrain(config)
brain.initialize()

# Core Simulation
brain.step(step_number)           # Basic step
brain.step(step_number, time)     # Step with timestamp
brain.reset()                     # Reset to initial state

# Input/Output
brain.receiveSensoryInput(data)   # Inject sensory data
action = brain.produceAction()   # Get motor action

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate (Hz)
brain.getExcitationInhibitionRatio()  # E/I balance

# State Management
brain.save("checkpoint.bin")      # Save to file
brain.load("checkpoint.bin")      # Load from file

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

#### Config Class

The `Config` class manages all simulation parameters.

**Key Methods:**

```python
# Creation
config = pynlm.createDefaultConfig()

# Query
if config.has("brain.neuron_count"):
    value = config.get("brain.neuron_count")

# Modify
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")

# Enumerate
keys = config.getKeys()
```

#### AgentBrain Class

The `AgentBrain` class bridges the brain and world.

**Key Methods:**

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)

# Sensory Processing
agent.processSensoryInput(percept)

# Motor Decoding
motor_cmd = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
development_stage = agent.getDevelopmentalStage()

# Subsystem Control
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

#### SimpleWorld Class

The `SimpleWorld` class provides a simple 2D simulation environment.

**Key Methods:**

```python
# Creation
world = pynlm.createSimpleWorld()

# Configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Agent Interaction
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)

# State Access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# World Objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)

# Properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
```

### Enumerations

#### Neuron Types

```python
pynlm.NeuronType.Excitatory    # Standard excitatory neuron
pynlm.NeuronType.Inhibitory    # Inhibitory neuron
pynlm.NeuronType.Sensory       # Sensory neuron
pynlm.NeuronType.Motor         # Motor neuron
pynlm.NeuronType.Modulatory    # Neuromodulatory neuron
pynlm.NeuronType.Internal     # Internal regulatory neuron
```

#### Developmental Stages

```python
pynlm.DevelopmentalStage.Initial       # Starting state
pynlm.DevelopmentalStage.CriticalPeriod # Sensitive learning period
pynlm.DevelopmentalStage.Maturation    # Maturing brain
pynlm.DevelopmentalStage.Adult         # Fully developed
pynlm.DevelopmentalStage.Aging         # Age-related changes
```

#### Action Types

```python
pynlm.ActionType.MoveForward    # Move forward
pynlm.ActionType.TurnLeft       # Turn left
pynlm.ActionType.TurnRight      # Turn right
pynlm.ActionType.Interact       # Interact with object
pynlm.ActionType.Eat           # Consume resource
pynlm.ActionType.Sleep         # Rest/consolidate memory
pynlm.ActionType.Restart       # Reset simulation
```

#### World Object Types

```python
pynlm.WorldObjectType.Empty     # Empty space
pynlm.WorldObjectType.Resource  # Resource object
pynlm.WorldObjectType.Hazard    # Hazardous object
pynlm.WorldObjectType.Wall      # Wall/obstacle
pynlm.WorldObjectType.Marker    # Special marker
```

### Configuration Parameters

#### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

#### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP LTP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP LTD time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

#### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

### Example Script Collection

#### Minimal Working Example

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

#### Environment Interaction Example

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run episode
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

#### Complete Agent with Learning

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_agent_simulation(1000)
```

## 4. Understanding the NLM Architecture

This section provides a comprehensive overview of the NLM framework architecture, explaining how all the components work together to create a functional artificial brain.

### Core Architecture Philosophy

NLM follows a **closed-loop brain architecture** where neural activity, plasticity, memory, neuromodulation, and behavior continuously influence each other in a dynamic feedback loop.

```
WORLD → SENSORY INPUT → NEURAL PROCESSING → INTERNAL STATE
                                           ↓
                                         MEMORY / PREDICTION
                                           ↓
                                      MOTIVATION / NEUROMODULATION
                                           ↓
                                        ACTION SELECTION
                                           ↓
                                        MOTOR OUTPUT
                                           ↓
                                        WORLD CONSEQUENCE
                                           ↓
                                      REWARD / SURPRISE / ERROR
                                           ↓
                                       PLASTICITY
                                           ↓
                                      MEMORY / DEVELOPMENT
                                           ↓
                                      CHANGED BRAIN
                                           ↓
                                      CHANGED FUTURE BEHAVIOR
```

### Layered Architecture

NLM is organized into several layers, each with specific responsibilities:

#### Layer 1: Core Layer (Foundation)

| Component | Purpose | Location |
|-----------|---------|----------|
| **Types** | Strongly-typed identifiers (NeuronId, SynapseId, etc.) | `src/core/Types/` |
| **Config** | Configuration management system | `src/core/Config/` |
| **Random** | Deterministic random number generator | `src/core/Random/` |
| **Logger** | Structured logging system | `src/core/Logger/` |
| **SimulationClock** | Time management for simulation | `src/core/SimulationClock/` |

**The Core Layer provides the fundamental data structures and infrastructure for all other components.**

#### Layer 2: Brain Layer (Neural Processing)

| Component | Purpose | Location |
|-----------|---------|----------|
| **Neuron** | Individual neuron representation (LIF dynamics) | `src/brain/Neuron.hpp/cpp` |
| **Synapse** | Synaptic connection representation | `src/brain/Synapse.hpp/cpp` |
| **NeuralPopulation** | Group of neurons with shared properties | `src/brain/NeuralPopulation.hpp/cpp` |
| **NeuralRegion** | Brain region with multiple populations | `src/brain/NeuralRegion.hpp/cpp` |
| **Brain** | Central coordinator | `src/brain/Brain.hpp/cpp` |

**The Brain Layer implements the actual neural computation using biologically-inspired dynamics.**

#### Layer 3: Dynamics Layer (Neural Computation)

| Component | Purpose | Location |
|-----------|---------|----------|
| **NeuralDynamics** | Abstract base for neuron dynamics | `src/dynamics/NeuralDynamics.hpp/cpp` |
| **SpikeSystem** | Event-driven spike processing | `src/dynamics/SpikeSystem.hpp/cpp` |

**The Dynamics Layer handles the timing and propagation of neural signals.**

#### Layer 4: Plasticity Layer (Learning Mechanisms)

| Component | Purpose | Location |
|-----------|---------|----------|
| **PlasticityRule** | Abstract base for plasticity rules | `src/plasticity/PlasticityRule.hpp/cpp` |
| **Hebbian** | Hebbian learning rule | `src/plasticity/Hebbian.hpp/cpp` |
| **STDP** | Spike-Timing-Dependent Plasticity | `src/plasticity/STDP.hpp/cpp` |
| **StructuralPlasticity** | Synapse formation/removal | `src/plasticity/StructuralPlasticity.hpp/cpp` |

**The Plasticity Layer implements learning rules that modify synaptic connections based on activity.**

#### Layer 5: Development Layer (Brain Maturation)

| Component | Purpose | Location |
|-----------|---------|----------|
| **DevelopmentSystem** | Orchestrates development | `src/development/DevelopmentSystem.hpp/cpp` |
| **Synaptogenesis** | New synapse formation | `src/development/Synaptogenesis.hpp/cpp` |
| **Pruning** | Weak synapse removal | `src/development/Pruning.hpp/cpp` |
| **Maturation** | Adult property development | `src/development/Maturation.hpp/cpp` |

**The Development Layer implements age-related changes to the brain's structure and function.**

#### Layer 6: Neuromodulation Layer (Motivation & Reward)

| Component | Purpose | Location |
|-----------|---------|----------|
| **Neuromodulator** | Abstract base for neuromodulators | `src/neuromodulation/Neuromodulator.hpp/cpp` |
| **Dopamine** | Reward and reinforcement | `src/neuromodulation/Reward.hpp/cpp` |
| **Novelty** | Novelty detection | `src/neuromodulation/Novelty.hpp/cpp` |
| **Curiosity** | Exploration motivation | `src/neuromodulation/Curiosity.hpp/cpp` |
| **PredictionError** | Error signals | `src/neuromodulation/PredictionError.hpp/cpp` |

**The Neuromodulation Layer provides motivational signals that guide behavior and learning.**

#### Layer 7: Memory Layer (Information Storage)

| Component | Purpose | Location |
|-----------|---------|----------|
| **NeuralWorkingMemory** | Persistent activity for temporary storage | `src/memory/NeuralWorkingMemory.hpp/cpp` |
| **NeuralEpisodicMemory** | Experience encoding in neural patterns | `src/memory/NeuralEpisodicMemory.hpp/cpp` |
| **NeuralSemanticMemory** | Fact-based knowledge storage | `src/memory/NeuralSemanticMemory.hpp/cpp` |
| **NeuralProceduralMemory** | Skill-based knowledge storage | `src/memory/NeuralProceduralMemory.hpp/cpp` |
| **NeuralAssociativeMemory** | Experience-based associations | `src/memory/NeuralAssociativeMemory.hpp/cpp` |

**The Memory Layer provides multiple forms of memory storage for the brain.**

#### Layer 8: Prediction Layer (Forward Models)

| Component | Purpose | Location |
|-----------|---------|----------|
| **NeuralPrediction** | Temporal sequence learning | `src/prediction/NeuralPrediction.hpp/cpp` |
| **ActionConsequencePredictor** | Action effect prediction | `src/prediction/ActionConsequencePredictor.hpp/cpp` |

**The Prediction Layer implements forward models that predict future states.**

#### Layer 9: Cognition Layer (Higher Functions)

| Component | Purpose | Location |
|-----------|---------|----------|
| **NeuralPlanner** | Action sequence evaluation | `src/cognition/NeuralPlanner.hpp/cpp` |
| **ConceptFormation** | Pattern discovery without labels | `src/cognition/ConceptFormation.hpp/cpp` |

**The Cognition Layer implements higher-level cognitive functions.**

#### Layer 10: Sensory Layer (Input Processing)

| Component | Purpose | Location |
|-----------|---------|----------|
| **SensoryInput** | Abstract base for sensory processing | `src/sensory/SensoryInput.hpp/cpp` |
| **Vision** | Visual processing | `src/sensory/Vision.hpp/cpp` |
| **Audio** | Auditory processing | `src/sensory/Audio.hpp/cpp` |
| **InternalSignals** | Internal state representation | `src/sensory/InternalSignals.hpp/cpp` |

**The Sensory Layer processes external and internal inputs to the brain.**

#### Layer 11: Motor Layer (Output Generation)

| Component | Purpose | Location |
|-----------|---------|----------|
| **MotorSystem** | Motor command generation | `src/motor/MotorSystem.hpp/cpp` |
| **Action** | Motor action representation | `src/motor/Action.hpp/cpp` |

**The Motor Layer generates motor commands for interaction with the environment.**

#### Layer 12: Environment Layer (Simulation Context)

| Component | Purpose | Location |
|-----------|---------|----------|
| **Environment** | Abstract base for environments | `src/environment/Environment.hpp/cpp` |
| **Observation** | Environmental state representation | `src/environment/Observation.hpp/cpp` |
| **SimpleWorld** | Simple 2D environment implementation | `src/world/SimpleWorld.hpp/cpp` |

**The Environment Layer provides the context in which the brain operates.**

#### Layer 13: Agent Layer (Interface)

| Component | Purpose | Location |
|-----------|---------|----------|
| **AgentBrain** | Interface between brain and environment | `src/agent/AgentBrain.hpp/cpp` |
| **SensoryPercept** | Processed sensory information | `src/agent/SensoryPercept.hpp/cpp` |
| **AgentBody** | Agent's physical representation | `src/agent/AgentBody.hpp/cpp` |

**The Agent Layer provides the interface between neural processing and environmental interaction.**

#### Layer 14: Visualization Layer (Output)

| Component | Purpose | Location |
|-----------|---------|----------|
| **VisualizationInterface** | Visualization output | `src/visualization/VisualizationInterface.hpp/cpp` |

**The Visualization Layer provides output mechanisms for observing brain activity.**

#### Layer 15: Experiments Layer (Testing & Validation)

| Component | Purpose | Location |
|-----------|---------|----------|
| **Experiment** | Base class for experiments | `src/experiments/Experiment.hpp/cpp` |
| **ExperimentRunner** | Manages and runs experiments | `src/experiments/ExperimentRunner.hpp/cpp` |

**The Experiments Layer provides tools for testing and validating NLM functionality.**

### Phase 6: Integration Architecture

Phase 6 represents the final integration of all systems into a coherent artificial brain. The key innovations of Phase 6 include:

#### 1. Unified Brain Loop

The main simulation loop (`Brain::step()`) now integrates all systems:

```cpp
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // 1. Process pending delayed spikes (deliver synaptic input)
    // 2. Update all neurons (LIF dynamics)
    // 3. Detect spikes and schedule spike events
    // 4. Update working memory
    // 5. Apply neuromodulation effects
    // 6. Apply plasticity rules (STDP and Hebbian)
    // 7. Update episodic memory with current experience
    // 8. Update prediction system
    // 9. Update attention system
    // 10. Update concept formation
    // 11. Apply structural plasticity (synaptogenesis/pruning)
    // 12. Replay important memories
    // 13. Apply development effects
    // 14. Memory consolidation
    // 15. Checkpoint management
}
```

#### 2. Connected Memory Systems

- **Working Memory** stores firing neurons each step and influences attention
- **Episodic Memory** captures experiences and replays them for consolidation
- **Associative Memory** forms connections between patterns
- All memory systems integrate with neural processing

#### 3. Integrated Neuromodulation

- **Dopamine** affects neural excitability and plasticity
- **Curiosity** drives exploration behavior
- **Novelty** detects surprising events
- All neuromodulators coordinate with plasticity rules

#### 4. Central Prediction

- **Prediction system** integrated into brain loop
- **Prediction error** signals affect learning
- **Confidence tracking** implemented
- **Forward models** become central organizing principle

#### 5. Developmental Integration

- **Developmental stages** affect plasticity rates
- **Structural plasticity** modulated by age
- **Neural excitability** changes with development
- **Critical periods** emerge naturally

#### 6. Persistence and Replay

- **Checkpoint save/load** implemented
- **Brain state serialization** working
- **Episodic memory replay** during simulation
- **Memory consolidation** for important episodes
- **Sleep/rest cycle** integrated

### Architectural Patterns

#### Event-Driven Design

NLM uses an event-driven architecture where:

1. **Neurons** generate spikes asynchronously
2. **Synapses** propagate spikes with delays
3. **Plasticity rules** trigger based on spike timing
4. **Memory systems** update based on neural activity

This design enables efficient simulation of large neural networks.

#### Component Composition

Each component follows a composition pattern:

- **Central coordination** by the Brain class
- **Specialized processing** by dedicated components
- **Flexible configuration** through the Config system
- **Extensible architecture** for adding new features

#### Data Flow Patterns

Data flows through the system in a predictable pattern:

1. **Input**: World → Agent → Brain
2. **Processing**: Neural dynamics, memory, prediction
3. **Output**: Brain → Agent → World
4. **Learning**: Plasticity, neuromodulation, development

### Performance Considerations

#### Memory Management

- **Memory pools** for efficient allocation
- **Reference counting** for object lifetime
- **Cache-friendly data structures** for neural components
- **Pre-allocated buffers** for simulation steps

#### Computational Efficiency

- **Event-driven processing** reduces unnecessary computations
- **SIMD vectorization** for neuron updates
- **Parallel processing** for independent regions
- **Lazy evaluation** for optional features

#### Scalability

- **Modular design** allows scaling individual components
- **Configuration-driven** performance tuning
- **Distributed computing** support for large simulations
- **Memory-efficient algorithms** for 1M+ neuron simulations

### Software Engineering Practices

#### Code Organization

- **Header-only declarations** for fast compilation
- **Implementation separation** for maintainability
- **Dependency injection** for testability
- **Template metaprogramming** for performance

#### Testing and Validation

- **Unit tests** for individual components
- **Integration tests** for system interactions
- **Performance benchmarks** for validation
- **Regression testing** for stability

#### Documentation and Examples

- **API documentation** for all public interfaces
- **Tutorial series** for progressive learning
- **Example scripts** for common use cases
- **Reference documentation** for technical details

### Future Extensions

#### Planned Enhancements

1. **Multi-region architectures** with specialized functions
2. **Hybrid neural models** combining different neuron types
3. **Advanced learning algorithms** beyond STDP
4. **Real-time visualization** and monitoring
5. **Cloud-based distributed simulations**
6. **Hardware acceleration** with GPU support

#### Research Directions

1. **Biological realism** improvements
2. **Cognitive modeling** applications
3. **Social interaction** simulations
4. **Developmental psychology** studies
5. **Neurological disorder** modeling
6. **Mind-brain problem** investigations

### Summary

The NLM framework represents a comprehensive artificial brain architecture that integrates:

- **Real neural dynamics** using LIF neurons
- **Multiple learning mechanisms** (STDP, Hebbian, structural)
- **Complex memory systems** with multiple memory types
- **Advanced cognitive functions** including planning and concept formation
- **Biological motivation** through neuromodulation
- **Developmental processes** that change brain properties over time

This architecture enables the creation of artificial brains that can learn, remember, adapt, and interact with their environments in complex ways, providing a platform for researching artificial general intelligence and computational neuroscience.
