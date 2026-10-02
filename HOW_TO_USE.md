# NLM (Neural Learning Machine) - COMPREHENSIVE HOW TO USE

## Overview

NLM is a **brain-inspired spiking neural network simulator** written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API
4. Advanced features and Phase 4 cognitive capabilities
5. Complete example programs and tutorials

---

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Steps

#### Step 1: Navigate to Project Directory
```bash
# Navigate to the NLM project directory
cd /path/to/nlm
```

#### Step 2: Create Build Directory
```bash
# Create a build directory
mkdir -p build
cd build
```

#### Step 3: Configure CMake
```bash
# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# For development with debugging symbols
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Custom compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Custom installation prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
```

#### Step 4: Build All Targets
```bash
# Build all targets using available CPU cores
make -j$(nproc)

# For detailed output
make -j4

# Build specific target
make nlm
make nlm_phase6_demo
make nlm_test
```

#### Step 5: Run Tests (Optional)
```bash
# Run unit tests
ctest --output-on-failure

# Run specific test
./nlm_test --gtest_filter="*TestCaseName*"
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_phase6_demo` | Executable | Phase 6 integration test |
| `nlm_test` | Executable | Unit test suite |

### Build Options

#### Debug vs Release Builds
```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Release build (optimized, no debug symbols)
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### Python Binding Configuration
```bash
# Ensure Python is available
cmake .. -DPython_EXECUTABLE=$(which python)

# Or specify full path
cmake .. -DPython_EXECUTABLE=/usr/bin/python3
```

---

## Part 2: Installing the Python Library

### Method 1: From Source (Recommended for Development)

#### Step 1: Install Build Dependencies
```bash
# Install Python package dependencies
pip install scikit-build-core pybind11 pytest numpy

# Install build tools
pip install build
```

#### Step 2: Build and Install
```bash
# Install the package
cd /path/to/nlm
pip install .

# Or install in development mode (recommended for development)
pip install -e .

# Build only (without installing)
pip install build
python -m build
```

#### Step 3: Verify Installation
```python
# Test the import
import pynlm
print(f"pynlm version: {pynlm.__version__}")

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Total neurons: {brain.getTotalNeuronCount()}")
```

### Method 2: Using pyproject.toml

The project uses modern `pyproject.toml` with scikit-build-core:

```toml
[build-system]
requires = ["scikit-build-core>=0.5.0", "pybind11>=2.11.0"]
build-backend = "scikit_build_core.build"
```

#### Build and Install
```bash
# Install using pip (automatically handles build)
pip install .

# Build to wheel file
pip install build
python -m build

# Install from wheel
pip install dist/pynlm-0.1.0-py3-none-manylinux_1_x86_64.whl
```

### Method 3: Development Installation

```bash
# Clone the repository
cd /path/to/nlm

# Install in development mode
pip install -e .

# Make changes and they will be reflected immediately
# Reinstall if needed
pip install -e --force-reinstall .
```

---

## Part 3: Using the Python Library

### Section 1: Basic Usage

#### Creating and Configuring a Brain

**Example 1: Minimal Setup**
```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

**Example 2: Custom Configuration**
```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Customize parameters
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("neuromod.dopamine.scale", 1.5)

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()

# Verify configuration
print(f"Neuron count: {brain.getTotalNeuronCount()}")
print(f"Synapse count: {brain.getTotalSynapseCount()}")
```

#### Running a Simulation

**Example 1: Simple Brain Activity**
```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
        print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

**Example 2: Brain with Sensory Input**
```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation with world interaction
for step in range(500):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Run brain
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Action: {action}")
        print(f"  Reward: {reward:.3f}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
```

#### Creating and Configuring a World

**Example 1: Simple 2D World**
```python
import pynlm

# Create a simple 2D world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8,
    visionFOV=90.0  # Field of view in degrees
)

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Add objects to world
world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0))
world.addObject(pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Hazard, value=1.0))

# Reset world
world.reset()

# Run world update
world.update(timestep=0.1)

print(f"World size: {world.getWidth()} x {world.getHeight()}")
print(f"Agent position: ({world.getAgentX()}, {world.getAgentY()})")
```

**Example 2: Complex World Configuration**
```python
import pynlm

# Create world with multiple zones
world = pynlm.createSimpleWorld()
world.configure(
    width=30,
    height=30,
    visionWidth=12,
    visionHeight=12,
    visionFOV=90.0,
    maxEnergy=100.0,
    energyRegeneration=1.0
)

# Create different terrain types
# Empty space
world.addObject(pynlm.WorldObject(0.0, 0.0, pynlm.WorldObjectType.Empty))

# Resource zone (high value)
for x in range(5, 15):
    for y in range(5, 15):
        world.addObject(pynlm.WorldObject(
            float(x), float(y), 
            pynlm.WorldObjectType.Resource, 
            value=5.0
        ))

# Hazard zone (negative value)
for x in range(20, 30):
    for y in range(20, 30):
        world.addObject(pynlm.WorldObject(
            float(x), float(y),
            pynlm.WorldObjectType.Hazard,
            value=-2.0
        ))

world.setAgentStart(15.0, 15.0)
world.reset()
```

### Section 2: Complete Agent Example

#### Example 1: Basic Agent with All Features

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration with advanced settings
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("neuromod.dopamine.scale", 1.2)
    
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
    
    # 6. Enable ALL subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Set up advanced capabilities
    agent.setPlanningDepth(3)
    agent.setCurrentGoal([0.8, 0.8, 0.0])
    
    # 8. Run simulation loop
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
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Action: {motor_cmd}")
            print(f"  Reward: {reward:.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Planning confidence: {agent.getPlanningConfidence():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_agent_simulation(1000)
```

#### Example 2: Working Memory for Sequential Tasks

```python
import pynlm

# Create brain with working memory
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access working memory system
working_memory = brain.getWorkingMemory()
working_memory.setCapacity(10)
working_memory.setDecayRate(0.01)

# Store a sequence of actions
sequence = [
    pynlm.ActionType.MoveForward,
    pynlm.ActionType.TurnLeft,
    pynlm.ActionType.Interact,
    pynlm.ActionType.Eat
]

print("Storing action sequence in working memory...")
for i, action in enumerate(sequence):
    # Encode action with contextual information
    pattern = [
        float(action.value),  # Action type
        float(i) / len(sequence),  # Position in sequence
        1.0  # Strength
    ]
    working_memory.store(pattern, strength=1.0)
    print(f"  Stored action {action} at position {i}")

# Use working memory for planning
retrieved_pattern = working_memory.retrieve()
print(f"\nRetrieved working memory pattern: {retrieved_pattern}")
print(f"Memory activity: {working_memory.getMemoryActivity():.3f}")
print(f"Active traces: {working_memory.getActiveTraces()}")

# Demonstrate competition
working_memory.runCompetition()
winners = working_memory.getWinners()
print(f"\nCompetition winners: {winners}")
```

#### Example 3: Self-Model Development

```python
import pynlm

# Create agent system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable self-model development
self_model = brain.getSelfModel()

print("Developing self-model through sensorimotor experience...")

# Run sequence to develop self-model
for step in range(400):
    world.update(0.1)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get sensory states before and after action
    before_state = world.getSensoryPercept()
    
    # Perform action and get after state
    agent.processSensoryInput(before_state)
    brain.step(step)
    after_state = world.getSensoryPercept()
    
    # Record action-effect relationship
    self_model.recordSelfAction(action, before_state, after_state)
    
    if step % 100 == 0:
        confidence = self_model.getSelfModelConfidence(action)
        print(f"  Step {step}: Action {action}")
        print(f"    Self-model confidence: {confidence:.3f}")
        print(f"    Body awareness: {self_model.getBodyAwareness():.3f}")
        print(f"    Capability level: {self_model.getCapabilityLevel():.3f}")

print(f"\nSelf-model development complete!")
print(f"Has self-model: {self_model.hasSelfModel()}")
print(f"Number of learned action effects: {len(self_model.getActionEffects())}")
```

#### Example 4: Social Learning

```python
import pynlm

# Create two agents for social learning demonstration
brain1 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())

brain1.initialize()
brain2.initialize()

agent1 = pynlm.createAgentBrain(brain1)
agent2 = pynlm.createAgentBrain(brain2)

world1 = pynlm.createSimpleWorld()
world2 = pynlm.createSimpleWorld()

world1.configure(width=25, height=25, visionWidth=10, visionHeight=10)
world2.configure(width=25, height=25, visionWidth=10, visionHeight=10)

world1.reset()
world2.reset()

agent1.initialize(world1)
agent2.initialize(world2)

# Enable social learning
social1 = agent1.getSocialLearning()
social2 = agent2.getSocialLearning()

print("Demonstrating social learning...")

# Agent 2 performs observable actions
print("\nAgent 2 performing observable actions:")
for step in range(100):
    # Agent 2 moves around (observable to agent1)
    action2 = pynlm.ActionType.MoveForward
    world2.applyMotorCommand(action2, world2.getSimulationTime())
    
    # Agent 1 observes Agent 2's action
    # In a real scenario, Agent1 would observe Agent2's sensory input
    # and world state changes
    if step % 20 == 0:
        # Simulate observation
        state_before = world1.getSensoryPercept()
        # Agent1 sees Agent2's action
        agent1.observeAgentAction(action2, state_before, world2.getSensoryPercept())
        
        print(f"  Step {step}: Agent 2 performed {action2}")
        print(f"    Agent 1 observation count: {social1.getObservationCount()}")
        print(f"    Can imitate action: {social1.canImitate(action2)}")

print(f"\nSocial learning results:")
print(f"  Agent 1 observations: {social1.getObservationCount()}")
print(f"  Agent 2 observations: {social2.getObservationCount()}")
print(f"  Agent 1 can imitate: {social1.canImitate(pynlm.ActionType.MoveForward)}")
```

#### Example 5: Prediction-Based Planning

```python
import pynlm

# Create agent with prediction system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(5.0, 5.0)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable planning
agent.setPlanningDepth(4)
agent.setCurrentGoal([0.7, 0.7, 0.0])  # Target location

print("Running prediction-based planning...")
print(f"Goal: {agent.getCurrentGoal()}")
print(f"Planning depth: {agent.getPlanningDepth()}")

# Current state
current_state = agent.getCurrentState()

for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Get planned action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Evaluate current plan
    if step % 50 == 0:
        plan = agent.evaluateSequence(
            [action, pynlm.ActionType.MoveForward, pynlm.ActionType.Interact],
            current_state
        )
        
        print(f"\nStep {step}:")
        print(f"  Current action: {action}")
        print(f"  Plan evaluation:")
        print(f"    Expected reward: {plan.expectedReward:.3f}")
        print(f"    Confidence: {plan.confidence:.3f}")
        print(f"    Depth: {plan.depth}")
        
        # Update plan quality based on actual outcome
        actual_reward = 1.0 if action == pynlm.ActionType.MoveForward else 0.0
        agent.updatePlanQuality([action], [action], actual_reward)
```

### Section 3: Key Classes and Methods

#### Brain Class

**Creation and Initialization**
```python
# Creation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)                    # Run with current time
brain.step(step_number, timestamp)          # Run with specific time
brain.reset()                              # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)    # Inject sensory data
action = brain.produceAction()             # Get motor action

# Statistics
brain.getTotalNeuronCount()                 # Total neurons
brain.getTotalSynapseCount()                # Total synapses
brain.getFiringNeuronCount()               # Currently firing
brain.getAverageFiringRate()                # Average firing rate
brain.getExcitationInhibitionRatio()       # E/I balance

# State
brain.save("checkpoint.bin")               # Save state
brain.load("checkpoint.bin")               # Load state

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)

# Advanced brain features
brain.getWorkingMemory()                    # Access working memory
brain.getPredictionSystem()                 # Access prediction system
brain.getPlanner()                          # Access planning system
brain.getSelfModel()                        # Access self-model
brain.getSocialLearning()                   # Access social learning
brain.getAttention()                        # Access attentional selection
brain.getDevelopmentSystem()                # Access development system
brain.getDopamine()                        # Access dopamine system
```

#### Config Class

**Configuration Management**
```python
# Create
config = pynlm.createDefaultConfig()

# Query
if config.has("brain.neuron_count"):
    keys = config.getKeys()
    print(f"All keys: {keys}")

# Modify
config.set("brain.neuron_count", 5000)
config.set("learning.rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")

# Advanced features
config.loadBatch(["config1.nlmconfig", "config2.nlmconfig"])
config.saveBatch(["backup1.nlmconfig", "backup2.nlmconfig"])

# Validation
errors = []
if config.validateConfig(errors):
    print("Configuration is valid")
else:
    print(f"Configuration errors: {errors}")
```

#### AgentBrain Class

**Agent-Brain Interface**
```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
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

# Subsystem enable/disable
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()

# Advanced agent features
agent.setCurrentGoal(goal)
agent.setPlanningDepth(depth)
agent.evaluateSequence(actions, state)
agent.updatePlanQuality(planned, actual, reward)
```

#### SimpleWorld Class

**World Management**
```python
# Creation
world = pynlm.createSimpleWorld()

# Configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Agent interaction
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# World objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)

# Properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)

# Advanced world features
world.addHazards()
world.addResources()
world.generateTerrain()
world.setBoundaries()
```

#### Enumerations

**Available Types**
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
pynlm.ActionType.TurnRight
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Sleep
pynlm.ActionType.Explore

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

### Section 4: Configuration Options

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
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

#### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

#### Advanced Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `working_memory.capacity` | int | 100 | Working memory capacity |
| `working_memory.decay_rate` | float | 0.01 | Working memory decay rate |
| `planning.depth` | int | 3 | Planning horizon depth |
| `planning.confidence_threshold` | float | 0.5 | Minimum planning confidence |
| `prediction.temporal_horizon` | float | 10.0 | Prediction time horizon |
| `development.plasticity_rate` | float | 0.1 | Development-dependent plasticity rate |

---

## Part 4: Example Scripts

### Example 1: Minimal Working Brain

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

### Example 2: Basic Agent-Environment Interaction

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

print("Agent episode complete!")
```

### Example 3: Advanced Agent with All Features

```python
import pynlm

# Complete advanced agent example
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)
config.set("plasticity.structural.enable", True)
config.set("neuromod.dopamine.scale", 1.5)

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
world.reset()
agent.initialize(world)

# Enable all advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Set up goal-directed behavior
agent.setCurrentGoal([0.5, 0.5, 1.0])  # Find resource
agent.setPlanningDepth(5)

# Run sophisticated agent
for step in range(1000):
    world.update(0.1)
    
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processing
    brain.step(step)
    
    # Planned action selection
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Learning from experience
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)
    
    # Progress monitoring
    if step % 200 == 0:
        print(f"Step {step}:")
        print(f"  Action: {action}")
        print(f"  Reward: {reward:.3f}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Development: {brain.getDevelopmentalStage()}")
        print(f"  Planning confidence: {agent.getPlanningConfidence():.3f}")

print("Advanced agent simulation complete!")
```

---

## Part 5: Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
# Install Python dependencies
pip install scikit-build-core pybind11

# Configure with Python path
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

**Import errors:**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Check configuration parameters for validity

### Performance Issues

**Slow simulation:**
- Reduce brain size: `config.set("brain.neuron_count", 500)`
- Disable structural plasticity if not needed
- Reduce planning depth: `agent.setPlanningDepth(2)`

**Memory issues:**
- Clear working memory periodically: `brain.getWorkingMemory().clear()`
- Save brain state instead of keeping in memory: `brain.save("checkpoint.bin")`

### Example Program Issues

**Agent doesn't learn:**
- Check that `agent.enableRewardModulation(True)` is called
- Verify that world provides reward signals
- Ensure sensory input is properly connected

**Planning doesn't work:**
- Check that `agent.setCurrentGoal()` is set
- Verify that `agent.setPlanningDepth()` is set
- Check that planner is properly initialized

---

## Part 6: Further Reading

### Recommended Documentation

1. **[ARCHITECTURE.md](docs/ARCHITECTURE.md)** - System architecture overview
2. **[SCIENCE.md](docs/SCIENCE.md)** - Scientific background and limitations
3. **[EXPERIMENTS.md](docs/EXPERIMENTS.md)** - Experiment descriptions and results
4. **[PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md)** - Phase 6 implementation audit

### Additional Resources

- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io
- **API Reference**: Generated from source code headers
- **Research Papers**: Available in project documentation

### Advanced Topics

- **Memory Systems**: Deep dive into working memory, episodic memory, and associative memory
- **Prediction Systems**: Understanding temporal sequence prediction and action consequences
- **Cognitive Architecture**: How attention, planning, and self-model integrate
- **Neuromodulation**: Role of dopamine, curiosity, and novelty in learning
- **Development**: Critical periods and developmental trajectory modeling
- **Performance Optimization**: Scaling techniques for large brain simulations

---

## Quick Start Guide

### For Beginners (5 minutes)

```python
import pynlm

# 1. Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# 2. Create simple agent
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
world.reset()
agent.initialize(world)

# 3. Run simulation
for step in range(50):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Done! Basic NLM simulation complete.")
```

### For Advanced Users (2 hours)

```python
import pynlm

# Create advanced brain with all features
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)
config.set("plasticity.structural.enable", True)
config.set("neuromod.dopamine.scale", 1.5)

brain = pynlm.createBrain(config)
brain.initialize()

# Create sophisticated agent
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()
agent.initialize(world)

# Enable all advanced features
agent.enableAllFeatures()
agent.setPlanningDepth(5)
agent.setCurrentGoal([0.7, 0.7, 1.0])

# Run complex simulation
brain, agent, world = run_agent_simulation(2000)

print("Advanced NLM simulation with all Phase 6 features complete!")
```

---

## Conclusion

This comprehensive HOW TO USE guide provides:

1. **Complete build instructions** for both C++ and Python components
2. **Detailed API documentation** for all major classes and methods
3. **Practical example programs** demonstrating basic to advanced usage
4. **Advanced feature examples** for Phase 4 cognitive capabilities
5. **Troubleshooting guidance** for common issues
6. **Performance optimization tips** for large-scale simulations

The NLM framework supports everything from simple neural simulations to sophisticated agents with working memory, prediction systems, and full cognitive architectures. Whether you're a beginner looking to understand brain-inspired computing or an advanced researcher implementing complex cognitive models, NLM provides the tools and documentation you need.

For the latest updates, bug reports, and community support, visit the NLM project documentation and GitHub repository.