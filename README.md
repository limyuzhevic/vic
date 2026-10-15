# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

### What NLM IS NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

### What NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## System Architecture

NLM follows a phase-based approach to brain development and functionality:

```
Phase 1-2: Core Neural Computation
  ┌─────────────────────────────┐
  │         Brain              │
  │  ┌─────────────┐            │
  │  │  Neuron    │            │
  │  │ (LIF, LIF) │            │
  │  └─────────────┘            │
  │  ┌─────────────┐  ┌─────────┐
  │  │  Synapse    │  │   STDP  │
  │  │ (Weights)  │  │  Hebbian │
  │  └─────────────┘  └─────────┘
  │  ┌─────────────┐            │
  │  │  Brain      │            │
  │  │ (Coordinator)           │
  │  └─────────────┘            │
  └─────────────────────────────┘

Phase 3: World Interaction
  ┌─────────────────────────────────┐
  │        Agent Interface          │
  │  ┌─────────────┐              │
  │  │ Sensory     │              │
  │  │Transduction │              │
  │  └─────────────┘              │
  │  ┌─────────────┐  ┌─────────┐  │
  │  │   World     │  │   Motor │  │
  │  │ (Simple 2D)│  │Output   │  │
  │  └─────────────┘  └─────────┘  │
  └─────────────────────────────────┘

Phase 4: Emerging Cognition
  ┌─────────────────────────────────┐
  │           Cranial Intelligence   │
  │  ┌─────────────┐  ┌─────────┐  │
  │  │ Attention   │  │  Plan   │  │
  │  │Selection    │  │ning     │  │
  │  └─────────────┘  └─────────┘  │
  │  ┌─────────────┐  ┌─────────┐  │
  │  │ Prediction  │  │  Self   │  │
  │  │  System     │  │  Model  │  │
  │  └─────────────┘  └─────────┘  │
  │  ┌─────────────┐  ┌─────────┐  │
  │  │  Memory     │  │ Social │  │
  │  │Systems      │  │Learning │  │
  │  └─────────────┘  └─────────┘  │
  └─────────────────────────────────┘
```

## Current Phase: 6 (Final Integration)

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Getting Started

### Installation

NLM requires a C++20 compatible compiler and CMake. The project uses a modern build system that separates the core neural computation from the test infrastructure.

#### Basic Build

```bash
# Clone the repository
cd nlm-project

# Create build directory and configure
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j4
```

#### Build with Tests

```bash
# Build with test infrastructure
cmake .. -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug
make -j4
```

### Running Tests

NLM includes comprehensive test suites that test:

- **Neuron tests**: LIF dynamics, plasticity, spike processing
- **Synapse tests**: Weight management, spike history, plasticity flags
- **STDP tests**: Temporal-based learning
- **Config tests**: Configuration management and validation
- **Integration tests**: Full system behavior

```bash
# Run comprehensive test suite
./nlm_test  # or ./build/nlm_test

# Or run specific test modules
./nlm_test --test neuron_enhanced
./nlm_test --test config_enhanced
./nlm_test --test comprehensive
```

### Running NLM Examples

#### Phase 2: Basic Neural Computation

Run the core neural computation example:

```bash
# Run the Phase 2 neural computation demo
cd build
./nlm_phase2
```

This runs three fundamental tests:
1. **Basic Connectivity**: Demonstrates spike propagation through the network
2. **Plasticity Learning**: Shows synaptic changes through experience
3. **STDP Verification**: Verifies temporal-based learning rules

#### Phase 6: Integration Demo

Run the comprehensive integration test:

```bash
# Run Phase 6 integration demo
cd build
./nlm_phase6_demo
```

This verifies all systems are properly connected and functional.

## Command Line Interface

NLM provides a comprehensive command-line interface for controlling simulation behavior:

### Basic Options

```bash
# Show help
./nlm --help
./nlm -h

# Show version info
./nlm --version

# Verbose output
./nlm --verbose
./nlm -v

# Quiet mode (minimal output)
./nlm --quiet
./nlm -q

# Set number of neurons
./nlm --neurons 1000
./nlm -n 1000

# Set simulation timestep
./nlm --timestep 0.0005
./nlm -t 0.0005

# Specify experiment type
./nlm --experiment plasticity
./nlm -e plasticity
```

### Configuration Options

```bash
# Load configuration from file
./nlm --config my_config.cfg
./nlm -c my_config.cfg

# Save current configuration
./nlm --save-config
./nlm --save-config my_config.cfg

# Set individual parameters
./nlm --neuron_count=2000
./nlm --simulation_timestep=0.002
./nlm --connection_probability=0.1
```

### Output and Logging

```bash
# Save results to file
./nlm --output results.txt

# Log to file instead of stdout
./nlm --log-file simulation.log

# Set log level
./nlm --log-level debug
./nlm --log-level info
```

### Advanced Options

```bash
# Enable GPU acceleration (if supported)
./nlm --enable-gpu

# Set thread count for parallel processing
./nlm --threads 4
./nlm -j 4

# Run in benchmark mode
./nlm --benchmark

# Custom experiment combinations
./nlm --experiment all --neurons 500 --timestep 0.001
./nlm -e plasticity -n 1000 -t 0.0005
```

## Configuration System

NLM uses a robust configuration system that supports:

### Configuration File Format

Configuration files use a simple `key = value` format:

```ini
# NLM Configuration File
# Generated by NLM
# Simulation parameters

random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Feature flags
enable_checkpointing = true
enable_replay = true
enable_development = true
```

### Command Line Override

Command line arguments override configuration file values:

```bash
./nlm --config defaults.cfg --neuron_count=1000 --simulation_timestep=0.002
```

### Configuration Validation

The configuration system validates all parameters:
- Type checking (int, double, string, bool)
- Range validation
- Custom validators for specific parameters
- Source tracking (Default, File, CommandLine, Runtime)

## Error Handling and Robustness

NLM implements comprehensive error handling:

### Error Types

1. **Input Validation**: Validates all parameters and inputs
2. **Range Checking**: Ensures values are within biologically plausible ranges
3. **State Validation**: Checks for consistent neural state
4. **Memory Safety**: Null pointer checks and bounds checking
5. **Exception Safety**: Proper resource cleanup on errors

### Error Recovery

- Graceful degradation on configuration errors
- Automatic recovery from numerical instability
- Detailed error messages with actionable information
- Safe defaults when invalid values are encountered

### Testing and Validation

All error handling is thoroughly tested:

- Unit tests for error conditions
- Integration tests for error recovery
- Property-based testing for edge cases
- Memory safety validation

## Advanced Features

### Development Tools

NLM includes advanced development tools:

#### Configuration Inspector

```bash
# Inspect current configuration
./nlm --config --output config_summary.txt
```

#### Experiment Configuration Generator

```bash
# Generate experiment configurations
./nlm --generate-experiment --neurons 1000 --steps 5000 > my_experiment.cfg
```

#### Benchmark Suite

```bash
# Run performance benchmarks
./nlm --benchmark --output benchmark_results.csv
```

### Extensibility

Adding new components is straightforward:

1. **New Neuron Types**: Implement new dynamics classes
2. **New Plasticity Rules**: Add new plasticity rule implementations
3. **New Memory Systems**: Integrate with existing memory infrastructure
4. **New Sensory Modalities**: Extend input processing

## Performance Considerations

### Memory Management

NLM uses efficient memory management:
- Memory pools for common objects
- Reuse of synaptic states
- Lazy initialization of complex structures

### Computational Efficiency

Optimizations include:
- Vectorized operations where possible
- Lock-free spike processing
- Efficient data structures for sparse networks
- SIMD optimization (where supported)

### Scalability

NLM scales to:
- **10,000+ neurons**: Large-scale networks
- **Multiple regions**: Hierarchical organization
- **Complex behaviors**: Cognitively rich behaviors
- **Real-time performance**: Suitable for interactive applications

## Development Guidelines

### Coding Standards

1. **Consistency**: Follow the established patterns and conventions
2. **Documentation**: Document all public APIs and design decisions
3. **Testing**: Write comprehensive tests for all components
4. **Modularity**: Design clear interfaces between components
5. **Performance**: Profile and optimize critical paths

### Testing Strategy

1. **Unit Tests**: Test individual components in isolation
2. **Integration Tests**: Test component interactions
3. **System Tests**: Test end-to-end behavior
4. **Performance Tests**: Validate performance characteristics
5. **Regression Tests**: Ensure no regressions

### Debugging

NLM provides comprehensive debugging tools:

#### Debug Output

```bash
# Enable detailed debug logging
./nlm --log-level debug --verbose

# Focus on specific components
./nlm --log-level debug --log-file debug.log
```

#### Memory Debugging

```bash
# Memory debugging (requires special build)
./nlm --enable-memory-debug --log-file memory.log
```

#### Performance Profiling

```bash
# Performance profiling
./nlm --profile --output profile_data.csv
```

## Contributing

### Project Structure

```
NLM/
├── src/
│   ├── core/           # Core utilities
│   │   ├── Config/    # Configuration system
│   │   ├── Random/    # Random number generation
│   │   ├── Logger/    # Logging system
│   │   └── ...        # Other core components
│   ├── brain/         # Neural components
│   ├── dynamics/      # Neural dynamics
│   ├── plasticity/    # Plasticity rules
│   ├── development/   # Development system
│   ├── neuromodulation/# Neuromodulators
│   ├── memory/       # Memory systems
│   ├── prediction/   # Prediction systems
│   ├── cognition/    # Cognitive mechanisms
│   ├── sensory/      # Sensory processing
│   ├── motor/        # Motor system
│   └── ...           # Other components
│
├── tests/            # Test suite
│   ├── test_main.cpp # Test runner
│   ├── test_*.cpp    # Individual test modules
│   └── CMakeLists.txt # Test build configuration
│
├── docs/             # Documentation
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── PHASE*.md    # Phase documentation
│   └── HOW_TO_USE.md
│
├── python/           # Python bindings
│   └── ...          # Pybind11 bindings
│
├── configs/          # Default configurations
│   └── default.cfg  # Default configuration
│
├── CMakeLists.txt    # Main build configuration
└── README.md         # This file
```

### Development Workflow

1. **Development Branch**: Work on the main development branch
2. **Feature Branch**: Create feature branches for new functionality
3. **Code Review**: Use pull requests for code review
4. **Testing**: Ensure all tests pass before merging
5. **Documentation**: Update documentation for new features

### Best Practices

1. **Testing First**: Write tests before implementing features
2. **Modular Design**: Design with clear separation of concerns
3. **Error Handling**: Handle errors gracefully and provide informative messages
4. **Performance**: Profile and optimize critical code paths
5. **Documentation**: Document APIs and design decisions

## Troubleshooting

### Common Issues

#### Build Failures

**Issue**: Compiler not found or incompatible
**Solution**: Install C++20 compatible compiler

**Issue**: Missing dependencies
**Solution**: Ensure all build dependencies are installed

**Issue**: CMake version too old
**Solution**: Update CMake to version 3.16 or newer

#### Runtime Errors

**Issue**: "Failed to initialize brain"
**Solution**: Check configuration parameters

**Issue**: Memory errors
**Solution**: Check system resources and increase limits

**Issue**: Invalid configuration
**Solution**: Validate configuration file format and values

#### Test Failures

**Issue**: Test suite fails
**Solution**: Run tests with detailed output to identify failures

**Issue**: Memory test failures
**Solution**: Check for memory leaks or buffer overflows

### Getting Help

#### Online Resources

- **Documentation**: https://nlm.readthedocs.io
- **GitHub**: https://github.com/nlm-project/nlm
- **Discussions**: https://github.com/nlm-project/nlm/discussions
- **Issues**: https://github.com/nlm-project/nlm/issues

#### Community

- **Contributing**: Guidelines for contributing to the project
- **Code of Conduct**: Project code of conduct
- **License**: MIT License

## License

MIT License

## Authors

NLM is a research project developed by:
- NLM Research Team
- Contributors: [List contributors]

For more information about the scientific background and research goals, see the SCIENCE.md and ARCHITECTURE.md files in the docs directory.

---

**Last Updated**: October 15, 2026
**Version**: 2.0.0
**Status**: Phase 6 (Final Integration)