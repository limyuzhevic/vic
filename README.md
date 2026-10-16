# NLM (Neural Learning Machine) - Project Documentation

## Overview

NLM (熙然) is an experimental artificial developmental brain system that implements real spiking neural computation. This is Phase 2 of the NLM project, focusing on implementing real neural dynamics with plasticity, learning, and development capabilities.

## Key Features

### Core Neural Computation
- **Leaky Integrate-and-Fire (LIF) neuron dynamics**
- **Event-driven spike propagation with synaptic delays**
- **STDP and Hebbian plasticity rules**
- **Structural plasticity (synaptogenesis and pruning)**
- **Real neural substrate capable of self-modification**

### Learning and Development
- **Experience-action-consequence learning loops**
- **Reward modulation and neuromodulation systems**
- **Working, episodic, semantic, and procedural memory systems**
- **Cognitive capabilities: prediction, planning, concept formation, attention**
- **Neural development with critical periods and maturation stages**

### Advanced Capabilities (Phase 4)
- **Temporal prediction learning**
- **Working memory and episodic recall**
- **Concept formation and pattern discovery**
- **Neural attention and selective processing**
- **Multi-step planning and decision making**
- **Self-model and body awareness**
- **Social learning and imitation**
- **Continual learning and transfer performance**

## System Architecture

### Layer 1: Core Infrastructure
- Configuration management system
- Random number generation (Mersenne Twister)
- Logging system with configurable levels
- Simulation clock with timestep management

### Layer 2: Neural Computation
- LIF neuron implementation
- Synapse management with plasticity rules
- Neural population and region organization
- Main Brain class orchestrating all computation

### Layer 3: Learning Systems
- Plasticity rules (STDP, Hebbian, structural)
- Neuromodulation systems (reward, novelty, curiosity)
- Memory systems (working, episodic, semantic, procedural)
- Cognitive modules (concept formation, neural planning)

### Layer 4: Perception and Action
- Sensory perception systems (vision, audio, internal signals)
- Agent body with energy management
- Action systems and world interaction
- Agent-Brain interface for decoding motor commands

### Layer 5: Specialized Systems
- Motor control and sensory processing
- Prediction systems and working memory
- Visualization interfaces and diagnostics
- Development systems (synaptogenesis, pruning, maturation)

### Layer 6: Experiments
- Modular experiment frameworks
- Metrics collection and analysis
- Integration experiments combining cognitive modules

## Usage

### Command Line Interface

```bash
# Basic usage with default settings
./nlm

# Advanced configuration options
./nlm [OPTIONS]

Options:
  --help, -h                  Show this help message
  --config=<file>             Load configuration from file (default: configs/default.cfg)
  --enable-logging            Enable detailed logging output
  --enable-visualization      Enable visualization output
  --neurons=<count>           Set number of neurons (default: 500)
  --regions=<count>           Set number of regions (default: 1)
  --timestep=<value>          Set simulation timestep in seconds (default: 0.001)
  --seed=<value>              Set random seed (default: 42)
  --connprob=<value>          Set connection probability (default: 0.15)

Examples:
  nlm                                    Run with default settings
  nlm --neurons=1000 --regions=2         Run with 1000 neurons in 2 regions
  nlm --config=myconfig.cfg              Load custom configuration
  nlm --seed=12345                       Run with reproducible random seed
```

### Configuration File Format

NLM uses a simple key=value format for configuration files (Phase 1 format), with plans for JSON/YAML support in Phase 2.

```ini
# NLM Configuration File

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log
```

### Phase 3 Demo

```bash
# Experience-action-consequence learning demo
./nlm_phase3_demo [episodes] [steps]

Arguments:
  episodes         Number of episodes to run (default: 5)
  steps           Steps per episode (default: 200)

Example:
  ./nlm_phase3_demo 10 500
```

### Phase 4 Demo

```bash
# Comprehensive cognitive capabilities demonstration
./nlm_phase4_demo [trials]

Arguments:
  trials          Number of trials per experiment (default: 50)

Example:
  ./nlm_phase4_demo 100
```

## Testing

The NLM project includes comprehensive unit tests covering all major components:

```bash
# Build and run tests
mkdir build
cd build
cmake ..
make
ctest
```

Available test suites:
- **test_types.cpp** - Neural ID types (NeuronId, SynapseId, RegionId, PopulationId)
- **test_config.cpp** - Configuration management system
- **test_random.cpp** - Random number generation (Mersenne Twister)
- **test_clock.cpp** - Simulation clock and time management
- **test_neuron.cpp** - LIF neuron dynamics and spiking
- **test_synapse.cpp** - Synapse behavior and transmission
- **test_stdp.cpp** - Spike-Timing Dependent Plasticity
- **test_brain.cpp** - Brain initialization and basic functionality

## Configuration Validation

The configuration system includes comprehensive validation:

### Neuron Count Validation
- Range: 1 to 1,000,000
- Ensures positive, manageable brain size

### Region Count Validation  
- Range: 1 to 100
- Ensures reasonable regional organization

### Connection Probability Validation
- Range: 0.0 to 1.0
- Ensures valid probability for synaptic connections

### Simulation Timestep Validation
- Range: 0.0001 to 1.0 seconds
- Ensures numerical stability and reasonable time steps

### Plasticity Rate Validation
- Range: 0.0 to 1.0
- Ensures valid learning rates

### STDP Weight Validation
- Range: -10.0 to 10.0
- Ensures reasonable synaptic weight modifications

## Advanced Features

### Command Line Argument Parsing
- Enhanced argument parsing with validation
- Support for both --key=value and -k value formats
- Comprehensive error messages for invalid arguments
- Backward compatibility with existing CLI interface

### Configuration Override System
- Multiple configuration sources with priority resolution
- Runtime configuration can override file/command-line settings
- Detailed logging of configuration changes

### Performance Monitoring
- Real-time statistics collection
- Memory usage tracking
- Firing rate analysis
- Simulation progress monitoring

### Advanced Diagnostics
- Brain health checks
- Connectivity analysis
- Plasticity status monitoring
- Development stage tracking

## Development Guidelines

### Code Quality
- C++20 standard with modern C++ practices
- Pimpl idiom for implementation hiding
- RAII for resource management
- Exception safety and proper error handling

### Testing
- Comprehensive unit test coverage
- Test-driven development approach
- Mock objects for testing external dependencies
- Automated test execution with CMake

### Configuration
- Environment-specific configuration files
- Command-line argument flexibility
- Runtime configuration modification
- Validation and error recovery

## Future Directions (Phase 3)

### Enhanced Configuration System
- JSON/YAML configuration file support
- Configuration schemas and validation rules
- Configuration templates and presets
- Dynamic configuration reloading

### Advanced Neural Features
- Multi-layer network topologies
- Long short-term memory (LSTM) units
- Attention mechanisms
- Graph neural networks

### Enhanced Learning Capabilities
- Curriculum learning systems
- Meta-learning and few-shot learning
- Transfer learning and knowledge distillation
- Inverse reinforcement learning

### Advanced Cognitive Functions
- Theory of mind and social cognition
- Causal reasoning and counterfactual thinking
- Abstract reasoning and symbolic AI integration
- Consciousness and self-awareness models

## Troubleshooting

### Common Issues

1. **Configuration file not found**
   - Ensure the file exists at the specified path
   - Check file permissions
   - Use absolute paths if relative paths fail

2. **Invalid configuration values**
   - Check configuration file for correct syntax
   - Verify value ranges for known configuration keys
   - Use `--config=default` to reset to working configuration

3. **Memory issues**
   - Reduce neuron_count and region_count
   - Check system resources
   - Consider running with `--enable-logging=error` to reduce memory usage

4. **Simulation instability**
   - Check simulation_timestep value (must be positive)
   - Verify connection_probability is within valid range
   - Try different random_seed values

### Debugging

- Enable debug logging: `./nlm --enable-logging`
- Check console output for error messages
- Review system logs if logging to file is enabled
- Use configuration validation tools for complex setups

## License

This project is part of the NLM (Neural Learning Machine) research initiative.
See individual source files for specific licensing information.

## Acknowledgments

This project builds upon numerous contributions to computational neuroscience,
neural network research, and artificial intelligence development. The NLM
project aims to create a biologically-inspired artificial developmental brain
system with the capacity for autonomous learning and adaptation.
