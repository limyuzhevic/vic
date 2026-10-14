# NLM (熙然) - Neural Learning Machine
# Comprehensive API Reference & User Guide

**Version:** 2.0.0 - Phase 2: Advanced Neural Computation
**Last Updated:** 2026-10-14

## Table of Contents

1. [Quick Start Guide](#quick-start-guide)
2. [System Architecture](#system-architecture)
3. [Core API Classes](#core-api-classes)
   - [Brain Class](#brain-class)
   - [Config Class](#config-class)
   - [Neuron Class](#neuron-class)
   - [Synapse Class](#synapse-class)
4. [Advanced Features](#advanced-features)
   - [Memory Systems](#memory-systems)
   - [Neuromodulation](#neuromodulation)
   - [Development](#development)
   - [Plasticity](#plasticity)
5. [Command Line Interface](#command-line-interface)
6. [Configuration Guide](#configuration-guide)
7. [Troubleshooting](#troubleshooting)
8. [Future Directions](#future-directions)

---

## Quick Start Guide

### What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project that implements real spiking neural computation. Unlike transformers or LLMs, NLM creates a brain-like system that:

- Uses Leaky Integrate-and-Fire (LIF) neurons
- Implements event-driven spike propagation with synaptic delays
- Supports STDP and Hebbian plasticity rules
- Features structural plasticity (synaptogenesis/pruning)
- Integrates memory, prediction, and cognition systems

### Installation

The NLM project uses CMake for building. For most users, the Python bindings provide the easiest interface:

```bash
# Build from source
cd /path/to/nlm
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4

# For Python users
pip install -e .  # Install Python bindings
```

### Basic Usage Examples

#### 1. Simple Neural Computation Demo

```bash
# Run the basic neural computation demo (Phase 2)
./nlm  # Runs basic connectivity and plasticity tests

# With custom configuration
./nlm --neurons 1000 --steps 5000 --seed 12345
```

#### 2. Phase 6 Integration Demo

```bash
# Run comprehensive integration test
./nlm --mode integration --phase6-demo

# Quick integration verification only
./nlm --mode integration --phase6-quick
```

#### 3. Python Interface

```python
from nlm import Brain, Config

# Create and configure brain
config = Config()
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("connection_probability", 0.15)

brain = Brain(config)
if not brain.initialize():
    print("Failed to initialize brain!")
    exit(1)

# Run simulation
brain.step(0, 0.0)
brain.step(1, 0.001)
print(f"Spikes: {brain.getTotalSpikeCount()}")
```

#### 4. Experimental Mode

```bash
# Run specific plasticity experiments
./nlm --mode experimental --experiment plasticity

# Run STDP verification
./nlm --mode experimental --experiment stdp

# Run memory system test
./nlm --mode experimental --experiment memory
```

### Understanding the Architecture

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

---

## System Architecture

### Core Components

#### Neural Computation Layer
- **Brain**: Central coordinator with neural populations and inter-region connections
- **Neuron**: Leaky Integrate-and-Fire (LIF) neuron model
- **Synapse**: Event-driven synaptic transmission with delays
- **SpikeSystem**: Event-driven spike propagation engine

#### Plasticity Systems
- **STDP**: Spike-Timing-Dependent Plasticity (LTP/LTD)
- **Hebbian**: Activity-dependent synaptic strengthening
- **StructuralPlasticity**: Synaptogenesis and pruning

#### Memory Systems
- **WorkingMemory**: Persistent activity for short-term information storage
- **EpisodicMemory**: Experience encoding and replay
- **SemanticMemory**: Pattern association and knowledge representation
- **ProceduralMemory**: Skill and behavior storage

#### Neuromodulation
- **Dopamine**: Reward prediction and reinforcement
- **Curiosity**: Exploration motivation and novelty detection
- **PredictionError**: Surprise signaling
- **Novelty**: Change detection and salience

#### Cognitive Systems
- **NeuralPlanner**: Action sequence evaluation and planning
- **ConceptFormation**: Pattern discovery and abstraction
- **AttentionalSelection**: Competitive selection and focus

#### Development
- **DevelopmentSystem**: Age-dependent plasticity and maturation
- **Synaptogenesis**: Formation of new synaptic connections
- **Pruning**: Elimination of unused connections
- **Maturation**: System refinement over time

#### Integration Layer
- **AgentBrain**: Connection between NLM and world
- **SimpleWorld**: Controlled environment for testing
- **VisualizationInterface**: Output and monitoring

---

## Core API Classes

### Brain Class

The Brain class is the central coordinator of the NLM system. It manages:

- Neural populations and regions
- Spike generation and propagation
- Plasticity mechanisms (STDP, Hebbian, structural)
- Memory systems integration
- Neuromodulation signals
- Development stages
- Checkpoint save/load

#### Key Methods

```cpp
// Constructor
Brain(std::shared_ptr<Config> config);

// Initialization
bool initialize();

// Core simulation
void step(SimulationStep step);
void step(SimulationStep step, Timestamp currentTime);

// Input/output
void receiveSensoryInput(const SensoryInput& input);
void injectCurrent(NeuronId neuron, MembranePotential current);
void injectCurrentToNeurons(NeuronType type, MembranePotential current);

// Memory access
NeuralWorkingMemory* getWorkingMemory();
NeuralEpisodicMemory* getEpisodicMemory();

// Plasticity
void updatePlasticity();

// Development
void develop();

// Output
std::unique_ptr<Action> produceAction();

// Checkpoint
bool save(const std::string& filepath) const;
bool load(const std::string& filepath);

// Statistics
size_t getTotalNeuronCount() const;
size_t getTotalSynapseCount() const;
size_t getTotalSpikeCount() const;
float getAverageFiringRate() const;
```

#### Important Brain Features

1. **Real LIF Neural Dynamics**: Neurons integrate inputs and fire spikes when threshold is reached
2. **Event-Driven Spike Propagation**: Spikes propagate through synapses with configurable delays
3. **Multiple Plasticity Mechanisms**: STDP, Hebbian, and structural plasticity all work together
4. **Integrated Memory Systems**: All memory types are connected to neural processing
5. **Neuromodulation Integration**: Dopamine, curiosity, and novelty affect learning and behavior
6. **Development**: Age-dependent plasticity rates and structural changes
7. **Checkpoint System**: Save/load functionality for state persistence

### Config Class

The Config class manages system parameters and settings. It provides:

- Type-safe value storage (int, double, string, bool, vectors)
- Configuration persistence (file and command-line loading)
- Source tracking (default, file, command-line, runtime)
- Value validation and error handling

#### Key Methods

```cpp
// Constructor
Config();

// Loading
bool loadFromFile(const std::string& filepath);
bool loadFromArgs(int argc, char** argv);

// Getting values
std::optional<int> get<int>(const std::string& key) const;
template<typename T>
T getOr(const std::string& key, const T& defaultValue) const;

// Setting values
void set(const std::string& key, const std::string& value);
void set(const std::string& key, int value);
void set(const std::string& key, double value);
void set(const std::string& key, bool value);

// Configuration management
bool has(const std::string& key) const;
void remove(const std::string& key);
std::vector<std::string> getKeys() const;
```

#### Common Configuration Parameters

```yaml
# Neural parameters
neuron_count: 1000                    # Total neurons in system
region_count: 1                       # Number of neural regions
connection_probability: 0.15          # Probability of synapse formation
simulation_timestep: 0.001            # Time step for integration
random_seed: 42                       # Random number generator seed

# Plasticity parameters
stdp_ltp_weight: 0.02                 # STDP LTP weight
stdp_ltd_weight: 0.015                # STDP LTD weight
stdp_tau: 20.0                        # STDP time constant
synaptogenesis_rate: 0.0001           # Rate of new synapse formation
pruning_rate: 0.00001                 # Rate of synapse elimination

# Memory parameters
working_memory_size: 100              # Capacity of working memory
episodic_memory_capacity: 50          # Number of episodes

# Development parameters
developmental_stage: "adolescent"    # Current developmental stage
plasticity_modifier: 1.0              # Multiplier for plasticity rate

# Neuromodulation parameters
 dopamine_baseline: 0.5               # Baseline dopamine level
curiosity_sensitivity: 0.3            # Sensitivity to novel stimuli
```

### Neuron Class

The Neuron class implements a Leaky Integrate-and-Fire (LIF) neuron model:

#### Key Features

- **Membrane Potential Dynamics**: Integration of synaptic inputs with exponential decay
- **Threshold-based Firing**: Spikes when membrane potential reaches threshold
- **Reset Mechanism**: Membrane potential resets after firing
- **Neuron Types**: Differentiates between sensory, motor, and internal neurons
- **Current Injection**: External current input for testing and control

#### Key Methods

```cpp
// State management
void injectCurrent(double current);   // External current input
void update(double dt);               // Integration step
bool hasSpiked() const;               // Check if neuron fired

// Properties
MembranePotential getMembranePotential() const;
MembranePotential getThreshold() const;
MembranePotential getResetPotential() const;
NeuronType getNeuronType() const;
```

### Synapse Class

The Synapse class models synaptic transmission:

#### Key Features

- **Event-Driven Propagation**: Spikes propagate with configurable delays
- **Excitatory/Inhibitory**: Differentiates between positive and negative effects
- **Plasticity Support**: Can be modified by STDP, Hebbian, and structural plasticity
- **Weight Management**: Synaptic strength can be adjusted over time
- **Delay Management**: Configurable transmission delays

#### Key Methods

```cpp
// State management
void onPreSynapticSpike(Timestamp time);
bool isReady(double currentTime) const;
void applyToPostSynaptic(Neuron* postNeuron);

// Plasticity
void enablePlasticity(bool hebbian, bool stdp, bool structural);
void applyPlasticity(double dt, bool preFired, bool postFired);

// Properties
float getWeight() const;
void setWeight(float weight);
float getDelay() const;
```

---

## Advanced Features

### Memory Systems

#### Working Memory
- **Purpose**: Persistent activity for short-term information storage
- **Mechanism**: Recurrent connections maintain activity through competition
- **Capacity**: Configurable number of active traces
- **Integration**: Connected to neural processing and attention systems

#### Episodic Memory
- **Purpose**: Storage of experienced events and their temporal sequences
- **Mechanism**: Pattern completion and replay during sleep/rest
- **Capacity**: Configurable number of stored episodes
- **Integration**: Connected to prediction and development systems

#### Semantic Memory
- **Purpose**: Abstract knowledge and pattern associations
- **Mechanism**: Hebbian learning forms connections between related patterns
- **Capacity**: Large storage capacity for learned associations
- **Integration**: Connected to concept formation and planning systems

### Neuromodulation

#### Dopamine System
- **Function**: Reward prediction, reinforcement learning, and motivation
- **Mechanism**: Phasic bursts signal prediction errors
- **Effect**: Modulates plasticity and neural excitability
- **Integration**: Works with curiosity and prediction error systems

#### Curiosity System
- **Function**: Exploration motivation and novelty detection
- **Mechanism**: Combines novelty detection with prediction error
- **Effect**: Increases exploration in novel or unpredictable environments
- **Integration**: Connected to action selection and learning

#### Prediction Error
- **Function**: Surprise signaling and model updating
- **Mechanism**: Compares predicted vs. actual sensory input
- **Effect**: Triggers learning and plasticity
- **Integration**: Central to learning and development

### Development

#### Developmental Stages
- **Initial**: High plasticity, rapid learning, exploratory behavior
- **Adolescent**: Declining plasticity, increased efficiency
- **Adult**: Stable performance with refined connections
- **Maturation**: Continued structural and functional refinement

#### Synaptogenesis
- **Mechanism**: Formation of new synaptic connections based on activity
- **Rate**: Declines with developmental age
- **Effect**: Expands neural network capacity
- **Integration**: Connected to structural plasticity and development systems

#### Pruning
- **Mechanism**: Elimination of weak or unused synaptic connections
- **Rate**: Varies with developmental stage
- **Effect**: Increases efficiency and reduces noise
- **Integration**: Balanced with synaptogenesis for optimal development

### Plasticity

#### STDP (Spike-Timing-Dependent Plasticity)
- **Mechanism**: Synaptic strength depends on precise timing of pre- and post-synaptic spikes
- **LTP**: Pre before post strengthens synapse
- **LTD**: Post before pre weakens synapse
- **Integration**: Works with other plasticity mechanisms

#### Hebbian Plasticity
- **Mechanism**: "Neurons that fire together, wire together"
- **Rule**: Strengthen synapses for simultaneous activity
- **Application**: Pattern completion and memory formation
- **Integration**: Complements STDP for comprehensive learning

#### Structural Plasticity
- **Mechanism**: Physical changes to neural connections
- **Components**: Synaptogenesis and pruning
- **Time Scale**: Longer-term changes compared to synaptic plasticity
- **Integration**: Affects overall network architecture and capacity

---

## Command Line Interface

### Overview

The NLM command-line interface provides comprehensive control over neural simulation experiments:

### Basic Usage

```bash
# Basic demo (default mode)
./nlm

# Run with custom configuration
./nlm --neurons 2000 --steps 10000 --seed 12345

# Run Phase 6 integration test
./nlm --mode integration --phase6-demo

# Run experimental plasticity study
./nlm --mode experimental --experiment plasticity
```

### Command Line Options

#### Mode Selection

- `--mode basic` - Run basic connectivity and plasticity tests
- `--mode experimental` - Run experimental setup with custom configurations
- `--mode integration` - Run Phase 6 integration test
- `--mode interactive` - Interactive exploration mode
- `--mode analysis` - Analyze brain state and visualize results
- `--mode replay` - Replay from checkpoint files
- `--mode checkpoint` - Checkpoint management demo
- `--mode plasticity` - Advanced plasticity exploration

#### Configuration Options

- `--config <file>` - Load configuration from file
- `--neurons <n>` - Set neuron count (default: 500)
- `--regions <n>` - Set region count (default: 1)
- `--connections <p>` - Set connection probability (default: 0.15)
- `--timestep <t>` - Set simulation timestep (default: 0.001)
- `--steps <n>` - Set maximum simulation steps (default: 1000)
- `--seed <n>` - Set random seed for reproducibility

#### Advanced Options

- `--checkpoint <file>` - Checkpoint file path
- `--visualize` - Enable visualization mode
- `--debug` - Enable debug logging
- `--quiet` - Quiet mode (only errors)
- `--save-progress` - Save progress periodically
- `--stats` - Print detailed statistics
- `--export <format>` - Export results (json, binary, csv)

#### Phase 6 Integration Options

- `--phase6` - Run complete Phase 6 integration test
- `--phase6-quick` - Quick integration verification only
- `--phase6-demo` - Demo mode with reduced complexity

#### Experiment Types

- `--experiment basic` - Basic connectivity test
- `--experiment plasticity` - Plasticity learning experiment
- `--experiment stdp` - STDP verification
- `--experiment memory` - Memory system test
- `--experiment full` - All experiments combined

#### Output Options

- `--output <dir>` - Output directory for logs and results
- `--log-level <level>` - Log level: DEBUG, INFO, WARNING, ERROR, CRITICAL
- `--no-banner` - Skip banner display

### Interactive Mode

When running in interactive mode (`--mode interactive`), the following commands are available:

#### System Control
- `status` - Show current brain status and statistics
- `reset` - Reset brain to initial state
- `step <n>` - Advance simulation by n steps
- `help` - Show this help message

#### Brain Exploration
- `neurons` - List all neurons and their properties
- `synapses` - List all synapses and their weights
- `memory` - Check memory system status
- `neuromod` - Check neuromodulation system

#### Experiment Control
- `run <experiment>` - Run specific experiment (basic, plasticity, stdp)
- `save <file>` - Save current brain state to file
- `load <file>` - Load brain state from file

#### Configuration
- `config <key> <value>` - Set configuration parameter
- `showconfig` - Display current configuration
- `resetconfig` - Reset to default configuration

#### Simulation
- `simulate <mode>` - Start simulation in specified mode
- `visualize` - Show visualization (if available)
- `analyze` - Run analysis on current brain state

---

## Configuration Guide

### Understanding Configuration

The NLM configuration system uses key-value pairs to control all aspects of neural simulation. Configuration can be specified through:

1. **Configuration Files**: Plain text files with `key = value` format
2. **Command Line Arguments**: Override configuration via command-line
3. **Runtime Settings**: Programmatically modify during execution

### Configuration File Format

NLM uses a simple key-value format for configuration files:

```ini
# Configuration for NLM neural simulation

# Neural parameters
neuron_count = 1000
region_count = 2
connection_probability = 0.15
simulation_timestep = 0.001

# Plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Memory parameters
working_memory_size = 100
episodic_memory_capacity = 50

# Development parameters
developmental_stage = adolescent
plasticity_modifier = 1.0

# Neuromodulation parameters
dopamine_baseline = 0.5
curiosity_sensitivity = 0.3

# Simulation parameters
max_steps = 5000
replay_interval = 100
consolidation_interval = 1000
```

### Configuration Files vs Command Line

Configuration files provide organized, persistent settings, while command-line arguments provide quick overrides for specific experiments. The precedence order is:

1. **Runtime settings** (highest priority)
2. **Command-line arguments**
3. **Configuration files**
4. **Default values** (lowest priority)

### Common Configuration Scenarios

#### Research Simulation

For scientific research and experimentation:

```bash
./nlm \
  --config research_config.cfg \
  --neurons 5000 \
  --steps 20000 \
  --seed 42 \
  --experiment plasticity \
  --stats \
  --export json
```

#### Development and Testing

For debugging and development:

```bash
./nlm \
  --config dev_config.cfg \
  --neurons 100 \
  --steps 1000 \
  --debug \
  --visualize
```

#### Educational Demo

For demonstrating NLM capabilities:

```bash
./nlm \
  --config demo_config.cfg \
  --neurons 200 \
  --steps 500 \
  --quiet \
  --stats
```

### Configuration Validation

The configuration system validates parameters and provides informative error messages. Common issues include:

- **Invalid numeric values**: Non-numeric values for numeric parameters
- **Out-of-range values**: Connection probability outside [0.0, 1.0]
- **Missing required parameters**: Essential parameters not specified
- **Conflicting parameters**: Incompatible parameter combinations

### Configuration Troubleshooting

#### Symptom: Brain fails to initialize

```bash
# Check error messages and configuration
./nlm --debug --config your_config.cfg

# Try with minimal configuration
./nlm --neurons 100 --steps 1000
```

#### Symptom: Simulation runs too slowly

```bash
# Reduce complexity
./nlm --neurons 500 --steps 2000

# Use more efficient configuration
./nlm --config optimized_config.cfg
```

#### Symptom: Memory issues

```bash
# Reduce memory usage
./nlm --neurons 500 --memory_limit 512MB

# Monitor memory usage
./nlm --monitor_memory
```

---

## Troubleshooting

### Common Issues and Solutions

#### 1. Brain Initialization Fails

**Symptoms**: "Failed to initialize brain!" error messages

**Solutions**:

1. **Check configuration parameters**:
   ```bash
   ./nlm --debug --config your_config.cfg
   ```

2. **Use default configuration**:
   ```bash
   ./nlm --neurons 100 --steps 1000
   ```

3. **Verify system resources**:
   - Check available memory
   - Ensure proper permissions for output directory

#### 2. Simulation Runs Too Slowly

**Symptoms**: Simulation takes much longer than expected

**Solutions**:

1. **Reduce neuron count**:
   ```bash
   ./nlm --neurons 500  # Instead of 1000+
   ```

2. **Reduce simulation steps**:
   ```bash
   ./nlm --steps 1000  # Instead of 10000+
   ```

3. **Optimize configuration**:
   ```bash
   ./nlm --config optimized_config.cfg
   ```

#### 3. Memory Issues

**Symptoms**: System runs out of memory or crashes

**Solutions**:

1. **Reduce complexity**:
   ```bash
   ./nlm --neurons 500  # Fewer neurons
   ```

2. **Enable memory monitoring**:
   ```bash
   ./nlm --monitor_memory
   ```

3. **Use memory-efficient configuration**:
   ```bash
   ./nlm --config memory_efficient.cfg
   ```

#### 4. Checkpoint Issues

**Symptoms**: Failed to save/load checkpoints

**Solutions**:

1. **Check file permissions**:
   ```bash
   chmod 755 /path/to/checkpoint_dir
   ```

2. **Verify disk space**:
   ```bash
   df -h /path/to/checkpoint_dir
   ```

3. **Try different checkpoint location**:
   ```bash
   ./nlm --checkpoint ./checkpoints/brain_01.bin
   ```

### Error Message Reference

| Error Message | Cause | Solution |
|---------------|-------|----------|
| "Failed to initialize brain!" | Invalid configuration | Check configuration parameters |
| "Config file not found" | Missing configuration file | Use --config option or create file |
| "Out of memory" | Insufficient system resources | Reduce neuron count or increase memory |
| "Division by zero" | Invalid parameter values | Check configuration for zero values |
| "File permission denied" | Insufficient file permissions | Change file permissions or use different location |

### Getting Help

#### Command Line Help

```bash
# Get help on command line options
./nlm --help

# Get version information
./nlm --version

# Get help on specific mode
./nlm --mode experimental --help
```

#### Interactive Help

In interactive mode, use:

```
help                    # Show general help
help status            # Show status command help
help config            # Show configuration command help
```

#### Documentation

- **Main Documentation**: `README.md`
- **API Reference**: `docs/` directory
- **User Guide**: `HOW_TO_USE.md`
- **Advanced Usage**: `easy_usage.md`

---

## Future Directions

### Planned Improvements

1. **Enhanced Memory Integration**
   - Full integration of all memory systems with neural processing
   - Advanced episodic memory with replay mechanisms
   - Semantic memory with pattern discovery

2. **Advanced Neuromodulation**
   - Multi-dimensional neuromodulation system
   - Adaptive learning rates based on neuromodulatory signals
   - Integration with developmental processes

3. **Improved Development Model**
   - More realistic developmental trajectory
   - Activity-dependent developmental changes
   - Individual differences in development

4. **Performance Optimizations**
   - SIMD vectorization for faster neural computation
   - Memory pooling for reduced allocation overhead
   - Parallel processing for larger networks

5. **Enhanced Visualization**
   - Real-time neural activity visualization
   - Memory system monitoring
   - Development trajectory tracking

6. **Advanced User Interfaces**
   - Web-based interface for remote access
   - API for custom applications
   - Plugin system for extensions

### Research Opportunities

1. **Biological Fidelity**
   - More realistic neuron models
   - Incorporation of biological constraints
   - Validation against experimental data

2. **Learning Mechanisms**
   - Investigation of learning algorithms
   - Study of developmental learning trajectories
   - Analysis of memory consolidation processes

3. **Behavioral Modeling**
   - Development of complex behaviors
   - Integration with motor systems
   - Study of decision-making processes

4. **Applications**
   - Neuromorphic computing
   - Brain-inspired artificial intelligence
   - Educational and training applications

---

## Acknowledgments

The NLM project builds on decades of research in computational neuroscience, artificial intelligence, and neural engineering. Special thanks to:

- **Research Contributors**: All scientists who have contributed to neural modeling research
- **Open Source Community**: Libraries and tools that enable scientific computing
- **Funding Agencies**: Organizations that support fundamental research in AI and neuroscience
- **Users and Testers**: Who provide feedback and help improve the system

---

## License

MIT License. See `LICENSE` file for details.

---

*Document generated automatically from NLM source code and documentation.*
*Last updated: 2026-10-14*
*Contact: nlm-project@nlm.org*}