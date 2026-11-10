# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API

---

## Part 1: Building the C++ Project from Source

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
```

---

## Part 2: Installing the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies (use a venv on PEP 668 systems, where
# system pip refuses to install packages)
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

> Phase 5 note: `pip install .` now genuinely builds `pynlm` (verified:
> fresh venv, `pip install .`, then `import pynlm` and the easy_usage
> Example 1 flow run). Previously the root CMakeLists never built the
> module, so `pip install .` succeeded yet `import pynlm` failed. A
> plain C++ `cmake` build still does NOT include pynlm unless you pass
> `-DNLM_BUILD_PYTHON_BINDINGS=ON` (and have pybind11 installed); the
> standalone `cmake -S python -B build` route also still works.

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Verifying Installation

```python
import pynlm

# Check version and system information
print(pynlm.version())
print(pynlm.buildInfo())

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

---

## Part 3: Using the Python Library

### Basic Usage

#### Creating a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters using the improved API
# Method 1: Direct setting
config.set("neuron_count", 1000)

# Method 2: Dictionary (new!)
config.setBatch({
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "stdp_ltp_weight": 0.01,
})

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Get brain statistics (new!)
stats = brain.getBrainStats()
print(f"Brain initialized: {stats['totalNeurons']} neurons, {stats['totalSynapses']} synapses")
```

#### Running a Simulation

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enhanced error handling
try:
    # Run simulation for 1000 steps
    for step in range(1000):
        brain.step(step)
        
        # Log statistics periodically
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
            
except Exception as e:
    print(f"Simulation error at step {step}: {e}")
```

#### Creating a Simple World

```python
import pynlm

# Create a simple 2D world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Run world update
world.update(timestep=0.1)
```

#### Complete Agent Example

```python
import pynlm

# Enhanced agent example with new features

def run_agent_simulation_enhanced(num_steps=1000):
    """Enhanced complete agent simulation with brain and world."""
    
    # 1. Create configuration with improved API
    config = pynlm.createDefaultConfig()
    config.setBatch({
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.15,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "plasticity.stdp.enable": True,
        "neuromod.curiosity.enable": True,
        "neuromod.novelty.enable": True,
    })
    
    # 2. Create brain and initialize with error handling
    brain = pynlm.createBrain(config)
    try:
        brain.initialize()
        print(f"Brain initialized: {brain.getTotalNeuronCount()} neurons")
    except Exception as e:
        print(f"Brain initialization failed: {e}")
        return
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable advanced subsystems (new API!)
    agent.enableAllSystems()  # Enables reward modulation, structural plasticity, development, curiosity
    
    # 7. Enhanced simulation loop with monitoring and error handling
    print(f"Starting enhanced simulation for {num_steps} steps...")
    for step in range(num_steps):
        try:
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
            
            # Apply reward modulation if agent is enabled
            if agent.isRewardModulationEnabled():
                # Get reward from world (in real experiment, this would be from actions)
                percept = world.getSensoryPercept()
                reward = 0.0  # Simplified reward system
                agent.applyRewardModulation(reward, 0.0)
            
            # Update development system
            if agent.isDevelopmentEnabled():
                agent.updateDevelopment(0.1)
            
            # Print progress every 100 steps with new diagnostics
            if step % 100 == 0:
                # Get enhanced diagnostics
                brain_stats = brain.getBrainStats()
                agent_stats = agent.getAgentStats()
                
                print(f"Step {step}:")
                print(f"  Firing neurons: {brain_stats['firingNeurons']}")
                print(f"  Curiosity: {agent_stats['curiosity']:.3f}")
                print(f"  Novelty: {agent_stats['novelty']:.3f}")
                print(f"  Prediction error: {agent_stats['predictionError']:.3f}")
                print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
                
        except Exception as e:
            print(f"Error at step {step}: {e}")
            break
    
    # Final status report with enhanced diagnostics
    print(f"\n=== Enhanced Simulation Complete ===")
    print(f"Total steps completed: {step + 1}")
    
    # Get final diagnostics
    brain_stats = brain.getBrainStats()
    agent_stats = agent.getAgentStats()
    
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"Final developmental stage: {agent.getDevelopmentalStage()}")
    
    # Get detailed diagnostics
    print(f"\n=== Final Diagnostics ===")
    print(f"Motor system: {agent_stats['visionNeurons']} vision neurons, "
          f"{agent_stats['touchNeurons']} touch neurons, "
          f"{agent_stats['internalNeurons']} internal neurons")
    print(f"Brain regions: {len(brain.getRegionIds())}")
    print(f"Planner decisions: {agent.getPlannerDecisionCount()}")
    print(f"Planner agreements: {agent.getPlannerAgreementCount()}")
    print(f"Planner overrides: {agent.getPlannerOverrideCount()}")
    print(f"Concept biases: {agent.getConceptBiasCount()}")
    print(f"Self biases: {agent.getSelfBiasCount()}")
    print(f"Social biases: {agent.getSocialBiasCount()}")
    
    # Save brain state with error handling
    try:
        brain.save("enhanced_agent_brain_checkpoint.bin")
        print(f"Brain state saved successfully!")
    except Exception as e:
        print(f"Brain save failed: {e}")
    
    return {
        'steps_completed': step + 1,
        'total_spikes': brain.getTotalSpikeCount(),
        'final_curiosity': agent.getCuriosityLevel(),
        'final_novelty': agent.getNoveltyLevel(),
        'diagnostics': {
            'brain': brain_stats,
            'agent': agent_stats,
            'region_count': len(brain.getRegionIds()),
            'planner_decisions': agent.getPlannerDecisionCount(),
            'planner_agreements': agent.getPlannerAgreementCount(),
            'planner_overrides': agent.getPlannerOverrideCount(),
        }
    }

# Run the enhanced simulation
results = run_agent_simulation_enhanced(1000)
print(f"\n=== Simulation Results ===")
print(f"Success: {results is not None}")
if results:
    print(f"Steps completed: {results['steps_completed']}")
    print(f"Final curiosity: {results['final_curiosity']:.3f}")
    print(f"Final novelty: {results['final_novelty']:.3f}")
```

### Key Classes and Methods

#### Brain Class

```python
# Creation
brain = pynlm.createBrain(config)

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()             # Get motor action

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance

# State
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

#### Config Class (Enhanced)

```python
# Create
config = pynlm.createDefaultConfig()

# Enhanced query methods
if config.hasKey("neuron_count"):
    print(f"Neuron count: {config.getInt('neuron_count', 1000)}")

keys = config.getKeys()

# Enhanced batch operations
config.setBatch({
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
})

# Enhanced command-line loading (simplified)
args = ["--neuron-count", "500", "--connection-probability", "0.15"]
config.loadFromArgs(args)

# Load from dictionary (new!)
config.loadFromDict({
    "neuron_count": 1000,
    "plasticity.stdp.enable": True,
    "neuromod.curiosity.enable": True,
})

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

#### AgentBrain Class (Enhanced)

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Enhanced initialization
agent.initialize(world)

# Enhanced sensory processing with better error handling
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Enhanced motor decoding with diagnostics
motor_cmd = agent.decodeMotorCommand()

# Enhanced neuromodulation with error handling
reward = 0.0  # Get from world in real experiment
agent.applyRewardModulation(reward, 0.0)

# Enhanced development
agent.updateDevelopment(0.1)

# Get developmental stage
agent.getDevelopmentalStage()

# Enhanced subsystem management (new API!)
agent.enableAllSystems()  # Enables all neuromodulation and plasticity
# or individually:
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Enhanced diagnostics (new API!)
agent_stats = agent.getAgentStats()
print(f"Vision neurons: {agent_stats['visionNeurons']}")
print(f"Touch neurons: {agent_stats['touchNeurons']}")
print(f"Curiosity: {agent_stats['curiosity']:.3f}")
print(f"Novelty: {agent_stats['novelty']:.3f}")

# Get enhanced thought trace
thought_trace = agent.getThoughtTrace()
print(f"Thought trace: {thought_trace}")

# Get advanced diagnostics
counters = {
    'reflex_bonus_count': agent.getReflexBonusCount(),
    'reflex_suppress_count': agent.getReflexSuppressCount(),
    'wta_pulse_count': agent.getWtaPulseCount(),
    'planner_override_count': agent.getPlannerOverrideCount(),
    'planner_agreement_count': agent.getPlannerAgreementCount(),
    'planner_decision_count': agent.getPlannerDecisionCount(),
    'concept_bias_count': agent.getConceptBiasCount(),
    'self_bias_count': agent.getSelfBiasCount(),
    'social_bias_count': agent.getSocialBiasCount(),
}

# Reset agent
agent.reset()

# Get brain pointer
brain_ptr = agent.getBrain()
```

#### SimpleWorld Class (Enhanced)

```python
# Creation
world = pynlm.createSimpleWorld()

# Enhanced configuration
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Enhanced control
world.reset()

# Enhanced agent interaction
world.setAgentStart(x=10.0, y=10.0)
motor_cmd = agent.decodeMotorCommand()
world.applyMotorCommand(motor_cmd, current_time=0.0)

# Enhanced state access
percept = world.getSensoryPercept()
agent_body = world.getAgentBody()

# Enhanced diagnostics
world_stats = world.getWorldStats()
print(f"World size: {world_stats['width']}x{world_stats['height']}")
print(f"Simulation time: {world_stats['simulationTime']}")

# Enhanced agent body state
body = world.getAgentBody()
body_stats = {
    'x': body.x,
    'y': body.y,
    'orientation': body.orientation,
    'energy': body.energy,
    'health': body.health,
    'age': body.age,
    'isMoving': body.isMoving,
    'isTurning': body.isTurning,
}

# Enhanced world objects
pynlm.WorldObject(
    x=5.0,
    y=5.0,
    type=pynlm.WorldObjectType.Resource,
    value=1.0,
    radius=0.5
)

# Enhanced world interaction
if world.isValidPosition(x, y):
    print("Position is valid")

# Enhanced sensory percept
percept = world.getSensoryPercept()
percept_stats = {
    'vision': percept.getVision(),
    'touch': percept.getTouch(),
    'internal': percept.getInternal(),
    'proprioception': percept.getProprioception(),
}

# Enhanced update with error handling
try:
    world.update(timestep=0.1)
    print("World updated successfully")
except Exception as e:
    print(f"World update failed: {e}")

# Enhanced random seed
world.setRandomSeed(42)
```

#### Enumerations

```python
# Neuron types (same in both)
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal

# Developmental stages (same in both)
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging

# Action types (used by brain's cognition/planner systems)
pynlm.ActionType.MoveForward
pynlm.ActionType.MoveBackward
pynlm.ActionType.MoveLeft
pynlm.ActionType.MoveRight
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Look
pynlm.ActionType.LookUp
pynlm.ActionType.LookDown
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Drink
pynlm.ActionType.Rest
pynlm.ActionType.Wait
pynlm.ActionType.Custom

# Motor commands (used by agent's motor output interface - more restricted)
pynlm.MotorCommand.MoveForward    # Propel forward
pynlm.MotorCommand.MoveBackward   # Propel backward  
pynlm.MotorCommand.TurnLeft       # Rotate left
pynlm.MotorCommand.TurnRight      # Rotate right
pynlm.MotorCommand.LookLeft       # Pan sensor left
pynlm.MotorCommand.LookRight      # Pan sensor right
pynlm.MotorCommand.Interact       # Interact with nearby object
pynlm.MotorCommand.Wait           # No movement

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

---

## Part 4: Configuration Options

### Brain Configuration

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

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

---

## Part 5: Example Scripts

### Minimal Example

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

### Environment Interaction

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

---

## Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
