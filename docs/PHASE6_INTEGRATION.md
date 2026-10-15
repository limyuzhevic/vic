# NLM Phase 6 Comprehensive Documentation

This document provides comprehensive information about NLM Phase 6, including usage examples, configuration options, and advanced features.

## Table of Contents

1. [Overview](#overview)
2. [Quick Start](#quick-start)
3. [Command Line Interface](#command-line-interface)
4. [Configuration Files](#configuration-files)
5. [Demo Modes](#demo-modes)
6. [Phase 6 Integration Features](#phase-6-integration-features)
7. [Performance Optimization](#performance-optimization)
8. [Troubleshooting](#troubleshooting)
9. [API Reference](#api-reference)
10. [Example Scripts](#example-scripts)

## Overview

NLM (熙然) is an experimental artificial developmental brain system at Phase 6 (Final Integration). Phase 6 represents the culmination of all previous development phases, integrating all systems into a coherent, functioning artificial brain.

### Key Features

- **Real Spiking Neural Computation**: Leaky Integrate-and-Fire (LIF) neurons with event-driven dynamics
- **Complete Integration**: All systems work together in a unified brain architecture
- **Memory Systems**: Working memory, episodic memory, and associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection, and prediction error
- **Cognition**: Planning, concept formation, and attentional selection
- **Development**: Multi-stage developmental progression with structural plasticity
- **Persistence**: Checkpoint save/load and memory replay

## Quick Start

### Basic Installation

1. Build NLM:
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

2. Run Phase 6 Demo:
```bash
./nlm --demo=PHASE6
```

### Python Demo

Run the Python demonstration script:
```bash
python3 scripts/demo_phase6.py
```

### Configuration Generation

Generate configuration files:
```bash
python3 scripts/generate_config.py --demo=PHASE6 --neurons 1000 --steps 5000
```

## Command Line Interface

### Basic Usage

```bash
./nlm [OPTIONS]
```

### Demo Modes

| Mode | Description | Command |
|------|-------------|---------|
| `PHASE2` | Basic neural computation demo | `--demo=PHASE2` |
| `PHASE6` | Phase 6 integration demo | `--demo=PHASE6` |
| `BENCHMARK` | Performance benchmarking | `--demo=BENCHMARK` |
| `CUSTOM` | Custom simulation with user config | `--demo=CUSTOM` |

### Options

| Option | Description | Default |
|--------|-------------|---------|
| `--neurons=<N>` | Number of neurons | 500 |
| `--steps=<N>` | Number of simulation steps | 2000 |
| `--verbose` | Enable verbose output | false |
| `--checkpoint=<file>` | Save/load checkpoint file | none |
| `--help` | Show help message | |

### Examples

```bash
# Run Phase 6 integration demo
./nlm --demo=PHASE6 --neurons 1000 --steps 5000

# Run Phase 2 basic demo with verbose output
./nlm --demo=PHASE2 --verbose

# Run performance benchmark
./nlm --demo=BENCHMARK --neurons 200 --steps 1000

# Run custom demo with checkpoint
./nlm --demo=CUSTOM --checkpoint=./checkpoints/my_brain.bin
```

## Configuration Files

### Format

Configuration files are simple key=value text files. Comments start with `#` or `#`.

### Basic Configuration

```ini
# NLM Configuration File
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 1
connection_probability = 0.15f

# Phase 2 neural computation parameters
stdp_ltp_weight = 0.02f
stdp_ltd_weight = 0.015f
stdp_tau = 20.0f

# Structural plasticity parameters
synaptogenesis_rate = 0.0001f
pruning_rate = 0.00001f

# Simulation settings
max_steps = 5000
enable_learning = true
enable_memory = true
enable_neuromodulation = true
enable_development = true
```

### Example Configuration Files

The following example configuration files are available in the `configs/` directory:

1. **`configs/demo_phase2.cfg`** - Basic Phase 2 demonstration
2. **`configs/demo_phase6.cfg`** - Complete Phase 6 integration
3. **`configs/benchmark.cfg`** - Performance benchmarking setup
4. **`configs/custom_learning.cfg`** - Custom learning scenario

### Configuration Generator

Use the configuration generator script:
```bash
python3 scripts/generate_config.py --output my_config.cfg
```

## Demo Modes

### Phase 2: Basic Neural Computation

This mode demonstrates the core neural computation capabilities:

- Basic neural connectivity
- Spike propagation through networks
- STDP (Spike-Timing-Dependent Plasticity)
- Hebbian learning
- Structural plasticity

**Usage:**
```bash
./nlm --demo=PHASE2
```

### Phase 6: Integrated Brain

This mode demonstrates the complete Phase 6 integration:

- Memory systems integration
- Neuromodulation effects
- Cognitive functions (planning, attention)
- Development and learning
- Checkpoint and replay systems

**Usage:**
```bash
./nlm --demo=PHASE6 --neurons 1000 --steps 5000
```

### Benchmark: Performance Testing

This mode measures performance characteristics:

- Wall clock time measurements
- Steps per second performance
- Spike processing efficiency
- Memory usage analysis

**Usage:**
```bash
./nlm --demo=BENCHMARK --neurons 500 --steps 2000
```

### Custom: User-Defined Simulation

This mode allows custom simulation scenarios:

- Custom neuron counts
- Custom step counts
- Enable/disable specific features
- Progress monitoring

**Usage:**
```bash
./nlm --demo=CUSTOM --neurons 2000 --steps 10000 --verbose
```

## Phase 6 Integration Features

### Memory Systems

Phase 6 integrates three memory systems:

1. **Working Memory**: Persistent activity for short-term storage
2. **Episodic Memory**: Experience encoding and replay
3. **Associative Memory**: Pattern associations and relationships

### Neuromodulation Systems

Five neuromodulation systems coordinate learning and behavior:

1. **Dopamine**: Reward prediction and reinforcement
2. **Curiosity**: Exploration motivation and novelty seeking
3. **Novelty**: Detection of unexpected sensory input
4. **Prediction Error**: Discrepancy between expected and actual outcomes
5. **Dopamine Modulates**: Plasticity and neural excitability

### Cognitive Systems

Three cognitive systems enable intelligent behavior:

1. **Planning**: Multi-step action sequence evaluation
2. **Concept Formation**: Pattern discovery and categorization
3. **Attention**: Competitive selection and focus

### Development System

The development system orchestrates brain maturation:

- **Initial**: High plasticity, rapid learning
- **Critical Period**: Reduced plasticity, specialization
- **Maturation**: Further specialization
- **Adult**: Stable, efficient operation

### Prediction System

The prediction system enables temporal and causal learning:

- Sequence prediction
- Action consequence prediction
- Error-based learning
- Confidence tracking

### Integration Architecture

```
WORLD (Environment)
  ↓
SENSORY INPUT (Perception)
  ↓
BRAIN (Neural Processing)
  ↓
    ├── MEMORY SYSTEMS
    ├── NEUROMODULATION
    ├── COGNITION
    ├── PREDICTION
    └── DEVELOPMENT
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / ERROR
  ↓
PLASTICITY
  ↓
UPDATED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## Performance Optimization

### Compilation

Use optimized builds:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Runtime Optimization

- Use larger simulation timesteps for faster simulation
- Adjust connection probabilities for desired network density
- Enable SIMD vectorization (if available)
- Use checkpointing for long-running simulations

### Memory Management

- Use memory pools for efficient allocation
- Configure working memory capacity based on neuron count
- Set appropriate episodic memory episode limits

## Troubleshooting

### Common Issues

#### "Brain initialization failed"

**Solution:**
1. Check system resources (memory, CPU)
2. Verify configuration file syntax
3. Try reducing neuron count
4. Check for build errors

#### "No spikes generated"

**Solution:**
1. Increase input current to sensory neurons
2. Check neuron parameters (threshold, resting potential)
3. Verify synaptic weights
4. Increase simulation time

#### "Integration verification failed"

**Solution:**
1. Check that all memory systems are initialized
2. Verify neuromodulation system setup
3. Ensure cognitive systems are configured
4. Run Phase 2 demo first to verify basic functionality

### Debugging Tips

- Use verbose mode for detailed output
- Check log files for error details
- Run smaller test scenarios first
- Use checkpointing to save progress

## API Reference

### Brain API

The main NLM API provides access to brain functionality:

```cpp
class Brain {
public:
    // Core functionality
    bool initialize();           // Initialize brain
    void step(SimulationStep);   // Simulation step
    void reset();                 // Reset brain state
    
    // System access
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    PredictionSystem* getPredictionSystem();
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    DevelopmentSystem* getDevelopmentSystem();
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Statistics and monitoring
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Motor and action
    std::unique_ptr<Action> produceAction();
    
    // Saving and loading
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
};
```

### Configuration API

```cpp
class Config {
public:
    template<typename T> std::optional<T> get(const std::string& key) const;
    template<typename T> T getOr(const std::string& key, const T& defaultValue) const;
    
    void set(const std::string& key, const ConfigValue& value, ConfigSource source);
    void set(const std::string& key, const std::string& value, ConfigSource source);
    void set(const std::string& key, int value, ConfigSource source);
    void set(const std::string& key, double value, ConfigSource source);
    void set(const std::string& key, bool value, ConfigSource source);
    
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();
    std::string summary() const;
};
```

## Example Scripts

### Python Demo Script

The `scripts/demo_phase6.py` script provides a comprehensive Python interface for running NLM demonstrations:

```bash
python3 scripts/demo_phase6.py
```

### Configuration Generator

The `scripts/generate_config.py` script generates configuration files:

```bash
python3 scripts/generate_config.py --help
```

## Technical Specifications

### Performance

- **Neuron Scale**: Up to 10,000+ neurons
- **Simulation Speed**: Real-time or faster (depending on hardware)
- **Memory Usage**: Configurable based on simulation requirements
- **Checkpoint Size**: Compressed, disk-efficient storage

### Accuracy

- **Neuron Model**: Leaky Integrate-and-Fire (LIF)
- **Plasticity Rules**: STDP and Hebbian
- **Developmental Stages**: 4 distinct stages
- **Integration Level**: Phase 6 complete integration

### Compatibility

- **Compiler**: C++20 compatible
- **Platform**: Linux, macOS, Windows (with CMake)
- **Build System**: CMake
- **Python**: Python 3.6+ (for demo scripts)

## Future Development

### Phase 7: Enhanced Cognition

- Advanced social learning
- Abstract reasoning capabilities
- Meta-cognitive functions
- Transfer learning and generalization

### Phase 8: Real-world Application

- Sensor integration
- Motor control
- Environmental interaction
- Continuous learning

### Research Directions

- Neuromorphic hardware acceleration
- Biological fidelity improvements
- Cognitive architecture research
- AI alignment and safety

## Conclusion

NLM Phase 6 represents a significant milestone in artificial brain development. The integrated architecture demonstrates that complex brain-like systems can be implemented computationally, providing a foundation for future research in artificial intelligence and computational neuroscience.

The improved main entry point provides multiple demo modes, better error handling, and enhanced usability, making NLM accessible to researchers, developers, and enthusiasts alike.

For more information and source code, visit the NLM project repository.
