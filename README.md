# NLM (熙然) - Neural Learning Machine

**An Experimental Artificial Developmental Brain**

## Quick Start

If you want to **get up and running quickly**:

```python
import pynlm

# Create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Make it think
for i in range(100):
    brain.step(i)

print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
```

For the **complete integrated brain experience** with all systems:

```bash
./nlm  # Phase 6 integration demo
```

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

### Key Features

- **Real Spiking Neurons**: Leaky Integrate-and-Fire (LIF) neurons with biological constraints
- **Multiple Memory Systems**: Working memory, episodic memory, semantic memory, procedural memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Predictive Processing**: Brain predicts sensory consequences
- **Development**: Brain matures and adapts over time
- **Learning**: Multiple plasticity rules (STDP, Hebbian, reward-modulated)

### What NLM IS NOT

- NOT a transformer or LLM
- NOT a chatbot
- NOT a deep learning model
- NOT a pretrained AI system

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### What Phase 6 Achieves

✅ **Memory Integration**: Working memory, episodic memory, and associative memory connected to neural processing

✅ **Neuromodulation Integration**: Dopamine affects neural excitability and plasticity, curiosity drives exploration

✅ **Prediction System**: Prediction errors affect learning, confidence tracking implemented

✅ **Cognition Systems**: Neural planning, concept formation, attention, and more

✅ **Development Integration**: Developmental stages affect plasticity rates

✅ **Persistence**: Checkpoint save/load implemented

✅ **Replay and Consolidation**: Episodic memory replay during simulation

## Building from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Steps

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | **Phase 6 Integration Demo** (Main executable) |
| `nlm_phase6_demo` | Executable | Alternative Phase 6 demo with detailed logging |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |

## Running

### Phase 6 Integration Demo (Recommended)

```bash
./nlm
```

This runs the comprehensive Phase 6 integration test that verifies:
- All brain systems are connected
- Memory integration works
- Neuromodulation affects neural dynamics
- Development affects plasticity
- Checkpointing and replay function

### Alternative Phase 6 Demo

```bash
./nlm_phase6_demo
```

This provides more detailed logging and extended testing of individual systems.

### Python Bindings (pynlm)

NLM provides comprehensive Python bindings through `pynlm`, a Python package built with pybind11 that exposes the entire C++ neural simulation framework to Python.

#### What You Get

The Python bindings provide access to all NLM brain systems:

- **Brain System**: Create and simulate neural networks with LIF neurons
- **Memory Systems**: Working memory, episodic memory, semantic memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Prediction System**: Predictive coding and planning
- **Cognition**: Neural planning, concept formation
- **Agent Framework**: Bridge between brain and environment
- **World Simulation**: Simple physics-based environments

#### Installation

**From Source (Recommended for Development)**

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install the Python bindings
pip install .

# Or install in development mode (can modify source code)
pip install -e .
```

**Using Pre-built Wheels**

If you prefer not to build from source:

```bash
pip install pynlm
```

**Verify Installation**

```python
import pynlm

print(f"NLM Python bindings version: {pynlm.__version__}")

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Brain has {brain.getTotalSynapseCount()} synapses")
```

#### Basic Python API

**Creating and Running a Brain**

```python
import pynlm

# 1. Create configuration
config = pynlm.createDefaultConfig()

# 2. Create brain
brain = pynlm.createBrain(config)

# 3. Initialize
brain.initialize()

# 4. Run simulation
for step in range(100):
    brain.step(step)
    
print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
```

**Working with Agents and Worlds**

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Initialize agent with world
agent.initialize(world)

# Run simulation
for step in range(100):
    # Environment update
    world.update(0.1)
    
    # Sensory processing
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain computation
    brain.step(step)
    
    # Action execution
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
```

**Complete Example with Learning**

```python
import pynlm

def run_complete_simulation(steps=500):
    """Run a complete simulation with learning."""
    
    # 1. Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # 2. Enable learning subsystems
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableCuriosity(True)             # Explore new things
    agent.enableDevelopment(True)           # Brain matures over time
    agent.enableStructuralPlasticity(True)  # Grow new connections
    
    # 3. Run simulation
    for step in range(steps):
        # Environment update
        world.update(0.1)
        
        # Sensory processing
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Action execution
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development: {brain.getDevelopmentalStage().name}")
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_complete_simulation(500)

print("\nSimulation complete!")
print(f"Final stats:")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
print(f"  Avg firing rate: {brain.getAverageFiringRate():.3f}")
print(f"  Final curiosity: {agent.getCuriosityLevel():.3f}")
print(f"  Development stage: {brain.getDevelopmentalStage().name}")
```

#### Key Classes (Python API)

**Brain Class**

```python
# Creation and initialization
brain = pynlm.createBrain(config)
brain.initialize()

# Simulation
brain.step(step_number)
brain.step(step_number, timestamp)
brain.reset()

# Input/output
brain.receiveSensoryInput(sensory_input)
action = brain.produceAction()

# Statistics
brain.getTotalNeuronCount()           # Total neurons
brain.getTotalSynapseCount()          # Total synapses
brain.getFiringNeuronCount()         # Currently firing
brain.getAverageFiringRate()          # Average firing rate
brain.getExcitationInhibitionRatio() # E/I balance

# State
brain.save("checkpoint.bin")           # Save state
brain.load("checkpoint.bin")           # Load state

# Development
brain.getDevelopmentalStage()         # Current development stage
brain.updateDevelopment(timestep)      # Advance development
```

**Config Class**

```python
# Create
config = pynlm.createDefaultConfig()

# Query
if config.has("brain.neuron_count"):
    value = config.get("brain.neuron_count")

# Modify
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

**AgentBrain Class**

```python
# Creation and initialization
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
motor_cmd = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Subsystem control
agent.enableRewardModulation(True/False)
agent.enableCuriosity(True/False)
agent.enableDevelopment(True/False)
agent.enableStructuralPlasticity(True/False)

# Statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

**SimpleWorld Class**

```python
# Creation
world = pynlm.createSimpleWorld()

# Configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control
world.reset()
world.update(timestep)

# Agent interaction
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# World objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
```

**Enumerations**

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Internal

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging

# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

## Project Structure

```
NLM/
├── src/
│   ├── core/           # Core utilities (config, random, logging)
│   ├── brain/          # Neural components (LIF neurons, synapses)
│   ├── dynamics/       # Neural dynamics and spike propagation
│   ├── plasticity/     # Plasticity rules (STDP, Hebbian)
│   ├── development/    # Developmental system
│   ├── neuromodulation/ # Neuromodulators (dopamine, curiosity)
│   ├── memory/        # Memory systems (working, episodic)
│   ├── prediction/     # Prediction systems
│   ├── cognition/      # Cognitive mechanisms
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/ # Visualization
├── configs/            # Configuration files
├── docs/              # Scientific documentation
└── tests/             # Unit tests and benchmarks
```

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Key Classes (C++ API)

### Brain Class

**Creation and initialization**
```cpp
auto config = std::make_shared<Config>();
auto brain = std::make_shared<Brain>(config);
brain->initialize();
```

**Simulation**
```cpp
brain->step(step, time);
brain->reset();
```

**Input/output**
```cpp
brain->receiveSensoryInput(sensoryInput);
auto action = brain->produceAction();
```

**Statistics**
```cpp
brain->getTotalNeuronCount();
brain->getTotalSynapseCount();
brain->getFiringNeuronCount();
brain->getAverageFiringRate();
brain->getExcitationInhibitionRatio();
```

**State**
```cpp
brain->save("checkpoint.bin");
brain->load("checkpoint.bin");
```

### Memory Systems

**Working Memory**
```cpp
auto* workingMemory = brain->getWorkingMemory();
workingMemory->store(workingMemoryId, memoryTrace);
auto trace = workingMemory->recall(workingMemoryId);
```

**Episodic Memory**
```cpp
auto* episodicMemory = brain->getEpisodicMemory();
episodicMemory->store(episode);
episodicMemory->replay(episodeId);
```

### Neuromodulation Systems

**Dopamine**
```cpp
auto* dopamine = brain->getDopamine();
dopamine->modulate(excitability, plasticity);
```

**Curiosity**
```cpp
auto* curiosity = brain->getCuriosity();
curiosity->driveExploration();
```

### Development System

```cpp
brain->updateDevelopment(timestep);
auto stage = brain->getDevelopmentalStage();
```

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 6 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 DOES investigate:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## Architecture Philosophy

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## License

MIT

## Authors

Research project — See docs for scientific background.
