# NLM User Guide

This document provides comprehensive information about the NLM (Neural Learning Machine) project, including all the improvements and new features made in this enhancement.

## Overview

NLM is an experimental computational brain project that aims to create a neural system beginning in a primitive developmental state and acquiring increasingly complex abilities through interaction with an environment. It implements brain-inspired architecture with neurons and synapses, developing and adapting over time as cognition emerges from neural dynamics.

## Project Structure

The project is organized into several phases, each focusing on specific aspects of neural computation:

1. **Phase 1**: Core types, configuration, and neural interfaces
2. **Phase 2**: Real spiking neural computation (currently running)
3. **Phase 3**: World interaction loop and sensory-motor coordination
4. **Phase 4**: Cognitive mechanisms (neural attention, planning, prediction)
5. **Phase 5**: Performance optimizations and lifetime learning
6. **Phase 6**: Final integration (completed)

## Building and Running

### Prerequisites

- C++20 compiler
- CMake 3.16 or higher
- Parallel build tools (make -j4, ninja, etc.)

### Build Commands

```bash
# Create build directory and configure
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j4

# Or build with all demos
make nlm nlm_phase3_demo nlm_phase4_demo nlm_phase5_demo nlm_phase6_demo
```

### Available Executables

1. **nlm** - Main Phase 2 demo (real neural computation)
   - Demonstrates: LIF neuron dynamics, spike propagation, STDP/Hebbian plasticity

2. **nlm_phase3_demo** - Phase 3 world interaction demo
   - Features: reward modulation, curiosity, structural plasticity, development

3. **nlm_phase4_demo** - Phase 4 emerging cognition demo
   - Tests: temporal prediction, working memory, episodic recall, concept formation, planning

4. **nlm_phase5_demo** - Phase 5 lifetime experiments (NEW)
   - Runs comprehensive lifetime learning experiments with development phases
   - Usage: `./nlm_phase5_demo [num_seeds] [steps_per_seed]`

5. **nlm_phase6_demo** - Phase 6 integration test (NEW)
   - Comprehensive integration test verifying all brain systems are connected
   - Usage: `./nlm_phase6_demo`

## Configuration and Command Line Options

### Configuration Files

The NLM system uses simple key=value format configuration files. Common configuration files are located in the `configs/` directory:

- `configs/default.cfg` - Default configuration

Configuration file format:
```
# Comments start with # or /
key = value
another_key = "quoted value"
number_key = 42
float_key = 0.5
bool_key = true
```

### Command Line Arguments

Most executables support the following command line options:

```bash
# Specify configuration file
./executable --config=custom.cfg

# Override configuration values
./executable --neuron_count=2000 --region_count=2 --random_seed=123

# -key value format (alternative to --key=value)
./executable -neuron_count 2000 -region_count 2
```

### Common Configuration Options

- `neuron_count` - Number of neurons in the simulation
- `region_count` - Number of neural regions
- `connection_probability` - Probability of synaptic connections
- `random_seed` - Random seed for reproducible experiments
- `simulation_timestep` - Time step for simulation (in seconds)
- `stdp_ltp_weight` - STDP long-term potentiation weight
- `stdp_ltd_weight` - STDP long-term depression weight
- `stdp_tau` - STDP time constant
- `synaptogenesis_rate` - Rate of new synapse formation
- `pruning_rate` - Rate of synapse elimination

## AgentBrain Advanced Control (NEW)

The `AgentBrain` class now includes comprehensive advanced control mechanisms for fine-tuning agent behavior:

### Exploration vs. Exploitation Control

```cpp
// Set exploration level (0.0 = deterministic, 1.0 = fully random)
agent->setExplorationLevel(0.3f);

// Check if agent is currently exploring
if (agent->isExploring()) {
    // Agent is exploring new behaviors
}
```

### Neuromodulation and Learning Control

```cpp
// Enable/disable reward prediction
agent->enableRewardPrediction(true);

// Set reward sensitivity (affects motivation)
agent->setRewardSensitivity(2.0f);

// Set learning rate modifier (affects plasticity)
agent->setLearningRateModifier(0.8f);
```

### Memory and Development Control

```cpp
// Control memory consolidation
agent->enableMemoryConsolidation(true);
agent->setMemoryRetentionTarget(0.7f);  // 0.0=short-term, 1.0=long-term

// Control developmental acceleration
agent->setDevelopmentalAcceleration(1.5f);
```

### Social and Curiosity Control

```cpp
// Enable/disable social learning
agent->enableSocialLearning(true);
agent->setImitationThreshold(0.8f);

// Control curiosity-driven exploration
agent->setCuriosityThreshold(0.3f);
```

### Debugging and Monitoring

```cpp
// Enable debug output
agent->enableDebugOutput(true);

// Get behavioral state description
std::string state = agent->getBehavioralState();

// Get performance metrics
std::string metrics = agent->getPerformanceMetrics();
```

### Task Context and Adaptation

```cpp
// Set current task for behavior adaptation
agent->setTaskContext("exploration");

// Get adaptation progress for current task
float progress = agent->getAdaptationProgress();
```

## Experiment Framework Improvements (NEW)

### Enhanced Experiment Classes

#### Phase 4 Experiments (Improved)

1. **ObjectPermanenceExperiment** - Tests if hidden objects remain relevant to behavior
   - Now uses the brain's working memory system for realistic object persistence
   - Returns success rate based on similarity matching

2. **All Phase 4 Experiments** - Enhanced with proper initialization and cleanup
   - Constructor/destructor implementations added
   - Better error handling and validation

#### Phase 5 Experiments (Enhanced)

1. **DamageRecoveryExperiment** - Simulates neural damage and recovery
   - Now actually applies damage by weakening synapses below threshold
   - Includes post-damage recovery measurement
   - Calculates recovery success ratio

2. **All Phase 5 Experiments** - Implemented with realistic simulation logic
   - Phases 1-6 now have meaningful implementations
   - Proper error handling and progress tracking

### ExperimentRunner Improvements

The `ExperimentRunner` class now includes:

- Better error handling and validation
- Proper cleanup and resource management
- Enhanced result processing capabilities

## Logging and Error Handling Improvements

### Enhanced Logging System

The logging system now includes:

1. **Advanced Log Levels**: Debug, Info, Warning, Error, Critical
2. **File Output Support**: ConsoleLogger can log to files in addition to console
3. **Color Output**: Optional colored terminal output
4. **Structured Logging**: Log entries include file, line number, and function information

### Improved Error Handling

- Better exception handling throughout the codebase
- More informative error messages
- Graceful degradation when components fail to initialize
- Validation of inputs and parameters

## Configuration System Enhancements

### Enhanced Config Class

The `Config` class now supports:

1. **Type-safe value access**: Templates for getting values with type checking
2. **Multiple value types**: Strings, integers, doubles, booleans, vectors
3. **Source tracking**: Tracks whether values come from defaults, files, or command line
4. **Summary generation**: Detailed configuration summary with source information
5. **Better error handling**: Graceful handling of missing or invalid values

### Configuration File Format

```bash
# Simple key=value format
neuron_count = 1000
connection_probability = 0.1
random_seed = 42
simulation_timestep = 0.001

# Quoted strings support
config_name = "default"
description = "NLM Phase 4 demo with enhanced cognition"

# Boolean values
enable_debug = true
enable_colors = false
```

## Build System Improvements (NEW)

### Updated CMakeLists.txt

The build system now includes:

1. **Phase 5 and Phase 6 Libraries**: All experiments now have corresponding build targets
2. **Phase 5 Demo Executable**: `nlm_phase5_demo` for lifetime experiments
3. **Phase 6 Demo Executable**: `nlm_phase6_demo` for integration testing
4. **Proper Dependencies**: Correct library linking between phases
5. **Install Targets**: Updated install commands to include all new targets

### Build Targets

```bash
# All targets
make nlm nlm_phase3_demo nlm_phase4_demo nlm_phase5_demo nlm_phase6_demo

# Just Phase 5 experiments
make nlm_phase5

# Just Phase 6 demo
make nlm_phase6_demo
```

## Code Quality Improvements

### Memory Management

- Fixed memory leaks in `ExperimentLogger` with proper virtual destructor
- Implemented RAII patterns for resource management
- Added proper cleanup in destructors

### Documentation

- Enhanced header comments
- Added parameter descriptions
- Improved code organization and structure

### Error Handling

- Consistent error handling patterns throughout
- Better error messages with context
- Graceful failure modes

## Testing and Validation

### Integration Tests

The new Phase 6 demo includes comprehensive integration tests:

1. **Integration Verification**: Verifies all brain systems are connected
2. **Memory Integration**: Tests working memory and episodic memory
3. **Neuromodulation Integration**: Tests dopamine, curiosity, novelty systems
4. **Checkpoint Testing**: Validates save/load functionality
5. **Replay Testing**: Tests episodic memory replay capabilities

### Experiment Results

All experiments now provide:

- Detailed performance metrics
- Statistical analysis
- Comparative results
- JSON and CSV export capabilities

## Performance Considerations

### Memory Efficiency

- Working memory usage optimized
- Memory pools for frequently allocated objects
- Sparse connectivity representations

### Computational Efficiency

- Event-driven simulation for spike propagation
- SIMD optimization opportunities
- Multi-threading support for large-scale simulations
- Parallel processing capabilities

### Scalability

- Support for 10,000+ neurons
- Configurable memory pools
- Event-driven architecture for handling sparse activity

## Future Extensions

### Planned Features

1. **Full Neural Architecture**: Hodgkin-Huxley neuron models
2. **Advanced Learning Rules**: Spike-timing-dependent plasticity variants
3. **Cognitive Architectures**: More complex reasoning mechanisms
4. **Multi-Modal Perception**: Integration of multiple sensory modalities
5. **Social Learning**: Advanced multi-agent interaction

### Research Areas

- Neuronal dynamics and spike generation
- Memory consolidation and replay
- Developmental trajectories and plasticity
- Learning theory and optimization
- Neural invariants and abstraction

## Support and Troubleshooting

### Common Issues

1. **Build failures**: Ensure CMake 3.16+ and C++20 support
2. **Configuration errors**: Check file syntax and value types
3. **Memory issues**: Monitor memory usage with large neuron counts
4. **Performance bottlenecks**: Consider parallelization options

### Getting Help

- Check the documentation in the `docs/` directory
- Review experiment source code for configuration examples
- Use debug output for troubleshooting
- Experiment with different parameter settings

## License

MIT License - See LICENSE file for details.

---

## Version Information

- **Project**: NLM (Neural Learning Machine) v0.1.0
- **Current Phase**: Phase 6 (Final Integration)
- **Build Date**: $(date)
- **Compiler**: C++20

This document covers all improvements made to the NLM project, from build system updates to new features and enhanced functionality. For specific questions about particular components or experiments, please refer to the relevant source code documentation.
