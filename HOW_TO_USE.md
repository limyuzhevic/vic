# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

**Current Status:** NLM is in Phase 6 (Final Integration) with approximately 35.8% system integration achieved. The neural core (LIF neurons, spikes, basic plasticity) works excellently, but many cognitive systems (memory, prediction, higher cognition) are disconnected from the main brain loop.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API with current capabilities
4. CLI usage for running simulations
5. Advanced usage patterns

---

---

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Current Implementation Note

Phase 6 integration is in progress. The neural core (LIF neurons, spikes, basic plasticity) is fully functional, but many higher-level systems are still being integrated.

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
| `nlm_phase3_demo` | Executable | Phase 3 demonstration (working) |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration (framework exists, not fully working) |
| `nlm_test` | Executable | Unit test suite |
| `nlm_cli` | Executable | Command-line interface (Phase 2 real neural computation) |

**Note:** The main `nlm` executable may be minimal since Phase 6 integration is not complete.

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
# Install build dependencies
pip install scikit-build-core pybind11 numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

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

# Check version
print(pynlm.__version__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Working: Neural core is functional!")

# Try enhanced features
agent = pynlm.createAgentBrain(brain)
print(f"Agent has {agent.getSensoryInputSize()} sensory inputs and {agent.getMotorOutputSize()} motor outputs")
```

---

## Part 3: Using the Python Library (Current Capabilities)

### Basic Usage

#### Creating a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters
# config.set("brain.neuron_count", 1000)
# config.set("brain.synapse_density", 0.1)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

#### Running a Simulation

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
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

### Complete Agent Example (Phase 2 Integration Working)

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
    
    # 6. Enable working subsystems
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableCuriosity(True)             # Exploration drive
    agent.enableDevelopment(True)           # Brain maturation
    # Note: structural_plasticity is enabled by default
    
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

# Input/Output (NOT IMPLEMENTED - returns nullptr)
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
brain.produceAction()                     # Get motor action (STUB)

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance

# State (NOT IMPLEMENTED)
brain.save("checkpoint.bin")       # Save state (STUB - returns false)
brain.load("checkpoint.bin")       # Load state (STUB - returns false)

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)

# Development tracking
brain.getDevelopmentalStage()     # Current stage
```

#### Config Class

```python
# Create
config = pynlm.createDefaultConfig()

# Query
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Modify
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O (NOT IMPLEMENTED)
config.loadFromFile("config.json")  # STUB
config.saveToFile("config.json")    # Not yet implemented
```

#### AgentBrain Class

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
agent.enableCuriosity(True/False)
agent.enableDevelopment(True/False)
agent.enableStructuralPlasticity(True/False)

# Statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

#### SimpleWorld Class

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
```

### Enumerations

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
# ... and more

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

---

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

---

## Part 6: CLI Usage

### Basic CLI Commands

The NLM CLI (`nlm_cli`) provides a command-line interface for running simulations:

```bash
# Run a simple brain simulation
./nlm_cli run --neuron-count 100 --max-steps 500

# Run with verbose output
./nlm_cli run --neuron-count 500 --max-steps 1000 --verbose

# Run with custom timestep
./nlm_cli run --neuron-count 200 --timestep 0.0001
```

### Available Commands

#### `./nlm_cli run`
Run a simple brain simulation with configurable parameters.

**Options:**
- `--neuron-count, -n <n>`: Number of neurons (default: 1000)
- `--timestep, -t <t>`: Simulation timestep (default: 0.001)
- `--max-steps <n>`: Maximum simulation steps (default: 1000)
- `--output <file>`: Output file for brain state
- `--verbose, -v`: Enable verbose output
- `--log-level <level>`: Set log level (DEBUG, INFO, WARN, ERROR)

**Examples:**
```bash
nlm_cli run --neuron-count 500 --max-steps 2000
nlm_cli run -v -t 0.0001
```

#### `./nlm_cli experiment <name>`
Run predefined experiments.

**Available experiments:**
- `connectivity`: Test basic neural connectivity
- `plasticity`: Plasticity learning experiment
- `stdp`: STDP verification experiment
- `phase2`: Complete Phase 2 demonstration

**Options:**
- `--output <file>`: Output file for experiment results

**Examples:**
```bash
nlm_cli experiment connectivity
nlm_cli experiment plasticity --output plasticity_results.json
```

#### `./nlm_cli config`
Configuration management.

**Subcommands:**
- `list`: List current configuration
- `generate`: Generate default configuration file
- `validate <file>`: Validate a configuration file

**Options:**
- `--config <file>`: Configuration file to use (default: configs/default.cfg)
- `--output <file>`: Output file for generated config

**Examples:**
```bash
nlm_cli config list
nlm_cli config generate --output my_config.cfg
nlm_cli config validate custom_config.cfg
```

#### `./nlm_cli export <format>`
Export brain state to various formats.

**Supported formats:**
- `json`: JSON format (default)
- `xml`: XML format
- `csv`: CSV format for neural data
- `binary`: Binary format

**Options:**
- `--output <file>`: Output file for exported data
- `--brain <id>`: Specific brain region to export

**Examples:**
```bash
nlm_cli export json
nlm_cli export csv --output neurons.csv
```

#### `./nlm_cli import <file>`
Import brain state from a file.

**Options:**
- `--format <format>`: Format of the input file (JSON, XML, etc.)
- `--region <id>`: Region to import into

**Examples:**
```bash
nlm_cli import saved_brain.json
nlm_cli import neural_data.xml --format xml
```

#### `./nlm_cli profile`
Profile performance metrics.

**Options:**
- `--output <file>`: Output file for profile data (JSON)
- `--duration <seconds>`: Duration of profiling (default: 60)

**Examples:**
```bash
nlm_cli profile --output profile.json
nlm_cli profile --duration 300
```

#### `./nlm_cli test`
Run unit tests and integration tests.

**Options:**
- `--unit`: Run only unit tests
- `--integration`: Run only integration tests
- `--output <file>`: Output file for test results
- `--format <format>`: Output format (text, json, xml)

**Examples:**
```bash
nlm_cli test
nlm_cli test --unit --format json
```

#### `./nlm_cli monitor`
Monitor brain status in real-time.

**Options:**
- `--interval <seconds>`: Update interval (default: 1)
- `--output <file>`: Log file for monitoring
- `--metrics <list>`: Metrics to monitor (spikes, firing_rate, memory, etc.)

**Examples:**
```bash
nlm_cli monitor --interval 0.5
nlm_cli monitor --metrics spikes,firing_rate --output monitor.log
```

#### `./nlm_cli benchmark`
Run performance benchmarks.

**Options:**
- `--output <file>`: Output file for benchmark results
- `--scale <factor>`: Scale factor for benchmark (1=small, 2=medium, 3=large)
- `--iterations <n>`: Number of iterations (default: 10)

**Examples:**
```bash
nlm_cli benchmark
nlm_cli benchmark --scale 2 --iterations 20
```

---

## Troubleshooting

### Current Known Issues (Phase 6 Status)

The following issues are documented in `docs/PHASE6_FINAL_AUDIT.md`:

#### Memory Systems (NOT INTEGRATED)
- `Brain::getWorkingMemory()`, `Brain::getEpisodicMemory()`, etc. return nullptr
- Memory systems exist but are not connected to the brain loop
- No episodic memory storage or replay

#### Prediction System (NOT INTEGRATED)
- `Brain::getPredictionSystem()` returns nullptr
- Prediction system exists but never trained or used

#### Checkpoint System (STUBS)
- `Brain::save()` and `Brain::load()` are stubs (return false)
- CheckpointSystem exists but not connected to Brain

#### Cognition Systems (NOT USED)
- NeuralPlanner, ConceptFormation, SelfModel, SocialLearning exist but never instantiated
- All cognitive mechanisms implemented but disconnected

#### Development (MINIMAL)
- Development stages exist but only affect structural plasticity rates
- No effect on neural excitability, attention, memory consolidation

#### Neuromodulation (PARTIAL)
- Dopamine only scales STDP weights
- Other neuromodulators (ACh, NE, 5-HT) are stubs
- No integration with memory or attention

### Working Solutions

#### What Works:
1. **Neural Core**: LIF neurons, spikes, synaptic transmission, STDP, Hebbian plasticity
2. **Agent System**: Sensory processing, motor decoding, reward modulation, curiosity, novelty detection
3. **World Simulation**: 2D environment, vision, touch, internal sensing
4. **Basic Integration**: Brain-Agent-world loop works for Phase 3/4

#### What to Focus On:
1. Phase 4 cognitive experiments (they test the disconnected systems)
2. The CLI interface for simulation control
3. Building on the working neural core foundation

### Example: Fixing Common Issues

#### Issue: Brain not learning from rewards
```python
# Problem: Reward modulation may not be working properly

# Solution 1: Check if reward modulation is enabled
agent.enableRewardModulation(True)

# Solution 2: Debug reward values
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Check reward
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    print(f"Step {step}: reward = {reward}")
    agent.applyRewardModulation(reward, 0.0)
```

#### Issue: Low curiosity levels
```python
# Problem: Curiosity system may not be responding

# Solution: Debug curiosity computation
agent.enableCuriosity(True)
for step in range(100):
    # ... simulation loop ...
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    print(f"Step {step}: curiosity={curiosity:.3f}, novelty={novelty:.3f}")
```

---

## Further Reading

- [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Current Phase 6 audit (35.8% integration)
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [easy_usage.md](easy_usage.md) - Quick start guide
- [src/cli/CLI_USER_GUIDE.md](src/cli/CLI_USER_GUIDE.md) - Detailed CLI documentation
- [docs/PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Current system integration status
