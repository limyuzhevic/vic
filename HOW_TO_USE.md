# NLM (Neural Learning Machine) - Comprehensive User Guide

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20 that implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This comprehensive guide covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API with detailed examples
4. Configuration options and advanced usage
5. Troubleshooting and performance optimization

## Part 1: Building the C++ Project from Source

### Prerequisites

#### System Requirements

| Component | Minimum Version | Recommended |
|-----------|----------------|-------------|
| C++ Compiler | GCC 10+, Clang 12+, or MSVC 2019+ | GCC 11+, Clang 14+ |
| CMake | 3.16 | 3.20+ |
| Python | 3.8 | 3.9+ |
| pybind11 | 2.11.0 | 2.11.0+ |

#### Operating System Support
- **Linux**: All major distributions
- **macOS**: Intel and ARM64 architectures
- **Windows**: Visual Studio 2019+ with C++ toolchain

### Build Steps

```bash
# Navigate to the project directory
mkdir nlm-workspace
cd nlm-workspace
git clone <repository-url>
cd nlm

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

After successful compilation, you will have the following targets:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install

# Build with performance optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON

# Build with verbose output for debugging
cmake .. -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON
```

### Build Troubleshooting

#### Common Build Issues

**Issue**: CMake cannot find Python
```bash
# Install Python development headers and build tools
# Ubuntu/Debian:
apt-get update
apt-get install python3-dev python3-pip build-essential

# macOS:
brew install python3

# Then reconfigure CMake
cmake .. -DPython_EXECUTABLE=$(which python3)
```

**Issue**: Missing required dependencies
```bash
# Ubuntu/Debian
./build_all.sh  # or run from README.md

# Or install individually
apt-get install cmake build-essential git
```

## Part 2: Installing the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy matplotlib

# Build and install in development mode
pip install -e .

# Verify installation
python -c "import pynlm; print(pynlm.__version__)"
```

### Using pyproject.toml (Modern Python Packaging)

The project uses scikit-build-core for modern Python packaging. No additional configuration needed.

```bash
# Install normally
pip install .

# Build only (without installing)
pip install build
python -m build

# Install from wheel
pip install dist/*.whl
```

### Installation Verification

```python
import pynlm

print(f"NLM Python version: {pynlm.__version__}")

# Create a simple brain to test functionality
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
print(f"Brain initialized successfully")
```

## Part 3: Using the NLM Python API

### Environment Setup

```python
import pynlm

# Set random seed for reproducible experiments
pynlm.setRandomSeed(42)

# Optional: Configure global logging
import logging
logging.basicConfig(level=logging.INFO)
```

### Basic Usage Examples

#### Example 1: Simple Brain Simulation

```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()

# Optionally customize configuration
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.1)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Run simulation
for step in range(100):
    brain.step(step)
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

#### Example 2: Complete Embodied Agent

```python
import pynlm

def run_learning_agent(num_steps=1000):
    """Run a complete learning agent with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    # 2. Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=30,
        height=30,
        visionWidth=12,
        visionHeight=12
    )
    world.reset()
    world.setAgentStart(15.0, 15.0)  # Center start position
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable all learning subsystems
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableStructuralPlasticity(True)  # Grow new connections
    agent.enableDevelopment(True)           # Brain matures over time
    agent.enableCuriosity(True)             # Explore novel stimuli
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update environment state
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
        
        # Get reward and apply neuromodulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development stage
        agent.updateDevelopment(0.1)
        
        # Print progress every 200 steps
        if step % 200 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Agent energy: {world.getAgentBody().getEnergy():.1f}")

# Run the simulation
run_learning_agent(1000)
```

#### Example 3: Multi-Agent World

```python
import pynlm

def run_multi_agent_experiment(agent_configs):
    """Run experiment with multiple interacting agents."""
    
    # Create shared world
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
    world.reset()
    world.setRandomSeed(123)
    
    # Create all agents
    agents = []
    for config in agent_configs:
        brain = pynlm.createBrain(config)
        brain.initialize()
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agents.append(agent)
    
    # Run simulation
    for step in range(500):
        world.update(0.1)
        
        # Each agent perceives and acts
        for i, agent in enumerate(agents):
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply learning
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)

# Create different agent configurations
agent_configs = []

# Conservative agent (fewer neurons, slower learning)
conservative_config = pynlm.createDefaultConfig()
conservative_config.set("brain.neuron_count", 500)
agent_configs.append(conservative_config)

# Aggressive agent (more neurons, faster learning)
aggressive_config = pynlm.createDefaultConfig()
aggressive_config.set("brain.neuron_count", 2000)
agent_configs.append(aggressive_config)

# Run experiment
run_multi_agent_experiment(agent_configs)
```

### Key Classes and Methods

#### Brain Class (`pynlm.Brain`)

The `Brain` class represents the core neural processing system implementing Leaky Integrate-and-Fire (LIF) dynamics.

**Constructor/Factory Methods:**
```python
# Create brain from configuration
brain = pynlm.createBrain(config)
```

**Initialization:**
```python
# Initialize brain state and connections
brain.initialize()
brain.reset()  # Reset to initial state
```

**Simulation:**
```python
# Process one simulation step
brain.step(step_number)                    # Basic step
brain.step(step_number, time)             # Step with timestamp
```

**Input/Output:**
```python
# Inject sensory data into brain
brain.receiveSensoryInput(sensory_input)

# Get motor action from brain
action = brain.produceAction()
```

**State Management:**
```python
# Save brain state to file
brain.save("checkpoint.bin")

# Load brain state from file
brain.load("checkpoint.bin")
```

**Statistics:**
```python
# Neural population statistics
brain.getTotalNeuronCount()               # Total neurons
brain.getTotalSynapseCount()              # Total synapses
brain.getFiringNeuronCount()             # Currently firing
brain.getAverageFiringRate()              # Average firing rate
brain.getExcitationInhibitionRatio()    # E/I balance

# Developmental state
brain.getDevelopmentalStage()            # Current developmental phase
brain.getDevelopmentalAge()              # Age in simulation time
```

**Region Management:**
```python
# Create neural regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

#### Config Class (`pynlm.Config`)

The `Config` class manages NLM configuration parameters.

**Creation:**
```python
# Create configuration from defaults
config = pynlm.createDefaultConfig()
```

**Query:**
```python
# Check if parameter exists
if config.has("brain.neuron_count"):
    keys = config.getKeys()
```

**Modification:**
```python
# Set configuration parameters
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)
config.set("plasticity.stdp.enable", True)
```

**File I/O:**
```python
# Save configuration to file
config.saveToFile("config.json")

# Load configuration from file
config.loadFromFile("config.json")
```

#### AgentBrain Class (`pynlm.AgentBrain`)

The `AgentBrain` class serves as the integration layer between the neural brain and the embodied world.

**Constructor/Factory Methods:**
```python
# Create agent interface
agent = pynlm.createAgentBrain(brain)
```

**Initialization:**
```python
# Initialize with world
agent.initialize(world)
```

**Sensory Processing:**
```python
# Convert world percepts to neural input
agent.processSensoryInput(percept)
```

**Motor Decoding:**
```python
# Convert neural activity to motor commands
motor_cmd = agent.decodeMotorCommand()
```

**Neuromodulation:**
```python
# Apply learning signals
agent.applyRewardModulation(reward, predicted_reward)
agent.applyCuriosity(novelty, prediction_error)
```

**Development:**
```python
# Update developmental stage
agent.updateDevelopment(timestep)

# Get developmental information
agent.getDevelopmentalStage()
```

**Subsystem Control:**
```python
# Enable/disable subsystems
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)
agent.enablePrediction(True/False)
agent.enableMemory(True/False)
```

**Statistics:**
```python
# Get subsystem states
agent.getCuriosityLevel()           # Curiosity motivation
agent.getNoveltyLevel()             # Novelty detection
agent.getPredictionError()          # Prediction error
agent.getNeuromodulationLevel()     # Overall neuromodulation
agent.getLearningRate()             # Current learning rate
```

#### SimpleWorld Class (`pynlm.SimpleWorld`)

The `SimpleWorld` class represents the embodied environment where agents operate.

**Constructor/Factory Methods:**
```python
# Create simple 2D world
world = pynlm.createSimpleWorld()
```

**Configuration:**
```python
# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)
```

**Control:**
```python
# Control environment state
world.reset()
world.update(timestep)
world.setRandomSeed(42)
```

**Agent Interaction:**
```python
# Set agent starting position
world.setAgentStart(x, y)

# Execute agent's action
world.applyMotorCommand(motor_cmd, current_time)
```

**State Access:**
```python
# Get sensory percept
percept = world.getSensoryPercept()

# Get agent body information
body = world.getAgentBody()
```

**World Object Management:**
```python
# Add world objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))

# Remove world objects
world.removeObject(x, y)

# Check position validity
world.isValidPosition(x, y)
```

**Properties:**
```python
# World properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()

# Modify properties
world.setMaxEnergy(100.0)
```

### Enumerations

#### Neuron Types (`pynlm.NeuronType`)

```python
# Neuron classification
pynlm.NeuronType.Excitatory    # Excitatory neurons (positive feedback)
pynlm.NeuronType.Inhibitory    # Inhibitory neurons (negative feedback)
pynlm.NeuronType.Sensory       # Sensory neurons (receive external input)
pynlm.NeuronType.Motor         # Motor neurons (produce actions)
pynlm.NeuronType.Modulatory    # Neuromodulatory neurons (regulate plasticity)
pynlm.NeuronType.Internal     # Internal neurons (interneurons)
```

#### Developmental Stages (`pynlm.DevelopmentalStage`)

```python
# Developmental phases
pynlm.DevelopmentalStage.Initial         # Newborn brain
pynlm.DevelopmentalStage.CriticalPeriod # Enhanced learning period
pynlm.DevelopmentalStage.Maturation     # Rapid development
pynlm.DevelopmentalStage.Adult         # Mature capabilities
pynlm.DevelopmentalStage.Aging         # Age-related decline
```

#### Action Types (`pynlm.ActionType`)

```python
# Available motor actions
pynlm.ActionType.MoveForward    # Move forward in direction of heading
pynlm.ActionType.TurnLeft       # Rotate counterclockwise
pynlm.ActionType.TurnRight      # Rotate clockwise
pynlm.ActionType.Interact       # Interact with nearby objects
pynlm.ActionType.Eat            # Consume food/resources
pynlm.ActionType.Sleep         # Rest and consolidate memories
```

#### World Object Types (`pynlm.WorldObjectType`)

```python
# Types of objects in the world
pynlm.WorldObjectType.Empty      # Empty space
pynlm.WorldObjectType.Resource   # Consumable resource
pynlm.WorldObjectType.Hazard     # Dangerous object
pynlm.WorldObjectType.Wall       # Physical barrier
pynlm.WorldObjectType.Marker     # Goal/waypoint
```

## Part 4: Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons in brain |
| `brain.synapse_density` | float | 0.1 | Probability of connection between neurons |
| `brain.connection_probability` | float | 0.05 | Synapse connectivity density |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable Spike-Timing-Dependent Plasticity |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant for LTP (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant for LTD (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.hebbian.learning_rate` | float | 0.001 | Hebbian learning rate |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |
| `plasticity.structural.rate` | float | 0.01 | Rate of structural plasticity events |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.dopamine.baseline` | float | 0.1 | Baseline dopamine level |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.curiosity.sensitivity` | float | 1.0 | Sensitivity to novelty |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.novelty.threshold` | float | 0.5 | Novelty detection threshold |

### Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.critical_period` | float | 100.0 | Length of critical period (simulation steps) |
| `development.maturation_rate` | float | 0.01 | Rate of maturation during development |
| `development.plasticity_decay` | float | 0.001 | Rate at which plasticity decreases with age |
| `development.synaptogenesis_rate` | float | 0.05 | Rate of new synapse formation |
| `development.pruning_rate` | float | 0.02 | Rate of synapse removal |

## Part 5: Example Scripts

### Example 1: Minimal Working Example

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simple simulation
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### Example 2: Complete Learning Environment

```python
import pynlm

def create_and_run_experiment(neurons=1000, steps=1000):
    """Create a complete learning environment and run experiment."""
    
    # Setup brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neurons)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Setup world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=40,
        height=40,
        visionWidth=16,
        visionHeight=16
    )
    world.reset()
    world.setAgentStart(20.0, 20.0)
    
    # Setup agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
    
    return brain, world, agent

# Run experiment
brain, world, agent = create_and_run_experiment(500, 500)
print(f"Experiment completed with {brain.getTotalNeuronCount()} neurons")
```

### Example 3: Development Study

```python
import pynlm

def study_development_across_ages():
    """Study how learning changes across developmental stages."""
    
    # Create brain with development enabled
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 800)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create environment
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable development
    agent.enableDevelopment(True)
    agent.enableRewardModulation(True)
    
    # Track development over time
    developmental_stages = []
    learning_rates = []
    
    for step in range(1000):
        world.update(0.1)
        
        # Record current state
        developmental_stages.append(brain.getDevelopmentalStage())
        learning_rates.append(agent.getLearningRate())
        
        # Process and learn
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Print development milestones
        if step % 100 == 0:
            stage = brain.getDevelopmentalStage()
            age = brain.getDevelopmentalAge()
            print(f"Step {step}: Age {age:.1f}, Stage {stage}, "
                  f"Learning rate {agent.getLearningRate():.4f}")
    
    return developmental_stages, learning_rates

# Run development study
stages, rates = study_development_across_ages()
```

## Part 6: Advanced Usage

### Error Handling and Best Practices

```python
import pynlm
import traceback

def safe_simulation(brain, world, agent, max_steps=1000):
    """Run simulation with proper error handling."""
    
    try:
        for step in range(max_steps):
            # Check if brain is still valid
            if brain is None:
                print("Brain became invalid - stopping simulation")
                break
            
            # Check world state
            if world.getSimulationTime() > 1000:  # Prevent infinite time
                print("World time limit reached")
                break
            
            # Normal simulation steps
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            if percept is None:
                print(f"Warning: No percept at step {step}")
                continue
                
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Check for agent death
            if world.getAgentBody().getEnergy() <= 0:
                print("Agent died - simulation ended")
                break
                
    except Exception as e:
        print(f"Simulation error: {e}")
        traceback.print_exc()
        return False
    
    print(f"Simulation completed {step} steps successfully")
    return True
```

### Performance Optimization

```python
import pynlm
import time

def profile_simulation_performance(brain, world, agent, steps=100):
    """Profile simulation performance and identify bottlenecks."""
    
    # Time individual components
    times = {
        'world_update': 0,
        'sensory_input': 0,
        'brain_step': 0,
        'motor_output': 0
    }
    
    for step in range(steps):
        # Time world update
        start = time.time()
        world.update(0.1)
        times['world_update'] += time.time() - start
        
        # Time sensory processing
        start = time.time()
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        times['sensory_input'] += time.time() - start
        
        # Time brain step
        start = time.time()
        brain.step(step)
        times['brain_step'] += time.time() - start
        
        # Time motor output
        start = time.time()
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        times['motor_output'] += time.time() - start
    
    # Print performance summary
    total_time = sum(times.values())
    print(f"Performance over {steps} steps:")
    for component, t in times.items():
        percentage = (t / total_time) * 100
        print(f"  {component}: {t:.3f}s ({percentage:.1f}%)")
    
    print(f"Total time: {total_time:.3f}s")
    print(f"Steps per second: {steps / total_time:.1f}")
    
    return times
```

### Debugging Utilities

```python
import pynlm

def debug_brain_state(brain, world, agent, step):
    """Debug current brain state with comprehensive information."""
    
    print(f"=== DEBUG STATE at step {step} ===")
    
    # Brain state
    print(f"Brain State:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Excitatory/inhibitory ratio: {brain.getExcitationInhibitionRatio():.2f}")
    print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    print(f"  Developmental age: {brain.getDevelopmentalAge():.1f}")
    
    # Agent state
    print(f"\nAgent State:")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Prediction error: {agent.getPredictionError():.3f}")
    print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    
    # World state
    print(f"\nWorld State:")
    print(f"  Simulation time: {world.getSimulationTime():.1f}")
    body = world.getAgentBody()
    if body:
        print(f"  Agent energy: {body.getEnergy():.1f}")
    
    percept = world.getSensoryPercept()
    if percept:
        print(f"  Sensory input available: Yes")
        # Try to get internal values if available
        internal = percept.getInternal()
        if internal:
            print(f"  Internal signal: {internal[0]:.3f}")
    else:
        print(f"  Sensory input available: No")
    
    print(f"=== END DEBUG STATE ===\n")

# Usage example
debug_brain_state(brain, world, agent, step)
```

## Part 7: Troubleshooting

### Common Runtime Issues

**Issue 1: ImportError - No module named 'pynlm'**
**Solution:**
```bash
# Uninstall any existing version
pip uninstall pynlm

# Force reinstall from source
pip install --force-reinstall -e .

# Verify installation
python -c "import pynlm; print(pynlm.__version__)"
```

**Issue 2: Brain crashes with segmentation fault**
**Solution:**
```python
import pynlm

# Add debugging before critical operations
config = pynlm.createDefaultConfig()
print(f"Creating brain with {config.get('brain.neuron_count')} neurons")

brain = pynlm.createBrain(config)
print("Brain created successfully")

brain.initialize()
print("Brain initialized successfully")

# Test with small steps first
for i in range(5):
    brain.step(i)
    print(f"Step {i} completed")
```

**Issue 3: Memory usage too high**
**Solution:**
```python
import pynlm

# Create brain with lower neuron count
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # Reduced from default 1000

brain = pynlm.createBrain(config)
brain.initialize()

# Monitor memory usage
import psutil
process = psutil.Process()
print(f"Memory usage: {process.memory_info().rss / 1024 / 1024:.1f} MB")
```

### Build Troubleshooting

**Issue 1: CMake cannot find Python**
```bash
# Check Python installation
which python3
python3 --version

# Ensure Python development headers are installed
apt-get install python3-dev  # Ubuntu/Debian
brew install python3        # macOS

# Set Python executable explicitly
cmake .. -DPython_EXECUTABLE=$(which python3)
```

**Issue 2: Compiler compatibility problems**
```bash
# Check compiler version
g++ --version

# Try different CMake build types
# Debug build with symbols
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Release build with optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release

# Try different compilers if available
cmake .. -DCMAKE_CXX_COMPILER=clang++
```

## Part 8: Further Reading

### Related Documentation

- **[ARCHITECTURE.md](docs/ARCHITECTURE.md)** - System architecture overview
- **[SCIENCE.md](docs/SCIENCE.md)** - Scientific background and biological inspiration
- **[EXPERIMENTS.md](docs/EXPERIMENTS.md)** - Experiment protocols and results
- **[LIMITATIONS.md](docs/LIMITATIONS.md)** - Known limitations and approximations
- **[ROADMAP.md](docs/ROADMAP.md)** - Development roadmap and status
- **[PERFORMANCE.md](docs/PERFORMANCE.md)** - Performance infrastructure and optimization
- **[BIOLOGICAL_FIDELITY.md](docs/BIOLOGICAL_FIDELITY.md)** - Biological modeling details
- **[RESEARCH_METHODS.md](docs/RESEARCH_METHODS.md)** - Scientific methodology
- **[FAILURES.md](docs/FAILURES.md)** - Documented experiment failures and lessons learned

### External Resources

- **[NLM GitHub Repository](https://github.com/nlm-project/nlm)** - Source code and issues
- **[NLM Documentation](https://nlm-project.github.io/docs/)** - Online documentation
- **[Scientific Publications](https://scholar.google.com/scholar?q=NLM+brain)** - Related research papers

### Community and Support

- **GitHub Issues**: Report bugs and request features
- **Documentation**: Submit pull requests to improve this guide
- **Experiments**: Share your experimental results and findings
- **Development**: Contribute to Phase 6 integration

---

**Last Updated**: 2026-10-09
**Version**: Phase 6 Integration
**Status**: Active Development

This guide is continuously updated as NLM Phase 6 integration progresses. For the latest features and fixes, refer to the online documentation.
