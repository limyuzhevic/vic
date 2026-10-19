# NLM Phase 6 User Guide
# Neural Learning Machine - Final Integration

Version: 0.1.0 (Phase 6 - Final Integration)

This document provides comprehensive guidance for using NLM Phase 6, the final integration of all neural systems into a complete artificial brain.

## Table of Contents

1. [Overview](#overview)
2. [Quick Start](#quick-start)
3. [Command Line Interface](#command-line-interface)
4. [Configuration](#configuration)
5. [Build System](#build-system)
6. [Running Experiments](#running-experiments)
7. [Memory Systems](#memory-systems)
8. [Neuromodulation](#neuromodulation)
9. [Cognitive Processing](#cognitive-processing)
10. [Development](#development)
11. [Performance Optimization](#performance-optimization)
12. [Troubleshooting](#troubleshooting)
13. [API Reference](#api-reference)

---

## Overview

NLM (熙然) Phase 6 represents the complete integration of all neural systems into a coherent artificial brain. This is the culmination of years of development across multiple phases:

### What NLM Phase 6 Is

- **Event-driven spiking neural network** with Leaky Integrate-and-Fire (LIF) neurons
- **Complete brain system** with all major subsystems integrated:
  - Working memory (transient information holding)
  - Episodic memory (experience storage and replay)
  - Associative memory (pattern associations)
  - Neuromodulation (dopamine, curiosity, novelty, prediction error)
  - Prediction systems (predictive coding and error computation)
  - Cognitive systems (planning, concept formation, attention)
  - Development system (age-dependent plasticity and maturation)

### What Makes Phase 6 Special

1. **Unified Architecture**: All subsystems work together in a closed-loop system
2. **Real-time Learning**: Experience-driven synaptic plasticity through interaction with environment
3. **Multi-timescale Processing**: Fast neural dynamics (spikes) combined with slow plasticity (learning)
4. **Memory Integration**: Seamless interaction between different memory systems
5. **Neuromodulation Coordination**: All learning signals coordinated through neuromodulators

### Key Capabilities

- **Experience-driven learning** from environmental interaction
- **Autonomous exploration** through curiosity and novelty detection
- **Goal-directed behavior** through planning and prediction
- **Memory consolidation** during rest and sleep cycles
- **Developmental progression** from naive to mature brain
- **Complex task solving** through integrated cognitive processing

---

## Quick Start

This section provides the quickest way to get started with NLM Phase 6.

### Prerequisites

1. **C++17 compatible compiler** (GCC 9+, Clang 10+, or MSVC 19+)
2. **CMake 3.16 or higher**
3. **Make or Ninja build system**
4. **大约 2GB RAM** for full simulation

### Installation Steps

1. **Clone the repository**:
   ```bash
   git clone https://github.com/nlm-project/nlm.git
   cd nlm
   ```

2. **Run the setup script** (recommended):
   ```bash
   chmod +x setup.sh
   ./setup.sh quick
   ```

3. **Or build manually**:
   ```bash
   mkdir build
   cd build
   cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
   cmake --build . -- -j$(nproc)
   ```

### First Run

After building, run the integration demo:

```bash
# Run the complete integration demo
./nlm --experiment demo

# Or run a specific experiment
./nlm --experiment plasticity
./nlm --experiment stdp
./nlm --benchmark
```

### Common Commands

```bash
# Help and information
./nlm --help
./nlm --version

# Run with different configurations
./nlm --config configs/basic.cfg --verbose
./nlm --steps 1000 --timestep 0.0005
./nlm --save-brain brain_state.bin
```

---

## Command Line Interface

NLM Phase 6 provides a comprehensive command-line interface for controlling the simulation and experimentation.

### Basic Options

| Option | Description | Example |
|--------|-------------|---------|
| `--help` | Show help message | `./nlm --help` |
| `--version` | Show version information | `./nlm --version` |
| `--verbose` | Enable verbose logging | `./nlm --verbose` |
| `--benchmark` | Run performance benchmark | `./nlm --benchmark` |

### Configuration Options

| Option | Description | Default |
|--------|-------------|---------|
| `--config FILE` | Load configuration from file | `configs/default.cfg` |
| `--steps N` | Run for N simulation steps | `0` (full demo) |
| `--timestep SECONDS` | Set simulation timestep | `0.001` |
| `--seed SEED` | Set random seed | `42` |
| `--log-level LEVEL` | Set log level (DEBUG, INFO, WARNING, ERROR) | `INFO` |

### Experiment Types

| Experiment Type | Description |
|----------------|-------------|
| `demo` | Complete integration demonstration |
| `plasticity` | Plasticity learning experiment |
| `stdp` | Spike-timing dependent plasticity experiment |
| `connectivity` | Basic neural connectivity test |

### Advanced Options

| Option | Description |
|--------|-------------|
| `--save-brain FILE` | Save brain state to file |
| `--load-brain FILE` | Load brain state from file |
| `--output FILE` | Save run results to file |
| `--experiment TYPE` | Run specific experiment |

### Usage Examples

#### 1. Full Integration Demo
```bash
./nlm --experiment demo
```

#### 2. Custom Configuration
```bash
./nlm --config configs/basic.cfg --steps 5000 --verbose
```

#### 3. Save Brain State
```bash
./nlm --experiment demo --save-brain brain_state.bin
```

#### 4. Load and Continue
```bash
./nlm --experiment demo --load-brain brain_state.bin
```

#### 5. Performance Benchmark
```bash
./nlm --benchmark --verbose --output benchmark_results.dat
```

#### 6. Specific Experiment
```bash
./nlm --experiment plasticity --steps 10000
```

---

## Configuration

NLM Phase 6 uses flexible configuration files to control all aspects of the simulation.

### Configuration File Format

The configuration files use simple key-value pairs:

```
# Comment
key = value
another_key = another_value
```

### Available Configuration Options

#### Core Simulation Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `random_seed` | integer | `42` | Random seed for reproducible experiments |
| `simulation_timestep` | float | `0.001` | Simulation timestep in seconds |
| `neuron_count` | integer | `500` | Number of neurons in the brain |
| `region_count` | integer | `3` | Number of brain regions |
| `connection_probability` | float | `0.15` | Probability of synaptic connections |

#### Plasticity Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `plasticity_learning_rate` | float | `0.02` | General learning rate |
| `stdp_ltp_weight` | float | `0.02` | STDP long-term potentiation weight |
| `stdp_ltd_weight` | float | `0.015` | STDP long-term depression weight |
| `stdp_time_constant` | float | `20.0` | STDP time constant (ms) |
| `hebbian_learning_rate` | float | `0.01` | Hebbian learning rate |
| `structural_plasticity_rate` | float | `0.0001` | Rate of structural plasticity |

#### Development Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `development_synaptogenesis_rate` | float | `0.0005` | Rate of new synapse formation |
| `development_pruning_rate` | float | `0.00005` | Rate of synapse pruning |
| `development_plasticity_boost` | float | `2.0` | Plasticity boost during development |

#### Neuromodulation Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `dopamine_baseline` | float | `0.1` | Baseline dopamine level |
| `dopamine_learning_rate` | float | `0.05` | Dopamine learning rate |
| `dopamine_time_constant` | float | `100.0` | Dopamine time constant |
| `curiosity_rate` | float | `0.3` | Curiosity-driven exploration rate |
| `novelty_threshold` | float | `0.1` | Novelty detection threshold |
| `prediction_error_discount` | float | `0.95` | Prediction error discount factor |

#### Memory System Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `working_memory_capacity` | integer | `100` | Maximum working memory items |
| `episodic_memory_max_episodes` | integer | `1000` | Maximum episodic memory episodes |
| `associative_memory_pattern_count` | integer | `500` | Maximum associative memory patterns |

#### Cognitive System Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `neural_planner_depth` | integer | `5` | Planning depth for neural planner |
| `concept_formation_threshold` | float | `0.5` | Concept formation threshold |
| `attention_inhibition_strength` | float | `0.5` | Attention inhibition strength |
| `attention_excitation_strength` | float | `1.5` | Attention excitation strength |

#### Prediction System Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `prediction_accuracy_threshold` | float | `0.8` | Prediction accuracy threshold |
| `prediction_error_threshold` | float | `0.1` | Prediction error threshold |
| `prediction_update_rate` | float | `0.1` | Prediction update rate |

#### Environment Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `environment_name` | string | `ComplexWorld` | Environment name |
| `environment_width` | integer | `20` | Environment width |
| `environment_height` | integer | `20` | Environment height |
| `max_steps_per_episode` | integer | `100` | Maximum steps per episode |

#### Logging Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `log_level` | string | `INFO` | Log level (DEBUG, INFO, WARNING, ERROR) |
| `log_to_file` | boolean | `true` | Write logs to file |
| `log_filename` | string | `nlm.log` | Log file name |
| `log_file_max_size` | string | `100MB` | Maximum log file size |
| `log_rotation_count` | integer | `5` | Number of log rotations |

#### Performance Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `use_simd` | boolean | `true` | Use SIMD optimizations |
| `use_parallel_processing` | boolean | `true` | Use parallel processing |
| `use_memory_pools` | boolean | `true` | Use memory pools |
| `max_concurrent_threads` | integer | `4` | Maximum concurrent threads |

#### Experimental Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `integration_experiment_enabled` | boolean | `true` | Enable integration experiments |
| `integration_test_steps` | integer | `5000` | Number of steps for integration test |
| `integration_benchmark_mode` | boolean | `false` | Run in benchmark mode |
| `integration_detailed_logging` | boolean | `true` | Enable detailed logging |

### Configuration Files

The project includes several pre-configured files:

1. **`configs/default.cfg`** - Complete Phase 6 configuration
2. **`configs/basic.cfg`** - Simplified configuration for quick testing
3. **`configs/custom_template.cfg`** - Template for creating custom configurations

### Creating Custom Configurations

To create a custom configuration:

1. Copy the template: `cp configs/custom_template.cfg my_config.cfg`
2. Edit `my_config.cfg` with your desired settings
3. Run NLM with: `./nlm --config my_config.cfg`

### Command Line Override

Command-line arguments override configuration file settings:

```bash
./nlm --config configs/default.cfg --steps 10000 --log-level DEBUG
```

In this example, `--steps 10000` and `--log-level DEBUG` override the corresponding values in the config file.

---

## Build System

NLM Phase 6 provides a simplified build system with multiple build options.

### Quick Build Commands

```bash
# Build with default settings (Release)
mkdir build
cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -- -j$(nproc)

# Build with debug symbols
mkdir build_debug
cd build_debug
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . -- -j$(nproc)
```

### Makefile Targets

If you have the Makefile (in the project root), you can use these commands:

| Target | Description |
|--------|-------------|
| `make build` | Build the project (Release) |
| `make build-debug` | Build with debug symbols |
| `make rebuild` | Clean and rebuild |
| `make test` | Run unit tests |
| `make integration-test` | Run integration tests |
| `make benchmark` | Run performance benchmarks |
| `make run` | Run Phase 6 integration demo |
| `make run-plasticity` | Run plasticity experiment |
| `make run-stdp` | Run STDP experiment |
| `make run-benchmark` | Run performance benchmark |
| `make format` | Format code with clang-format |
| `make analyze` | Run static analysis |
| `make clean` | Clean build artifacts |
| `make clean-all` | Clean everything |
| `make help` | Show help message |

### Build Options

You can customize the build with CMake options:

```bash
cmake -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Release \
    -DTESTS_ENABLED=ON \
    -DBENCHMARKS_ENABLED=ON \
    -DUSE_SIMD=ON \
    -DUSE_PARALLEL=ON \
    ..
```

### Dependency Management

NLM Phase 6 has no external dependencies beyond standard C++ libraries. The build system handles all internal dependencies automatically.

### Cross-Compilation

For cross-compilation, you can specify different generators:

```bash
# For Windows
cmake -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ ..

# For macOS (if targeting ARM)
cmake -G "Unix Makefiles" -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" ..
```

---

## Running Experiments

This section provides detailed instructions for running different types of experiments with NLM Phase 6.

### 1. Integration Demo (`--experiment demo`)

The integration demo showcases all integrated systems working together.

**What it tests:**
- Complete brain systems integration
- Memory system functionality (working, episodic, associative)
- Neuromodulation coordination (dopamine, curiosity, novelty, prediction error)
- Cognitive processing (planning, concept formation, attention)
- Development system progression
- Experience-driven learning

**Command:**
```bash
./nlm --experiment demo
```

**Output:**
- Comprehensive logs showing system status
- Real-time progress indicators
- Final brain status report
- Integration success verification

### 2. Plasticity Experiment (`--experiment plasticity`)

The plasticity experiment focuses on experience-driven synaptic changes.

**What it tests:**
- Hebbian learning
- STDP (spike-timing dependent plasticity)
- Structural plasticity (synaptogenesis/pruning)
- Long-term memory consolidation

**Command:**
```bash
./nlm --experiment plasticity --steps 10000
```

**Configuration notes:**
- Uses higher learning rates to ensure observable changes
- Applies repeated sensory input patterns
- Tracks synaptic weight changes over time

### 3. STDP Experiment (`--experiment stdp`)

The STDP experiment specifically tests spike-timing dependent plasticity.

**What it tests:**
- Pre-before-post spike pairs (potentiation)
- Post-before-pre spike pairs (depression)
- Weight bounds and clamping
- Eligibility trace dynamics

**Command:**
```bash
./nlm --experiment stdp
```

**Output:**
- Detailed weight change analysis
- Pre-before-post potentiation verification
- Post-before-pre depression verification
- Statistical analysis of synaptic changes

### 4. Connectivity Test (`--experiment connectivity`)

The connectivity test verifies basic neural network functionality.

**What it tests:**
- Neural activation propagation
- Spike transmission between neurons
- Network stability
- Basic learning capabilities

**Command:**
```bash
./nlm --experiment connectivity
```

### 5. Performance Benchmark (`--benchmark`)

The benchmark measures performance characteristics of the integrated system.

**What it tests:**
- Simulation speed (steps per second)
- Memory usage patterns
- CPU utilization
- Scalability with different neuron counts

**Command:**
```bash
./nlm --benchmark --verbose
```

**Output:**
- Performance metrics
- Timing information
- Resource usage statistics
- Scalability analysis

---

## Memory Systems

NLM Phase 6 integrates three major memory systems, each with unique capabilities.

### 1. Working Memory

**Purpose:** Transient information holding for immediate processing
**Capacity:** Configurable via `working_memory_capacity`
**Duration:** Short-term (seconds to minutes)

**Key Features:**
- Pattern storage and retrieval
- Competitive selection among patterns
- Integration with attention system
- Rapid consolidation to episodic memory

**Usage:**
```cpp
// Store information
workingMemory->store("pattern", std::vector<float>{0.1f, 0.2f, 0.3f});

// Retrieve information
auto pattern = workingMemory->retrieve("pattern");
```

### 2. Episodic Memory

**Purpose:** Storage of experienced events and episodes
**Capacity:** Configurable via `episodic_memory_max_episodes`
**Duration:** Long-term (hours to years)

**Key Features:**
- Temporal sequencing of events
- Pattern completion from partial cues
- Replay during rest/sleep for consolidation
- Integration with development system

**Usage:**
```cpp
// Store episodic memory
episodicMemory->storeEpisodicMemory("episode_1", std::vector<float>{0.5f, 0.6f, 0.7f});

// Retrieve episodic memory
auto episode = episodicMemory->retrieveEpisodicMemory("episode_1");
```

### 3. Associative Memory

**Purpose:** Pattern associations and connections
**Capacity:** Configurable via `associative_memory_pattern_count`
**Duration:** Stable (throughout life)

**Key Features:**
- Bidirectional associations
- Strength-based retrieval
- Integration with prediction system
- Support for hierarchical associations

**Usage:**
```cpp
// Create association
associativeMemory->createAssociation("key", "value", 0.8f);

// Retrieve association
auto value = associativeMemory->getAssociation("key");
```

### 4. Memory Integration

The memory systems work together synergistically:

1. **Working to Episodic:** Working memory patterns are consolidated into episodic memory
2. **Episodic to Associative:** Episodic memories form new associations
3. **Associative to Working:** Existing associations influence working memory content
4. **All to Prediction:** All memory systems contribute to predictive processing

---

## Neuromodulation

NLM Phase 6 integrates four neuromodulation systems that coordinate learning and behavior.

### 1. Dopamine System

**Purpose:** Reward prediction and reinforcement learning
**Function:** Modulates plasticity based on reward prediction errors
**Key Variables:**
- Current modulation level
- Reward prediction error
- Learning rate adaptation

**Key Features:**
- Predicts expected rewards
- Updates based on actual outcomes
- Influences future behavior selection
- Integrates with curiosity system

### 2. Curiosity System

**Purpose:** Exploration motivation and intrinsic reward
**Function:** Drives exploration of novel stimuli
**Key Variables:**
- Exploration drive
- Novelty sensitivity
- Reward prediction error

**Key Features:**
- Increases exploration for novel stimuli
- Balances exploration and exploitation
- Works with dopamine for extrinsic rewards
- Adapts based on learning progress

### 3. Novelty System

**Purpose:** Novelty detection and change detection
**Function:** Identifies and responds to novel stimuli
**Key Variables:**
- Novelty detection threshold
- Pattern mismatch detection
- Salience weighting

**Key Features:**
- Detects changes from expected patterns
- Increases attention to novel stimuli
- Triggers curiosity responses
- Integrates with prediction system

### 4. Prediction Error System

**Purpose:** Error computation for predictive models
**Function:** Computes differences between predictions and actual outcomes
**Key Variables:**
- Error magnitude
- Prediction accuracy
- Learning signal strength

**Key Features:**
- Updates predictive models
- Influences future predictions
- Coordinates with other neuromodulators
- Drives concept formation

### 5. Neuromodulation Integration

The neuromodulation systems work together:

1. **Dopamine + Curiosity:** Reward prediction with exploration motivation
2. **Novelty + Prediction Error:** Change detection with model updating
3. **All Systems:** Coordinated learning across multiple timescales
4. **Feedback Loops:** Continuous adjustment and adaptation

---

## Cognitive Processing

NLM Phase 6 integrates multiple cognitive systems for intelligent behavior.

### 1. Neural Planner

**Purpose:** Action planning and sequence evaluation
**Function:** Evaluates action sequences and selects optimal behaviors
**Key Features:**
- Recursive planning (planning within planning)
- Cost-benefit analysis
- Integration with working memory
- Development-dependent planning complexity

### 2. Concept Formation

**Purpose:** Pattern discovery and abstraction
**Function:** Extracts patterns and forms concepts from experience
**Key Features:**
- Similarity-based clustering
- Hierarchical concept organization
- Integration with associative memory
- Development-dependent concept complexity

### 3. Attentional Selection

**Purpose:** Focus and competition among patterns
**Function:** Selects relevant information for processing
**Key Features:**
- Competitive selection dynamics
- Inhibition and excitation balance
- Integration with working memory
- Development-dependent attentional span

### 4. Prediction System

**Purpose:** Predictive coding and error computation
**Function:** Generates predictions and computes prediction errors
**Key Features:**
- Multi-level prediction hierarchy
- Prediction error minimization
- Integration with all memory systems
- Development-dependent prediction accuracy

### 5. Cognitive Integration

The cognitive systems work together:

1. **Planning + Attention:** Plans guide attentional focus
2. **Concept Formation + Prediction:** Concepts enable better predictions
3. **All Systems + Memory:** Cognition is memory-driven
4. **All Systems + Development:** Cognitive capabilities mature over time

---

## Development

NLM Phase 6 includes a comprehensive development system that models brain maturation.

### Developmental Stages

1. **Initial Stage:** Naive, unformed neural connections
2. **Critical Period:** Heightened plasticity and rapid learning
3. **Maturation:** Structural refinement and specialization
4. **Adult Stage:** Optimized function and stable performance
5. **Aging:** Gradual decline in plasticity and function

### Developmental Changes

| Process | Early Development | Adult Stage | Aging |
|---------|-------------------|-------------|-------|
| Synaptogenesis | High | Low | Very low |
| Pruning | Moderate | High | Increasing |
| Plasticity | High | Moderate | Decreasing |
| Memory Capacity | Low | High | Declining |

### Development Integration

Development affects all other systems:

1. **Memory Development:** Working memory capacity increases
2. **Cognitive Development:** Planning and concept formation improve
3. **Neuromodulation Development:** Regulatory systems mature
4. **Plasticity Development:** Learning mechanisms change
5. **Performance Development:** Overall efficiency improves

### Key Developmental Parameters

- **Synaptogenesis Rate:** Formation of new synapses
- **Pruning Rate:** Elimination of unused connections
- **Plasticity Boost:** Enhanced learning during development
- **Maturation Timeline:** Timeline for developmental stages

---

## Performance Optimization

NLM Phase 6 includes multiple performance optimization features.

### Optimization Techniques

1. **SIMD Vectorization:** Single instruction, multiple data processing
2. **Memory Pools:** Efficient memory allocation and reuse
3. **Parallel Processing:** Multi-threading for CPU-bound tasks
4. **Event-Driven Computation:** Spike events processed asynchronously
5. **Lazy Evaluation:** Only compute what's necessary

### Performance Settings

| Setting | Description | Default |
|---------|-------------|---------|
| `use_simd` | Enable SIMD optimizations | `true` |
| `use_parallel_processing` | Enable parallel processing | `true` |
| `use_memory_pools` | Use memory pools | `true` |
| `max_concurrent_threads` | Maximum threads | `4` |

### Performance Monitoring

The system includes built-in performance monitoring:

- **Timing Statistics:** Measurement of simulation speed
- **Memory Usage:** Tracking of memory allocation patterns
- **CPU Utilization:** Monitoring of processor usage
- **Scalability Analysis:** Testing performance with different neuron counts

### Profiling Tools

For detailed performance analysis:

1. **GPROF:** Classic profiling tool
2. **Intel VTune:** Advanced profiling (Linux)
3. **Xcode Instruments:** Profiling (macOS)
4. **Visual Studio Profiler:** Profiling (Windows)

---

## Troubleshooting

This section provides common solutions to problems that may occur while using NLM Phase 6.

### Build Issues

#### Problem: CMake not found

**Solution:** Install CMake:
```bash
# Ubuntu/Debian
sudo apt install cmake

# macOS
brew install cmake

# Windows
# Download from https://cmake.org/download/
```

#### Problem: Compiler not supported

**Solution:** Install a compatible C++ compiler:
```bash
# GCC (Linux)
sudo apt install g++

# Clang (macOS)
brew install llvm

# MSVC (Windows)
# Install Visual Studio with C++ workload
```

#### Problem: Build fails with errors

**Solution:** Check the build output for specific errors. Common issues:

1. **Memory issues:** Increase swap space or use a machine with more RAM
2. **Compiler version:** Update to a supported version
3. **Missing dependencies:** Install required system libraries
4. **CMake version:** Upgrade to the latest stable version

### Runtime Issues

#### Problem: Simulation runs too slowly

**Solutions:**
1. Reduce neuron count: `--steps 1000 --neuron_count 100`
2. Disable optimizations: `-DUSE_SIMD=OFF -DUSE_PARALLEL=OFF`
3. Use fewer threads: `-DMAX_CONCURRENT_THREADS=2`
4. Reduce simulation time: `--steps 100`

#### Problem: Memory usage too high

**Solutions:**
1. Use memory pools: `-DUSE_MEMORY_POOLS=ON`
2. Reduce simulation complexity: smaller networks
3. Enable checkpointing: `--save-brain`
4. Use disk-based storage for large datasets

#### Problem: Configuration file errors

**Solutions:**
1. Check syntax: all lines must be `key = value`
2. Check for typos in configuration keys
3. Ensure numeric values are properly formatted
4. Use `configs/basic.cfg` as a reference

#### Problem: No output or crashes

**Solutions:**
1. Add `--verbose` flag for more output
2. Check log files for error details
3. Verify all required files are present
4. Run with a simpler configuration

### Debugging

#### Running with GDB

```bash
# Build with debug symbols
make build-debug

# Run with GDB
make interactive

# Or run the executable directly with GDB
make run
```

#### Common GDB commands

```gdb
# Start debugging
break main
run

# Step through code
next
step

# Check variables
info locals
print variable_name

# Continue execution
continue

# Exit
gquit
```

### Performance Tuning

#### For large simulations

1. **Use production build:** `make build` (Release)
2. **Enable optimizations:** `-DUSE_SIMD=ON -DUSE_PARALLEL=ON`
3. **Tune thread count:** `-DMAX_CONCURRENT_THREADS=8`
4. **Use SSD storage:** Faster disk I/O
5. **Monitor system resources:** `top`, `htop`

#### For development and testing

1. **Use debug build:** `make build-debug`
2. **Disable optimizations:** `-O0 -g`
3. **Enable assertions:** `-DDEBUG`
4. **Use smaller networks:** fewer neurons and connections
5. **Enable detailed logging:** `--verbose`

---

## API Reference

This section provides API documentation for NLM Phase 6.

### Core Types

#### NeuronId
```cpp
struct NeuronId {
    uint64_t value;
    NeuronId() : value(0) {}
    explicit NeuronId(uint64_t v) : value(v) {}
    bool operator==(const NeuronId& other) const = default;
    bool operator!=(const NeuronId& other) const = default;
    bool operator<(const NeuronId& other) const { return value < other.value; }
    uint64_t index() const { return value; }
};
```

#### SynapseId
```cpp
struct SynapseId {
    uint64_t value;
    SynapseId() : value(0) {}
    explicit SynapseId(uint64_t v) : value(v) {}
    // Comparison operators...
    uint64_t index() const { return value; }
};
```

#### RegionId
```cpp
struct RegionId {
    uint64_t value;
    RegionId() : value(0) {}
    explicit RegionId(uint64_t v) : value(v) {}
    // Comparison operators...
    uint64_t index() const { return value; }
};
```

#### NeuronType
```cpp
enum class NeuronType : uint8_t {
    Excitatory,
    Inhibitory,
    Modulatory,
    Sensory,
    Motor,
    Internal
};
```

#### SynapseType
```cpp
enum class SynapseType : uint8_t {
    Excitatory,
    Inhibitory,
    Modulatory,
    Electrical,
    GapJunction
};
```

### Brain Class

#### Constructor
```cpp
explicit Brain(std::shared_ptr<Config> config);
```

#### Key Methods

##### Initialization
```cpp
bool initialize();                    // Initialize brain with configuration
void reset();                         // Reset brain to initial state
```

##### Simulation
```cpp
void step(SimulationStep currentStep);      // Advance one simulation step
void step(SimulationStep currentStep, Timestamp currentTime);  // Advance with time
```

##### Input/Output
```cpp
void receiveSensoryInput(const SensoryInput& input);    // Receive sensory input
void injectCurrent(NeuronId neuron, MembranePotential current);  // Inject current
void injectCurrentToNeurons(NeuronType type, MembranePotential current);  // Inject to all neurons of type
```

##### Memory Systems
```cpp
NeuralWorkingMemory* getWorkingMemory();                // Get working memory
NeuralEpisodicMemory* getEpisodicMemory();              // Get episodic memory
NeuralAssociativeMemory* getAssociativeMemory();        // Get associative memory
```

##### Neuromodulation Systems
```cpp
Dopamine* getDopamine();                              // Get dopamine system
Curiosity* getCuriosity();                            // Get curiosity system
Novelty* getNovelty();                                // Get novelty system
PredictionError* getPredictionErrorSignal();          // Get prediction error system
```

##### Cognitive Systems
```cpp
NeuralPlanner* getPlanner();                          // Get neural planner
ConceptFormation* getConceptFormation();              // Get concept formation
AttentionalSelection* getAttention();                 // Get attentional selection
PredictionSystem* getPredictionSystem();              // Get prediction system
```

##### Development System
```cpp
DevelopmentSystem* getDevelopmentSystem();            // Get development system
DevelopmentalStage getDevelopmentalStage() const;    // Get current developmental stage
void setDevelopmentalStage(DevelopmentalStage stage); // Set developmental stage
```

##### Plasticity Systems
```cpp
STDP* getSTDP();                                      // Get STDP system
Hebbian* getHebbian();                                // Get Hebbian system
StructuralPlasticity* getStructuralPlasticity();      // Get structural plasticity
```

##### Statistics
```cpp
size_t getTotalNeuronCount() const;                   // Get total neuron count
size_t getTotalSynapseCount() const;                   // Get total synapse count
size_t getActiveNeuronCount() const;                   // Get active neuron count
size_t getFiringNeuronCount() const;                   // Get firing neuron count
float getAverageFiringRate() const;                    // Get average firing rate
```

### Configuration Class

#### Key Methods

```cpp
bool loadFromFile(const std::string& filename);        // Load from file
void loadFromArgs(int argc, char** argv);             // Load from command line
void set(const std::string& key, const T& value, ConfigSource source);  // Set value
T getOr(const std::string& key, const T& defaultValue) const;        // Get value with default
```

### Memory System Classes

#### NeuralWorkingMemory
```cpp
void store(const std::string& key, const std::vector<float>& pattern);  // Store pattern
std::vector<float> retrieve(const std::string& key);  // Retrieve pattern
void store(const std::string& key, const std::vector<float>& pattern, float importance);  // Store with importance
float getImportance(const std::string& key) const;  // Get importance
```

#### NeuralEpisodicMemory
```cpp
void storeEpisodicMemory(const std::string& episode, const std::vector<float>& pattern);  // Store episode
std::vector<float> retrieveEpisodicMemory(const std::string& episode);  // Retrieve episode
void replayEpisodes();  // Replay all episodes for consolidation
```

#### NeuralAssociativeMemory
```cpp
void createAssociation(const std::string& key1, const std::string& key2, float strength);  // Create association
std::string getAssociation(const std::string& key);  // Get associated value
float getAssociationStrength(const std::string& key1, const std::string& key2) const;  // Get association strength
```

### Neuromodulation Classes

#### Dopamine
```cpp
void processReward(float reward, float prediction);  // Process reward
float getCurrentModulation() const;  // Get current modulation level
```

#### Curiosity
```cpp
triggerExploration(float explorationLevel);  // Trigger exploration
float getExplorationDrive() const;  // Get exploration drive
```

#### Novelty
```cpp
void processNovelty(const std::vector<float>& stimulus);  // Process novelty
float getNoveltyDetection() const;  // Get novelty detection
```

#### PredictionError
```cpp
void computePredictionError(float prediction, float outcome);  // Compute prediction error
float getErrorMagnitude() const;  // Get error magnitude
```

### Cognitive System Classes

#### NeuralPlanner
```cpp
void setPlanningDepth(int depth);  // Set planning depth
int getPlanningDepth() const;  // Get planning depth
void planAction();  // Plan action
```

#### ConceptFormation
```cpp
void processPattern(const std::vector<float>& pattern);  // Process pattern
int getActiveConceptCount() const;  // Get active concept count
```

#### AttentionalSelection
```cpp
void setInhibitionStrength(float strength);  // Set inhibition strength
float getInhibitionStrength() const;  // Get inhibition strength
void setExcitationStrength(float strength);  // Set excitation strength
float getExcitationStrength() const;  // Get excitation strength
```

#### PredictionSystem
```cpp
void setPrediction(const std::vector<float>& prediction);  // Set prediction
float getPredictionAccuracy() const;  // Get prediction accuracy
```

### Development System Classes

#### DevelopmentSystem
```cpp
void develop();  // Advance development
float getPlasticityRate() const;  // Get plasticity rate
float getSynaptogenesisRate() const;  // Get synaptogenesis rate
float getPruningRate() const;  // Get pruning rate
```

#### DevelopmentalStage
```cpp
enum class DevelopmentalStage : uint8_t {
    Initial,
    CriticalPeriod,
    Maturation,
    Adult,
    Aging
};
```

### Plasticity System Classes

#### STDP
```cpp
void update(Synapse* synapse, const std::vector<Timestamp>& preSpikes, const std::vector<Timestamp>& postSpikes, float timestep);  // Update synapse
float getLTPWeight() const;  // Get LTP weight
float getLTDWeight() const;  // Get LTD weight
float getTimeConstant() const;  // Get time constant
```

#### Hebbian
```cpp
void update(Synapse* synapse, const std::vector<Timestamp>& preSpikes, const std::vector<Timestamp>& postSpikes, float timestep);  // Update synapse
```

#### StructuralPlasticity
```cpp
void update(Synapse* synapse, const std::vector<Timestamp>& preSpikes, const std::vector<Timestamp>& postSpikes, float timestep);  // Update synapse
```

### Random Generator

#### RandomGenerator
```cpp
RandomGenerator(uint64_t seed = 42);  // Constructor
float uniformReal(float min, float max);  // Uniform random float
int uniformInt(int min, int max);  // Uniform random integer
bool bernoulli(float p);  // Bernoulli trial
```

### Simulation Clock

#### SimulationClock
```cpp
SimulationClock(TimestepDuration timestep = 0.001);  // Constructor
void advance();  // Advance one step
void advanceBy(SimulationStep steps);  // Advance by multiple steps
void setStep(SimulationStep step);  // Set step
void setTime(Timestamp time);  // Set time
SimulationStep getStep() const;  // Get current step
Timestamp getTime() const;  // Get current time
TimestepDuration getTimestep() const;  // Get timestep
```

### Logger

#### Logger
```cpp
void addLogger(std::shared_ptr<Logger> logger);  // Add logger
void setGlobal(std::shared_ptr<Logger> logger);  // Set global logger
```

#### ConsoleLogger
```cpp
ConsoleLogger(LogLevel level = LogLevel::Info);  // Constructor
setLogLevel(LogLevel level);  // Set log level
```

### LogLevel
```cpp
enum class LogLevel : uint8_t {
    Debug,
    Info,
    Warning,
    Error
};
```

### File I/O

#### Checkpoint System
```cpp
bool save(const std::string& filepath) const;  // Save brain state
bool load(const std::string& filepath);  // Load brain state
```

---

## References

1. Dayan, P., & Abbott, L. F. (2001). *Theoretical Neuroscience*. MIT Press.
2. Gerstner, W., Kistler, W., Naud, R., & Challiss, L. (2014). *Neuronal Dynamics*. Cambridge University Press.
3. Maass, W., Bishop, T., & Plunkett, R. (1997). *Computational Models of Neural Systems*. MIT Press.
4. Song, S., Miller, K. D., & Abbott, L. F. (2000). Adjustable asymmetry in the striatum controls velocity-dependent synaptic plasticity. *Nature Neuroscience*, 3(5), 492-499.
5. Widrow, B., & Hoff, M. C. (1960). Adaptive switching circuits. *IRE WESCON Convention Record*, 4, 96-104.

---

## Acknowledgements

This project was developed as part of the NLM (熙然) research initiative, investigating computational brain-like systems and artificial developmental intelligence.

We acknowledge the contributions of all researchers and developers who have participated in this project.

For more information, visit: https://github.com/nlm-project/nlm

---

This document is continually updated as NLM Phase 6 evolves. Please report any issues or suggestions to the development team.