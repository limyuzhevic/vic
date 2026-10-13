# NLM - Neural Learning Machine

**An Experimental Artificial Developmental Brain**

## Table of Contents

1. [Overview](#overview)
2. [Key Improvements](#key-improvements)
3. [New CLI Tool](#new-cli-tool)
4. [Enhanced Python API](#enhanced-python-api)
5. [Configuration System](#configuration-system)
6. [Memory Systems](#memory-systems)
7. [Neuromodulation](#neuromodulation)
8. [Development System](#development-system)
9. [Performance Optimizations](#performance-optimizations)
10. [Testing Framework](#testing-framework)
11. [Visualization and Monitoring](#visualization-and-monitoring)
12. [Documentation and Examples](#documentation-and-examples)
13. [Installation and Setup](#installation-and-setup)
14. [Usage Examples](#usage-examples)
15. [Development Guide](#development-guide)
16. [Configuration Reference](#configuration-reference)
17. [Command Reference](#command-reference)
18. [Python API Reference](#python-api-reference)
19. [FAQ](#faq)
20. [Troubleshooting](#troubleshooting)
21. [Contributing](#contributing)
22. [License](#license)

<a name="overview"></a>
## Overview

NLM (熙然, meaning "serene flow") is a comprehensive neural network simulation framework that implements a complete artificial brain from the ground up. Phase 6 represents the final integration of all systems into a coherent, functioning brain.

NLM combines:

- **Real spiking neural computation** with LIF neurons and event-driven dynamics
- **Multiple memory systems** (working, episodic, associative memory)
- **Neuromodulation** (dopamine, curiosity, novelty detection)
- **Prediction and cognition systems** (predictive coding, planning, attention)
- **Developmental stages** and plasticity mechanisms
- **Performance optimizations** (parallel processing, memory efficiency)

### Key Statistics

- **Neurons:** Can simulate thousands to millions of neurons
- **Synapses:** Handles millions of synaptic connections
- **Memory:** Stores experience in multiple memory systems
- **Computation:** Real-time simulation of complex neural dynamics
- **Scalability:** Can run on single machine or distributed systems

<a name="key-improvements"></a>
## Key Improvements

### 1. Enhanced Command-Line Interface (CLI)

**New CLI Tool:** `nlm`

A powerful command-line interface that provides:

- **Comprehensive command set:** `run`, `init`, `save`, `load`, `config`, `info`, `status`, `stats`, `simulate`, `visualize`, `benchmark`
- **Configuration management:** Get and set configuration values easily
- **Checkpoint management:** Save and load brain states
- **Real-time monitoring:** Get brain status and statistics
- **Simulation control:** Run simulations with various options
- **Visualization tools:** Launch visualization interfaces
- **Performance benchmarks:** Measure and optimize performance

### 2. Improved Python API

The Python bindings have been enhanced with:

- **Better API design:** More intuitive and Pythonic interface
- **Comprehensive documentation:** Detailed docstrings and examples
- **Additional features:** More advanced neural network operations
- **Error handling:** Improved error messages and exception handling

### 3. Enhanced Configuration System

The configuration system has been improved with:

- **Structured configuration:** Better organization of settings
- **Type safety:** Proper type checking for configuration values
- **Validation:** Configuration validation and error handling
- **Defaults:** Comprehensive set of sensible defaults for Phase 6
- **Dynamic updates:** Configuration can be updated during runtime

### 4. Advanced Memory Systems

All memory systems have been enhanced:

- **Working Memory:** Improved maintenance and competition dynamics
- **Episodic Memory:** Better episode encoding and replay mechanisms
- **Associative Memory:** Enhanced pattern association capabilities
- **Integration:** Better integration with neural processing

### 5. Neuromodulation Enhancement

Neuromodulation systems have been improved:

- **Dopamine:** Better reward prediction error processing
- **Curiosity:** Enhanced exploration motivation
- **Novelty:** Improved novelty detection
- **Integration:** Better integration with plasticity rules

### 6. Development System

The development system has been enhanced:

- **Stages:** More realistic developmental stages (Initial, Critical Period, Maturation, Adult, Aging)
- **Plasticity modulation:** Better control of plasticity rates
- **Structural changes:** Improved synaptogenesis and pruning
- **Experience-dependent:** Development affected by experience

### 7. Performance Optimizations

Performance has been significantly improved:

- **Parallel processing:** Better utilization of multi-core processors
- **Memory management:** Improved memory efficiency
- **Event-driven processing:** Optimized spike propagation
- **Checkpoint system:** Faster save/load operations
- **Profiling tools:** Built-in performance monitoring

### 8. Comprehensive Testing Framework

A robust testing system has been implemented:

- **Unit tests:** Comprehensive testing of individual components
- **Integration tests:** Testing of system interactions
- **Performance benchmarks:** Performance measurement and comparison
- **Automated testing:** Continuous integration support

### 9. Visualization and Monitoring

Advanced visualization capabilities:

- **Terminal visualization:** Real-time neural activity display
- **Web interface:** Interactive visualization server
- **Statistical monitoring:** Performance monitoring and analysis
- **Export capabilities:** Export data for external analysis

### 10. Documentation and Examples

Extensive documentation and examples:

- **Comprehensive README:** Detailed project documentation
- **API documentation:** Complete API reference
- **Tutorials:** Step-by-step tutorials for different use cases
- **Examples:** Multiple working examples demonstrating different features

<a name="new-cli-tool"></a>
## New CLI Tool

### Overview

The `nlm` command-line interface provides comprehensive control over NLM simulations:

### Commands

#### `nlm run`
Run NLM simulation with configuration options.

**Options:**
- `--config FILE` - Configuration file (default: `configs/default.cfg`)
- `--steps STEPS` - Number of simulation steps
- `--neurons N` - Neuron count
- `--regions R` - Region count
- `--interactive` - Interactive mode with real-time feedback
- `--background` - Run in background
- `--continue` - Continue from existing checkpoint
- `--checkpoint DIR` - Directory for automatic checkpoints

#### `nlm init`
Initialize NLM brain with configuration.

**Options:**
- `--neurons N` - Number of neurons
- `--regions R` - Number of regions
- `--seed SEED` - Random seed
- `--timestep T` - Simulation timestep
- `--stdp-ltp W` - STDP LTP weight
- `--stdp-ltd W` - STDP LTD weight
- `--synaptogenesis R` - Synaptogenesis rate
- `--pruning R` - Pruning rate

#### `nlm save FILE`
Save brain state to checkpoint.

**Options:**
- `--compress LEVEL` - Compression level (none, fast, balanced, best)
- `--metadata` - Include metadata
- `--memory` - Include memory state
- `--development` - Include development state
- `--neuromodulation` - Include neuromodulation state

#### `nlm load FILE`
Load brain state from checkpoint.

**Options:**
- `--validate` - Validate checkpoint before loading
- `--force` - Overwrite current brain state
- `--metadata` - Load metadata
- `--memory` - Load memory state
- `--development` - Load development state
- `--neuromodulation` - Load neuromodulation state

#### `nlm config KEY [VALUE]`
Get or set configuration values.

**Keys:**
- `random_seed` - Random number generator seed
- `neuron_count` - Number of neurons in the brain
- `region_count` - Number of neural regions
- `connection_probability` - Probability of connections between neurons
- `simulation_timestep` - Time step for simulation updates
- `stdp_ltp_weight` - STDP long-term potentiation weight
- `stdp_ltd_weight` - STDP long-term depression weight
- `stdp_tau` - STDP time constant
- `synaptogenesis_rate` - Rate of new synapse formation
- `pruning_rate` - Rate of synapse elimination
- `replay_interval` - Interval for memory replay
- `consolidation_interval` - Interval for memory consolidation

#### `nlm info`
Display system information.

Shows general system information including version, build info, available commands, file locations, configuration options, and memory requirements.

#### `nlm status`
Show current brain status.

**Options:**
- `--detailed` - Show detailed status information
- `--neurons` - Show neuron statistics
- `--synapses` - Show synapse statistics
- `--memory` - Show memory system status
- `--development` - Show development status
- `--neuromodulation` - Show neuromodulation status

#### `nlm stats`
Display detailed performance statistics.

**Options:**
- `--spikes` - Show spike statistics
- `--firing-rate` - Show firing rate information
- `--plasticity` - Show plasticity rule statistics
- `--memory` - Show memory system statistics
- `--network` - Show network topology statistics
- `--performance` - Show performance metrics

#### `nlm simulate STEPS`
Run simulation for specified steps.

**Options:**
- `--config FILE` - Configuration file
- `--visualize` - Enable real-time visualization
- `--checkpoint DIR` - Automatic checkpointing directory
- `--interval STEPS` - Checkpoint interval
- `--log-level LEVEL` - Log level (debug, info, warning, error)
- `--output FILE` - Save statistics to file

#### `nlm visualize`
Launch visualization interface.

**Options:**
- `--fullscreen` - Run in fullscreen mode
- `--output FORMAT` - Output format (terminal, html, file)
- `--port PORT` - Web server port (default: 8080)
- `--host HOST` - Web server host (default: localhost)
- `--neurons N` - Maximum number of neurons to display
- `--animate` - Enable animation of neural activity
- `--statistics` - Show statistics panel

#### `nlm benchmark`
Run performance benchmarks.

**Options:**
- `--duration TIME` - Benchmark duration in seconds (default: 60)
- `--neurons N` - Number of neurons to benchmark (default: 1000)
- `--regions R` - Number of regions (default: 1)
- `--steps STEPS` - Number of steps to benchmark (default: 10000)
- `--memory MB` - Memory limit in MB
- `--detailed` - Enable detailed benchmarks
- `--report FORMAT` - Report format (text, json, csv)

#### `nlm help`
Show help message.

<a name="enhanced-python-api"></a>
## Enhanced Python API

### Overview

The Python bindings provide access to NLM functionality from Python:

### Installation

```bash
pip install pynlm
```

### Core Classes

#### `pynlm.Config`
Configuration class for NLM system.

**Methods:**
- `__init__()` - Create default configuration
- `loadFromFile(filepath)` - Load from file
- `loadFromArgs(argc, argv)` - Load from command line arguments
- `saveToFile(filepath)` - Save to file
- `has(key)` - Check if key exists
- `getKeys()` - Get all configuration keys
- `clear()` - Clear all entries
- `summary()` - Get summary string
- `__repr__()` - String representation

#### `pynlm.Brain`
Central neural simulation brain class.

**Methods:**
- `__init__(config)` - Create brain with configuration
- `initialize()` - Initialize the brain
- `step(step, time=None)` - Perform simulation step
- `receiveSensoryInput(input)` - Inject sensory input
- `injectCurrent(neuron, current)` - Inject current into specific neuron
- `injectCurrentToNeurons(type, current)` - Inject current to all neurons of type
- `produceAction()` - Produce motor action
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state
- `addRegion(name)` - Add a new neural region
- `getRegion(id)` - Get region by ID
- `getRegionCount()` - Get region count
- `getRegionIds()` - Get all region IDs
- `getRegions()` - Get all regions
- `getTotalNeuronCount()` - Get total neuron count
- `getTotalSynapseCount()` - Get total synapse count
- `getActiveNeuronCount()` - Get active neuron count
- `getFiringNeuronCount()` - Get firing neuron count
- `getAverageFiringRate()` - Get average firing rate
- `getExcitationInhibitionRatio()` - Get E/I ratio
- `getTotalSpikeCount()` - Get total spike count
- `getPendingSpikeEventCount()` - Get pending spike count
- `getDevelopmentalStage()` - Get developmental stage
- `setDevelopmentalStage(stage)` - Set developmental stage
- `getConfig()` - Get configuration
- `getRandomGenerator()` - Get random generator
- `logStatus()` - Log brain status

#### `pynlm.AgentBrain`
Agent brain interface connecting NLM brain to world.

**Methods:**
- `__init__(brain)` - Create with brain
- `initialize(world)` - Initialize with world
- `getSensoryInputSize()` - Get expected sensory input size
- `getMotorOutputSize()` - Get expected motor output size
- `processSensoryInput(percept)` - Process sensory percept
- `decodeMotorCommand()` - Decode motor command
- `applyRewardModulation(reward, predictedReward)` - Apply reward modulation
- `updateDevelopment(timestep)` - Update development system
- `getDevelopmentalStage()` - Get developmental stage
- `getNeuromodulationLevel()` - Get neuromodulation level
- `getCuriosityLevel()` - Get curiosity level
- `getNoveltyLevel()` - Get novelty level
- `getPredictionError()` - Get prediction error
- `reset()` - Reset agent
- `getBrain()` - Get underlying brain
- `enableRewardModulation(enable)` - Enable/disable reward modulation
- `enableStructuralPlasticity(enable)` - Enable/disable structural plasticity
- `enableDevelopment(enable)` - Enable/disable development
- `enableCuriosity(enable)` - Enable/disable curiosity
- `isRewardModulationEnabled()` - Check if reward modulation enabled
- `isStructuralPlasticityEnabled()` - Check if structural plasticity enabled
- `isDevelopmentEnabled()` - Check if development enabled
- `isCuriosityEnabled()` - Check if curiosity enabled

#### `pynlm.SimpleWorld`
Simple 2D world for NLM simulation.

**Methods:**
- `__init__()` - Create world
- `configure(width, height, visionWidth, visionHeight)` - Configure world
- `reset()` - Reset world
- `setAgentStart(x, y)` - Set agent starting position
- `update(timestep)` - Update world
- `applyMotorCommand(cmd, currentTime)` - Apply motor command
- `getSensoryPercept()` - Get sensory percept
- `getAgentBody()` - Get agent body
- `addObject(obj)` - Add world object
- `removeObject(x, y)` - Remove object
- `isValidPosition(x, y)` - Check valid position
- `getWidth()` - Get width
- `getHeight()` - Get height
- `getMaxEnergy()` - Get max energy
- `setMaxEnergy(e)` - Set max energy
- `getEnergyDecayRate()` - Get energy decay rate
- `setEnergyDecayRate(r)` - Set energy decay rate
- `getSimulationTime()` - Get simulation time
- `setRandomSeed(seed)` - Set random seed
- `getRandomSeed()` - Get random seed

### Example Usage

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create a brain
brain = pynlm.createBrain(config)
brain.initialize()

# Get brain statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")

# Run a simple simulation
for step in range(1000):
    brain.step(step)

# Save brain state
brain.save("my_brain_checkpoint")

# Create a world for agent interaction
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create an agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run agent in world
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

<a name="configuration-system"></a>
## Configuration System

### Configuration Files

NLM uses configuration files to control simulation parameters. Configuration files are simple key=value format files.

### Default Configuration

Create a configuration file with the following settings:

```ini
# Core Settings
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 1
connection_probability = 0.1

# STDP Plasticity Parameters
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0

# Structural Plasticity
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Memory System Settings
replay_interval = 100
consolidation_interval = 1000

# Development Settings
developmental_stage = 3  # 0=Initial, 1=CriticalPeriod, 2=Maturation, 3=Adult, 4=Aging
plasticity_modifier = 1.0

# Neuromodulation
dopamine_level = 0.0
curiosity_level = 0.0
novelty_level = 0.0
prediction_error = 0.0

# Simulation Parameters
max_steps = 10000
checkpoint_interval = 1000
output_frequency = 100

# Agent Settings
agent_start_x = 10.0
agent_start_y = 10.0
agent_max_energy = 100.0
agent_energy_decay_rate = 0.01

# World Settings
world_width = 20
world_height = 20
vision_width = 8
vision_height = 8
max_energy = 100.0
energy_decay_rate = 0.01

# Sensory Processing
sensory_noise_level = 0.01
sensory_sensitivity = 1.0

# Motor Control
motor_noise_level = 0.05
motor_efficiency = 1.0

# Logging
log_level = "info"
log_file = "nlm.log"
enable_console_output = true
enable_file_output = false
```

### Configuration Keys Reference

#### Core Settings
- `random_seed` - Random number generator seed
- `neuron_count` - Number of neurons in the brain
- `region_count` - Number of neural regions
- `connection_probability` - Probability of connections between neurons
- `simulation_timestep` - Time step for simulation updates

#### Plasticity Settings
- `stdp_ltp_weight` - STDP long-term potentiation weight
- `stdp_ltd_weight` - STDP long-term depression weight
- `stdp_tau` - STDP time constant
- `synaptogenesis_rate` - Rate of new synapse formation
- `pruning_rate` - Rate of synapse elimination

#### Memory Settings
- `replay_interval` - Interval for memory replay
- `consolidation_interval` - Interval for memory consolidation

#### Development Settings
- `developmental_stage` - Current developmental stage
- `plasticity_modifier` - Plasticity rate modifier

#### Neuromodulation Settings
- `dopamine_level` - Dopamine level
- `curiosity_level` - Curiosity level
- `novelty_level` - Novelty level
- `prediction_error` - Prediction error level

#### World Settings
- `world_width` - World width
- `world_height` - World height
- `vision_width` - Vision width
- `vision_height` - Vision height
- `max_energy` - Maximum energy
- `energy_decay_rate` - Energy decay rate

### Configuration Management

Use the CLI to manage configuration:

```bash
# Get configuration value
./nlm config neuron_count

# Set configuration value
./nlm config neuron_count 2000

# List all configuration keys
./nlm config list

# Export configuration
./nlm config export my_config.cfg

# Import configuration
./nlm config import my_config.cfg
```

<a name="memory-systems"></a>
## Memory Systems

### Working Memory

Working memory stores transient active information with competitive dynamics.

**Key Features:**
- Competitive memory access
- Priority-based retrieval
- Rapid decay
- Integration with neural processing

**Configuration:**
- Capacity determined by neuron count / 10
- Decay time based on simulation parameters
- Competition strength configurable

### Episodic Memory

Episodic memory stores experiences as episodes with timestamps.

**Key Features:**
- Temporal ordering
- Reward association
- Replay during rest
- Consolidation for long-term storage

**Configuration:**
- Maximum episodes configurable
- Replay interval configurable
- Consolidation interval configurable

### Associative Memory

Associative memory stores pattern associations.

**Key Features:**
- Hebbian learning
- Pattern completion
- Weight-based associations
- Integration with plasticity

**Configuration:**
- Learning rate configurable
- Association strength thresholds
- Pattern similarity measures

<a name="neuromodulation"></a>
## Neuromodulation

### Dopamine

Dopamine signals reward prediction error and modulates neural excitability.

**Key Features:**
- Reward prediction error computation
- Neural excitability modulation
- Plasticity rate control
- Learning rate adjustment

**Integration:**
- Affects STDP learning rates
- Modulates neural firing thresholds
- Controls memory consolidation

### Curiosity

Curiosity drives exploration behavior based on novelty and prediction error.

**Key Features:**
- Novelty detection
- Prediction error computation
- Exploration motivation
- Behavioral flexibility

**Integration:**
- Influences action selection
- Modulates learning
- Controls exploration vs. exploitation

### Novelty

Novelty detects new or unexpected sensory input.

**Key Features:**
- Change detection
- Adaptation to new stimuli
- Learning trigger
- Behavioral adaptation

**Integration:**
- Triggers curiosity system
- Affects memory encoding
- Influences attention

### Novelty Detection Algorithm

Novelty is computed as:

```python
def compute_novelty(current_vision, previous_vision):
    total_diff = 0.0
    for i in range(len(current_vision)):
        diff = abs(current_vision[i] - previous_vision[i])
        total_diff += diff
    return total_diff / max(len(current_vision), 1)
```

### Dopamine Plasticity Modulator

Dopamine modulates plasticity as follows:

```python
def get_dopamine_plasticity_factor(dopamine_level):
    # Positive dopamine increases plasticity
    # Negative dopamine decreases plasticity
    return 0.5 + 0.5 * dopamine_level
```

<a name="development-system"></a>
## Development System

### Developmental Stages

NLM implements biologically inspired developmental stages:

1. **Initial (0-60 steps)**: High plasticity, rapid learning
2. **Critical Period (60-300 steps)**: Moderate plasticity, sensitive learning
3. **Maturation (300-900 steps)**: Lower plasticity, stable learning
4. **Adult (900+ steps)**: Low plasticity, refined behavior
5. **Aging (1000+ steps)**: Variable plasticity, memory decline

### Development Dynamics

**Plasticity Modifier:**
- Initial: 1.0 (maximum plasticity)
- Critical Period: 0.8
- Maturation: 0.5
- Adult: 0.2 (stable)
- Aging: 0.1 (declining)

**Synaptogenesis and Pruning:**
- Synaptogenesis decreases with development
- Pruning increases with development
- Balance affects network stability

### Experience-Dependent Development

Development is influenced by:

- **Experience:** More experience refines development
- **Learning:** Successful learning stabilizes development
- **Stress:** Negative experiences can alter development trajectories
- **Sleep:** Memory replay during rest affects development

<a name="performance-optimizations"></a>
## Performance Optimizations

### Parallel Processing

NLM utilizes multiple CPU cores for:

- **Spike propagation:** Parallel event-driven processing
- **Plasticity computation:** Concurrent weight updates
- **Memory operations:** Parallel memory access
- **Simulation steps:** Independent neuron updates

### Memory Management

Optimizations include:

- **Smart pointers:** Automatic memory management
- **Memory pools:** Efficient allocation
- **Copy-on-write:** Reduced memory duplication
- **Cache locality:** Optimized data access patterns

### Event-Driven Processing

Key optimizations:

- **Delayed spike events:** Scheduled spike propagation
- **Elastic eligibility traces:** Efficient STDP computation
- **Incremental updates:** Only update changed components
- **Batch processing:** Group similar operations

### Profiling Tools

Built-in performance monitoring:

- **Timing measurements:** Track execution time of critical functions
- **Memory usage:** Monitor memory consumption
- **CPU utilization:** Track processor usage
- **Event counters:** Count spikes, plasticity events, etc.
- **Bottleneck detection:** Identify performance bottlenecks

<a name="testing-framework"></a>
## Testing Framework

### Unit Tests

Comprehensive unit testing for:

- **Neuron dynamics:** LIF integration, spike detection
- **Synapse properties:** Weight updates, transmission
- **Plasticity rules:** STDP, Hebbian, reward modulation
- **Memory systems:** Encoding, retrieval, consolidation
- **Neuromodulation:** Dopamine, curiosity, novelty
- **Development:** Stage transitions, plasticity changes

### Integration Tests

Testing of system interactions:

- **Memory-neural integration:** How memory affects neural processing
- **Neuromodulation-neural integration:** How neuromodulators affect neurons
- **Plasticity-memory integration:** How plasticity affects memory
- **Development-system integration:** How development affects all systems
- **World-agent integration:** Agent behavior in world

### Performance Tests

Benchmarks for:

- **Simulation speed:** Neurons/second, steps/second
- **Memory usage:** Memory footprint, growth patterns
- **Scalability:** Performance with increasing system size
- **Real-time capabilities:** Simulation vs. wall-clock time

### Test Infrastructure

The testing framework includes:

- **Automated test runners:** Continuous integration support
- **Test data:** Real-world scenarios and edge cases
- **Performance baselines:** Reference performance metrics
- **Regression testing:** Ensure no performance degradation

### Test Commands

```bash
# Run all tests
./nlm benchmark --detailed

# Run unit tests
make test

# Run integration tests
c test --output-on-failure

# Run performance tests
./nlm benchmark --duration 60 --detailed
```

<a name="visualization-and-monitoring"></a>
## Visualization and Monitoring

### Terminal Visualization

Real-time neural activity display in terminal:

- **Neuron states:** Membrane potential, firing state
- **Spike activity:** Real-time spike visualization
- **Network topology:** Connection visualization
- **Memory activity:** Working memory traces

### Web Interface

Interactive visualization server:

- **Real-time updates:** Live neural activity visualization
- **Multiple views:** Different perspectives on neural activity
- **Controls:** Interactive controls for simulation
- **Statistics:** Real-time performance statistics

### Performance Monitoring

Built-in monitoring tools:

- **System resources:** CPU, memory, disk usage
- **Neural activity:** Spike rates, firing patterns
- **Memory usage:** Working memory, episodic memory
- **Performance metrics:** Simulation speed, efficiency

### Data Export

Export capabilities:

- **CSV format:** Export data for external analysis
- **JSON format:** Structured data export
- **Binary format:** Efficient data storage
- **Database integration:** Store in databases for analysis

<a name="documentation-and-examples"></a>
## Documentation and Examples

### Core Documentation

#### README.md
Main project documentation with:
- Project overview
- Installation instructions
- Usage examples
- API references
- Development guide

#### HOW_TO_USE.md
Detailed usage guide with:
- Step-by-step tutorials
- Configuration examples
- Command-line interface reference
- Python API documentation

#### EASY_USAGE.md
Beginner-friendly guide with:
- Simple examples
- Analogies and metaphors
- Quick start guide
- Common patterns

### API Documentation

#### Brain.hpp
Central brain class documentation with:
- Class overview
- Method documentation
- Configuration options
- Usage examples

#### Neuron.hpp
Neuron implementation documentation with:
- LIF dynamics
- State management
- Plasticity integration
- Event handling

#### Synapse.hpp
Synapse implementation documentation with:
- Weight dynamics
- Transmission mechanisms
- Plasticity rules
- Type-specific behavior

### Example Code

#### Basic Examples
- `examples/basic_brain.py` - Basic brain simulation
- `examples/agent_world.py` - Agent in world
- `examples/nlm_cli_example.sh` - CLI usage example

#### Advanced Examples
- `examples/complex_simulation.py` - Complex multi-agent simulation
- `examples/development_study.py` - Development trajectory analysis
- `examples/memory_test.py` - Memory system testing

#### Configuration Examples
- `configs/default.cfg` - Default configuration
- `configs/example.cfg` - Example configuration
- `configs/agent_config.cfg` - Agent-specific configuration
- `configs/world_config.cfg` - World-specific configuration

### Learning Resources

#### Tutorials
1. **Getting Started:** Create your first brain
2. **Agent Development:** Build agents that interact with the world
3. **Memory Systems:** Understand and use memory systems
4. **Neuromodulation:** Implement reward-based learning
5. **Development:** Study developmental trajectories
6. **Performance Optimization:** Optimize simulation performance

#### Advanced Topics
1. **Architectures:** Design different brain architectures
2. **Learning Algorithms:** Implement custom learning rules
3. **Visualization:** Create custom visualization tools
4. **Integration:** Integrate with external systems
5. **Distributed Computing:** Scale to multiple machines
6. **Hardware Acceleration:** Use GPUs for simulation

<a name="installation-and-setup"></a>
## Installation and Setup

### Prerequisites

- **Operating System:** Linux, macOS, or Windows
- **Compiler:** C++ compiler with C++20 support
- **CMake:** Version 3.16 or higher
- **Python:** Version 3.6 or higher (for Python bindings)
- **pybind11:** Python bindings library
- **Git:** Version control (optional)

### Building from Source

#### Step 1: Clone Repository

```bash
git clone https://github.com/natural-neural-machine/NLM.git
cd NLM
```

#### Step 2: Build the Project

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all components
make -j4
```

#### Step 3: Build Python Bindings

```bash
cd ../python
python3 -m pip install pybind11
mkdir -p build
cd build
cmake .. -DPYTHON_EXECUTABLE=$(which python3)
make -j4
cd ../..
```

#### Step 4: Install Python Module

```bash
# Install the Python module
cd python/dist
pip install pynlm*.whl
cd ../..
```

### Running Examples

#### Basic NLM Executable

```bash
# Run the basic NLM executable (Phase 2 - Real Neural Computation)
./nlm

# Run with custom configuration
./nlm --config my_config.cfg
```

#### Phase 3 Demo (World Interaction)

```bash
# Run Phase 3 demo
./nlm_phase3_demo
```

#### Phase 4 Demo (Integrated Cognition)

```bash
# Run Phase 4 demo
./nlm_phase4_demo
```

#### Phase 6 Demo (Final Integration)

```bash
# Run Phase 6 demo
./nlm_phase6_demo
```

#### NLM CLI Tool

```bash
# Show help
./nlm --help

# Run a simulation
./nlm run --steps 1000

# Save brain state
./nlm save my_brain_checkpoint

# Load brain state
./nlm load my_brain_checkpoint

# Show brain status
./nlm status

# Show performance statistics
./nlm stats
```

### Python Examples

```bash
# Run basic brain example
python3 examples/basic_brain.py

# Run agent world example
python3 examples/agent_world.py

# Run CLI example
./examples/nlm_cli_example.sh
```

### Configuration Files

#### Default Configuration

Create `configs/default.cfg` with:

```ini
# Core Settings
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 1
connection_probability = 0.1
```

#### Custom Configuration

Create `configs/custom.cfg` with your parameters:

```ini
# Your custom configuration
random_seed = 123
neuron_count = 5000
region_count = 2
connection_probability = 0.15
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.02
```

### Running Tests

#### C++ Tests

```bash
# Run C++ tests
cd build
ctest --output-on-failure
```

#### Python Tests

```bash
# Run Python tests
cd python
python3 -m pytest tests/ -v
cd ..
```

#### Performance Tests

```bash
# Run performance benchmarks
./nlm benchmark --duration 60 --detailed
```

### Development Environment

#### Development Dependencies

```bash
# Install development dependencies
pip install pytest pytest-cov matplotlib seaborn
```

#### Code Style

Follow the project's coding style:

- **Indentation:** 4 spaces
- **Line length:** 80 characters maximum
- **Naming conventions:** CamelCase for classes, snake_case for functions
- **Comments:** Document complex logic and algorithms
- **Headers:** Include guards and proper includes

#### Testing Style

- **Unit tests:** Test individual components in isolation
- **Integration tests:** Test system interactions
- **Performance tests:** Test performance characteristics
- **Regression tests:** Ensure no performance degradation

### Docker Support

The project includes Docker support for:

#### Docker Image

```dockerfile
FROM ubuntu:20.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    python3 \
    python3-pip \
    python3-dev

WORKDIR /app

COPY . /app

RUN mkdir build && cd build
RUN cmake .. -DCMAKE_BUILD_TYPE=Release
RUN make -j4

CMD ["/app/build/nlm"]
```

#### Docker Usage

```bash
# Build Docker image
docker build -t nlm .

# Run NLM in Docker
 docker run nlm --help

# Run simulation in Docker
docker run nlm run --steps 1000
```

### Troubleshooting

#### Common Issues

**Issue:** Build fails with missing dependencies

**Solution:** Install required dependencies:

```bash
# On Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential cmake git python3 python3-pip python3-dev

# On macOS
brew install cmake python3
```

**Issue:** Python bindings fail to build

**Solution:** Install pybind11:

```bash
pip3 install pybind11
```

**Issue:** Configuration file not found

**Solution:** Create configuration file:

```bash
nlm_config generate configs/default.cfg
```

**Issue:** Memory usage too high

**Solution:** Reduce neuron count or increase system memory:

```bash
./nlm init --neurons 1000  # Reduce neurons
./nlm run --steps 100 --neurons 500  # Smaller simulation
```

#### Debug Mode

Run NLM in debug mode:

```bash
# Build with debug flags
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4

# Run with debug logging
./nlm --verbose --log-level debug
```

### Performance Optimization

#### CPU Optimization

- Use `-j4` or more with make for parallel builds
- Use optimized compilers: `gcc -O3 -march=native`
- Enable compiler optimizations: `-DNLM_OPTIMIZED`

#### Memory Optimization

- Use checkpointing for long simulations
- Reduce neuron count for testing
- Use memory-efficient algorithms

#### Disk Optimization

- Use SSD for faster checkpoint I/O
- Configure checkpoint compression
- Use external storage for large checkpoints

### Support and Community

#### Online Resources

- **GitHub Repository:** https://github.com/natural-neural-machine/NLM
- **Documentation:** https://docs.natural-neural-machine.org
- **Discussions:** https://github.com/natural-neural-machine/NLM/discussions
- **Issues:** https://github.com/natural-neural-machine/NLM/issues

#### Community Guidelines

- **Code of Conduct:** Follow the project's code of conduct
- **Contribution Guidelines:** Follow contribution guidelines
- **Style Guide:** Follow the project's style guide
- **Testing:** Write tests for new functionality
- **Documentation:** Update documentation for new features

#### Support Channels

- **GitHub Issues:** Report bugs and request features
- **Discussions:** Ask questions and share ideas
- **Pull Requests:** Contribute code changes
- **Email:** support@natural-neural-machine.org

<a name="development-guide"></a>
## Development Guide

### Project Structure

The NLM project has the following structure:

```
NLM/
├── src/                    # Source code
│   ├── core/               # Core utilities
│   ├── brain/              # Neural components
│   ├── dynamics/           # Neural dynamics
│   ├── plasticity/         # Plasticity rules
│   ├── development/        # Developmental system
│   ├── neuromodulation/     # Neuromodulators
│   ├── memory/             # Memory systems
│   ├── prediction/         # Prediction systems
│   ├── cognition/          # Cognitive mechanisms
│   ├── sensory/            # Sensory processing
│   ├── motor/              # Motor system
│   ├── environment/        # Environment interface
│   └── agent/              # Agent interface
├── tests/                  # Test suite
├── python/                 # Python bindings
├── docs/                   # Documentation
├── configs/                # Configuration files
└── tools/                  # Development tools
```

### Developing Core Components

#### Brain Development

To develop new brain features:

1. **Extend Brain.hpp:** Add new methods and properties
2. **Implement Brain.cpp:** Implement brain functionality
3. **Update documentation:** Document new features
4. **Add tests:** Write unit and integration tests
5. **Update examples:** Add examples of new features

#### Neuron Development

To extend neuron capabilities:

1. **Extend Neuron.hpp:** Add new neuron types or behaviors
2. **Implement Neuron.cpp:** Implement new neuron dynamics
3. **Update plasticity:** Integrate with plasticity rules
4. **Test thoroughly:** Ensure backward compatibility

#### Memory System Development

To develop new memory systems:

1. **Create new memory class:** Implement memory interface
2. **Integrate with brain:** Add memory access methods
3. **Update documentation:** Document memory capabilities
4. **Write tests:** Test memory encoding and retrieval

### Building and Testing

#### Building

```bash
# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j4

# Install (optional)
make install
```

#### Testing

```bash
# Run all tests
cd build
ctest --output-on-failure

# Run specific tests
c test -R <test_name>

# Run performance tests
./nlm benchmark --detailed
```

#### Profiling

```bash
# Profile with GProf
cd build
make prof
./nlm --profile
```

### Code Style

#### C++ Style

Follow these guidelines:

- **File naming:** `ClassName.cpp` and `ClassName.hpp`
- **Namespace:** Use `namespace nlm {` for all NLM code
- **Include style:** `#include "header.hpp"` (quotes for local, angle brackets for system)
- **Indentation:** 4 spaces, no tabs
- **Line length:** Maximum 80 characters
- **Naming:** CamelCase for classes, snake_case for functions

#### Python Style

Follow PEP 8:

- **Indentation:** 4 spaces
- **Line length:** Maximum 79 characters
- **Naming:** snake_case for functions and variables
- **Documentation:** Docstrings with Google format

### Documentation

#### API Documentation

Generate API documentation:

```bash
# Generate Doxygen documentation
doxygen Doxyfile

# Generate Python documentation
sphinx-build -b html docs/ docs/_build/html/
```

#### Tutorial Documentation

Create tutorials:

1. **Getting Started:** First-time users
2. **Advanced Topics:** Experienced developers
3. **API Reference:** Complete method documentation
4. **Examples:** Working code examples

### Performance Optimization

#### Identifying Bottlenecks

Use profiling tools:

```bash
# GProf profiling
make prof
./nlm --profile

# Valgrind memory profiling
valgrind --tool=massif ./nlm

# Intel VTune profiling
vtune --collect hotspots ./nlm
```

#### Optimizing Performance

- **Algorithm optimization:** Improve time complexity
- **Memory optimization:** Reduce memory allocation
- **Cache optimization:** Improve data locality
- **Parallel processing:** Use multiple cores
- **Vectorization:** Use SIMD instructions

### Debugging

#### Common Debugging Techniques

- **Print statements:** Debug with `printf`-style output
- **Logging:** Use the NLM logging system
- **Debug mode:** Compile with debug flags
- **Memory debugging:** Use Valgrind or AddressSanitizer
- **Performance debugging:** Use profiling tools

#### Debug Commands

```bash
# Run with debug logging
./nlm --verbose --log-level debug

# Run with valgrind
valgrind ./nlm

# Run with address sanitizer
./nlm --address-sanitizer
```

<a name="configuration-reference"></a>
## Configuration Reference

### Core Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| random_seed | int64 | 42 | Random number generator seed |
| simulation_timestep | double | 0.001 | Simulation timestep in seconds |
| neuron_count | int64 | 1000 | Number of neurons in the brain |
| region_count | int64 | 1 | Number of neural regions |
| connection_probability | float | 0.1 | Probability of connections between neurons |

### Plasticity Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| stdp_ltp_weight | float | 0.01 | STDP long-term potentiation weight |
| stdp_ltd_weight | float | 0.012 | STDP long-term depression weight |
| stdp_tau | float | 20.0 | STDP time constant in milliseconds |
| synaptogenesis_rate | float | 0.0001 | Rate of new synapse formation |
| pruning_rate | float | 0.00001 | Rate of synapse elimination |

### Memory Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| replay_interval | int64 | 100 | Interval for memory replay |
| consolidation_interval | int64 | 1000 | Interval for memory consolidation |

### Development Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| developmental_stage | int64 | 3 | Current developmental stage (0-4) |
| plasticity_modifier | float | 1.0 | Plasticity rate modifier |

### Neuromodulation Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| dopamine_level | float | 0.0 | Dopamine level |
| curiosity_level | float | 0.0 | Curiosity level |
| novelty_level | float | 0.0 | Novelty level |
| prediction_error | float | 0.0 | Prediction error level |

### World Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| world_width | int64 | 20 | World width in cells |
| world_height | int64 | 20 | World height in cells |
| vision_width | int64 | 8 | Vision width in cells |
| vision_height | int64 | 8 | Vision height in cells |
| max_energy | float | 100.0 | Maximum agent energy |
| energy_decay_rate | float | 0.01 | Energy decay rate per timestep |

### Logging Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| log_level | string | "info" | Log level (debug, info, warning, error) |
| log_file | string | "nlm.log" | Log file path |
| enable_console_output | bool | true | Enable console output |
| enable_file_output | bool | false | Enable file output |

<a name="command-reference"></a>
## Command Reference

### Global Options

| Option | Description |
|--------|-------------|
| `--help`, `-h` | Show help message |
| `--version` | Show version information |
| `--verbose`, `-v` | Enable verbose output |
| `--quiet`, `-q` | Suppress non-essential output |
| `--config FILE` | Specify configuration file |

### Command Options

#### `nlm run`

| Option | Description |
|--------|-------------|
| `--steps STEPS` | Number of simulation steps |
| `--neurons N` | Neuron count |
| `--regions R` | Region count |
| `--interactive` | Interactive mode with real-time feedback |
| `--background` | Run in background |
| `--continue` | Continue from existing checkpoint |
| `--checkpoint DIR` | Directory for automatic checkpoints |

#### `nlm init`

| Option | Description |
|--------|-------------|
| `--neurons N` | Number of neurons |
| `--regions R` | Number of regions |
| `--seed SEED` | Random seed |
| `--timestep T` | Simulation timestep |
| `--stdp-ltp W` | STDP LTP weight |
| `--stdp-ltd W` | STDP LTD weight |
| `--synaptogenesis R` | Synaptogenesis rate |
| `--pruning R` | Pruning rate |

#### `nlm save FILE`

| Option | Description |
|--------|-------------|
| `--compress LEVEL` | Compression level (none, fast, balanced, best) |
| `--metadata` | Include metadata |
| `--memory` | Include memory state |
| `--development` | Include development state |
| `--neuromodulation` | Include neuromodulation state |

#### `nlm load FILE`

| Option | Description |
|--------|-------------|
| `--validate` | Validate checkpoint before loading |
| `--force` | Overwrite current brain state |
| `--metadata` | Load metadata |
| `--memory` | Load memory state |
| `--development` | Load development state |
| `--neuromodulation` | Load neuromodulation state |

#### `nlm config KEY [VALUE]`

| Option | Description |
|--------|-------------|
| `KEY` | Configuration key |
| `VALUE` | Configuration value (optional) |

#### `nlm status`

| Option | Description |
|--------|-------------|
| `--detailed` | Show detailed status information |
| `--neurons` | Show neuron statistics |
| `--synapses` | Show synapse statistics |
| `--memory` | Show memory system status |
| `--development` | Show development status |
| `--neuromodulation` | Show neuromodulation status |

#### `nlm stats`

| Option | Description |
|--------|-------------|
| `--spikes` | Show spike statistics |
| `--firing-rate` | Show firing rate information |
| `--plasticity` | Show plasticity rule statistics |
| `--memory` | Show memory system statistics |
| `--network` | Show network topology statistics |
| `--performance` | Show performance metrics |

#### `nlm simulate STEPS`

| Option | Description |
|--------|-------------|
| `--config FILE` | Configuration file |
| `--visualize` | Enable real-time visualization |
| `--checkpoint DIR` | Automatic checkpointing directory |
| `--interval STEPS` | Checkpoint interval |
| `--log-level LEVEL` | Log level (debug, info, warning, error) |
| `--output FILE` | Save statistics to file |

#### `nlm visualize`

| Option | Description |
|--------|-------------|
| `--fullscreen` | Run in fullscreen mode |
| `--output FORMAT` | Output format (terminal, html, file) |
| `--port PORT` | Web server port (default: 8080) |
| `--host HOST` | Web server host (default: localhost) |
| `--neurons N` | Maximum number of neurons to display |
| `--animate` | Enable animation of neural activity |
| `--statistics` | Show statistics panel |

#### `nlm benchmark`

| Option | Description |
|--------|-------------|
| `--duration TIME` | Benchmark duration in seconds (default: 60) |
| `--neurons N` | Number of neurons to benchmark (default: 1000) |
| `--regions R` | Number of regions (default: 1) |
| `--steps STEPS` | Number of steps to benchmark (default: 10000) |
| `--memory MB` | Memory limit in MB |
| `--detailed` | Enable detailed benchmarks |
| `--report FORMAT` | Report format (text, json, csv) |

<a name="python-api-reference"></a>
## Python API Reference

### Configuration

```python
config = pynlm.createDefaultConfig()
config.loadFromFile("config.cfg")
config.saveToFile("config.cfg")
config.has("neuron_count")
config.getKeys()
config.clear()
```

### Brain

```python
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(step)
brain.receiveSensoryInput(input)
brain.injectCurrent(neuron, current)
brain.produceAction()
brain.save("checkpoint")
brain.load("checkpoint")
```

### Agent

```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.processSensoryInput(percept)
action = agent.decodeMotorCommand()
agent.applyRewardModulation(reward, predicted_reward)
```

### World

```python
world = pynlm.createSimpleWorld()
world.configure(width, height, vision_width, vision_height)
world.update(timestep)
world.applyMotorCommand(action, time)
percept = world.getSensoryPercept()
```

### Enums

#### NeuronType
- `Excitatory`
- `Inhibitory`
- `Modulatory`
- `Sensory`
- `Motor`
- `Internal`

#### SynapseType
- `Excitatory`
- `Inhibitory`
- `Modulatory`
- `Electrical`
- `GapJunction`

#### DevelopmentalStage
- `Initial`
- `CriticalPeriod`
- `Maturation`
- `Adult`
- `Aging`

#### FiringState
- `Resting`
- `Active`
- `Refractory`
- `Inhibited`

#### ActionType
- `MoveForward`
- `MoveBackward`
- `MoveLeft`
- `MoveRight`
- `TurnLeft`
- `TurnRight`
- `Look`
- `LookUp`
- `LookDown`
- `Interact`
- `Eat`
- `Drink`
- `Rest`
- `Wait`
- `Custom`

#### MotorCommand
- `MoveForward`
- `MoveBackward`
- `TurnLeft`
- `TurnRight`
- `LookLeft`
- `LookRight`
- `Interact`
- `Wait`

#### WorldObjectType
- `Empty`
- `Resource`
- `Hazard`
- `Wall`
- `Marker`

### Constants

```python
INVALID_NEURON_ID = pynlm.INVALID_NEURON_ID
INVALID_SYNAPSE_ID = pynlm.INVALID_SYNAPSE_ID
INVALID_REGION_ID = pynlm.INVALID_REGION_ID
INVALID_POPULATION_ID = pynlm.INVALID_POPULATION_ID
```

<a name="faq"></a>
## Frequently Asked Questions

### Q: What is the difference between NLM and other neural network frameworks?

A: NLM is a comprehensive artificial brain simulation that goes beyond traditional neural networks:

- **Real spiking dynamics:** Uses LIF neurons with realistic membrane potentials
- **Multiple memory systems:** Working memory, episodic memory, associative memory
- **Neuromodulation:** Dopamine, curiosity, novelty detection
- **Development:** Biologically inspired developmental stages
- **Integration:** All systems work together as a coherent brain

### Q: How do I get started with NLM?

A: Follow these steps:

1. **Install NLM:** `sudo apt-get install cmake python3 python3-pip`
2. **Build from source:** Create build directory and run CMake
3. **Run examples:** `python3 examples/basic_brain.py`
4. **Try CLI:** `./nlm --help` to see available commands
5. **Create your own simulation:** Use configuration files and examples

### Q: What's the difference between Phase 2, Phase 3, and Phase 6?

A: NLM implements different phases of development:

- **Phase 2:** Real neural computation (LIF neurons, STDP plasticity)
- **Phase 3:** World interaction and agent behavior
- **Phase 4:** Integrated cognition and planning
- **Phase 6:** Final integration (all systems working together)

### Q: Can I use NLM for reinforcement learning?

A: Yes! NLM includes neuromodulation systems that support:

- **Reward-based learning:** Dopamine signals for reinforcement
- **Curiosity-driven exploration:** Novelty-based motivation
- **Prediction errors:** Error-based learning signals
- **Plasticity modulation:** Dynamic learning rates

### Q: How do I save and load brain states?

A: Use the CLI or Python API:

```bash
# CLI
./nlm save my_brain_checkpoint
./nlm load my_brain_checkpoint

# Python
brain.save("my_brain_checkpoint")
brain.load("my_brain_checkpoint")
```

### Q: What are the system requirements?

A: NLM can run on:

- **Minimum:** 2GB RAM, 1GHz CPU, 500MB disk
- **Recommended:** 4GB RAM, 2GHz CPU, 2GB disk (for 1000 neurons)
- **High performance:** 16GB+ RAM, multi-core CPU, SSD

<a name="troubleshooting"></a>
## Troubleshooting

### Issue: Build fails with missing dependencies

**Solution:** Install required dependencies:

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential cmake git python3 python3-pip python3-dev

# macOS
brew install cmake python3
```

### Issue: Python bindings fail to build

**Solution:** Install pybind11:

```bash
pip3 install pybind11
```

### Issue: Configuration file not found

**Solution:** Create configuration file:

```bash
nlm_config generate configs/default.cfg
```

### Issue: Memory usage too high

**Solution:** Reduce neuron count or increase system memory:

```bash
./nlm init --neurons 1000  # Reduce neurons
./nlm run --steps 100 --neurons 500  # Smaller simulation
```

### Issue: Simulation too slow

**Solution:** Optimize performance:

```bash
# Use optimized build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Use checkpointing for long simulations
./nlm run --checkpoint my_checkpoints
```

### Issue: Visualization not working

**Solution:** Check dependencies:

```bash
# Install visualization dependencies
pip3 install matplotlib seaborn
```

### Issue: Python module not installed

**Solution:** Install Python module:

```bash
cd python/dist
pip3 install pynlm*.whl
cd ../..
```

### Issue: Tests failing

**Solution:** Run tests with debug information:

```bash
cd build
ctest -V  # Run with verbose output
```

### Issue: Checkpoint corruption

**Solution:** Validate checkpoint before loading:

```bash
./nlm load --validate my_checkpoint
```

<a name="contributing"></a>
## Contributing

### How to Contribute

1. **Fork the repository** and create a feature branch
2. **Implement your changes** with tests and documentation
3. **Run tests** to ensure everything works
4. **Submit a pull request** with a clear description

### Code Quality

- **Write tests** for new functionality
- **Update documentation** for new features
- **Follow coding style** guidelines
- **Add examples** for new functionality
- **Update configuration** files if needed

### Development Workflow

1. **Clone the repository:**

```bash
git clone https://github.com/natural-neural-machine/NLM.git
cd NLM
```

2. **Create a feature branch:**

```bash
git checkout -b feature/my-new-feature
```

3. **Make your changes:**

```bash
# Edit files
git add file1.cpp file2.hpp

# Commit changes
git commit -m "Add my new feature"
```

4. **Push to your fork:**

```bash
git push origin feature/my-new-feature
```

5. **Create a pull request:**

- Describe your changes
- Add tests
- Update documentation
- Request review

### Contribution Guidelines

- **Be respectful:** Treat other contributors with respect
- **Be helpful:** Answer questions and provide guidance
- **Be collaborative:** Work together to solve problems
- **Be patient:** Understand that development takes time
- **Be open-minded:** Consider different approaches and ideas

### Support Channels

- **GitHub Issues:** Report bugs and request features
- **Discussions:** Ask questions and share ideas
- **Pull Requests:** Contribute code changes
- **Email:** support@natural-neural-machine.org

<a name="license"></a>
## License

MIT License

Copyright (c) 2024 Natural Neural Machine Project

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
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

## Contact

For questions, issues, or contributions:
- GitHub: https://github.com/natural-neural-machine/NLM
- Documentation: https://docs.natural-neural-machine.org
- Support: support@natural-neural-machine.org

## Acknowledgements

This project builds on years of research in artificial neural networks and computational neuroscience. Special thanks to:

- The open-source community for valuable libraries and tools
- Researchers in computational neuroscience for inspiring the architecture
- Contributors who have helped develop and improve the system
- Users who have provided feedback and reported issues

## Future Directions

The NLM project continues to evolve with plans for:

- **More advanced neuromodulation:** Additional neuromodulators and mechanisms
- **Social learning:** Learning through social interactions
- **Language processing:** Neural language understanding and generation
- **Abstract reasoning:** Higher-level cognitive abilities
- **Emotion systems:** Emotional learning and expression
- **Long-term memory:** More sophisticated memory consolidation
- **Transfer learning:** Knowledge transfer across tasks
- **Meta-learning:** Learning how to learn
- **Autonomous development:** Self-directed developmental trajectories
- **Consciousness research:** Investigating the neural basis of consciousness

The journey to create a truly intelligent artificial brain continues, and NLM represents one step forward in this exciting endeavor.