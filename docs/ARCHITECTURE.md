# NLM - Neural Learning Machine User Documentation
# Comprehensive Guide for Users and Developers

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project that implements a neural system capable of learning from experience through interaction with an environment. This document covers the enhanced NLM system with improvements made across all phases.

## Key Improvements Made

This version includes comprehensive improvements:

1. **Enhanced Command-Line Interface** - Flexible multi-mode execution
2. **Robust Configuration System** - Detailed config file with validation
3. **Improved Neuron Implementation** - Better numerical stability and validation
4. **Fixed Logger System** - Consistent global logger access
5. **Comprehensive Documentation** - Complete user and developer guides

## Quick Start Guide

### Basic Usage

```bash
# Run Phase 2: Basic neural computation demo (default)
nlm

# Run Phase 3: World interaction demo
nlm --phase3

# Run Phase 4: Cognitive experiments demo
nlm --phase4

# Run Phase 6: Full integration demo
nlm --phase6

# Custom simulation with configuration file
nlm --custom --config myconfig.cfg
```

### Command-Line Options

```bash
# Show help
nlm --help

# Show version information
nlm --version

# Run demo with custom settings
nlm --phase3 10 500

# Save brain state to file
nlm --save /tmp/brain.bin --phase2

# Enable verbose logging
nlm --verbose --phase3

# Use custom configuration
nlm --custom --config configs/my_settings.cfg
```

## Detailed Command-Line Interface

### Modes

1. **Phase 2 (`--phase2`)** - Basic neural computation
   - Demonstrates LIF neuron dynamics
   - Shows STDP and Hebbian plasticity
   - Includes structural plasticity
   - Best for understanding core neural mechanisms

2. **Phase 3 (`--phase3`)** - World interaction
   - Full sensory-motor loop integration
   - Experience -> Action -> Consequence -> Learning
   - Demonstrates embodied cognition
   - Best for studying behavior emergence

3. **Phase 4 (`--phase4`)** - Cognitive experiments
   - Comprehensive cognitive capabilities
   - Prediction, memory, planning, attention
   - 10 different cognitive experiments
   - Best for studying higher cognitive functions

4. **Phase 6 (`--phase6`)** - Final integration
   - Complete brain system integration
   - All memory, prediction, and neuromodulation systems
   - Checkpoint and replay capabilities
   - Best for testing complete functionality

5. **Custom (`--custom`)** - User-defined configuration
   - Load custom configuration file
   - Flexible parameter settings
   - Best for research and experimentation

### Key Options

- `--help` - Show detailed help and usage examples
- `--version` - Show NLM version information
- `--config <file>` - Load configuration from file
- `--neurons <N>` - Set number of neurons
- `--steps <N>` - Set simulation steps
- `--duration <sec>` - Set simulation duration
- `--demo` - Run quick demo instead of full experiment
- `--save <path>` - Save brain state to file
- `--verbose` - Enable detailed logging
- `--quiet` - Suppress informational messages

### Configuration File Format

The configuration file uses key-value pairs with comments:

```ini
# NLM Configuration File
# Phase 6: Final Integration Configuration

# Basic simulation settings
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1

# Neural dynamics parameters
neuron_membrane_capacitance = 1.0
neuron_resting_potential = -70.0
neuron_firing_threshold = -55.0

# Plasticity settings
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_window = 20.0

# Development settings
development_synaptogenesis_rate = 0.0001
development_pruning_rate = 0.00001

# Neuromodulation settings
dopamine_baseline = 0.1
curiosity_baseline = 0.1

# Environment settings
environment_name = SimpleWorld
environment_width = 20
environment_height = 20

# Logging settings
log_level = INFO
log_to_file = false
```

## System Architecture

### Core Components

1. **Brain System**
   - Neural populations and regions
   - Real LIF neuron dynamics
   - Event-driven spike propagation
   - Synaptic transmission with delays

2. **Plasticity Systems**
   - STDP (Spike-Timing-Dependent Plasticity)
   - Hebbian learning rules
   - Structural plasticity (synaptogenesis/pruning)
   - Reward-modulated learning

3. **Memory Systems**
   - Working memory (active information maintenance)
   - Episodic memory (experience storage)
   - Associative memory (pattern associations)
   - Neural implementations with biological constraints

4. **Neuromodulation**
   - Dopamine (reward and reinforcement)
   - Curiosity (exploration motivation)
   - Novelty detection
   - Prediction error signals

5. **Cognition Systems**
   - Neural planning (action sequence evaluation)
   - Concept formation (pattern discovery)
   - Attention (competitive selection)
   - Self-modeling and social learning

6. **Environment Interaction**
   - Sensory processing (vision, audio, proprioception)
   - Motor output systems
   - World simulation and consequences
   - Reward and feedback signals

### Processing Pipeline

```
WORLD → SENSORY INPUT → NEURAL PROCESSING → MEMORY/PREDICTION →
MOTIVATION/NEUROMODULATION → ACTION SELECTION → MOTOR OUTPUT →
WORLD CONSEQUENCE → REWARD/SURPRISE/ERROR → PLASTICITY →
MEMORY/DEVELOPMENT → CHANGED BRAIN → CHANGED FUTURE BEHAVIOR
```

## Performance Optimizations

### Phase 5 Optimizations (Active)

1. **Event-Driven Processing**
   - Only active neurons consume computation
   - Efficient spike event queues
   - Time-bucketed delayed spikes

2. **Structure-of-Arrays (SoA)**
   - Contiguous neuron state arrays
   - Better cache utilization
   - Improved SIMD vectorization

3. **Sparse Connectivity**
   - Adjacency lists instead of dense matrices
   - Efficient synapse lookup
   - Reduced memory for large networks

4. **Multithreading**
   - Parallel population updates
   - Work-stealing load balancing
   - Thread-local state accumulation

5. **Memory Efficiency**
   - Pre-allocated neuron/synapse pools
   - Reduced heap fragmentation
   - Compact data structures

### Performance Monitoring

The system provides detailed performance statistics:

- Neurons per second
- Spikes per second
- Synaptic events per second
- Real-time processing factor
- Active and firing neuron counts
- Pending spike events
- Memory usage statistics
- Step execution time

## Configuration Options Reference

### Core Simulation

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `random_seed` | integer | 42 | Random seed for reproducible experiments |
| `simulation_timestep` | float | 0.001 | Time step in seconds |
| `neuron_count` | integer | 500 | Number of neurons |
| `region_count` | integer | 1 | Number of brain regions |
| `connection_probability` | float | 0.15 | Probability of synaptic connections |

### Neural Dynamics

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_membrane_capacitance` | float | 1.0 | Membrane capacitance (nF) |
| `neuron_resting_potential` | float | -70.0 | Resting potential (mV) |
| `neuron_firing_threshold` | float | -55.0 | Firing threshold (mV) |
| `neuron_refractory_period` | integer | 5 | Refractory period (steps) |
| `neuron_leak_conductance` | float | 10.0 | Leak conductance (nS) |

### Plasticity Rules

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `stdp_ltp_weight` | float | 0.02 | STDP LTP weight change |
| `stdp_ltd_weight` | float | 0.015 | STDP LTD weight change |
| `stdp_time_window` | float | 20.0 | STDP time window (ms) |
| `plasticity_learning_rate` | float | 0.01 | General learning rate |

### Memory Systems

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `working_memory_capacity` | integer | 100 | Number of working memory traces |
| `working_memory_trace_decay` | float | 0.95 | Trace decay rate |
| `episodic_memory_capacity` | integer | 1000 | Maximum episodes stored |
| `episodic_memory_consolidation_threshold` | integer | 5 | Episode consolidation threshold |

### Neuromodulation

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `dopamine_baseline` | float | 0.1 | Baseline dopamine level |
| `dopamine_gain` | float | 1.0 | Dopamine gain factor |
| `curiosity_baseline` | float | 0.1 | Baseline curiosity level |
| `novelty_threshold` | float | 0.5 | Novelty detection threshold |

### Environment

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `environment_name` | string | "SimpleWorld" | Environment type |
| `environment_width` | integer | 20 | World width |
| `environment_height` | integer | 20 | World height |
| `vision_field_of_view` | float | 180.0 | Vision field of view (degrees) |

### Logging

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `log_level` | string | "INFO" | Log level (DEBUG, INFO, WARNING, ERROR, CRITICAL) |
| `log_to_file` | boolean | false | Enable file logging |
| `log_filename` | string | "nlm.log" | Log file name |
| `log_max_file_size` | integer | 100 | Max log file size (MB) |
| `log_rotation_count` | integer | 5 | Number of log backup files |

### Performance and Debugging

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `enable_multithreading` | boolean | false | Enable multithreading |
| `thread_count` | integer | 0 | Number of threads (0 = auto) |
| `enable_metrics` | boolean | true | Enable metrics collection |
| `enable_performance_monitoring` | boolean | false | Enable performance monitoring |
| `debug_output` | boolean | false | Enable debug output |

## Experimental Features

### Checkpointing

- **Enabled**: `--checkpoint-enabled true`
- **Interval**: `--checkpoint-interval 10000`
- **Path**: `--checkpoint-path /tmp/nlm_checkpoint_%.p`
- **Save**: `--save <path>` saves brain state after simulation
- **Load**: `--load <path>` loads brain state for continuation

### Replay and Consolidation

- **Replay**: Episodic memory replay during simulation
- **Consolidation**: Memory consolidation for important episodes
- **Sleep/Rest**: Integration with sleep/rest cycles

### Development

- **Stages**: Initial → CriticalPeriod → Maturation → Adult → Aging
- **Plasticity modulation**: Different rates in different stages
- **Excitability changes**: Neural excitability changes with development

## System Verification

### Integration Testing

The system includes comprehensive integration tests:

1. **Memory Integration**
   - Working memory presence and functionality
   - Episodic memory encoding and recall

2. **Neuromodulation Integration**
   - Dopamine system functionality
   - Curiosity-driven exploration
   - Novelty detection

3. **Prediction System Integration**
   - Predictive coding capabilities
   - Prediction error computation

4. **Development Integration**
   - Developmental stage changes
   - Structural plasticity modulation

5. **Checkpointing**
   - Save and load functionality
   - State persistence across simulations

6. **Replay**
   - Episodic memory replay capability
   - Consolidation through replay

## Common Issues and Solutions

### Issue 1: Simulation Crashes or Instability

**Symptoms**: Simulation terminates unexpectedly, numerical errors, neuron potentials go out of bounds.

**Solutions**:

1. Check configuration values are within valid ranges
2. Increase simulation timestep slightly (0.002 - 0.005)
3. Disable performance optimizations if using experimental features
4. Check for memory leaks in custom configurations

**Common fixes**:

```bash
# Use safe configuration
nlm --phase2 --config configs/safe.cfg

# Disable optimizations
nlm --phase2 --no-multithreading --no-simd
```

### Issue 2: Long Simulation Times

**Symptoms**: Simulation takes excessive time to complete.

**Solutions**:

1. Reduce neuron count: `--neurons 100`
2. Reduce simulation steps: `--steps 1000`
3. Disable multithreading if not needed
4. Use event-driven mode for sparse activity

**Example optimized run**:

```bash
nlm --phase2 --neurons 100 --steps 500 --no-multithreading
```

### Issue 3: Low Learning or No Plasticity

**Symptoms**: No synaptic changes observed, learning not occurring.

**Solutions**:

1. Check plasticity flags are enabled in configuration
2. Verify neuromodulation systems are working
3. Check reward signals are being generated
4. Increase plasticity weights in configuration

**Configuration check**:

```ini
# In your config file:
plasticity_learning_rate = 0.05
stdp_ltp_weight = 0.05
stdp_ltd_weight = 0.04
dopamine_baseline = 0.5
curiosity_baseline = 0.3
```

### Issue 4: Memory Usage Too High

**Symptoms**: System runs out of memory with large networks.

**Solutions**:

1. Use sparse connectivity: `--enable-sparse true`
2. Reduce memory system capacity
3. Use checkpointing to save intermediate states
4. Disable visualization and profiling

**Memory optimization**:

```bash
nlm --phase2 --enable-sparse true --no-visualization
```

## Advanced Usage

### Custom Configuration File Creation

Create custom configuration files for research experiments:

```bash
# Create a custom config
nlm --custom --config configs/research.cfg

# Template for research configs
nlm --save research_brain_001.bin --custom --config configs/research.cfg
```

### Performance Tuning

For large-scale simulations:

```bash
# High-performance configuration
nlm --phase6 --neurons 5000 --steps 10000 \
    --enable-multithreading --thread-count 8 \
    --enable-sparse true --enable-simd true \
    --checkpoint-interval 5000
```

### Development and Debugging

For debugging and development:

```bash
# Debug mode with verbose output
nlm --verbose --debug --phase3

# Run with performance profiling
nlm --performance-profiling both --phase6

# Small test for rapid iteration
nlm --demo --phase2 --neurons 50 --steps 100
```

## Scientific Notes

### Biological Plausibility

This implementation follows biological constraints where possible:

- **Neuron dynamics**: LIF model approximates real neuron behavior
- **Plasticity**: STDP follows experimental findings
- **Memory**: Inspired by working and episodic memory systems
- **Development**: Includes developmental stages and critical periods

### Limitations

Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No detailed ion channel dynamics
- Limited dendritic morphology modeling
- No claim of consciousness or human-like cognition

### Research Applications

This system can be used to investigate:

- How neural dynamics give rise to cognition
- The role of neuromodulation in learning
- Memory systems integration with neural processing
- Development effects on learning capabilities
- The emergence of behavior from neural activity

## Troubleshooting

### Command Not Found

If `nlm` command is not found:

```bash
# Check installation
ls build/nlm

# Build if needed
mkdir build && cd build && cmake .. && make
```

### Dependencies Missing

Required dependencies:

- C++17 compatible compiler (g++ 7.0+ or clang++ 5.0+)
- CMake 3.16+
- POSIX threads (pthread)

### Permission Errors

- Check write permissions for output files
- Ensure log directory exists if using file logging
- Check checkpoint directory permissions

## Getting Help

### Online Resources

- **Documentation**: `docs/ARCHITECTURE.md`
- **Scientific Background**: `docs/SCIENCE.md`
- **Development Roadmap**: `docs/ROADMAP.md`
- **Experiments Documentation**: `docs/EXPERIMENTS.md`
- **Limitations**: `docs/LIMITATIONS.md`
- **Phase 6 Audit**: `docs/PHASE6_FINAL_AUDIT.md`

### Community and Support

For additional help and support:

1. Check the documentation in the `docs/` directory
2. Review examples in the `examples/` directory (if present)
3. Check issue tracker for known issues
4. Review recent changes in version control

### Bug Reports

When reporting bugs, include:

1. Command used to reproduce the issue
2. Configuration file (if any)
3. Log output (set `--verbose`)
4. Expected vs. actual behavior
5. System specifications (CPU, memory, compiler)

## Future Enhancements

### Planned Features

1. **Enhanced Neuron Models**
   - Hodgkin-Huxley dynamics
   - Dendritic computation
   - Spike-frequency adaptation

2. **Advanced Plasticity**
   - Homeostatic plasticity
   - Metaplasticity
   - Neuromodulator-specific rules

3. **Rich Memory Systems**
   - Semantic memory
   - Procedural memory
   - Working memory with interference

4. **Sophisticated Cognition**
   - Theory of mind
   - Causal reasoning
   - Abstract conceptual thinking

5. **Improved Development**
   - Epigenetic factors
   - Experience-dependent development
   - Lifespan changes

### Research Opportunities

- Investigation of consciousness emergence
- Study of developmental trajectories
- Exploration of social learning mechanisms
- Analysis of computational efficiency
- Testing of various environmental factors

## Conclusion

The NLM system represents a significant advancement in artificial brain research, combining:

- Real neural computation with LIF dynamics
- Comprehensive plasticity mechanisms
- Integrated memory systems
- Sophisticated neuromodulation
- Rich cognitive capabilities
- Performance optimizations for scalability

This documentation provides a comprehensive guide to using, configuring, and understanding the NLM system for both users and developers. The enhanced command-line interface, robust configuration system, and improved neural implementations make this version particularly suitable for both research and educational purposes.

The system continues to evolve, and future versions will incorporate additional biological realism and cognitive capabilities while maintaining the core principles of neural inspiration and emergent behavior.
